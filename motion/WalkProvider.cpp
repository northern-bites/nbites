#include "WalkProvider.h"
using Kinematics::LLEG_CHAIN;
using Kinematics::RLEG_CHAIN;

WalkProvider::WalkProvider()
    : MotionProvider(),
      walkParameters(.02f, // motion frame length
                     310.0f, // COM height
                     0.0f, // hipOffsetX
                     1.00f, //stepDuration
                     0.25f, //fraction in double support mode
                     17.0f), // steHeight
      stepGenerator(&walkParameters)
{

}

WalkProvider::~WalkProvider() {

}

void WalkProvider::requestStop() {

}

void WalkProvider::calculateNextJoints() {
    //ask the step Generator to update ZMP values, com targets
    stepGenerator.tick_controller();

    // Now ask the step generator to get the leg angles
    WalkLegsTuple legs_result = stepGenerator.tick_legs();

    //Get the joints for each Leg
    vector<float> lleg_results = legs_result.get<LEFT_FOOT>();
    vector<float> rleg_results = legs_result.get<RIGHT_FOOT>();

    vector<float> rarm_results = vector<float>(ARM_JOINTS, 0.0f);
    vector<float> larm_results = vector<float>(ARM_JOINTS, 0.0f);

    cout << "2rarm size "<< rarm_results.size()
         << "larm size "<< larm_results.size() <<endl;


    //Return the joints for the legs
    setNextChainJoints(LARM_CHAIN,larm_results);
    setNextChainJoints(LLEG_CHAIN,lleg_results);
    setNextChainJoints(RLEG_CHAIN,rleg_results);
    setNextChainJoints(RARM_CHAIN,rarm_results);

}

void WalkProvider::setMotion(const float x, const float y, const float theta) {

}

