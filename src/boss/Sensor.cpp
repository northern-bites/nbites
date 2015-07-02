#include "Sensor.h"

#include "SensorTypes.h"
#include "JointNames.h"

#include <iostream>

namespace boss {
namespace sensor {

Sensor::Sensor(boost::shared_ptr<AL::ALBroker> broker_) :
    broker(broker_),
    fastAccess(new AL::ALMemoryFastAccess())
{
    initSensor();
    initSonarValues();
}

Sensor::~Sensor()
{
    delete fastAccess;
}

SensorValues Sensor::getSensors()
{
    fastAccess->GetValues(sensorValues);

    SensorValues v;
    v.joints = buildJointsMessage();
    v.currents = buildCurrentsMessage();
    v.temperature = buildTemperatureMessage();
    v.chestButton = buildChestboardButtonMessage();
    v.footBumper = buildFootbumperMessage();
    v.inertials = buildInertialsMessage();
    v.sonars = buildSonarsMessage();
    v.fsr = buildFSRMessage();
    v.battery = buildBatteryMessage();
    v.stiffStatus = buildStiffMessage();

    return v;
}

void Sensor::initSensor()
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

void Sensor::initSonarValues()
{
    // Get a proxy to the DCM.
    boost::shared_ptr<AL::DCMProxy> dcmProxy = broker->getDcmProxy();
    if(dcmProxy != 0)
    {
        try
        {
            // For DCM::set() see:
            // http://www.aldebaran-robotics.com/documentation/naoqi/sensors/dcm-api.html#DCMProxy::set__AL::ALValueCR
            AL::ALValue dcmSonarCommand;

            dcmSonarCommand.arraySetSize(3);
            dcmSonarCommand[0] = std::string("Device/SubDeviceList/US/Actuator/Value"); // Device name.
            dcmSonarCommand[1] = std::string("ClearAll"); // Delete all timed commands before adding this one.

            dcmSonarCommand[2].arraySetSize(1); // A list of (1) timed-commands.
            dcmSonarCommand[2][0].arraySetSize(2);
            dcmSonarCommand[2][0][0] = 68.0; // The command itself.
            dcmSonarCommand[2][0][1] = dcmProxy->getTime(0); // The DCM time for the command to be applied.

            // Send the timed command to the sonars.
            dcmProxy->set(dcmSonarCommand);
        }
        catch(AL::ALError& e)
        {
            std::cout << "SensorsModule : Failed to initialize sonars, "
                      << e.toString() << std::endl;
        }
    }
}

messages::JointAngles Sensor::buildJointsMessage()
{
    messages::JointAngles jointsMessage;

    jointsMessage.set_head_yaw(sensorValues[sensors::HeadYaw]);
    jointsMessage.set_head_pitch(sensorValues[sensors::HeadPitch]);
    jointsMessage.set_l_shoulder_pitch(sensorValues[sensors::LShoulderPitch]);
    jointsMessage.set_l_shoulder_roll(sensorValues[sensors::LShoulderRoll]);
    jointsMessage.set_l_elbow_yaw(sensorValues[sensors::LElbowYaw]);
    jointsMessage.set_l_elbow_roll(sensorValues[sensors::LElbowRoll]);
    jointsMessage.set_l_wrist_yaw(sensorValues[sensors::LWristYaw]);
    jointsMessage.set_l_hand(sensorValues[sensors::LHand]);
    jointsMessage.set_r_shoulder_pitch(sensorValues[sensors::RShoulderPitch]);
    jointsMessage.set_r_shoulder_roll(sensorValues[sensors::RShoulderRoll]);
    jointsMessage.set_r_elbow_yaw(sensorValues[sensors::RElbowYaw]);
    jointsMessage.set_r_elbow_roll(sensorValues[sensors::RElbowRoll]);
    jointsMessage.set_r_wrist_yaw(sensorValues[sensors::RWristYaw]);
    jointsMessage.set_r_hand(sensorValues[sensors::RHand]);
    // Hip yaw pitches have ALWAYS been the same, only now Aldebaran isn't
    // allowing you to access it from the right
    jointsMessage.set_l_hip_yaw_pitch(sensorValues[sensors::LHipYawPitch]);
    jointsMessage.set_r_hip_yaw_pitch(sensorValues[sensors::LHipYawPitch]);
    jointsMessage.set_l_hip_roll(sensorValues[sensors::LHipRoll]);
    jointsMessage.set_l_hip_pitch(sensorValues[sensors::LHipPitch]);
    jointsMessage.set_l_knee_pitch(sensorValues[sensors::LKneePitch]);
    jointsMessage.set_l_ankle_pitch(sensorValues[sensors::LAnklePitch]);
    jointsMessage.set_l_ankle_roll(sensorValues[sensors::LAnkleRoll]);
    jointsMessage.set_r_hip_roll(sensorValues[sensors::RHipRoll]);
    jointsMessage.set_r_hip_pitch(sensorValues[sensors::RHipPitch]);
    jointsMessage.set_r_knee_pitch(sensorValues[sensors::RKneePitch]);
    jointsMessage.set_r_ankle_pitch(sensorValues[sensors::RAnklePitch]);
    jointsMessage.set_r_ankle_roll(sensorValues[sensors::RAnkleRoll]);

    return jointsMessage;
}

messages::JointAngles Sensor::buildCurrentsMessage()
{
    messages::JointAngles jointsMessage;

    jointsMessage.set_head_yaw(sensorValues[sensors::HeadYawCurrent]);
    jointsMessage.set_head_pitch(sensorValues[sensors::HeadPitchCurrent]);
    jointsMessage.set_l_shoulder_pitch(sensorValues[sensors::LShoulderPitchCurrent]);
    jointsMessage.set_l_shoulder_roll(sensorValues[sensors::LShoulderRollCurrent]);
    jointsMessage.set_l_elbow_yaw(sensorValues[sensors::LElbowYawCurrent]);
    jointsMessage.set_l_elbow_roll(sensorValues[sensors::LElbowRollCurrent]);
    jointsMessage.set_l_wrist_yaw(sensorValues[sensors::LWristYawCurrent]);
    jointsMessage.set_l_hand(sensorValues[sensors::LHandCurrent]);
    jointsMessage.set_r_shoulder_pitch(sensorValues[sensors::RShoulderPitchCurrent]);
    jointsMessage.set_r_shoulder_roll(sensorValues[sensors::RShoulderRollCurrent]);
    jointsMessage.set_r_elbow_yaw(sensorValues[sensors::RElbowYawCurrent]);
    jointsMessage.set_r_elbow_roll(sensorValues[sensors::RElbowRollCurrent]);
    jointsMessage.set_r_wrist_yaw(sensorValues[sensors::RWristYawCurrent]);
    jointsMessage.set_r_hand(sensorValues[sensors::RHandCurrent]);
    jointsMessage.set_l_hip_yaw_pitch(sensorValues[sensors::LHipYawPitchCurrent]);
    jointsMessage.set_r_hip_yaw_pitch(sensorValues[sensors::LHipYawPitchCurrent]);
    jointsMessage.set_l_hip_roll(sensorValues[sensors::LHipRollCurrent]);
    jointsMessage.set_l_hip_pitch(sensorValues[sensors::LHipPitchCurrent]);
    jointsMessage.set_l_knee_pitch(sensorValues[sensors::LKneePitchCurrent]);
    jointsMessage.set_l_ankle_pitch(sensorValues[sensors::LAnklePitchCurrent]);
    jointsMessage.set_l_ankle_roll(sensorValues[sensors::LAnkleRollCurrent]);
    jointsMessage.set_r_hip_roll(sensorValues[sensors::RHipRollCurrent]);
    jointsMessage.set_r_hip_pitch(sensorValues[sensors::RHipPitchCurrent]);
    jointsMessage.set_r_knee_pitch(sensorValues[sensors::RKneePitchCurrent]);
    jointsMessage.set_r_ankle_pitch(sensorValues[sensors::RAnklePitchCurrent]);
    jointsMessage.set_r_ankle_roll(sensorValues[sensors::RAnkleRollCurrent]);

    return jointsMessage;
}

messages::JointAngles Sensor::buildTemperatureMessage()
{
    messages::JointAngles temperaturesMessage;

    temperaturesMessage.set_head_yaw(sensorValues[sensors::HeadYawTemp]);
    temperaturesMessage.set_head_pitch(sensorValues[sensors::HeadPitchTemp]);
    temperaturesMessage.set_l_shoulder_pitch(sensorValues[sensors::LShoulderPitchTemp]);
    temperaturesMessage.set_l_shoulder_roll(sensorValues[sensors::LShoulderRollTemp]);
    temperaturesMessage.set_l_elbow_yaw(sensorValues[sensors::LElbowYawTemp]);
    temperaturesMessage.set_l_elbow_roll(sensorValues[sensors::LElbowRollTemp]);
    temperaturesMessage.set_l_wrist_yaw(sensorValues[sensors::LWristYawTemp]);
    temperaturesMessage.set_l_hand(sensorValues[sensors::LHandTemp]);
    temperaturesMessage.set_r_shoulder_pitch(sensorValues[sensors::RShoulderPitchTemp]);
    temperaturesMessage.set_r_shoulder_roll(sensorValues[sensors::RShoulderRollTemp]);
    temperaturesMessage.set_r_elbow_yaw(sensorValues[sensors::RElbowYawTemp]);
    temperaturesMessage.set_r_elbow_roll(sensorValues[sensors::RElbowRollTemp]);
    temperaturesMessage.set_r_wrist_yaw(sensorValues[sensors::RWristYawTemp]);
    temperaturesMessage.set_r_hand(sensorValues[sensors::RHandTemp]);
    temperaturesMessage.set_l_hip_yaw_pitch(sensorValues[sensors::LHipYawPitchTemp]);
    temperaturesMessage.set_r_hip_yaw_pitch(sensorValues[sensors::LHipYawPitchTemp]);
    temperaturesMessage.set_l_hip_roll(sensorValues[sensors::LHipRollTemp]);
    temperaturesMessage.set_l_hip_pitch(sensorValues[sensors::LHipPitchTemp]);
    temperaturesMessage.set_l_knee_pitch(sensorValues[sensors::LKneePitchTemp]);
    temperaturesMessage.set_l_ankle_pitch(sensorValues[sensors::LAnklePitchTemp]);
    temperaturesMessage.set_l_ankle_roll(sensorValues[sensors::LAnkleRollTemp]);
    temperaturesMessage.set_r_hip_roll(sensorValues[sensors::RHipRollTemp]);
    temperaturesMessage.set_r_hip_pitch(sensorValues[sensors::RHipPitchTemp]);
    temperaturesMessage.set_r_knee_pitch(sensorValues[sensors::RKneePitchTemp]);
    temperaturesMessage.set_r_ankle_pitch(sensorValues[sensors::RAnklePitchTemp]);
    temperaturesMessage.set_r_ankle_roll(sensorValues[sensors::RAnkleRollTemp]);

    return temperaturesMessage;
}

messages::ButtonState Sensor::buildChestboardButtonMessage()
{
    messages::ButtonState chestboardMessage;

    chestboardMessage.set_pressed(
        sensorValues[sensors::ChestboardButton] > 0.5f ? true : false
        );

    return chestboardMessage;
}

messages::FootBumperState Sensor::buildFootbumperMessage()
{
    messages::FootBumperState footbumperMessage;

    footbumperMessage.mutable_l_foot_bumper_left() ->set_pressed(
        sensorValues[sensors::LFootBumperLeft]  > 0.5f ? true : false
        );
    footbumperMessage.mutable_l_foot_bumper_right()->set_pressed(
        sensorValues[sensors::LFootBumperRight] > 0.5f ? true : false
        );

    footbumperMessage.mutable_r_foot_bumper_left() ->set_pressed(
        sensorValues[sensors::RFootBumperLeft]  > 0.5f ? true : false
        );
    footbumperMessage.mutable_r_foot_bumper_right()->set_pressed(
        sensorValues[sensors::RFootBumperRight] > 0.5f ? true : false
        );

    return footbumperMessage;
}

messages::InertialState Sensor::buildInertialsMessage()
{
    messages::InertialState inertialsMessage;

    inertialsMessage.set_acc_x(sensorValues[sensors::AccX]);
    inertialsMessage.set_acc_y(sensorValues[sensors::AccY]);
    inertialsMessage.set_acc_z(sensorValues[sensors::AccZ]);

    inertialsMessage.set_gyr_x(sensorValues[sensors::GyrX]);
    inertialsMessage.set_gyr_y(sensorValues[sensors::GyrY]);
    inertialsMessage.set_gyr_z(sensorValues[sensors::GyrZ]);

    inertialsMessage.set_angle_x(sensorValues[sensors::AngleX]);
    inertialsMessage.set_angle_y(sensorValues[sensors::AngleY]);
    inertialsMessage.set_angle_z(sensorValues[sensors::AngleZ]);

    return inertialsMessage;
}

messages::SonarState Sensor::buildSonarsMessage()
{
    messages::SonarState sonarsMessage;

    sonarsMessage.set_us_left(sensorValues[sensors::USLeft]);
    sonarsMessage.set_us_right(sensorValues[sensors::USRight]);

    return sonarsMessage;
}

messages::FSR Sensor::buildFSRMessage()
{
    messages::FSR fsrMessage;

    // Left foot FSR values.
    fsrMessage.set_lfl(sensorValues[sensors::LFsrFL]);
    fsrMessage.set_lfr(sensorValues[sensors::LFsrFR]);
    fsrMessage.set_lrl(sensorValues[sensors::LFsrRL]);
    fsrMessage.set_lrr(sensorValues[sensors::LFsrRR]);

    // Right foot FSR values.
    fsrMessage.set_rfl(sensorValues[sensors::RFsrFL]);
    fsrMessage.set_rfr(sensorValues[sensors::RFsrFR]);
    fsrMessage.set_rrl(sensorValues[sensors::RFsrRL]);
    fsrMessage.set_rrr(sensorValues[sensors::RFsrRR]);

    return fsrMessage;
}

messages::BatteryState Sensor::buildBatteryMessage()
{
    messages::BatteryState batteryMessage;

    batteryMessage.set_charge(sensorValues[sensors::BatteryCharge]);

    return batteryMessage;
}

messages::StiffStatus Sensor::buildStiffMessage()
{
    messages::StiffStatus stiffMessage;

    if (sensorValues[sensors::HeadPitchStiff] > 0 ||
        sensorValues[sensors::HeadYawStiff] > 0 ||
        sensorValues[sensors::LAnklePitchStiff] > 0 ||
        sensorValues[sensors::LAnkleRollStiff] > 0 ||
        sensorValues[sensors::LElbowRollStiff] > 0 ||
        sensorValues[sensors::LElbowYawStiff] > 0 ||
        sensorValues[sensors::LHandStiff] > 0 ||
        sensorValues[sensors::LHipPitchStiff] > 0 ||
        sensorValues[sensors::LHipRollStiff] > 0 ||
        sensorValues[sensors::LHipYawPitchStiff] > 0 ||
        sensorValues[sensors::LKneePitchStiff] > 0 ||
        sensorValues[sensors::LShoulderPitchStiff] > 0 ||
        sensorValues[sensors::LShoulderRollStiff] > 0 ||
        sensorValues[sensors::LWristYawStiff] > 0 ||
        sensorValues[sensors::RAnklePitchStiff] > 0 ||
        sensorValues[sensors::RAnkleRollStiff] > 0 ||
        sensorValues[sensors::RElbowRollStiff] > 0 ||
        sensorValues[sensors::RElbowYawStiff] > 0 ||
        sensorValues[sensors::RHandStiff] > 0 ||
        sensorValues[sensors::RHipPitchStiff] > 0 ||
        sensorValues[sensors::RHipRollStiff] > 0 ||
        sensorValues[sensors::RKneePitchStiff] > 0 ||
        sensorValues[sensors::RShoulderPitchStiff] > 0 ||
        sensorValues[sensors::RShoulderRollStiff] > 0 ||
        sensorValues[sensors::RWristYawStiff] > 0)
    {
        stiffMessage.set_on(1);
    }
    else
    {
        stiffMessage.set_on(0);
    }

    return stiffMessage;
}


}
}
