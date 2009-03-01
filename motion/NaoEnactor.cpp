//TODO: look into replacing DCM->getTime with local time tracking

#include "NaoEnactor.h"
#include <iostream>
#include <boost/assign/std/vector.hpp>
using namespace boost::assign;

NaoEnactor::NaoEnactor(AL::ALPtr<AL::ALBroker> _pbroker,
                        boost::shared_ptr<Sensors> s)
    : MotionEnactor(),broker(_pbroker),sensors(s) {
        try {
            dcmProxy = AL::ALPtr<AL::DCMProxy>(new AL::DCMProxy(broker));
        } catch(AL::ALError &e) {
            cout << "Failed to initialize proxy to DCM" << endl;
        }
        try{
            alfastaccess =
                    AL::ALPtr<ALMemoryFastAccess >(new ALMemoryFastAccess());
        } catch(AL::ALError &e){
            cout << "Failed to initialize proxy to ALFastAccess"<<endl;
        }
    try{
        NaoEnactor::initSyncWithALMemory();
    } catch(AL::ALError &e){
        cout << "Failed to initialize sync with al memory"<<endl;
    }
    sensors = s;
}

vector<float> NaoEnactor::jointValues(22,0.0); //current values of joints
vector<float> NaoEnactor::motionValues(22,0.0); //commands sent to joints
const string NaoEnactor::PositionPostFix("/Position/Actuator/Value");
const string NaoEnactor::HardnessPostFix("/Hardness/Actuator/Value");
const string NaoEnactor::ValuePostFix("/Position/Sensor/Value");
const string NaoEnactor::ValuePreFix("Device/SubDeviceList/");
const int NaoEnactor::MOTION_FRAME_RATE = 50;
// 1 second * 1000 ms/s * 1000 us/ms
const float NaoEnactor::MOTION_FRAME_LENGTH_uS = 1.0f * 1000.0f * 1000.0f /
                                                NaoEnactor::MOTION_FRAME_RATE;
const float NaoEnactor::MOTION_FRAME_LENGTH_S = 1.0f /
                                                NaoEnactor::MOTION_FRAME_RATE;

const string NaoEnactor::jointsP[NaoEnactor::NUM_JOINTS] = {
    "HeadYaw" + PositionPostFix,
    "HeadPitch" + PositionPostFix,
    "LShoulderPitch" + PositionPostFix,
    "LShoulderRoll" + PositionPostFix,
    "LElbowYaw" + PositionPostFix,
    "LElbowRoll" + PositionPostFix,
    "LHipYawPitch" + PositionPostFix,
    "LHipRoll" + PositionPostFix,
    "LHipPitch" + PositionPostFix,
    "LKneePitch" + PositionPostFix,
    "LAnklePitch" + PositionPostFix,
    "LAnkleRoll" + PositionPostFix,
    //overrides previous LHipYawPitch, check how this functions in motionprovider
    "LHipYawPitch" + PositionPostFix,
    "RHipRoll" + PositionPostFix,
    "RHipPitch" + PositionPostFix,
    "RKneePitch" + PositionPostFix,
    "RAnklePitch" + PositionPostFix,
    "RAnkleRoll" + PositionPostFix,
    "RShoulderPitch" + PositionPostFix,
    "RShoulderRoll" + PositionPostFix,
    "RElbowYaw" + PositionPostFix,
    "RElbowRoll" + PositionPostFix
};
const string NaoEnactor::jointsH[NaoEnactor::NUM_JOINTS] = {
    "HeadYaw" + HardnessPostFix,
    "HeadPitch" + HardnessPostFix,
    "LShoulderPitch" + HardnessPostFix,
    "LShoulderRoll" + HardnessPostFix,
    "LElbowYaw" + HardnessPostFix,
    "LElbowRoll" + HardnessPostFix,
    "LHipYawPitch" + HardnessPostFix,
    "LHipRoll" + HardnessPostFix,
    "LHipPitch" + HardnessPostFix,
    "LKneePitch" + HardnessPostFix,
    "LAnklePitch" + HardnessPostFix,
    "LAnkleRoll" + HardnessPostFix,
    //overrides previous LHipYawPitch, check how this functions in motionprovider
    "LHipYawPitch" + HardnessPostFix,
    "RHipRoll" + HardnessPostFix,
    "RHipPitch" + HardnessPostFix,
    "RKneePitch" + HardnessPostFix,
    "RAnklePitch" + HardnessPostFix,
    "RAnkleRoll" + HardnessPostFix,
    "RShoulderPitch" + HardnessPostFix,
    "RShoulderRoll" + HardnessPostFix,
    "RElbowYaw" + HardnessPostFix,
    "RElbowRoll" + HardnessPostFix
};
const string NaoEnactor::jointsV[NaoEnactor::NUM_JOINTS] = {
    ValuePreFix + "HeadYaw" + ValuePostFix,
    ValuePreFix + "HeadPitch" + ValuePostFix,
    ValuePreFix + "LShoulderPitch" + ValuePostFix,
    ValuePreFix + "LShoulderRoll" + ValuePostFix,
    ValuePreFix + "LElbowYaw" + ValuePostFix,
    ValuePreFix + "LElbowRoll" + ValuePostFix,
    ValuePreFix + "LHipYawPitch" + ValuePostFix,
    ValuePreFix + "LHipRoll" + ValuePostFix,
    ValuePreFix + "LHipPitch" + ValuePostFix,
    ValuePreFix + "LKneePitch" + ValuePostFix,
    ValuePreFix + "LAnklePitch" + ValuePostFix,
    ValuePreFix + "LAnkleRoll" + ValuePostFix,
    //this double subscription is intentional
    ValuePreFix + "LHipYawPitch" + ValuePostFix,
    ValuePreFix + "RHipRoll" + ValuePostFix,
    ValuePreFix + "RHipPitch" + ValuePostFix,
    ValuePreFix + "RKneePitch" + ValuePostFix,
    ValuePreFix + "RAnklePitch" + ValuePostFix,
    ValuePreFix + "RAnkleRoll" + ValuePostFix,
    ValuePreFix + "RShoulderPitch" + ValuePostFix,
    ValuePreFix + "RShoulderRoll" + ValuePostFix,
    ValuePreFix + "RElbowYaw" + ValuePostFix,
    ValuePreFix + "RElbowRoll" + ValuePostFix
};
const float NaoEnactor::jointsMax[NaoEnactor::NUM_JOINTS] = {
    //head
    NaoEnactor::M2R2, NaoEnactor::M2R1,
    //left leg
    NaoEnactor::M1R1, NaoEnactor::M1R2, NaoEnactor::M1R2,
    NaoEnactor::M1R1, NaoEnactor::M1R2, NaoEnactor:: M1R1,
    //right leg
    NaoEnactor::M1R1, NaoEnactor::M1R2, NaoEnactor::M1R2,
    NaoEnactor::M1R1, NaoEnactor::M1R2, NaoEnactor::M1R1,
    //left arm
    NaoEnactor::M2R2, NaoEnactor::M2R1, NaoEnactor::M2R1, NaoEnactor::M2R2,
    //right arm
    NaoEnactor::M2R2, NaoEnactor::M2R1, NaoEnactor::M2R1, NaoEnactor::M2R2
};

void NaoEnactor::run() {
    std::cout << "NaoEnactor::run()" << std::endl;
    //basically postSensors() but need a different order for the first run
    //makes sure we have current joint values in sensors and in motionValues

    //set-up the aliases for sending commands to DCM
    ALValue positionCommandsAlias;
    positionCommandsAlias.arraySetSize(3);
    positionCommandsAlias[0] = string("AllActuatorPosition");
    positionCommandsAlias[1].arraySetSize(NaoEnactor::NUM_JOINTS);
    
    ALValue hardCommandsAlias;
    hardCommandsAlias.arraySetSize(3);
    hardCommandsAlias[0] = string("AllActuatorHardness");
    hardCommandsAlias[1].arraySetSize(NaoEnactor::NUM_JOINTS);

    for (int i = 0; i<NaoEnactor::NUM_JOINTS; i++){
        positionCommandsAlias[1][i] = jointsP[i];
        hardCommandsAlias[1][i] = jointsH[i];
    }

    dcmProxy->createAlias(positionCommandsAlias);
    dcmProxy->createAlias(hardCommandsAlias);
    
    //set-up the array for sending commands to DCM
    ALValue commands;
    commands.arraySetSize(6);
    commands[1] = string("ClearAll");
    commands[2] = string("time-separate");
    commands[3] = 0; //importance level
    commands[4].arraySetSize(1); //list of time to send commands
    commands[5].arraySetSize(NaoEnactor::NUM_JOINTS);
    
    //sets the hardness for all the joints
    commands[0] = string("AllActuatorHardness");
    for (int i = 0; i<NaoEnactor::NUM_JOINTS; i++) {
        //sets the value for hardness
        commands[5][i].arraySetSize(1);
        commands[5][i][0] = 0.85;
    }
    commands[4][0] = dcmProxy->getTime(100);
    
    //sends the hardness command to the DCM
    #ifndef NO_ACTUAL_MOTION
    try {
        dcmProxy->setAlias(commands);
    } catch(AL::ALError& a) {
        std::cout << "DCM Hardness set error" << a.toString() << "    " 
            << commands.toString() << std::endl;
    }
    #endif

    long long currentTime;
    //basically postSensors() but need a different order for the first run
    //makes sure we have current joint values in sensors and in motionValues
    syncWithALMemory();
    motionValues = jointValues;
    sensors->setMotionBodyAngles(motionValues);
    //for now we're leaving always leaving hardness the same, so we can
    //set the alias to be for actuator position in the while loop
    commands[0] = string("AllActuatorPosition");
    while (running) {        
        currentTime = micro_time();
        if(!switchboard){
            cout<< "Caution!! Switchboard is null, exiting NaoEnactor"<<endl;
            break;
        }

        // Get the angles we want to go to this frame from the switchboard
        motionValues = switchboard->getNextJoints();
        // Get most current joint values possible for performing checks
        alfastaccess->GetValues(jointValues);

        for (int i = 0; i<NaoEnactor::NUM_JOINTS; i++) {
            #ifdef DEBUG_ENACTOR_JOINTS
            cout << "result of joint " << i << " is " << motionValues[i] << endl;
            #endif
            //returns the fastest safe value if requested movement is too fast
            commands[5][i][0] = SafetyCheck(jointValues[i], motionValues[i], i);
            //may be better to use previous rounds motionValues[i] in case
            //sensor lag occurs. we risk unsafe values if motion is impeded
        }

        //send the array
        commands[4][0] = dcmProxy->getTime(0);
        #ifndef NO_ACTUAL_MOTION
        try {
            dcmProxy->setAlias(commands);
        } catch(AL::ALError& a) {
            std::cout << "dcm value set error " << a.toString() << std::endl;
        }
        #endif
        
        postSensors();
        const long long zero = 0;
        const long long processTime = micro_time() - currentTime;

        #if ! defined OFFLINE || ! defined SPEEDY_ENACTOR
        if (processTime > MOTION_FRAME_LENGTH_uS){
            cout << "Time spent in NaoEnactor longer than frame length: " 
                << processTime <<endl;
            //Don't sleep at all
        } else{
            usleep(static_cast<useconds_t>(MOTION_FRAME_LENGTH_uS - processTime));
        }
        #endif
    }

}

//makes sure that we don't tell the motors to move faster than they can
//the DCM takes care of trimming too large/ too small of values
float NaoEnactor::SafetyCheck(float currentVal, float toCheck, int i){
    
    float absDiffInRad = fabs(currentVal - toCheck);
    float allowedDiffInRad = jointsMax[i];
    if (absDiffInRad > allowedDiffInRad){
        if (toCheck > currentVal){
            #ifdef DEBUG_ENACTOR_JOINTS
            std::cout << jointsP[i] << "Current = " << currentVal << "  TRIM = "
                                << (currentVal + allowedDiffInRad) << std::endl;
            #endif
            return (currentVal + allowedDiffInRad); 
        }
        else {
            #ifdef DEBUG_ENACTOR_JOINTS
            std::cout << jointsP[i] << "Current = " << currentVal << "  TRIM = "
                                << (currentVal - allowedDiffInRad) << std::endl;
            #endif
            return (currentVal - allowedDiffInRad); 
        }
    }
    #ifdef DEBUG_ENACTOR_JOINTS
    std::cout << jointsP[i] << "Current = " << currentVal << "  Sent = "
                                                        << toCheck << std::endl;
    #endif
    return toCheck;
}

void NaoEnactor::postSensors(){
    //At the beginning of each cycle, we need to update the sensor values
    //We also call this from the Motion run method
    //This is important to ensure that the providers have access to the
    //actual joint post of the robot before any computation begins
    sensors->setMotionBodyAngles(motionValues);
    syncWithALMemory();
}
/**
 * ALFastAccess allows us to pull out values from ALMemory a lot faster
 * and in bulk. The order in which we declare the desired devices are also
 * the order in which we receive them (see syncWithALMemory).
 * In this class we only sync the sensors values we need for motion. The
 * rest are synced in Man.cpp (may change).
 */
void NaoEnactor::initSyncWithALMemory(){

    vector<string> jointNames;
    jointNames += 
        string(jointsV[0]), string(jointsV[1]), string(jointsV[2]),
        string(jointsV[3]), string(jointsV[4]), string(jointsV[5]),
        string(jointsV[6]), string(jointsV[7]), string(jointsV[8]),
        string(jointsV[9]), string(jointsV[10]), string(jointsV[11]),
        string(jointsV[12]), string(jointsV[13]), string(jointsV[14]),
        string(jointsV[15]), string(jointsV[16]), string(jointsV[17]),
        string(jointsV[18]), string(jointsV[19]), string(jointsV[20]),
        string(jointsV[21]);
    /*jointNames +=
      string("Device/SubDeviceList/HeadYaw/Position/Sensor/Value"),
      string("Device/SubDeviceList/HeadPitch/Position/Sensor/Value"),
      string("Device/SubDeviceList/LShoulderPitch/Position/Sensor/Value"),
      string("Device/SubDeviceList/LShoulderRoll/Position/Sensor/Value"),
      string("Device/SubDeviceList/LElbowYaw/Position/Sensor/Value"),
      string("Device/SubDeviceList/LElbowRoll/Position/Sensor/Value"),
      string("Device/SubDeviceList/LHipYawPitch/Position/Sensor/Value"),
      string("Device/SubDeviceList/LHipRoll/Position/Sensor/Value"),
      string("Device/SubDeviceList/LHipPitch/Position/Sensor/Value"),
      string("Device/SubDeviceList/LKneePitch/Position/Sensor/Value"),
      string("Device/SubDeviceList/LAnklePitch/Position/Sensor/Value"),
      string("Device/SubDeviceList/LAnkleRoll/Position/Sensor/Value"),
      string("Device/SubDeviceList/LHipYawPitch/Position/Sensor/Value"),
      string("Device/SubDeviceList/RHipRoll/Position/Sensor/Value"),
      string("Device/SubDeviceList/RHipPitch/Position/Sensor/Value"),
      string("Device/SubDeviceList/RKneePitch/Position/Sensor/Value"),
      string("Device/SubDeviceList/RAnklePitch/Position/Sensor/Value"),
      string("Device/SubDeviceList/RAnkleRoll/Position/Sensor/Value"),
      string("Device/SubDeviceList/RShoulderPitch/Position/Sensor/Value"),
      string("Device/SubDeviceList/RShoulderRoll/Position/Sensor/Value"),
      string("Device/SubDeviceList/RElbowYaw/Position/Sensor/Value"),
      string("Device/SubDeviceList/RElbowRoll/Position/Sensor/Value");*/

    try{
        alfastaccess->ConnectToVariables(broker,jointNames);
    } catch(AL::ALError& a) {
      std::cout << "NaoEnactor " << a.toString() << std::endl;}
  
}

void NaoEnactor::syncWithALMemory() {
    alfastaccess->GetValues(jointValues);
    sensors->setBodyAngles(jointValues);  
}
