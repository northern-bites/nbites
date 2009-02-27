
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
using Kinematics::LLEG_CHAIN;
using Kinematics::RLEG_CHAIN;

WalkProvider::WalkProvider(shared_ptr<Sensors> s)
    : MotionProvider(WALK_PROVIDER),
      sensors(s),
      curGait(NULL),nextGait(NULL),
      stepGenerator(sensors),
      pendingCommands(false),
      nextCommand(NULL)
{
    pthread_mutex_init(&walk_provider_mutex,NULL);

    //Make up something arbitrary for the arms
    const float larm[ARM_JOINTS] = {M_PI/2,M_PI/10,-M_PI/2,-M_PI/2};
    const float rarm[ARM_JOINTS] = {M_PI/2,-M_PI/10,M_PI/2,M_PI/2};
    larm_angles = vector<float>(larm,larm+ARM_JOINTS);
    rarm_angles = vector<float>(rarm,rarm+ARM_JOINTS);

    setActive();
}

WalkProvider::~WalkProvider() {
    pthread_mutex_destroy(&walk_provider_mutex);
}

void WalkProvider::requestStopFirstInstance() {
    setCommand(new WalkCommand(0.0f, 0.0f, 0.0f));
}

void WalkProvider::calculateNextJoints() {
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


    //ask the step Generator to update ZMP values, com targets
    stepGenerator.tick_controller();

    // Now ask the step generator to get the leg angles
    WalkLegsTuple legs_result = stepGenerator.tick_legs();

    //Get the joints for each Leg
    vector<float> lleg_results = legs_result.get<LEFT_FOOT>();
    vector<float> rleg_results = legs_result.get<RIGHT_FOOT>();

    // vector<float> rarm_results = vector<float>(ARM_JOINTS, 0.0f);
//     vector<float> larm_results = vector<float>(ARM_JOINTS, 0.0f);

//     cout << "2rarm size "<< rarm_results.size()
//          << "larm size "<< larm_results.size() <<endl;


    //Return the joints for the legs
    setNextChainJoints(LARM_CHAIN,larm_angles);
    setNextChainJoints(LLEG_CHAIN,lleg_results);
    setNextChainJoints(RLEG_CHAIN,rleg_results);
    setNextChainJoints(RARM_CHAIN,rarm_angles);

    setActive();
    pthread_mutex_unlock(&walk_provider_mutex);
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


void WalkProvider::setCommand(const GaitCommand * command){
    nextGait = new WalkingParameters(command->getGait());
    delete command;
}

void WalkProvider::setActive(){
    //check to see if the walk engine is active
    if(stepGenerator.isDone() && !pendingCommands){
        inactive();
    }else{
        active();
    }
}

BodyJointCommand * WalkProvider::getGaitTransitionCommand(){
    //pthread_mutex_lock(&walk_provider_mutex);
    vector<float> curJoints = sensors->getMotionBodyAngles();
    vector<float> * gaitJoints = nextGait->getWalkStance();

    float max_change = -M_PI*10.0f;

    for(unsigned int i = 0; i < gaitJoints->size(); i++){

        max_change = fmax(max_change,
                          fabs(gaitJoints->at(i)-curJoints.at(i+HEAD_JOINTS)));
    }

    const float  MAX_RAD_PER_SEC =  M_PI*0.20; //Technically its 220 deg/s or so
    float time = max_change/MAX_RAD_PER_SEC;

    //pthread_mutex_unlock(&walk_provider_mutex);
    return new BodyJointCommand(time,gaitJoints,
                                Kinematics::INTERPOLATION_LINEAR);
}
