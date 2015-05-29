#include "Boss.h"

#include "SharedData.h"
#include "SensorTypes.h"
#include "JointNames.h"

#include <iostream>


namespace boss {

Boss::Boss(boost::shared_ptr<AL::ALBroker> broker_, const std::string &name) :
    AL::ALModule(broker_, name),
    fastAccess(new AL::ALMemoryFastAccess()),
    broker(broker_),
    dcm(broker->getSpecialisedProxy<AL::DCMProxy>("DCM")),
    sensorValues(),
    sensorKeys(),
    manPID(-1),
    manRunning(false)
{
    std::cout << "Boss Constructor" << std::endl;

    initEnactor();
    initSensorAccess();

    try {
        dcmPreProcessConnection = broker_->getProxy("DCM")->getModule()->atPreProcess(
            boost::bind(&Boss::DCMPreProcessCallback, this));
    }
    catch(const AL::ALError& e) {
        std::cout << "Tried to bind preprocess, but failed, because " + e.toString() << std::endl;
    }
    try {
        dcmPostProcessConnection = broker_->getProxy("DCM")->getModule()->atPostProcess(
            boost::bind(&Boss::DCMPostProcessCallback, this));
    }
    catch(const AL::ALError& e) {
        std::cout << "Tried to bind postprocess, but failed, because " + e.toString() << std::endl;
    }


    std::cout << "Boss Constructed successfully!" << std::endl;
}

Boss::~Boss()
{
    std::cout << "Deconstructing" << std::endl;
    dcmPreProcessConnection.disconnect();
    dcmPostProcessConnection.disconnect();
}

int Boss::startMan() {
    // TODO make sure man isn't running yet
    pid_t child = fork();
    if (child > 0) {
        manPID = child;
        manRunning = true;
    }
    else if (child == 0) {
        execl("/home/nao/nbites/lib/man", "", NULL);
    }
    else {
        std::cout << "COULD NOT DETACH MAN" << std::endl;
        manRunning = false;
    }
}

int Boss::killMan() {
    // TODO make sure man is actually running

    kill(manPID, SIGTERM);
    return 0; // TODO actually return something
}

int Boss::constructSharedMem()
{
    //int shared_fd = shm_open(MEMORY_NAME, O_RDWR | O_CREAT, 0600);
}

void Boss::DCMPreProcessCallback()
{
    // Read in Joints to write from shared memory
    // Write them to the DCM
    // Needs to be FAST!
    std::vector<float> jointAngles = Kinematics::toJointAngles(latestJointAngles);
    std::vector<float> jointStiffnesses = Kinematics::toJointAngles(latestJointAngles);

    jointAngles.erase(jointAngles.begin() + Kinematics::R_HIP_YAW_PITCH);
    jointStiffnesses.erase(jointStiffnesses.begin() + Kinematics::R_HIP_YAW_PITCH);

    std::cout << "Preprocess!" << std::endl;
    std::cout << jointAngles.size() << std::endl;

    for (unsigned int i = 0; i < jointAngles.size(); ++i)
    {
        jointCommand[5][i][0] = jointAngles[i];
        stiffnessCommand[5][i][0] = jointStiffnesses[i];
    }
    return;

    try
    {
        jointCommand[4][0] = dcm->getTime(0);
        //dcm->setAlias(jointCommand);
    }
    catch (AL::ALError e) {
        std::cout << "Couldn't set joint angles becasue: " << e.toString() << std::endl;
    }
    try
    {
        stiffnessCommand[4][0] = dcm->getTime(0);
        //dcm->setAlias(stiffnessCommand);
    }
    catch (AL::ALError e) {
        std::cout << "Couldn't set stiffness becasue: " << e.toString() << std::endl;
    }
}

void Boss::DCMPostProcessCallback()
{
    fastAccess->GetValues(sensorValues);
    std::cout << "Postprocess!" << std::endl;
    // Do something with the sensor values here!
    //std::cout << "l_shoulder_roll is: " << sensorValues[sensors::LElbowRoll] << std::endl;
}

// Based on (stolen from) the original JointEnactorModule by Ellis Ratner
void Boss::initEnactor()
{
    AL::ALValue aliases;

    // Create aliases for issuing joint angle
    // commands to the joint actuators.
    aliases.arraySetSize(2);
    aliases[0] = std::string("jointActuator");
    aliases[1].arraySetSize(25);

    aliases[1][sensors::HeadYaw]         = std::string("Device/SubDeviceList/HeadYaw/Position/Actuator/Value");
    aliases[1][sensors::HeadPitch]       = std::string("Device/SubDeviceList/HeadPitch/Position/Actuator/Value");
    aliases[1][sensors::LShoulderPitch]  = std::string("Device/SubDeviceList/LShoulderPitch/Position/Actuator/Value");
    aliases[1][sensors::LShoulderRoll]   = std::string("Device/SubDeviceList/LShoulderRoll/Position/Actuator/Value");
    aliases[1][sensors::LElbowYaw]       = std::string("Device/SubDeviceList/LElbowYaw/Position/Actuator/Value");
    aliases[1][sensors::LElbowRoll]      = std::string("Device/SubDeviceList/LElbowRoll/Position/Actuator/Value");
    aliases[1][sensors::LHipYawPitch]    = std::string("Device/SubDeviceList/LHipYawPitch/Position/Actuator/Value");
    aliases[1][sensors::LHipRoll]        = std::string("Device/SubDeviceList/LHipRoll/Position/Actuator/Value");
    aliases[1][sensors::LHipPitch]       = std::string("Device/SubDeviceList/LHipPitch/Position/Actuator/Value");
    aliases[1][sensors::LKneePitch]      = std::string("Device/SubDeviceList/LKneePitch/Position/Actuator/Value");
    aliases[1][sensors::LAnklePitch]     = std::string("Device/SubDeviceList/LAnklePitch/Position/Actuator/Value");
    aliases[1][sensors::LAnkleRoll]      = std::string("Device/SubDeviceList/LAnkleRoll/Position/Actuator/Value");
    aliases[1][sensors::RHipRoll]        = std::string("Device/SubDeviceList/RHipRoll/Position/Actuator/Value");
    aliases[1][sensors::RHipPitch]       = std::string("Device/SubDeviceList/RHipPitch/Position/Actuator/Value");
    aliases[1][sensors::RKneePitch]      = std::string("Device/SubDeviceList/RKneePitch/Position/Actuator/Value");
    aliases[1][sensors::RAnklePitch]     = std::string("Device/SubDeviceList/RAnklePitch/Position/Actuator/Value");
    aliases[1][sensors::RAnkleRoll]      = std::string("Device/SubDeviceList/RAnkleRoll/Position/Actuator/Value");
    aliases[1][sensors::RShoulderPitch]  = std::string("Device/SubDeviceList/RShoulderPitch/Position/Actuator/Value");
    aliases[1][sensors::RShoulderRoll]   = std::string("Device/SubDeviceList/RShoulderRoll/Position/Actuator/Value");
    aliases[1][sensors::RElbowYaw]       = std::string("Device/SubDeviceList/RElbowYaw/Position/Actuator/Value");
    aliases[1][sensors::RElbowRoll]      = std::string("Device/SubDeviceList/RElbowRoll/Position/Actuator/Value");
    aliases[1][sensors::LWristYaw]       = std::string("Device/SubDeviceList/LWristYaw/Position/Actuator/Value");
    aliases[1][sensors::LHand]           = std::string("Device/SubDeviceList/LHand/Position/Actuator/Value");
    aliases[1][sensors::RWristYaw]       = std::string("Device/SubDeviceList/RWristYaw/Position/Actuator/Value");
    aliases[1][sensors::RHand]           = std::string("Device/SubDeviceList/RHand/Position/Actuator/Value");

    try
    {
        dcm->createAlias(aliases);
    }
    catch(const AL::ALError& e)
    {
        std::cout << "Error creating joint actuator aliases: "
                  << e.toString()
                  << std::endl;
    }

    // Create aliases for issuing stiffness commands
    // to the joint actuators.
    aliases.clear();
    aliases.arraySetSize(2);
    aliases[0] = std::string("jointStiffness");
    aliases[1].arraySetSize(25);

    aliases[1][sensors::HeadYaw]          = std::string("Device/SubDeviceList/HeadYaw/Hardness/Actuator/Value");
    aliases[1][sensors::HeadPitch]        = std::string("Device/SubDeviceList/HeadPitch/Hardness/Actuator/Value");
    aliases[1][sensors::LShoulderPitch]  = std::string("Device/SubDeviceList/LShoulderPitch/Hardness/Actuator/Value");
    aliases[1][sensors::LShoulderRoll]   = std::string("Device/SubDeviceList/LShoulderRoll/Hardness/Actuator/Value");
    aliases[1][sensors::LElbowYaw]       = std::string("Device/SubDeviceList/LElbowYaw/Hardness/Actuator/Value");
    aliases[1][sensors::LElbowRoll]      = std::string("Device/SubDeviceList/LElbowRoll/Hardness/Actuator/Value");
    aliases[1][sensors::LHipYawPitch]   = std::string("Device/SubDeviceList/LHipYawPitch/Hardness/Actuator/Value");
    aliases[1][sensors::LHipRoll]        = std::string("Device/SubDeviceList/LHipRoll/Hardness/Actuator/Value");
    aliases[1][sensors::LHipPitch]       = std::string("Device/SubDeviceList/LHipPitch/Hardness/Actuator/Value");
    aliases[1][sensors::LKneePitch]      = std::string("Device/SubDeviceList/LKneePitch/Hardness/Actuator/Value");
    aliases[1][sensors::LAnklePitch]     = std::string("Device/SubDeviceList/LAnklePitch/Hardness/Actuator/Value");
    aliases[1][sensors::LAnkleRoll]      = std::string("Device/SubDeviceList/LAnkleRoll/Hardness/Actuator/Value");
    aliases[1][sensors::RHipRoll]        = std::string("Device/SubDeviceList/RHipRoll/Hardness/Actuator/Value");
    aliases[1][sensors::RHipPitch]       = std::string("Device/SubDeviceList/RHipPitch/Hardness/Actuator/Value");
    aliases[1][sensors::RKneePitch]      = std::string("Device/SubDeviceList/RKneePitch/Hardness/Actuator/Value");
    aliases[1][sensors::RAnklePitch]     = std::string("Device/SubDeviceList/RAnklePitch/Hardness/Actuator/Value");
    aliases[1][sensors::RAnkleRoll]      = std::string("Device/SubDeviceList/RAnkleRoll/Hardness/Actuator/Value");
    aliases[1][sensors::RShoulderPitch]  = std::string("Device/SubDeviceList/RShoulderPitch/Hardness/Actuator/Value");
    aliases[1][sensors::RShoulderRoll]   = std::string("Device/SubDeviceList/RShoulderRoll/Hardness/Actuator/Value");
    aliases[1][sensors::RElbowYaw]       = std::string("Device/SubDeviceList/RElbowYaw/Hardness/Actuator/Value");
    aliases[1][sensors::RElbowRoll]      = std::string("Device/SubDeviceList/RElbowRoll/Hardness/Actuator/Value");
    aliases[1][sensors::LWristYaw]       = std::string("Device/SubDeviceList/LWristYaw/Hardness/Actuator/Value");
    aliases[1][sensors::LHand]            = std::string("Device/SubDeviceList/LHand/Hardness/Actuator/Value");
    aliases[1][sensors::RWristYaw]       = std::string("Device/SubDeviceList/RWristYaw/Hardness/Actuator/Value");
    aliases[1][sensors::RHand]            = std::string("Device/SubDeviceList/RHand/Hardness/Actuator/Value");

    try
    {
        dcm->createAlias(aliases);
    }
    catch(const AL::ALError& e)
    {
        std::cout << "Error creating joint stiffness aliases: "
                  << e.toString()
                  << std::endl;
    }

    // Initialize joint command.
    jointCommand.arraySetSize(6);
    jointCommand[0] = std::string("jointActuator");
    jointCommand[1] = std::string("ClearAll");
    jointCommand[2] = std::string("time-separate");
    jointCommand[3] = 0; // Importance level.
    jointCommand[4].arraySetSize(1);
    jointCommand[5].arraySetSize(sensors::NUM_SENSOR_VALUES);
    // Set default joint angle values.
    for(int i = 0; i < sensors::NUM_SENSOR_VALUES; ++i)
    {
        jointCommand[5][i].arraySetSize(1);
        jointCommand[5][i][0] = 0.0f; // This will be the new joint angle.
    }

    // Initialize stiffness command.
    stiffnessCommand.arraySetSize(6);
    stiffnessCommand[0] = std::string("jointStiffness");
    stiffnessCommand[1] = std::string("ClearAll");
    stiffnessCommand[2] = std::string("time-separate");
    stiffnessCommand[3] = 0; // Importance level.
    stiffnessCommand[4].arraySetSize(1);
    stiffnessCommand[5].arraySetSize(sensors::NUM_SENSOR_VALUES);
    // Set default joint stiffness values.
    for(int i = 0; i < sensors::NUM_SENSOR_VALUES; ++i)
    {
        stiffnessCommand[5][i].arraySetSize(1);
        stiffnessCommand[5][i][0] = 0.0f; // This will be the new joint stiffness.
    }

}

void Boss::initSensorAccess()
{
    std::cout << "Initializing sensor keys" << std::endl;
    int i = 0;

    // Joint Angles
    for(; i < sensors::END_JOINTS; ++i)
    {
        std::string key = std::string("Device/SubDeviceList/") +
            sensors::SensorNames[i] + std::string("/Position/Sensor/Value");
        sensorKeys.push_back(key);
    }

    // Joint Currents
    for(; i < sensors::END_CURRENTS; ++i)
    {
        // Subtract 25 from index in SensorsNames[] to get correct value.
        std::string key = std::string("Device/SubDeviceList/") +
            sensors::SensorNames[i-25] + std::string("/ElectricCurrent/Sensor/Value");
        sensorKeys.push_back(key);
    }

    // Temperatures
    for(; i < sensors::END_TEMPERATURES; ++i)
    {
        // Subtract 2*27 from index in SensorsNames[] to get correct value.
        std::string key = std::string("Device/SubDeviceList/") +
            sensors::SensorNames[i-2*25] + std::string("/Temperature/Sensor/Value");
        sensorKeys.push_back(key);
    }

    // FSR (Left foot)
    for(; i < sensors::END_FSRS_LEFT; ++i)
    {
        std::string key = std::string("Device/SubDeviceList/LFoot/FSR/") +
            sensors::SensorNames[i] + std::string("/Sensor/Value");
        sensorKeys.push_back(key);
    }

    // FSR (Right foot)
    for(; i < sensors::END_FSRS_RIGHT; ++i)
    {
        std::string key = std::string("Device/SubDeviceList/RFoot/FSR/") +
            sensors::SensorNames[i] + std::string("/Sensor/Value");
        sensorKeys.push_back(key);
    }

    // Inertial Sensors
    for(; i < sensors::END_INERTIALS; ++i)
    {
        std::string key = std::string("Device/SubDeviceList/InertialSensor/") +
            sensors::SensorNames[i] + std::string("/Sensor/Value");
        sensorKeys.push_back(key);
    }

    // There are 2 battery values.
    sensorKeys.push_back(std::string("Device/SubDeviceList/Battery/Charge/Sensor/Value"));
    i++;
    /* IMPORTANT for some reason, battery charge cannot be read correctly unless
     * battery current is read also, who knows why, bad aldebaran code?
     * NOT ACTUALLY OUTPORTALED OR USED AT ALL, current is needed for bug fix
     * TODO: Determine if this is still the case */
    sensorKeys.push_back(std::string("Device/SubDeviceList/Battery/Current/Sensor/Value"));
    i++;
    // There are 2 important sonars.
    sensorKeys.push_back(std::string("Device/SubDeviceList/US/Left/Sensor/Value"));
    i++;
    sensorKeys.push_back(std::string("Device/SubDeviceList/US/Right/Sensor/Value"));
    i++;
    // There are 4 foot bumpers.
    sensorKeys.push_back(std::string("Device/SubDeviceList/LFoot/Bumper/Left/Sensor/Value"));
    i++;
    sensorKeys.push_back(std::string("Device/SubDeviceList/LFoot/Bumper/Right/Sensor/Value"));
    i++;
    sensorKeys.push_back(std::string("Device/SubDeviceList/RFoot/Bumper/Left/Sensor/Value"));
    i++;
    sensorKeys.push_back(std::string("Device/SubDeviceList/RFoot/Bumper/Right/Sensor/Value"));
    i++;
    // There is a single chest button.
    sensorKeys.push_back(std::string("Device/SubDeviceList/ChestBoard/Button/Sensor/Value"));
    i++;
    // All joints have stiffnesses associated with them.
    sensorKeys.push_back(std::string("Device/SubDeviceList/HeadPitch/Hardness/Actuator/Value"));
    i++;
    sensorKeys.push_back(std::string("Device/SubDeviceList/HeadYaw/Hardness/Actuator/Value"));
    i++;
    sensorKeys.push_back(std::string("Device/SubDeviceList/LAnklePitch/Hardness/Actuator/Value"));
    i++;
    sensorKeys.push_back(std::string("Device/SubDeviceList/LAnkleRoll/Hardness/Actuator/Value"));
    i++;
    sensorKeys.push_back(std::string("Device/SubDeviceList/LElbowRoll/Hardness/Actuator/Value"));
    i++;
    sensorKeys.push_back(std::string("Device/SubDeviceList/LElbowYaw/Hardness/Actuator/Value"));
    i++;
    sensorKeys.push_back(std::string("Device/SubDeviceList/LHand/Hardness/Actuator/Value"));
    i++;
    sensorKeys.push_back(std::string("Device/SubDeviceList/LHipPitch/Hardness/Actuator/Value"));
    i++;
    sensorKeys.push_back(std::string("Device/SubDeviceList/LHipRoll/Hardness/Actuator/Value"));
    i++;
    sensorKeys.push_back(std::string("Device/SubDeviceList/LHipYawPitch/Hardness/Actuator/Value"));
    i++;
    sensorKeys.push_back(std::string("Device/SubDeviceList/LKneePitch/Hardness/Actuator/Value"));
    i++;
    sensorKeys.push_back(std::string("Device/SubDeviceList/LShoulderPitch/Hardness/Actuator/Value"));
    i++;
    sensorKeys.push_back(std::string("Device/SubDeviceList/LShoulderRoll/Hardness/Actuator/Value"));
    i++;
    sensorKeys.push_back(std::string("Device/SubDeviceList/LWristYaw/Hardness/Actuator/Value"));
    i++;
    sensorKeys.push_back(std::string("Device/SubDeviceList/RAnklePitch/Hardness/Actuator/Value"));
    i++;
    sensorKeys.push_back(std::string("Device/SubDeviceList/RAnkleRoll/Hardness/Actuator/Value"));
    i++;
    sensorKeys.push_back(std::string("Device/SubDeviceList/RElbowRoll/Hardness/Actuator/Value"));
    i++;
    sensorKeys.push_back(std::string("Device/SubDeviceList/RElbowYaw/Hardness/Actuator/Value"));
    i++;
    sensorKeys.push_back(std::string("Device/SubDeviceList/RHand/Hardness/Actuator/Value"));
    i++;
    sensorKeys.push_back(std::string("Device/SubDeviceList/RHipPitch/Hardness/Actuator/Value"));
    i++;
    sensorKeys.push_back(std::string("Device/SubDeviceList/RHipRoll/Hardness/Actuator/Value"));
    i++;
    sensorKeys.push_back(std::string("Device/SubDeviceList/RKneePitch/Hardness/Actuator/Value"));
    i++;
    sensorKeys.push_back(std::string("Device/SubDeviceList/RShoulderPitch/Hardness/Actuator/Value"));
    i++;
    sensorKeys.push_back(std::string("Device/SubDeviceList/RShoulderRoll/Hardness/Actuator/Value"));
    i++;
    sensorKeys.push_back(std::string("Device/SubDeviceList/RWristYaw/Hardness/Actuator/Value"));

    fastAccess->ConnectToVariables(broker, sensorKeys);

    std::cout << "\tSensor keys initialized." << std::endl;
}

}
