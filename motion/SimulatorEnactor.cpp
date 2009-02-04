#include "SimulatorEnactor.h"

#include <iostream>

void SimulatorEnactor::run() {
    std::cout << "SimulatorEnactor::run()" << std::endl;
#ifndef NAOQI1
    AL::ALMotionProxy *motionProxy = AL::ALMotionProxy::getInstance();
    motionProxy->setBodyStiffness(0.85f, 0.1f);
#endif
    //hack to keep from falling over in the simulator
    usleep(2*1000*1000);

    while (running) {
        postSensors();
        if(!switchboard){
            cout<< "Caution!! Switchboard has is null, exiting ALEnactor"<<endl;
            break;
        }
        /*
        cout<<"Joints are : [";
        for (unsigned int i=0; i <result.size(); i++){
            cout << result[i]<<", ";
        }
        cout<<"]"<<endl;
        */
        // Get the angles we want to go to this frame from the switchboard
		vector<float> result = switchboard->getNextJoints();
//  		for (unsigned int i=0; i<result.size();i++) {
//  			cout << "result of joint " << i << " is " << result.at(i) << endl;
//  		}
#ifndef NAOQI1
		motionProxy->postGotoBodyAngles(result,
                                        MOTION_FRAME_LENGTH_S,
                                        AL::ALMotionProxy::INTERPOLATION_LINEAR);
#endif
        // TODO: This is probably wrong!!!!1!ONE
        // We probably want to sleep webots time and this sleeps real time.
        usleep(static_cast<useconds_t>(MOTION_FRAME_LENGTH_uS));
    }
}


void SimulatorEnactor::postSensors(){
#ifndef NAOQI1
    AL::ALMotionProxy *motionProxy = AL::ALMotionProxy::getInstance();

    //At the beginning of each cycle, we need to update the sensor values
    //We also call this from the Motion run method
    //This is important to ensure that the providers have access to the
    //actual joint post of the robot before any computation begins
    vector<float> alAngles = motionProxy->getBodyAngles();
    sensors->setBodyAngles(alAngles);
#endif
}
