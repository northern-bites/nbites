#include "ALEnactor.h"

#include <iostream>

#ifdef NAOQI1
void ALEnactor::run() {
    std::cout << "ALEnactor::run()" << std::endl;

    mproxy->setBodyStiffness(0.0f, 0.1f);
    mproxy->setChainStiffness("LArm",0.40f,0.10f);

    static int x = 0;

//    while (running && x++ <200) {
    while (running) {
        postSensors();

        // Get the angles we want to go to this frame from the switchboard
		vector<float> result = switchboard->getNextJoints();
//  		for (int i=0; i<result.size();i++) {
//  			cout << "result of joint " << i << " is " << result.at(i) << endl;
//  		}

 		for (int i=2; i<6;i++) {
 			cout << "result of joint " << i << " is " << result.at(i) << endl;
 		}
//          mproxy->post.gotoBodyAngles(result,
//                                      MOTION_FRAME_LENGTH_S,
//                                      AL::ALMotionProxy::INTERPOLATION_LINEAR);
        mproxy->setBodyAngles(result);

        // TODO: This is probably wrong!!!!1!ONE
        // We probably want to sleep webots time and this sleeps real time.
        usleep(static_cast<useconds_t>(MOTION_FRAME_LENGTH_uS));
    }
    cout << "ALENACTOR Done"<<endl;
}

void ALEnactor::postSensors(){
    //At the beginning of each cycle, we need to update the sensor values
    //We also call this from the Motion run method
    //This is important to ensure that the providers have access to the
    //actual joint post of the robot before any computation begins
    vector<float> alAngles = mproxy->getBodyAngles();
    //vector<float> alAngleErrors = mproxy->getBodyAngleErrors();
    cout << "Sensor Angles" <<endl;
//     for(int i = 0; i < 22; i++){
//         //alAngles[i] -= alAngleErrors[i];
//         cout << alAngles[i]<<endl;
//     }
    for(int i = 2; i < 6; i++){
        //alAngles[i] -= alAngleErrors[i];
        cout << alAngles[i]<<endl;
    }

    sensors->setBodyAngles(alAngles);
}
#endif//NAOQI1
