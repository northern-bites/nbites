
#include "WBEnactor.h"
#include <webots/servo.h>
using boost::shared_ptr;
using namespace std;

#include "Kinematics.h"
using namespace Kinematics;
#include "WBNames.h"
using namespace WBNames;

const int WBEnactor::MOTION_FRAME_RATE = 50;
// 1 second * 1000 ms/s * 1000 us/ms
const float WBEnactor::MOTION_FRAME_LENGTH_uS = 1.0f * 1000.0f * 1000.0f / WBEnactor::MOTION_FRAME_RATE;
const float WBEnactor::MOTION_FRAME_LENGTH_S = 1.0f / WBEnactor::MOTION_FRAME_RATE;


WBEnactor::WBEnactor(shared_ptr<Sensors> _sensors,
                     shared_ptr<Transcriber> _transcriber)
    :MotionEnactor(),
     sensors(_sensors),
     transcriber(_transcriber),
     motionValues(NUM_JOINTS, 0.0f),
     jointDevices(NUM_JOINTS)
{

    //assign and enable the joint devices
    for(unsigned int joint = 0; joint < NUM_JOINTS; joint++){
        const string devName = JOINT_STRINGS[joint];
        jointDevices[joint] = wb_robot_get_device(devName.c_str());
        wb_servo_enable_position (jointDevices[joint], 20);
    }

}

WBEnactor::~WBEnactor(){}


void WBEnactor::postSensors(){

    //At the beginning of each cycle, we need to update the sensor values
    //We also call this from the Motion run method
    //This is important to ensure that the providers have access to the
    //actual joint post of the robot before any computation begins
    sensors->setMotionBodyAngles(motionValues);
    transcriber->postMotionSensors();

    if(!switchboard){
        return;
    }
    //We only want the switchboard to start calculating new joints once we've
    //updated the latest sensor information into Sensors
    switchboard->signalNextFrame();
}

void WBEnactor::sendCommands(){
//     cout << "About to attempt to set some joints..."<<endl;

    if(switchboard != NULL)
        motionValues = switchboard->getNextJoints();
    else
        cout << "warning, switchboard is null in WB enactor" <<endl;
//     cout << "Threadlock ??" <<endl;
    for(unsigned int joint = 0;  joint < NUM_JOINTS; joint++){

        wb_servo_set_position(jointDevices[joint],motionValues[joint]);
        //wb_servo_set_position(jointDevices[joint],0.1);//motionValues[joint]);
    }
//     //Ignore hardness in the simmulator
//     //motionHardness = switchboard->getNextStiffness();
}



