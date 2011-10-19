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

//TODO: this might cause a segfault if the pointer n becomes invalid
void staticPostSensors(NaoEnactor * n) {
    if (n != NULL) {
        n->postSensors();
        n->sendCommands();
    }
}
void staticSendCommands(NaoEnactor * n) {
    if (n != NULL) {
        // n->sendCommands();
    }
}

NaoEnactor::NaoEnactor(boost::shared_ptr<Sensors> s,
                       boost::shared_ptr<Transcriber> t,
                       AL::ALPtr<AL::ALBroker> _pbroker)
    : MotionEnactor(), broker(_pbroker), sensors(s),
      transcriber(t),
      motionValues(Kinematics::NUM_JOINTS,0.0f),  // commands sent to joints
      lastMotionHardness(Kinematics::NUM_JOINTS,0.0f) {
    try {
        dcmProxy = AL::ALPtr<AL::DCMProxy>(new AL::DCMProxy(broker));
    } catch(AL::ALError &e) {
        cout << "Failed to initialize proxy to DCM" << endl;
    }


    initDCMAliases();
    initDCMCommands();

    // connect to dcm using the static methods declared above

    // TODO: Should use specialized proxy created at start
    try {
        dcmPostConnection =
            broker->getProxy("DCM")->getModule()->
            atPostProcess(boost::bind(&staticPostSensors,this));
        // dcmPreConnection =
        //     broker->getProxy("DCM")->getModule()->
        //     atPreProcess(boost::bind(&staticSendCommands,this));
    } catch (AL::ALError& e){
        cout << "Failed to set pre/postprocess DCM commands" << endl;
    }
}

NaoEnactor::~NaoEnactor()
{
    cout << "Nao enactor destructor" << endl;
    dcmPreConnection.disconnect();
    dcmPostConnection.disconnect();
}


void NaoEnactor::sendCommands(){

    PROF_ENTER(P_DCM);
    PROF_ENTER(P_PRE_PROCESS);

    sendHardness();
    sendJoints();
    PROF_EXIT(P_PRE_PROCESS);
}

void NaoEnactor::sendJoints()
{
    PROF_ENTER(P_SEND_JOINTS);
    // Get the angles we want to go to this frame from the switchboard
    motionValues = switchboard->getNextJoints();

    for (unsigned int i = 0; i < Kinematics::NUM_JOINTS; i++) {
        joint_command[5][i][0] = motionValues[i];
    }

#ifndef NO_ACTUAL_MOTION
    try {
        joint_command[4][0] = dcmProxy->getTime(10);
        dcmProxy->setAlias(joint_command); // Takes a long time for some reason
    }
    catch(AL::ALError& a)
    {
        std::cout << "dcm value set error " << a.toString() << std::endl;
    }
#endif
    PROF_EXIT(P_SEND_JOINTS);
}


void NaoEnactor::sendHardness()
{
    PROF_ENTER(P_SEND_HARDNESS);
    motionHardness = switchboard->getNextStiffness();

    bool diffStiff = false;
    float hardness = 0.0;
    //TODO!!! ONLY ONCE PER CHANGE!sends the hardness command to the DCM
    for (unsigned int i = 0; i < Kinematics::NUM_JOINTS; i++) {
        hardness = motionHardness[i];
        //sets the value for hardness
        if(lastMotionHardness[i] != hardness){
            diffStiff = true;
            lastMotionHardness[i] = hardness; // store for next time
            hardness_command[5][i][0] = hardness; // set for this time
        }
    }

    if(!diffStiff)
        return;

#ifndef NO_ACTUAL_MOTION
    try {
        hardness_command[4][0] = dcmProxy->getTime(10);
        dcmProxy->setAlias(hardness_command);
    } catch(AL::ALError& a) {
        std::cout << "DCM Hardness set error" << a.toString() << "    "
                  << hardness_command.toString() << std::endl;
    }
#endif
    PROF_EXIT(P_SEND_HARDNESS);
}

void NaoEnactor::postSensors(){
    PROF_ENTER(P_POST_PROCESS);
    //At the beginning of each cycle, we need to update the sensor values
    //We also call this from the Motion run method
    //This is important to ensure that the providers have access to the
    //actual joint post of the robot before any computation begins
    sensors->setMotionBodyAngles(motionValues);
    transcriber->postMotionSensors();

    //We only want the switchboard to start calculating new joints once we've
    //updated the latest sensor information into Sensors
    switchboard->signalNextFrame();
    PROF_EXIT(P_POST_PROCESS);
    PROF_EXIT(P_DCM);
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

    try {
        dcmProxy->createAlias(positionCommandsAlias);
        dcmProxy->createAlias(hardCommandsAlias);
    } catch (AL::ALError& e){
        std::cout << "dcm error in initDCMAliases"
                  << e.toString() << std::endl;
    }
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
    try {
        us_command[2][0][1] = dcmProxy->getTime(0);
        dcmProxy->set(us_command);
    } catch(AL::ALError& a) {
        std::cout << "DCM ultrasound set error" << a.toString() << "    "
                  << us_command.toString() << std::endl;
    }
}

