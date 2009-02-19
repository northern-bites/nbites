
#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

#include "Sensors.h"
#include "WalkProvider.h"
using Kinematics::LLEG_CHAIN;
using Kinematics::RLEG_CHAIN;

WalkProvider::WalkProvider(shared_ptr<Sensors> s)
    : MotionProvider(WALK_PROVIDER),
      sensors(s),
      walkParameters(&WALK_PARAMS[DEFAULT_P]),
      stepGenerator(sensors),
      pendingCommands(false),
      nextCommand(NULL)
{
    pthread_mutex_init(&walk_command_mutex,NULL);

    //Make up something arbitrary for the arms
    const float larm[ARM_JOINTS] = {M_PI/2,M_PI/10,-M_PI/2,-M_PI/2};
    const float rarm[ARM_JOINTS] = {M_PI/2,-M_PI/10,M_PI/2,M_PI/2};
    larm_angles = vector<float>(larm,larm+ARM_JOINTS);
    rarm_angles = vector<float>(rarm,rarm+ARM_JOINTS);

    setActive();
}

WalkProvider::~WalkProvider() {
    pthread_mutex_destroy(&walk_command_mutex);
}

void WalkProvider::requestStopFirstInstance() {
    setCommand(new WalkCommand(0.0f, 0.0f, 0.0f));
}

void WalkProvider::calculateNextJoints() {
    pthread_mutex_lock(&walk_command_mutex);
    if(nextCommand){
        stepGenerator.setSpeed(nextCommand->x_mms,
                               nextCommand->y_mms,
                               nextCommand->theta_rads);
    }
    pendingCommands = false;
    nextCommand = NULL;
    pthread_mutex_unlock(&walk_command_mutex);


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
}

void WalkProvider::setCommand(const WalkCommand * command){
    //grab the velocities in mm/second rad/second from WalkCommand
    pthread_mutex_lock(&walk_command_mutex);
    if(nextCommand)
        delete nextCommand;
    nextCommand =command;
    pendingCommands = true;
    pthread_mutex_unlock(&walk_command_mutex);

    if(!isActive()){
        //then we must just be starting out, so we can update the
        //gait in StepGenerator if we want
        stepGenerator.resetGait(walkParameters);
        cout << "Set the walking parameters in stepgen"<<endl;
    }

    setActive();
}



void WalkProvider::setActive(){
    //check to see if the walk engine is active
    if(stepGenerator.isDone() && !pendingCommands){
        inactive();
    }else{
        active();
    }
}
