#include "SensorsModule.h"
#include "Common.h"
#include <stdio.h>

namespace man {
namespace sensors {

SensorsModule::SensorsModule(boost::shared_ptr<AL::ALBroker> broker)
    : portals::Module(),
      jointsOutput_(base()),
      currentsOutput_(base()),
      temperatureOutput_(base()),
      chestboardButtonOutput_(base()),
      footbumperOutput_(base()),
      inertialsOutput_(base()),
      sonarsOutput_(base()),
      fsrOutput_(base()),
      batteryOutput_(base()),
      stiffStatusOutput_(base()),
      broker_(broker),
      fastMemoryAccess_(new AL::ALMemoryFastAccess()),
      sensorValues_(NUM_SENSOR_VALUES),
      sensorKeys_(NUM_SENSOR_VALUES)
{
    // Initialize the Aldebaran fast access memory interface
    // to quickly read sensor values from memory.
    initializeSensorFastAccess();
    initializeSonarValues();
}

SensorsModule::~SensorsModule()
{
}

void SensorsModule::initializeSensorFastAccess()
{
    int i = 0;

    // Joint Angles
    for(; i < END_JOINTS; ++i)
    {
        sensorKeys_[i] = std::string("Device/SubDeviceList/") +
            SensorNames[i] + std::string("/Position/Sensor/Value");
    }
    i++;

    // Joint Currents
    for(; i < END_CURRENTS; ++i)
    {
        // Subtract 27 from index in SensorsNames[] to get correct value.
        sensorKeys_[i] = std::string("Device/SubDeviceList/") +
            SensorNames[i-27] + std::string("/ElectricCurrent/Sensor/Value");
    }
    i++;

    // Temperatures
    for(; i < END_TEMPERATURES; ++i)
    {
        // Subtract 2*27 from index in SensorsNames[] to get correct value.
        sensorKeys_[i] = std::string("Device/SubDeviceList/") +
            SensorNames[i-2*27] + std::string("/Temperature/Sensor/Value");
    }
    i++;

    // FSR (Left foot)
    for(; i < END_FSRS_LEFT; ++i)
    {
        sensorKeys_[i] = std::string("Device/SubDeviceList/LFoot/FSR/") +
            SensorNames[i] + std::string("/Sensor/Value");
    }
    i++;

    // FSR (Right foot)
    for(; i < END_FSRS_RIGHT; ++i)
    {
        sensorKeys_[i] = std::string("Device/SubDeviceList/RFoot/FSR/") +
            SensorNames[i] + std::string("/Sensor/Value");
    }
    i++;

    // Inertial Sensors
    for(; i < END_INTERTIALS; ++i)
    {
        sensorKeys_[i] = std::string("Device/SubDeviceList/InertialSensor/") +
            SensorNames[i] + std::string("/Sensor/Value");
    }
    i++;

    // There are 2 battery values.
    sensorKeys_[i] = std::string("Device/SubDeviceList/Battery/Charge/Sensor/Value");
    i++;
    /* IMPORTANT for some reason, battery charge cannot be read correctly unless
     * battery current is read also, who knows why, bad aldebaran code?
     * NOT ACTUALLY OUTPORTALED OR USED AT ALL, current is needed for bug fix */
    sensorKeys_[i] = std::string("Device/SubDeviceList/Battery/Current/Sensor/Value");
    i++;
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
    sensorKeys_[i] = std::string("Device/SubDeviceList/ChestBoard/Button/Sensor/Value");
    i++;
    // All joints have stiffnesses associated with them.
    sensorKeys_[i] = std::string("Device/SubDeviceList/HeadPitch/Hardness/Actuator/Value");
    i++;
    sensorKeys_[i] = std::string("Device/SubDeviceList/HeadYaw/Hardness/Actuator/Value");
    i++;
    sensorKeys_[i] = std::string("Device/SubDeviceList/LAnklePitch/Hardness/Actuator/Value");
    i++;
    sensorKeys_[i] = std::string("Device/SubDeviceList/LAnkleRoll/Hardness/Actuator/Value");
    i++;
    sensorKeys_[i] = std::string("Device/SubDeviceList/LElbowRoll/Hardness/Actuator/Value");
    i++;
    sensorKeys_[i] = std::string("Device/SubDeviceList/LElbowYaw/Hardness/Actuator/Value");
    i++;
    sensorKeys_[i] = std::string("Device/SubDeviceList/LHand/Hardness/Actuator/Value");
    i++;
    sensorKeys_[i] = std::string("Device/SubDeviceList/LHipPitch/Hardness/Actuator/Value");
    i++;
    sensorKeys_[i] = std::string("Device/SubDeviceList/LHipRoll/Hardness/Actuator/Value");
    i++;
    sensorKeys_[i] = std::string("Device/SubDeviceList/LHipYawPitch/Hardness/Actuator/Value");
    i++;
    sensorKeys_[i] = std::string("Device/SubDeviceList/LKneePitch/Hardness/Actuator/Value");
    i++;
    sensorKeys_[i] = std::string("Device/SubDeviceList/LShoulderPitch/Hardness/Actuator/Value");
    i++;
    sensorKeys_[i] = std::string("Device/SubDeviceList/LShoulderRoll/Hardness/Actuator/Value");
    i++;
    sensorKeys_[i] = std::string("Device/SubDeviceList/LWristYaw/Hardness/Actuator/Value");
    i++;
    sensorKeys_[i] = std::string("Device/SubDeviceList/RAnklePitch/Hardness/Actuator/Value");
    i++;
    sensorKeys_[i] = std::string("Device/SubDeviceList/RAnkleRoll/Hardness/Actuator/Value");
    i++;
    sensorKeys_[i] = std::string("Device/SubDeviceList/RElbowRoll/Hardness/Actuator/Value");
    i++;
    sensorKeys_[i] = std::string("Device/SubDeviceList/RElbowYaw/Hardness/Actuator/Value");
    i++;
    sensorKeys_[i] = std::string("Device/SubDeviceList/RHand/Hardness/Actuator/Value");
    i++;
    sensorKeys_[i] = std::string("Device/SubDeviceList/RHipPitch/Hardness/Actuator/Value");
    i++;
    sensorKeys_[i] = std::string("Device/SubDeviceList/RHipYawPitch/Hardness/Actuator/Value");
    i++;
    sensorKeys_[i] = std::string("Device/SubDeviceList/RHipRoll/Hardness/Actuator/Value");
    i++;
    sensorKeys_[i] = std::string("Device/SubDeviceList/RKneePitch/Hardness/Actuator/Value");
    i++;
    sensorKeys_[i] = std::string("Device/SubDeviceList/RShoulderPitch/Hardness/Actuator/Value");
    i++;
    sensorKeys_[i]= std::string("Device/SubDeviceList/RShoulderRoll/Hardness/Actuator/Value");
    i++;
    sensorKeys_[i] = std::string("Device/SubDeviceList/RWristYaw/Hardness/Actuator/Value");

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
            std::cout << "SensorsModule : Failed to initialize sonars, "
                      << e.toString() << std::endl;
        }
    }
}

void SensorsModule::updateSensorValues()
{
    //std::cout << "SensorsModule : Retrieving sensor values from NAOqi." << std::endl;
    // Update stored sensor values.
    fastMemoryAccess_->GetValues(sensorValues_);

    updateJointsMessage();
    updateCurrentsMessage();
    updateTemperatureMessage();
    updateChestboardButtonMessage();
    updateFootbumperMessage();
    updateInertialsMessage();
    updateSonarsMessage();
    updateFSRMessage();
    updateBatteryMessage();
    updateStiffMessage();

    // std::cout << "SensorsModule : Sensor values " << std::endl;
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

void SensorsModule::updateCurrentsMessage()
{
    portals::Message<messages::JointAngles> jointsMessage(0);

    jointsMessage.get()->set_head_yaw(sensorValues_[HeadYawCurrent]);
    jointsMessage.get()->set_head_pitch(sensorValues_[HeadPitchCurrent]);
    jointsMessage.get()->set_l_shoulder_pitch(sensorValues_[LShoulderPitchCurrent]);
    jointsMessage.get()->set_l_shoulder_roll(sensorValues_[LShoulderRollCurrent]);
    jointsMessage.get()->set_l_elbow_yaw(sensorValues_[LElbowYawCurrent]);
    jointsMessage.get()->set_l_elbow_roll(sensorValues_[LElbowRollCurrent]);
    jointsMessage.get()->set_l_wrist_yaw(sensorValues_[LWristYawCurrent]);
    jointsMessage.get()->set_l_hand(sensorValues_[LHandCurrent]);
    jointsMessage.get()->set_r_shoulder_pitch(sensorValues_[RShoulderPitchCurrent]);
    jointsMessage.get()->set_r_shoulder_roll(sensorValues_[RShoulderRollCurrent]);
    jointsMessage.get()->set_r_elbow_yaw(sensorValues_[RElbowYawCurrent]);
    jointsMessage.get()->set_r_elbow_roll(sensorValues_[RElbowRollCurrent]);
    jointsMessage.get()->set_r_wrist_yaw(sensorValues_[RWristYawCurrent]);
    jointsMessage.get()->set_r_hand(sensorValues_[RHandCurrent]);
    jointsMessage.get()->set_l_hip_yaw_pitch(sensorValues_[LHipYawPitchCurrent]);
    jointsMessage.get()->set_r_hip_yaw_pitch(sensorValues_[RHipYawPitchCurrent]);
    jointsMessage.get()->set_l_hip_roll(sensorValues_[LHipRollCurrent]);
    jointsMessage.get()->set_l_hip_pitch(sensorValues_[LHipPitchCurrent]);
    jointsMessage.get()->set_l_knee_pitch(sensorValues_[LKneePitchCurrent]);
    jointsMessage.get()->set_l_ankle_pitch(sensorValues_[LAnklePitchCurrent]);
    jointsMessage.get()->set_l_ankle_roll(sensorValues_[LAnkleRollCurrent]);
    jointsMessage.get()->set_r_hip_roll(sensorValues_[RHipRollCurrent]);
    jointsMessage.get()->set_r_hip_pitch(sensorValues_[RHipPitchCurrent]);
    jointsMessage.get()->set_r_knee_pitch(sensorValues_[RKneePitchCurrent]);
    jointsMessage.get()->set_r_ankle_pitch(sensorValues_[RAnklePitchCurrent]);
    jointsMessage.get()->set_r_ankle_roll(sensorValues_[RAnkleRollCurrent]);

    currentsOutput_.setMessage(jointsMessage);
}

void SensorsModule::updateTemperatureMessage()
{
    portals::Message<messages::JointAngles> temperaturesMessage(0);
    temperaturesMessage.get()->set_head_yaw(sensorValues_[HeadYawTemp]);
    temperaturesMessage.get()->set_head_pitch(sensorValues_[HeadPitchTemp]);
    temperaturesMessage.get()->set_l_shoulder_pitch(sensorValues_[LShoulderPitchTemp]);
    temperaturesMessage.get()->set_l_shoulder_roll(sensorValues_[LShoulderRollTemp]);
    temperaturesMessage.get()->set_l_elbow_yaw(sensorValues_[LElbowYawTemp]);
    temperaturesMessage.get()->set_l_elbow_roll(sensorValues_[LElbowRollTemp]);
    temperaturesMessage.get()->set_l_wrist_yaw(sensorValues_[LWristYawTemp]);
    temperaturesMessage.get()->set_l_hand(sensorValues_[LHandTemp]);
    temperaturesMessage.get()->set_r_shoulder_pitch(sensorValues_[RShoulderPitchTemp]);
    temperaturesMessage.get()->set_r_shoulder_roll(sensorValues_[RShoulderRollTemp]);
    temperaturesMessage.get()->set_r_elbow_yaw(sensorValues_[RElbowYawTemp]);
    temperaturesMessage.get()->set_r_elbow_roll(sensorValues_[RElbowRollTemp]);
    temperaturesMessage.get()->set_r_wrist_yaw(sensorValues_[RWristYawTemp]);
    temperaturesMessage.get()->set_r_hand(sensorValues_[RHandTemp]);
    temperaturesMessage.get()->set_l_hip_yaw_pitch(sensorValues_[LHipYawPitchTemp]);
    temperaturesMessage.get()->set_r_hip_yaw_pitch(sensorValues_[RHipYawPitchTemp]);
    temperaturesMessage.get()->set_l_hip_roll(sensorValues_[LHipRollTemp]);
    temperaturesMessage.get()->set_l_hip_pitch(sensorValues_[LHipPitchTemp]);
    temperaturesMessage.get()->set_l_knee_pitch(sensorValues_[LKneePitchTemp]);
    temperaturesMessage.get()->set_l_ankle_pitch(sensorValues_[LAnklePitchTemp]);
    temperaturesMessage.get()->set_l_ankle_roll(sensorValues_[LAnkleRollTemp]);
    temperaturesMessage.get()->set_r_hip_roll(sensorValues_[RHipRollTemp]);
    temperaturesMessage.get()->set_r_hip_pitch(sensorValues_[RHipPitchTemp]);
    temperaturesMessage.get()->set_r_knee_pitch(sensorValues_[RKneePitchTemp]);
    temperaturesMessage.get()->set_r_ankle_pitch(sensorValues_[RAnklePitchTemp]);
    temperaturesMessage.get()->set_r_ankle_roll(sensorValues_[RAnkleRollTemp]);

    temperatureOutput_.setMessage(temperaturesMessage);
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

    footbumperMessage.get()->mutable_l_foot_bumper_left() ->set_pressed(
        sensorValues_[LFootBumperLeft]  > 0.5f ? true : false
        );
    footbumperMessage.get()->mutable_l_foot_bumper_right()->set_pressed(
        sensorValues_[LFootBumperRight] > 0.5f ? true : false
        );

    footbumperMessage.get()->mutable_r_foot_bumper_left() ->set_pressed(
        sensorValues_[RFootBumperLeft]  > 0.5f ? true : false
        );
    footbumperMessage.get()->mutable_r_foot_bumper_right()->set_pressed(
        sensorValues_[RFootBumperRight] > 0.5f ? true : false
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
    fsrMessage.get()->set_lfl(sensorValues_[LFsrFL]);
    fsrMessage.get()->set_lfr(sensorValues_[LFsrFR]);
    fsrMessage.get()->set_lrl(sensorValues_[LFsrRL]);
    fsrMessage.get()->set_lrr(sensorValues_[LFsrRR]);

    // Right foot FSR values.
    fsrMessage.get()->set_rfl(sensorValues_[RFsrFL]);
    fsrMessage.get()->set_rfr(sensorValues_[RFsrFR]);
    fsrMessage.get()->set_rrl(sensorValues_[RFsrRL]);
    fsrMessage.get()->set_rrr(sensorValues_[RFsrRR]);

    fsrOutput_.setMessage(fsrMessage);
}

void SensorsModule::updateBatteryMessage()
{
    portals::Message<messages::BatteryState> batteryMessage(0);

    batteryMessage.get()->set_charge(sensorValues_[BatteryCharge]);

    batteryOutput_.setMessage(batteryMessage);
}

void SensorsModule::updateStiffMessage()
{
    portals::Message<messages::StiffStatus> stiffMessage(0);

    if (sensorValues_[HeadPitchStiff] > 0 ||
        sensorValues_[HeadYawStiff] > 0 ||
        sensorValues_[LAnklePitchStiff] > 0 ||
        sensorValues_[LAnkleRollStiff] > 0 ||
        sensorValues_[LElbowRollStiff] > 0 ||
        sensorValues_[LElbowYawStiff] > 0 ||
        sensorValues_[LHandStiff] > 0 ||
        sensorValues_[LHipPitchStiff] > 0 ||
        sensorValues_[LHipRollStiff] > 0 ||
        sensorValues_[LHipYawPitchStiff] > 0 ||
        sensorValues_[LKneePitchStiff] > 0 ||
        sensorValues_[LShoulderPitchStiff] > 0 ||
        sensorValues_[LShoulderRollStiff] > 0 ||
        sensorValues_[LWristYawStiff] > 0 ||
        sensorValues_[RAnklePitchStiff] > 0 ||
        sensorValues_[RAnkleRollStiff] > 0 ||
        sensorValues_[RElbowRollStiff] > 0 ||
        sensorValues_[RElbowYawStiff] > 0 ||
        sensorValues_[RHandStiff] > 0 ||
        sensorValues_[RHipPitchStiff] > 0 ||
        sensorValues_[RHipYawPitchStiff] > 0 ||
        sensorValues_[RHipRollStiff] > 0 ||
        sensorValues_[RKneePitchStiff] > 0 ||
        sensorValues_[RShoulderPitchStiff] > 0 ||
        sensorValues_[RShoulderRollStiff] > 0 ||
        sensorValues_[RWristYawStiff] > 0)
    {
        stiffMessage.get()->set_on(1);
    }
    else
    {
        stiffMessage.get()->set_on(0);
    }

    stiffStatusOutput_.setMessage(stiffMessage);
}

// Helper method so that we can print out a Sweet Moves joint angle
// tuple directly when we want to (ie 5 button presses)
std::string makeSweetMoveTuple(const messages::JointAngles* angles)
{
    char output[240];

    sprintf(output, "((%3.1f, %3.1f, %3.1f, %3.1f),\n(%3.1f, %3.1f, %3.1f, %3.1f, %3.1f, %3.1f),\n(%3.1f, %3.1f, %3.1f, %3.1f, %3.1f, %3.1f),\n(%3.1f, %3.1f, %3.1f, %3.1f))\n",
            TO_DEG*angles->l_shoulder_pitch(),
            TO_DEG*angles->l_shoulder_roll(),
            TO_DEG*angles->l_elbow_yaw(),
            TO_DEG*angles->l_elbow_roll(),
            TO_DEG*angles->l_hip_yaw_pitch(),
            TO_DEG*angles->l_hip_roll(),
            TO_DEG*angles->l_hip_pitch(),
            TO_DEG*angles->l_knee_pitch(),
            TO_DEG*angles->l_ankle_pitch(),
            TO_DEG*angles->l_ankle_roll(),
            TO_DEG*angles->r_hip_yaw_pitch(),
            TO_DEG*angles->r_hip_roll(),
            TO_DEG*angles->r_hip_pitch(),
            TO_DEG*angles->r_knee_pitch(),
            TO_DEG*angles->r_ankle_pitch(),
            TO_DEG*angles->r_ankle_roll(),
            TO_DEG*angles->r_shoulder_pitch(),
            TO_DEG*angles->r_shoulder_roll(),
            TO_DEG*angles->r_elbow_yaw(),
            TO_DEG*angles->r_elbow_roll());

    return std::string(output);
}

void SensorsModule::run_()
{
    printInput.latch();

    // Simply update all sensor readings from ALMemory.
    updateSensorValues();

    if(printInput.message().toggle() != lastPrint)
    {
        std::cout << makeSweetMoveTuple(jointsOutput_.getMessage(true).get())
                  << std::endl;
        lastPrint = !lastPrint;
    }
}

} // namespace sensors
} // namespace man
