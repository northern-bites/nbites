#include "WalkProvider.h"
using Kinematics::LLEG_CHAIN;
using Kinematics::RLEG_CHAIN;

WalkProvider::WalkProvider()
    : MotionProvider(),
      walkParameters(.02f, // motion frame length
                     310.0f, // COM height
                     0.0f, // hipOffsetX
                     1.0f, //stepDuration
                     15.0f), // steHeight
      controller_x(new PreviewController()),
      controller_y(new PreviewController()),
      left(LLEG_CHAIN,&walkParameters),
      right(RLEG_CHAIN,&walkParameters),
      stepGenerator(&walkParameters, &left, &right)
{

}

WalkProvider::~WalkProvider() {

}

void WalkProvider::requestStop() {

}

void WalkProvider::calculateNextJoints() {
    //Tick the step generator (ensure we have preview values ready)


    zmp_xy zmp_ref =
        stepGenerator.tick();
    //std::cout<< "zmp y: " << zmp_ref->front() << endl;
    //Tick the controller (input: ZMPref, sensors -- out: CoM x, y)
    float reference_zmp = 0.0f; //dummy
    float com_x = controller_x->tick(zmp_ref.get<0>());
    float com_y = controller_y->tick(zmp_ref.get<1>());
    cout << "Com x: " << com_x << endl;

    //Tick each leg (in: CoM x,y balance mode, out: joint angles)
    vector<float> lleg_results = left.tick(com_x,com_y);
    vector<float> rleg_results = right.tick(com_x,com_y);

    //Return the joints for the legs
    setNextChainJoints(LLEG_CHAIN,lleg_results);
    setNextChainJoints(RLEG_CHAIN,rleg_results);

}

void WalkProvider::setMotion(const float x, const float y, const float theta) {

}

