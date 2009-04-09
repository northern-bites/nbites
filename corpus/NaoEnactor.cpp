//TODO: look into replacing DCM->getTime with local time tracking

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
//using Kinematics::NUM_JOINTS;

void staticPostSensors(NaoEnactor * n) {
    n->postSensors();
}
void staticSendJoints(NaoEnactor * n) {
    n->sendJoints();
}

NaoEnactor::NaoEnactor(AL::ALPtr<AL::ALBroker> _pbroker,
                       boost::shared_ptr<Sensors> s,
                       boost::shared_ptr<Transcriber> t)
    : MotionEnactor(), broker(_pbroker), sensors(s),
      transcriber(t),
      jointValues(Kinematics::NUM_JOINTS,0.0f),  // current values of joints
      motionValues(Kinematics::NUM_JOINTS,0.0f)  // commands sent to joints

{
    try {
        dcmProxy = AL::ALPtr<AL::DCMProxy>(new AL::DCMProxy(broker));
    } catch(AL::ALError &e) {
        cout << "Failed to initialize proxy to DCM" << endl;
    }


    initDCMAliases();
    initDCMCommands();

    // connect to dcm using the static methods declared above
    broker->getProxy("DCM")->getModule()->onPostProcess()
        .connect(bind(staticPostSensors,this));
    broker->getProxy("DCM")->getModule()->onPreProcess()
        .connect(bind(staticSendJoints,this));
}

const int NaoEnactor::MOTION_FRAME_RATE = 50;
// 1 second * 1000 ms/s * 1000 us/ms
const float NaoEnactor::MOTION_FRAME_LENGTH_uS = 1.0f * 1000.0f * 1000.0f /
                                                NaoEnactor::MOTION_FRAME_RATE;
const float NaoEnactor::MOTION_FRAME_LENGTH_S = 1.0f /
                                                NaoEnactor::MOTION_FRAME_RATE;

void NaoEnactor::sendJoints() {

    if(!switchboard){
        if(switchboardSet)
            cout<< "Caution!! Switchboard is null, skipping NaoEnactor"<<endl;
        return;
    }

    // Get the angles we want to go to this frame from the switchboard
    motionValues = switchboard->getNextJoints();
    // Get most current joint values possible for performing checks
    //alfastaccessJoints->GetValues(jointValues);
    transcriber->postMotionSensors();
    jointValues = sensors->getBodyAngles();//Need these for velocity checks

    for (unsigned int i = 0; i<Kinematics::NUM_JOINTS; i++) {
#ifdef DEBUG_ENACTOR_JOINTS
        cout << "result of joint " << i << " is " << motionValues[i] << endl;
#endif
        //returns the fastest safe value if requested movement is too fast
        joint_command[5][i][0] = SafetyCheck(jointValues[i], motionValues[i], i);
        //joint_command[5][i][0] = motionValues[i];
        //may be better to use previous rounds motionValues[i] in case
        //sensor lag occurs. we risk unsafe values if motion is impeded
    }

    //TODO setBodyHardness() when necessary
    sendHardness();

    // Send the array with a 25 ms delay. This delay removes the jitter.
    // Note: I tried 20 ms and it didn't work quite as well. Maybe there is
    // a value in between that works though. Will look into it.
    joint_command[4][0] = dcmProxy->getTime(20);
#ifndef NO_ACTUAL_MOTION
    try {
        dcmProxy->setAlias(joint_command);
    } catch(AL::ALError& a) {
        std::cout << "dcm value set error " << a.toString() << std::endl;
    }
#endif
}


void NaoEnactor::sendHardness(){
    motionHardness = switchboard->getNextStiffness();

    //TODO!!! ONLY ONCE PER CHANGE!sends the hardness command to the DCM
    for (unsigned int i = 0; i<Kinematics::NUM_JOINTS; i++) {
        static float hardness =0.0f;
        hardness = NBMath::clip(motionHardness[i],0.0f,1.0f);

        //sets the value for hardness
        hardness_command[5][i].arraySetSize(1);
        hardness_command[5][i][0] = hardness;
    }
    hardness_command[4][0] = dcmProxy->getTime(0);
#ifndef NO_ACTUAL_MOTION
    try {
        dcmProxy->setAlias(hardness_command);
    } catch(AL::ALError& a) {
        std::cout << "DCM Hardness set error" << a.toString() << "    "
                  << hardness_command.toString() << std::endl;
    }
#endif
}

void NaoEnactor::setBodyHardness(float hardness){
    hardness = NBMath::clip(hardness,0,1.0f);

    //TODO!!! ONLY ONCE PER CHANGE!sends the hardness command to the DCM
    for (unsigned int i = 0; i<Kinematics::NUM_JOINTS; i++) {
        //sets the value for hardness
        hardness_command[5][i].arraySetSize(1);
        hardness_command[5][i][0] = hardness;
    }
    hardness_command[4][0] = dcmProxy->getTime(0);
#ifndef NO_ACTUAL_MOTION
    try {
        dcmProxy->setAlias(hardness_command);
    } catch(AL::ALError& a) {
        std::cout << "DCM Hardness set error" << a.toString() << "    "
                  << hardness_command.toString() << std::endl;
    }
#endif
}

//makes sure that we don't tell the motors to move faster than they can
//the DCM takes care of trimming too large/ too small of values
float NaoEnactor::SafetyCheck(float currentVal, float toCheck, int i){

    float absDiffInRad = fabs(currentVal - toCheck);
    float allowedDiffInRad = jointsMax[i];
    const float clippedVal =
        NBMath::clip(toCheck, currentVal - allowedDiffInRad,
                              currentVal + allowedDiffInRad);

#ifdef DEBUG_ENACTOR_JOINTS
    const float difference = abs(currentVal - toCheck);
    if (difference > allowedDiffInRad)
        cout << "Clipped " << Kinematics::JOINT_STRINGS[i]
             << ". Difference was " << difference << endl;
#endif
/*
    if (absDiffInRad > allowedDiffInRad){
        if (toCheck > currentVal){
//            #ifdef DEBUG_ENACTOR_JOINTS
            std::cout << jointsP[i] << " Current = " << currentVal << "  TRIM = "
                                << (currentVal + allowedDiffInRad) << std::endl;
//            #endif
            return (currentVal + allowedDiffInRad);
        }
        else {
//            #ifdef DEBUG_ENACTOR_JOINTS
            std::cout << jointsP[i] << "Current = " << currentVal << "  TRIM = "
                                << (currentVal - allowedDiffInRad) << std::endl;
//            #endif
            return (currentVal - allowedDiffInRad);
        }
    }

    #ifdef DEBUG_ENACTOR_JOINTS
    std::cout << jointsP[i] << "Current = " << currentVal << "  Sent = "
              << toCheck << std::endl;
    #endif
*/
    return clippedVal;
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
}

/**
 * Creates the appropriate aliases with the DCM
 */
void NaoEnactor::initDCMAliases(){
    ALValue positionCommandsAlias;
    positionCommandsAlias.arraySetSize(3);
    positionCommandsAlias[0] = string("AllActuatorPosition");
    positionCommandsAlias[1].arraySetSize(Kinematics::NUM_JOINTS);

    ALValue hardCommandsAlias;
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
        //sets the value for hardness
        joint_command[5][i].arraySetSize(1);
        joint_command[5][i][0] = 0.80;
    }

}

