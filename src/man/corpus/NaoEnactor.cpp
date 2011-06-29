//TODO: look into replacing DCM->getTime with local time tracking

#include "alvalue/alvalue.h"
#include "alcommon/alproxy.h"

#include "NaoEnactor.h"
#include <iostream>
using namespace std;
#include <boost/assign/std/vector.hpp>
using namespace boost::assign;
#include "NBMath.h"

#include <boost/bind.hpp>
using namespace boost;

#include "BasicWorldConstants.h"
#include "ALNames.h"
using namespace ALNames;

#include "Kinematics.h"
using Kinematics::jointsMaxVelNoLoad;

void staticPostSensors(NaoEnactor * n) {
    if (n != NULL) {
        n->postSensors();
    }
}
void staticSendCommands(NaoEnactor * n) {
    if (n != NULL) {
        n->sendCommands();
    }
}

NaoEnactor::NaoEnactor(boost::shared_ptr<Profiler> p,
                       boost::shared_ptr<Sensors> s,
                       boost::shared_ptr<Transcriber> t,
                       AL::ALPtr<AL::ALBroker> _pbroker)
    : MotionEnactor(), broker(_pbroker), sensors(s),
      transcriber(t),
      motionValues(Kinematics::NUM_JOINTS,0.0f),  // commands sent to joints
      lastMotionHardness(Kinematics::NUM_JOINTS,0.0f),
      profiler(p)
{
    try {
        dcmProxy = AL::ALPtr<AL::DCMProxy>(new AL::DCMProxy(broker));
    } catch(AL::ALError &e) {
        cout << "Failed to initialize proxy to DCM" << endl;
    }


    initDCMAliases();
    initDCMCommands();

    // connect to dcm using the static methods declared above

    // TODO: Should use specialized proxy created at start
    broker->getProxy("DCM")->getModule()->
        atPostProcess(boost::bind(&staticPostSensors,this));
    broker->getProxy("DCM")->getModule()->
        atPreProcess(boost::bind(&staticSendCommands,this));
}

void NaoEnactor::sendCommands(){

    PROF_ENTER(profiler.get(), P_DCM);
    if(!switchboard){
        if(switchboardSet)
            cout<< "Caution!! Switchboard is null, skipping NaoEnactor"<<endl;
        return;
    }

    sendJoints();
    sendHardness();
}

void NaoEnactor::sendJoints()
{
    // Send the array with a 25 ms delay. This delay removes the jitter.
    // Note: I tried 20 ms and it didn't work quite as well. Maybe there is
    // a value in between that works though. Will look into it.
    joint_command[4][0] = dcmProxy->getTime(20);

    // Get the angles we want to go to this frame from the switchboard
    motionValues = switchboard->getNextJoints();

    for (unsigned int i = 0; i < Kinematics::NUM_JOINTS; i++)
        {
            joint_command[5][i][0] = motionValues[i];
        }

#ifndef NO_ACTUAL_MOTION
    try
        {
            dcmProxy->setAlias(joint_command);
        }
    catch(AL::ALError& a)
        {
            std::cout << "dcm value set error " << a.toString() << std::endl;
        }
#endif
}


void NaoEnactor::sendHardness(){
    motionHardness = switchboard->getNextStiffness();

    bool diffStiff = false;
    static float hardness = 0.0f;
    //TODO!!! ONLY ONCE PER CHANGE!sends the hardness command to the DCM
    for (unsigned int i = 0; i < Kinematics::NUM_JOINTS; i++) {

        hardness = NBMath::clip(motionHardness[i], -1.0f, 1.0f);

        //sets the value for hardness
        if(lastMotionHardness[i] != hardness){
            diffStiff = true;
            hardness_command[5][i][0] = hardness;
            //store for next time
            lastMotionHardness[i] = hardness;
        }
    }

    if(!diffStiff)
        return;


    hardness_command[4][0] = dcmProxy->getTime(0);
    // #ifdef ROBOT_NAME_zaphod
/*    #ifdef ROBOT_NAME_zaphod
    //     // turn off broken shoulder
    hardness_command[5][Kinematics::L_SHOULDER_PITCH][0] = -1.0f;
    hardness_command[5][Kinematics::L_SHOULDER_ROLL][0] = -1.0f;
    #endif
*/
#ifndef NO_ACTUAL_MOTION
    try {
        dcmProxy->setAlias(hardness_command);
    } catch(AL::ALError& a) {
        std::cout << "DCM Hardness set error" << a.toString() << "    "
                  << hardness_command.toString() << std::endl;
    }
#endif
}

void NaoEnactor::postSensors(){
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
    PROF_EXIT(profiler.get(), P_DCM);
}

/**
 * Creates the appropriate aliases with the DCM
 */
void NaoEnactor::initDCMAliases(){
    AL::ALValue positionCommandsAlias;
    positionCommandsAlias.arraySetSize(3);
    positionCommandsAlias[0] = string("AllActuatorPosition");
    positionCommandsAlias[1].arraySetSize(Kinematics::NUM_JOINTS);

    AL::ALValue hardCommandsAlias;
    hardCommandsAlias.arraySetSize(3);
    hardCommandsAlias[0] = string("AllActuatorHardness");
    hardCommandsAlias[1].arraySetSize(Kinematics::NUM_JOINTS);

    for (unsigned int i = 0; i<Kinematics::NUM_JOINTS; i++){
        positionCommandsAlias[1][i] = jointsP[i];
        hardCommandsAlias[1][i] = jointsH[i];
    }

    dcmProxy->createAlias(positionCommandsAlias);
    dcmProxy->createAlias(hardCommandsAlias);
}


void NaoEnactor::initDCMCommands(){
    //set-up the array for sending hardness commands to DCM
    //ALValue hardness_command;
    hardness_command.arraySetSize(6);
    hardness_command[1] = string("ClearAll");
    hardness_command[2] = string("time-separate");
    hardness_command[3] = 0; //importance level
    hardness_command[4].arraySetSize(1); //list of time to send commands
    hardness_command[5].arraySetSize(Kinematics::NUM_JOINTS);

    //sets the hardness for all the joints
    hardness_command[0] = string("AllActuatorHardness");
    for (unsigned int i = 0; i<Kinematics::NUM_JOINTS; i++) {
        //sets default hardness for each joint, which will never be sent
        hardness_command[5][i].arraySetSize(1);
        hardness_command[5][i][0] = 0.0;
    }

    //set-up the array for sending commands to DCM
    joint_command.arraySetSize(6);
    joint_command[1] = string("ClearAll");
    joint_command[2] = string("time-separate");
    joint_command[3] = 0; //importance level
    joint_command[4].arraySetSize(1); //list of time to send commands
    joint_command[5].arraySetSize(Kinematics::NUM_JOINTS);

    //sets the hardness for all the joints
    joint_command[0] = string("AllActuatorPosition");
    for (unsigned int i = 0; i<Kinematics::NUM_JOINTS; i++) {
        //sets default value for each joint, which will never be sent
        joint_command[5][i].arraySetSize(1);
        joint_command[5][i][0] = 0.0;
    }

    us_command.arraySetSize(3);
    us_command[0] = string("US/Actuator/Value");
    us_command[1] = string("Merge");
    us_command[2].arraySetSize(1);
    us_command[2][0].arraySetSize(2);
    us_command[2][0][0] = (4.0 + 64.0);
    us_command[2][0][1] = dcmProxy->getTime(5);
    try {
        dcmProxy->set(us_command);
    } catch(AL::ALError& a) {
        std::cout << "DCM ultrasound set error" << a.toString() << "    "
                  << us_command.toString() << std::endl;
    }
}

