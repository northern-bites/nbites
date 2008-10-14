#include "WalkProvider.h"
using Kinematics::LLEG_CHAIN;
using Kinematics::RLEG_CHAIN;

WalkProvider::WalkProvider()
    : MotionProvider(),
      walkParameters()
{

}

WalkProvider::~WalkProvider() {

}

void WalkProvider::requestStop() {

}

void WalkProvider::calculateNextJoints() {

//Tick the step generator (ensure we have preview values ready)


//Tick the controller (input: ZMPref, sensors -- out: CoM x, y)


//Tick each leg (in: CoM x,y balance mode, out: joint angles)


//Return the joints for the legs
    setNextChainJoints(LLEG_CHAIN,vector<float>(LEG_JOINTS,0.2));
    setNextChainJoints(RLEG_CHAIN,vector<float>(LEG_JOINTS,0.2));
}

void WalkProvider::setMotion(float x, float y, float theta) {

}
