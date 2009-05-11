#include "WBTranscriber.h"
#include "webots/robot.h"

#include "Kinematics.h"
using boost::shared_ptr;
using namespace std;
using namespace Kinematics;

WBTranscriber::WBTranscriber(shared_ptr<Sensors> s)
    :Transcriber(s),
     jointValues(NUM_JOINTS,0.0f),
     jointDevices(NUM_JOINTS)
{

    //assign and enable the joint devices
    for(unsigned int joint = 0; joint < NUM_JOINTS; joint++){
        const string devName = JOINT_STRINGS[joint];
        jointDevices[joint] = wb_robot_get_device(devName.c_str());
        wb_servo_enable_position (jointDevices[joint], 20);
    }

}


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
        jointValues[joint] = wb_servo_get_position(jointDevices[joint]);
    }
    sensors->setBodyAngles(jointValues);

}
