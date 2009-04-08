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

void staticPostSensors(NaoEnactor * n) {
    n->postSensors();
}
void staticSendJoints(NaoEnactor * n) {
    n->sendJoints();
}

NaoEnactor::NaoEnactor(AL::ALPtr<AL::ALBroker> _pbroker,
                        boost::shared_ptr<Sensors> s)
    : MotionEnactor(), broker(_pbroker), sensors(s),
      jointValues(NUM_JOINTS,0.0f),  // current values of joints
      motionValues(NUM_JOINTS,0.0f),  // commands sent to joints
      accelerationFilter()
{
    try {
        dcmProxy = AL::ALPtr<AL::DCMProxy>(new AL::DCMProxy(broker));
    } catch(AL::ALError &e) {
        cout << "Failed to initialize proxy to DCM" << endl;
    }

    try{
        alfastaccessJoints =
            AL::ALPtr<ALMemoryFastAccess >(new ALMemoryFastAccess());
    } catch(AL::ALError &e){
        cout << "Failed to initialize proxy to ALFastAccess for joints"<<endl;
    }

    try{
        alfastaccessSensors =
            AL::ALPtr<ALMemoryFastAccess >(new ALMemoryFastAccess());
    } catch(AL::ALError &e){
        cout << "Failed to initialize proxy to ALFastAccess for sensors"<<endl;
    }

    try{
        NaoEnactor::initSyncWithALMemory();
    } catch(AL::ALError &e){
        cout << "Failed to initialize sync with al memory"<<endl;
    }

    initDCMAliases();
    initDCMCommands();
    initSensorBodyJoints();

    // connect to dcm using the static methods declared above
    broker->getProxy("DCM")->getModule()->onPostProcess()
        .connect(bind(staticPostSensors,this));
    broker->getProxy("DCM")->getModule()->onPreProcess()
        .connect(bind(staticSendJoints,this));
}

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



void NaoEnactor::sendJoints() {

    if(!switchboard){
        if(switchboardSet)
            cout<< "Caution!! Switchboard is null, skipping NaoEnactor"<<endl;
        return;
    }

    // Get the angles we want to go to this frame from the switchboard
    motionValues = switchboard->getNextJoints();
    // Get most current joint values possible for performing checks
    alfastaccessJoints->GetValues(jointValues);

    for (int i = 0; i<NaoEnactor::NUM_JOINTS; i++) {
#ifdef DEBUG_ENACTOR_JOINTS
        cout << "result of joint " << i << " is " << motionValues[i] << endl;
#endif
        //returns the fastest safe value if requested movement is too fast
        joint_command[5][i][0] = SafetyCheck(jointValues[i], motionValues[i], i);
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
    for (int i = 0; i<NaoEnactor::NUM_JOINTS; i++) {
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
    for (int i = 0; i<NaoEnactor::NUM_JOINTS; i++) {
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
}


void NaoEnactor::initSensorBodyJoints(){
    //We need to correctly initialize the value in Sensors so that
    //motion can run based on the actual pose of the robot.
    //This is basically postSensors() but need a different order
    syncWithALMemory();
    motionValues = jointValues;
    sensors->setMotionBodyAngles(motionValues);
}

void NaoEnactor::initDCMCommands(){
    //set-up the array for sending hardness commands to DCM
    //ALValue hardness_command;
    hardness_command.arraySetSize(6);
    hardness_command[1] = string("ClearAll");
    hardness_command[2] = string("time-separate");
    hardness_command[3] = 0; //importance level
    hardness_command[4].arraySetSize(1); //list of time to send commands
    hardness_command[5].arraySetSize(NaoEnactor::NUM_JOINTS);

    //sets the hardness for all the joints
    hardness_command[0] = string("AllActuatorHardness");


    //set-up the array for sending commands to DCM
    joint_command.arraySetSize(6);
    joint_command[1] = string("ClearAll");
    joint_command[2] = string("time-separate");
    joint_command[3] = 0; //importance level
    joint_command[4].arraySetSize(1); //list of time to send commands
    joint_command[5].arraySetSize(NaoEnactor::NUM_JOINTS);

    //sets the hardness for all the joints
    joint_command[0] = string("AllActuatorPosition");
    for (int i = 0; i<NaoEnactor::NUM_JOINTS; i++) {
        //sets the value for hardness
        joint_command[5][i].arraySetSize(1);
        joint_command[5][i][0] = 0.80;
    }

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

    try{
        alfastaccessJoints->ConnectToVariables(broker,jointNames);
    } catch(AL::ALError& a) {
        std::cout << "NaoEnactor " << a.toString() << std::endl;
    }

    // Now connect to all the sensor values we need to update on the motion
    // frame rate: FSR and inertial sensors for now.
    vector<string> sensorNames;
    sensorNames +=
        string("Device/SubDeviceList/LFoot/FSR/FrontLeft/Sensor/Value"),
        string("Device/SubDeviceList/LFoot/FSR/FrontRight/Sensor/Value"),
        string("Device/SubDeviceList/LFoot/FSR/RearLeft/Sensor/Value"),
        string("Device/SubDeviceList/LFoot/FSR/RearRight/Sensor/Value"),
        string("Device/SubDeviceList/RFoot/FSR/FrontLeft/Sensor/Value"),
        string("Device/SubDeviceList/RFoot/FSR/FrontRight/Sensor/Value"),
        string("Device/SubDeviceList/RFoot/FSR/RearLeft/Sensor/Value"),
        string("Device/SubDeviceList/RFoot/FSR/RearRight/Sensor/Value"),
        string("Device/SubDeviceList/InertialSensor/AccX/Sensor/Value"),
        string("Device/SubDeviceList/InertialSensor/AccY/Sensor/Value"),
        string("Device/SubDeviceList/InertialSensor/AccZ/Sensor/Value"),
        string("Device/SubDeviceList/InertialSensor/GyrX/Sensor/Value"),
        string("Device/SubDeviceList/InertialSensor/GyrY/Sensor/Value"),
        string("Device/SubDeviceList/InertialSensor/AngleX/Sensor/Value"),
        string("Device/SubDeviceList/InertialSensor/AngleY/Sensor/Value");

    try {
        alfastaccessSensors->ConnectToVariables(broker, sensorNames);
    } catch(AL::ALError& a) {
        std::cout << "NaoEnactor " << a.toString() << std::endl;
    }
}

// for marvin!
static const float ACCEL_CONVERSION_X = (-GRAVITY_mss) / 50.0f;
static const float ACCEL_CONVERSION_Y = (-GRAVITY_mss) / 54.0f;
static const float ACCEL_CONVERSION_Z = (-GRAVITY_mss) / 56.5f;

const float NaoEnactor::calibrate_acc_x(const float x) {
    return x * ACCEL_CONVERSION_X;
}

const float NaoEnactor::calibrate_acc_y(const float y) {
    return y * ACCEL_CONVERSION_Y;
}

const float NaoEnactor::calibrate_acc_z(const float z) {
    return z * ACCEL_CONVERSION_Z;
}

void NaoEnactor::syncWithALMemory() {
    alfastaccessJoints->GetValues(jointValues);
    sensors->setBodyAngles(jointValues);

    // There are 16 sensor values we want.
    // The vector is static so that it is initialized only once for this
    // method.
    static vector<float> sensorValues(16, 0.0f);
    alfastaccessSensors->GetValues(sensorValues);

    // The indices here are determined by the order in which we requested
    // the sensors values (see initSyncWithALMemory).
    const float LfrontLeft = sensorValues[0], LfrontRight = sensorValues[1],
        LrearLeft = sensorValues[2], LrearRight = sensorValues[3],
        RfrontLeft = sensorValues[4], RfrontRight = sensorValues[5],
        RrearLeft = sensorValues[6], RrearRight = sensorValues[7];

    const float accX = calibrate_acc_x(sensorValues[8]),
        accY = calibrate_acc_y(sensorValues[9]),
        accZ = calibrate_acc_z(sensorValues[10]),
        gyrX = sensorValues[11], gyrY = sensorValues[12],
        angleX = sensorValues[13], angleY = sensorValues[14];

    accelerationFilter.update(accX, accY, accZ);
    const float filteredX = accelerationFilter.getX();
    const float filteredY = accelerationFilter.getY();
    const float filteredZ = accelerationFilter.getZ();


    sensors->
        setMotionSensors(FSR(LfrontLeft, LfrontRight, LrearLeft, LrearRight),
                         FSR(RfrontLeft, RfrontRight, RrearLeft, RrearRight),
                         //Inertial(filteredX, filteredY, filteredZ,
                         Inertial(accX, accY, accZ,
                                  gyrX, gyrY, angleX, angleY));
}
