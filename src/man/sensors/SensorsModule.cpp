#include "SensorsModule.h"

namespace man
{
namespace sensors
{

SensorsModule::SensorsModule(boost::shared_ptr<AL::ALBroker> broker)
    : portals::Module(),
      jointsOutput_(base()),
      chestboardButtonOutput_(base()),
      footbumperOutput_(base()),
      inertialsOutput_(base()),
      sonarsOutput_(base()),
      fsrOutput_(base()),
      broker_(broker),
      fastMemoryAccess_(new AL::ALMemoryFastAccess()),
      sensorValues_(NUM_SENSOR_VALUES),
      sensorKeys_(NUM_SENSOR_VALUES)
{
    //std::cout << "SensorsModule : Constructor." << std::endl;

    // Initialize the Aldebaran fast access memory interface
    // to quickly read sensor values from memory.
    initializeSensorFastAccess();
}

SensorsModule::~SensorsModule()
{
    //std::cout << "SensorsModule : Destructor." << std::endl;
}

void SensorsModule::initializeSensorFastAccess()
{
    // There are 26 joints.
    int i = 0;
    for(; i < 26; ++i)
    {
        sensorKeys_[i] = std::string("Device/SubDeviceList/") + SensorNames[i] + std::string("/Position/Sensor/Value");
    }
    // There are 8 FSR sensors.
    // (Left foot)
    for(; i < 30; ++i)
    {
        sensorKeys_[i] = std::string("Device/SubDeviceList/LFoot/FSR/") + SensorNames[i] + std::string("/Sensor/Value");
    }
    // (Right foot)
    for(; i < 34; ++i)
    {
        sensorKeys_[i] = std::string("Device/SubDeviceList/RFoot/FSR/") + SensorNames[i] + std::string("/Sensor/Value");
    }
    // There are 7 inertial sensors.
    for(; i < 41; ++i)
    {
        sensorKeys_[i] = std::string("Device/SubDeviceList/InertialSensor/") + SensorNames[i] + std::string("/Sensor/Value");
    }
    // There are 2 important sonars.
    sensorKeys_[i] = std::string("Device/SubDeviceList/US/Left/Sensor/Value");
    i++;
    sensorKeys_[i] = std::string("Device/SubDeviceList/US/Right/Sensor/Value");
    i++;
    // There are 4 foot bumpers.
    sensorKeys_[i] = std::string("Device/SubDeviceList/LFoot/Bumper/Left/Sensor/Value");
    i++;
    sensorKeys_[i] = std::string("Device/SubDeviceList/LFoot/Bumper/Right/Sensor/Value");
    i++;
    sensorKeys_[i] = std::string("Device/SubDeviceList/RFoot/Bumper/Left/Sensor/Value");
    i++;
    sensorKeys_[i] = std::string("Device/SubDeviceList/RFoot/Bumper/Right/Sensor/Value");
    i++;
    // There is a single chest button.
    sensorKeys_[i] = std::string("Device/SubDeviceList/Chestboard/Button/Sensor/Value");

    fastMemoryAccess_->ConnectToVariables(broker_, sensorKeys_);

    std::cout << "SensorsModule : Sensor keys initialized." << std::endl;
    // for(std::vector<std::string>::iterator iter = sensorKeys_.begin();
    //  iter != sensorKeys_.end();
    //  ++iter)
    // {
    //  std::cout << *iter << std::endl;
    // }
}

void SensorsModule::initializeSonarValues()
{
    // Get a proxy to the DCM.
    boost::shared_ptr<AL::DCMProxy> dcmProxy = broker_->getDcmProxy();
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
            std::cout << "SensorsModule : Failed to initialize sonars, " << e.toString() << std::endl;
        }
    }
}

void SensorsModule::updateSensorValues()
{
    //std::cout << "SensorsModule : Retrieving sensor values from NAOqi." << std::endl;
    // Update stored sensor values.
    fastMemoryAccess_->GetValues(sensorValues_);

    updateJointsMessage();
    updateChestboardButtonMessage();
    updateFootbumperMessage();
    updateInertialsMessage();
    updateSonarsMessage();

    //std::cout << "SensorsModule : Sensor values " << std::endl;
    // for(int i = 0; i < NUM_SENSOR_VALUES; ++i)
    // {
    //  std::cout << SensorNames[i] << " = " << sensorValues_[i] << std::endl;
    // }
}

void SensorsModule::updateJointsMessage()
{
    portals::Message<messages::JointAngles> jointsMessage(0);

    jointsMessage.get()->set_head_yaw(sensorValues_[HeadYaw]);
    jointsMessage.get()->set_head_pitch(sensorValues_[HeadPitch]);
    jointsMessage.get()->set_l_shoulder_pitch(sensorValues_[LShoulderPitch]);
    jointsMessage.get()->set_l_shoulder_roll(sensorValues_[LShoulderRoll]);
    jointsMessage.get()->set_l_elbow_yaw(sensorValues_[LElbowYaw]);
    jointsMessage.get()->set_l_elbow_roll(sensorValues_[LElbowRoll]);
    jointsMessage.get()->set_l_wrist_yaw(sensorValues_[LWristYaw]);
    jointsMessage.get()->set_l_hand(sensorValues_[LHand]);
    jointsMessage.get()->set_r_shoulder_pitch(sensorValues_[RShoulderPitch]);
    jointsMessage.get()->set_r_shoulder_roll(sensorValues_[RShoulderRoll]);
    jointsMessage.get()->set_r_elbow_yaw(sensorValues_[RElbowYaw]);
    jointsMessage.get()->set_r_elbow_roll(sensorValues_[RElbowRoll]);
    jointsMessage.get()->set_r_wrist_yaw(sensorValues_[RWristYaw]);
    jointsMessage.get()->set_r_hand(sensorValues_[RHand]);
    jointsMessage.get()->set_l_hip_yaw_pitch(sensorValues_[LHipYawPitch]);
    jointsMessage.get()->set_r_hip_yaw_pitch(sensorValues_[RHipYawPitch]);
    jointsMessage.get()->set_l_hip_roll(sensorValues_[LHipRoll]);
    jointsMessage.get()->set_l_hip_pitch(sensorValues_[LHipPitch]);
    jointsMessage.get()->set_l_knee_pitch(sensorValues_[LKneePitch]);
    jointsMessage.get()->set_l_ankle_pitch(sensorValues_[LAnklePitch]);
    jointsMessage.get()->set_l_ankle_roll(sensorValues_[LAnkleRoll]);
    jointsMessage.get()->set_r_hip_roll(sensorValues_[RHipRoll]);
    jointsMessage.get()->set_r_hip_pitch(sensorValues_[RHipPitch]);
    jointsMessage.get()->set_r_knee_pitch(sensorValues_[RKneePitch]);
    jointsMessage.get()->set_r_ankle_pitch(sensorValues_[RAnklePitch]);
    jointsMessage.get()->set_r_ankle_roll(sensorValues_[RAnkleRoll]);

    jointsOutput_.setMessage(jointsMessage);
}

void SensorsModule::updateChestboardButtonMessage()
{
    portals::Message<messages::ButtonState> chestboardMessage(0);

    chestboardMessage.get()->set_pressed(
        sensorValues_[ChestboardButton] > 0.5f ? true : false
        );

    chestboardButtonOutput_.setMessage(chestboardMessage);
}

void SensorsModule::updateFootbumperMessage()
{
    portals::Message<messages::FootBumperState> footbumperMessage(0);

    footbumperMessage.get()->mutable_l_foot_bumper_left()->set_pressed(
        sensorValues_[LFootBumperLeft] > 0.5f ? true : false
        );

    footbumperOutput_.setMessage(footbumperMessage);
}

void SensorsModule::updateInertialsMessage()
{
    portals::Message<messages::InertialState> inertialsMessage(0);

    inertialsMessage.get()->set_acc_x(sensorValues_[AccX]);
    inertialsMessage.get()->set_acc_y(sensorValues_[AccY]);
    inertialsMessage.get()->set_acc_z(sensorValues_[AccZ]);

    inertialsMessage.get()->set_gyr_x(sensorValues_[GyrX]);
    inertialsMessage.get()->set_gyr_y(sensorValues_[GyrY]);

    inertialsMessage.get()->set_angle_x(sensorValues_[AngleX]);
    inertialsMessage.get()->set_angle_y(sensorValues_[AngleY]);

    inertialsOutput_.setMessage(inertialsMessage);
}

void SensorsModule::updateSonarsMessage()
{
    portals::Message<messages::SonarState> sonarsMessage(0);

    sonarsMessage.get()->set_us_left(sensorValues_[USLeft]);
    sonarsMessage.get()->set_us_right(sensorValues_[USRight]);

    sonarsOutput_.setMessage(sonarsMessage);
}

void SensorsModule::updateFSRMessage()
{
    portals::Message<messages::FSR> fsrMessage(0);

    // Left foot FSR values.
    fsrMessage.get()->set_fsr_lfl(sensorValues_[LFsrFL]);
    fsrMessage.get()->set_fsr_lfr(sensorValues_[LFsrFR]);
    fsrMessage.get()->set_fsr_lrl(sensorValues_[LFsrRL]);
    fsrMessage.get()->set_fsr_lrr(sensorValues_[LFsrRR]);

    // Right foot FSR values.
    fsrMessage.get()->set_fsr_rfl(sensorValues_[RFsrFL]);
    fsrMessage.get()->set_fsr_rfr(sensorValues_[RFsrFR]);
    fsrMessage.get()->set_fsr_rrl(sensorValues_[RFsrRL]);
    fsrMessage.get()->set_fsr_rrr(sensorValues_[RFsrRR]);

    fsrOutput_.setMessage(fsrMessage);
}

void SensorsModule::run_()
{
    // Simply update all sensor readings from ALMemory.
    updateSensorValues();
}

} // namespace sensors
} // namespace man
