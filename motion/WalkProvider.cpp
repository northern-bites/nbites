
// This file is part of Man, a robotic perception, locomotion, and
// team strategy application created by the Northern Bites RoboCup
// team of Bowdoin College in Brunswick, Maine, for the Aldebaran
// Nao robot.
//
// Man is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Man is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser Public License for more details.
//
// You should have received a copy of the GNU General Public License
// and the GNU Lesser Public License along with Man.  If not, see
// <http://www.gnu.org/licenses/>.

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

#include "Sensors.h"
#include "WalkProvider.h"
using namespace std;

using namespace Kinematics;

WalkProvider::WalkProvider(shared_ptr<Sensors> s,
						   shared_ptr<Profiler> p)
    : MotionProvider(WALK_PROVIDER, p),
      sensors(s),
      curGait(NULL),nextGait(NULL),
      stepGenerator(sensors),
      pendingCommands(false),
      nextCommand(NULL)
{
    pthread_mutex_init(&walk_provider_mutex,NULL);

    //Currently, the arm angles are static, and never change
    larm_angles = vector<float>(LARM_WALK_ANGLES,
                                LARM_WALK_ANGLES + ARM_JOINTS);
    rarm_angles = vector<float>(RARM_WALK_ANGLES,
                                RARM_WALK_ANGLES + ARM_JOINTS);
    setActive();
}

WalkProvider::~WalkProvider() {
    pthread_mutex_destroy(&walk_provider_mutex);
}

void WalkProvider::requestStopFirstInstance() {
    setCommand(new WalkCommand(0.0f, 0.0f, 0.0f));
}

void WalkProvider::hardReset(){
    pthread_mutex_lock(&walk_provider_mutex);
    stepGenerator.resetHard();

    setActive();
    pthread_mutex_unlock(&walk_provider_mutex);
}

void WalkProvider::calculateNextJointsAndStiffnesses() {
	PROF_ENTER(profiler,P_WALK);
    pthread_mutex_lock(&walk_provider_mutex);
    if ( nextGait != curGait){
        if( stepGenerator.resetGait(nextGait)){
            curGait = nextGait;
        }else{
            cout << "Failed to set gait, trying again next time"<<endl;
        }
    }
    if(nextCommand){
        stepGenerator.setSpeed(nextCommand->x_mms,
                               nextCommand->y_mms,
                               nextCommand->theta_rads);
    }
    pendingCommands = false;
    nextCommand = NULL;


    if(!isActive()){
        cout << "WARNING, I wouldn't be calling the Walkprovider while"
            " it thinks its DONE if I were you!" <<endl;
    }

    //ask the step Generator to update ZMP values, com targets
    stepGenerator.tick_controller();

    // Now ask the step generator to get the leg angles
	PROF_ENTER(profiler,P_TICKLEGS);
    WalkLegsTuple legs_result = stepGenerator.tick_legs();
	PROF_EXIT(profiler,P_TICKLEGS);

    //Get the joints and stiffnesses for each Leg
    vector<float> lleg_joints = legs_result.get<LEFT_FOOT>().get<JOINT_INDEX>();
    vector<float> rleg_joints = legs_result.get<RIGHT_FOOT>().get<JOINT_INDEX>();
    vector<float> lleg_gains = legs_result.get<LEFT_FOOT>().get<STIFF_INDEX>();
    vector<float> rleg_gains = legs_result.get<RIGHT_FOOT>().get<STIFF_INDEX>();

    //grab the stiffnesses for the arms
    vector<float> larm_gains(ARM_JOINTS, curGait->armStiffness);
    vector<float> rarm_gains(ARM_JOINTS, curGait->armStiffness);


    //Return the joints for the legs
    setNextChainJoints(LARM_CHAIN,larm_angles);
    setNextChainJoints(LLEG_CHAIN,lleg_joints);
    setNextChainJoints(RLEG_CHAIN,rleg_joints);
    setNextChainJoints(RARM_CHAIN,rarm_angles);

    //Return the stiffnesses for each joint
    setNextChainStiffnesses(LARM_CHAIN,larm_gains);
    setNextChainStiffnesses(LLEG_CHAIN,lleg_gains);
    setNextChainStiffnesses(RLEG_CHAIN,rleg_gains);
    setNextChainStiffnesses(RARM_CHAIN,rarm_gains);


    setActive();
    pthread_mutex_unlock(&walk_provider_mutex);
	PROF_EXIT(profiler,P_WALK);
}

void WalkProvider::setCommand(const WalkCommand * command){
    //grab the velocities in mm/second rad/second from WalkCommand
    pthread_mutex_lock(&walk_provider_mutex);
    if(nextCommand)
        delete nextCommand;
    nextCommand =command;
    pendingCommands = true;

    setActive();
    pthread_mutex_unlock(&walk_provider_mutex);
}


void WalkProvider::setCommand(const boost::shared_ptr<GaitCommand> command){
    nextGait = new WalkingParameters(command->getGait());
}

void WalkProvider::setActive(){
    //check to see if the walk engine is active
    if(stepGenerator.isDone() && !pendingCommands){
        inactive();
    }else{
        active();
    }
}

std::vector<BodyJointCommand *> WalkProvider::getGaitTransitionCommand(){
    vector<float> curJoints = sensors->getMotionBodyAngles();
    vector<float> * gaitJoints = nextGait->getWalkStance();

    float max_change = -M_PI_FLOAT*10.0f;
    int max_index = -1;
    for(unsigned int i = 0; i < gaitJoints->size(); i++){

        if (max_change < abs(gaitJoints->at(i) - curJoints.at(i+HEAD_JOINTS)))
            max_index = i;
        max_change = max(max_change,
						 fabs(gaitJoints->at(i) - curJoints.at(i+HEAD_JOINTS)));
    }

    // this is the max we allow, not the max the hardware can do
    const float  MAX_RAD_PER_SEC =  M_PI_FLOAT*0.15f;
    float time = max_change/MAX_RAD_PER_SEC;

    vector<BodyJointCommand *> commands;

    if(time <= 0.02f)
        return commands;

    //larm: (0.,90.,0.,0.)
    //rarm: (0.,-90.,0.,0.)
    float larm_angles[] = {0.0f, M_PI_FLOAT*0.35f,0.0f,0.0f};
    float rarm_angles[] = {0.0f,-M_PI_FLOAT*0.35f,0.0f,0.0f};

    vector<float> *safe_larm = new vector<float>(larm_angles,
                                                 &larm_angles[ARM_JOINTS]);
    vector<float> * safe_rarm =new vector<float>(rarm_angles,
                                                 &rarm_angles[ARM_JOINTS]);

	// HACK @joho get gait stiffness params. nextGait->maxStiffness
	vector<float> * stiffness = new vector<float>(Kinematics::NUM_JOINTS,
												  0.85f);
	vector<float> * stiffness2 = new vector<float>(Kinematics::NUM_JOINTS,
												  0.85f);

    commands.push_back(new BodyJointCommand(1.0f,safe_larm,NULL,NULL,safe_rarm,
											stiffness,
                                            Kinematics::INTERPOLATION_SMOOTH));

    commands.push_back(new BodyJointCommand(time,gaitJoints,
											stiffness2,
                                            Kinematics::INTERPOLATION_SMOOTH));
    return commands;
}
