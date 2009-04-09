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
using Kinematics::jointsMaxVelNoLoad;

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
      motionValues(Kinematics::NUM_JOINTS,0.0f),  // commands sent to joints
      lastMotionCommandAngles(Kinematics::NUM_JOINTS,0.0f)

{
    try {
        dcmProxy = AL::ALPtr<AL::DCMProxy>(new AL::DCMProxy(broker));
    } catch(AL::ALError &e) {
        cout << "Failed to initialize proxy to DCM" << endl;
    }


    initDCMAliases();
    initDCMCommands();

    //Assumes the constructor of the Transcriber is updating these
    //before this constructor is called
    lastMotionCommandAngles = sensors->getMotionBodyAngles();

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
    jointValues = sensors->getBodyAngles();//Need these for velocity checks

    for (unsigned int i = 0; i<Kinematics::NUM_JOINTS; i++) {
#ifdef DEBUG_ENACTOR_JOINTS
        cout << "result of joint " << i << " is " << motionValues[i] << endl;
#endif
        //returns the fastest safe value if requested movement is too fast
        const float clipped_angle  = SafetyCheck(jointValues[i],
                                                 motionValues[i],
                                                 lastMotionCommandAngles[i],
                                                 i);
        //save the clipped value for next time
        motionValues[i] =  clipped_angle;
        joint_command[5][i][0] = clipped_angle;

        //Save the values we sent this cycle for next cycle
        lastMotionCommandAngles[i] = clipped_angle;
    }

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
float NaoEnactor::SafetyCheck(float currentVal, float toCheck, float motionAngle, int i){

    //We need to clip angles twice. Why? Because the sensor values are between
    //20 and 40 ms old, so we can't strictly use the sensor reports to clip
    // the velocity.
    //We also can't just use the internaly held motion angles because these
    // could be out of sync with reality, and thus allow us to send bad
    // commands.
    //As a balance, we clip both with respect to sensor readings which we
    //ASSUME are 40 ms old (even if they are newer), AND we clip with respect
    //to the internally held motion command angles, which ensures that we'
    //arent sending commands which are in general too fast for the motors.
    //For the sensor angles, we clip with TWICE the max speed.

    const float absDiffInRad = fabs(currentVal - toCheck);
    const float allowedMotionDiffInRad = jointsMaxVelNoLoad[i];
    const float allowedSensorDiffInRad = allowedMotionDiffInRad*2.0f;
    const float clippedMotionVal =
        NBMath::clip(toCheck, motionAngle - allowedMotionDiffInRad,
                              motionAngle + allowedMotionDiffInRad);

    const float clippedSensorVal =
        NBMath::clip(clippedMotionVal, currentVal - allowedSensorDiffInRad,
                                       currentVal + allowedSensorDiffInRad);

#ifdef DEBUG_ENACTOR_CLIPPING
    const float difference = abs(currentVal - toCheck);
    const float motionDiff = abs(currentVal - motionAngle);
    if ( difference > allowedSensorDiffInRad )
        cout << "Clipped " << Kinematics::JOINT_STRINGS[i]
             << ". Difference due to SENSORS was " << difference << endl
             << "  Reduction limitation is "<<jointsMaxVelNoLoad[i]<<"/20ms"<<endl;
    if( motionDiff > allowedMotionDiffInRad ){
        cout << "Clipped " << Kinematics::JOINT_STRINGS[i]
             << "  Difference due to MOTION was " <<motionDiff <<endl
             << "  Reduction limitation is "<<jointsMaxVelNoLoad[i]<<"/20ms"<<endl;
    }
#endif

    return clippedSensorVal;
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

