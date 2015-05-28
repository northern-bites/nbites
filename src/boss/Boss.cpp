#include "Boss.h"

#include "SharedData.h"
#include "SensorTypes.h"

#include <iostream>


namespace boss {

Boss::Boss(boost::shared_ptr<AL::ALBroker> broker_, const std::string &name) :
    AL::ALModule(broker_, name),
    fastAccess(new AL::ALMemoryFastAccess()),
    broker(broker_),
    sensorValues(),
    sensorKeys()
{
    initSensorAccess();
    dcm = broker->getSpecialisedProxy<AL::DCMProxy>("DCM");
    try {
        dcmPreProcessConnection = broker_->getProxy("DCM")->getModule()->atPostProcess(
            boost::bind(&Boss::DCMPostProcessCallback, this));
    }
    catch(const AL::ALError& e) {
        std::cout << "Tried to bind, but failed, because " + e.toString() << std::endl;
    }
    std::cout << "Boss Constructed successfully!" << std::endl;
}

Boss::~Boss()
{
    std::cout << "Deconstructing" << std::endl;
    dcmPreProcessConnection.disconnect();
}

void Boss::DCMPostProcessCallback()
{
    fastAccess->GetValues(sensorValues);
    // Do something with the sensor values here!
    //std::cout << "l_shoulder_roll is: " << sensorValues[sensors::LElbowRoll] << std::endl;
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
