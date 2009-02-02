#include "ALEnactor.h"

#include <iostream>

const int ALEnactor::MOTION_FRAME_RATE = 50;
// 1 second * 1000 ms/s * 1000 us/ms
const float ALEnactor::MOTION_FRAME_LENGTH_uS = 1.0f * 1000.0f * 1000.0f / ALEnactor::MOTION_FRAME_RATE;
const float ALEnactor::MOTION_FRAME_LENGTH_S = 1.0f / ALEnactor::MOTION_FRAME_RATE;

#define NO_ACTUAL_MOTION
#define SPEEDY_ENACTOR

#ifdef NAOQI1
void ALEnactor::run() {
    std::cout << "ALEnactor::run()" << std::endl;

#ifdef NO_ACTUAL_MOTION
    mproxy->setBodyStiffness(0.0f, 0.1f);
#else
    mproxy->setBodyStiffness(0.80f, 0.1f);
#endif

    //vector<float> rarm(4,M_PI/4);
    //mproxy->gotoChainAngles("RArm",rarm,2.0,INTERPOLATION_LINEAR);
    //cout << "Done with AL motion stuff" << endl;

    while (running) {
        postSensors();

        // Get the angles we want to go to this frame from the switchboard
		vector<float> result = switchboard->getNextJoints();

#ifdef DEBUG_ENACTOR_JOINTS
 		for (int i=0; i<result.size();i++)
 			cout << "result of joint " << i << " is " << result.at(i) << endl;
#endif

#ifndef NO_ACTUAL_MOTION
        mproxy->setBodyAngles(result);
#endif

        // TODO: This is probably wrong!!!!1!ONE
        // We probably want to sleep webots time and this sleeps real time.
#if ! defined OFFLINE || ! defined SPEEDY_ENACTOR
        usleep(static_cast<useconds_t>(MOTION_FRAME_LENGTH_uS));
#endif
    }
}

void ALEnactor::postSensors(){
    //At the beginning of each cycle, we need to update the sensor values
    //We also call this from the Motion run method
    //This is important to ensure that the providers have access to the
    //actual joint post of the robot before any computation begins
    vector<float> alAngles = mproxy->getBodyAngles();
    sensors->setBodyAngles(alAngles);
}
#endif//NAOQI1
