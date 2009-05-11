#include "WBTranscriber.h"
#include "webots/robot.h"

#include "Kinematics.h"
using boost::shared_ptr;
using namespace std;
using namespace Kinematics;

WBTranscriber::WBTranscriber(shared_ptr<Sensors> s)
    :Transcriber(s){}


WBTranscriber::~WBTranscriber(){}



void WBTranscriber::postVisionSensors(){}
void WBTranscriber::postMotionSensors(){
//The following sensors need to be updated on the motion cycle:
//Foot sensors
//Button (always off)
//Inertials (including angleX!)
//Joints
//Temperaturs (always zero)


    vector<float> jointTemps(NUM_JOINTS,0.0f);
    sensors->setBodyTemperatures(jointTemps);

    for(unsigned int joint = 0; joint < NUM_JOINTS; joint++){
        //const float curAngle = wb_server_get

    }

}
