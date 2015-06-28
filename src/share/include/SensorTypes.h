/**
 * @brief  Contains useful sensor structs, definitions, and types.
 * @author Ellis Ratner <eratner@bowdoin.edu>
 * @date   January 2013 (modified)
 */
#pragma once

namespace sensors
{
// Source: V4 NAO Documentation.
enum SensorTypes
{
    HeadYaw = 0,
    HeadPitch,
    // LARM,
    LShoulderPitch,
    LShoulderRoll,
    LElbowYaw,
    LElbowRoll,
    // LLEG,
    LHipYawPitch,
    LHipRoll,
    LHipPitch,
    LKneePitch,
    LAnklePitch,
    LAnkleRoll,
    // RLEG,
    RHipRoll,
    RHipPitch,
    RKneePitch,
    RAnklePitch,
    RAnkleRoll,
    // RARM,
    RShoulderPitch,
    RShoulderRoll,
    RElbowYaw,
    RElbowRoll,
    // WRIST/HAND,
    LWristYaw,
    LHand,
    RWristYaw,
    RHand,

    //END_JOINTS,

    /** Currents **/

    // Head joints
    HeadYawCurrent,
    HeadPitchCurrent,

    // Left Arm joints
    LShoulderPitchCurrent,
    LShoulderRollCurrent,
    LElbowYawCurrent,
    LElbowRollCurrent,

    // Left Leg joints
    LHipYawPitchCurrent,
    LHipRollCurrent,
    LHipPitchCurrent,
    LKneePitchCurrent,
    LAnklePitchCurrent,
    LAnkleRollCurrent,

    // Right Leg joints
    RHipRollCurrent,
    RHipPitchCurrent,
    RKneePitchCurrent,
    RAnklePitchCurrent,
    RAnkleRollCurrent,

    // Right Arm joints
    RShoulderPitchCurrent,
    RShoulderRollCurrent,
    RElbowYawCurrent,
    RElbowRollCurrent,

    // Hand joints
    LWristYawCurrent,
    LHandCurrent,
    RWristYawCurrent,
    RHandCurrent,

    //END_CURRENTS,

    /** Temperatures **/

    // Head joints
    HeadYawTemp,
    HeadPitchTemp,

    // Left Arm joints
    LShoulderPitchTemp,
    LShoulderRollTemp,
    LElbowYawTemp,
    LElbowRollTemp,

    // Left Leg joints
    LHipYawPitchTemp,
    LHipRollTemp,
    LHipPitchTemp,
    LKneePitchTemp,
    LAnklePitchTemp,
    LAnkleRollTemp,

    // Right Leg joints
    RHipRollTemp,
    RHipPitchTemp,
    RKneePitchTemp,
    RAnklePitchTemp,
    RAnkleRollTemp,

    // Right Arm joints
    RShoulderPitchTemp,
    RShoulderRollTemp,
    RElbowYawTemp,
    RElbowRollTemp,

    // Hand joints
    LWristYawTemp,
    LHandTemp,
    RWristYawTemp,
    RHandTemp,

    //END_TEMPERATURES,

    /** Force Sensitive Resistors (FSR) **/
    // 4 on each foot
    // (Left Foot)
    LFsrFL, // front left
    LFsrFR, // front right
    LFsrRL, // rear left
    LFsrRR, // rear right

    //END_FSRS_LEFT,

    // (Right Foot)
    RFsrFL,
    RFsrFR,
    RFsrRL,
    RFsrRR,

    //END_FSRS_RIGHT,

    /** Inertial Measurement Units (IMUs) or
        Inertial Sensors (2 axis gyrometers,
        1 3 axis accelerometer)              **/
    // Raw accelerometer data
    AccX,
    AccY,
    AccZ,
    // Raw gyrometer data
    GyrX,
    GyrY,
    GyrZ,
    // Filtered angle data
    AngleX,
    AngleY,
    AngleZ,

    //END_INTERTIALS,

    /* Past here, unneccessary to have END_VARS because
       the string keys are too unique to loop over.     */
    // The battery charge.
    BatteryCharge,
    BatteryCurrent,

    /** Ultrasonic (sonar) sensors **/
    USLeft,
    USRight,

    /** Foot bumpers (push buttons) **/
    // 2 on the left foot,
    LFootBumperLeft,
    LFootBumperRight,
    // and 2 on the right foot.
    RFootBumperLeft,
    RFootBumperRight,

    // The chest push-button.
    ChestboardButton,

    // Stiffness values
    HeadPitchStiff,
    HeadYawStiff,
    LAnklePitchStiff,
    LAnkleRollStiff,
    LElbowRollStiff,
    LElbowYawStiff,
    LHandStiff,
    LHipPitchStiff,
    LHipRollStiff,
    LHipYawPitchStiff,
    LKneePitchStiff,
    LShoulderPitchStiff,
    LShoulderRollStiff,
    LWristYawStiff,
    RAnklePitchStiff,
    RAnkleRollStiff,
    RElbowRollStiff,
    RElbowYawStiff,
    RHandStiff,
    RHipPitchStiff,
    RHipRollStiff,
    RKneePitchStiff,
    RShoulderPitchStiff,
    RShoulderRollStiff,
    RWristYawStiff,

    NUM_SENSOR_VALUES
};

enum KeyEnds
{
    END_JOINTS = HeadYawCurrent,
    END_CURRENTS = HeadYawTemp,
    END_TEMPERATURES = LFsrFL,
    END_FSRS_LEFT = RFsrFL,
    END_FSRS_RIGHT = AccX,
    END_INERTIALS = BatteryCharge
};

static const std::string SensorNames[] =
{
    "HeadYaw",
    "HeadPitch",
    // LARM
    "LShoulderPitch",
    "LShoulderRoll",
    "LElbowYaw",
    "LElbowRoll",
    // LLEG
    "LHipYawPitch",
    "LHipRoll",
    "LHipPitch",
    "LKneePitch",
    "LAnklePitch",
    "LAnkleRoll",
    // RLEG
    "RHipRoll",
    "RHipPitch",
    "RKneePitch",
    "RAnklePitch",
    "RAnkleRoll",
    // RARM
    "RShoulderPitch",
    "RShoulderRoll",
    "RElbowYaw",
    "RElbowRoll",
    // HAND/WRIST
    "LWristYaw",
    "LHand",
    "RWristYaw",
    "RHand",
    //"!!END!!",
    // CURRENTS
    "HeadYawCurrent",
    "HeadPitchCurrent",
    "LShoulderPitchCurrent",
    "LShoulderRollCurrent",
    "LElbowYawCurrent",
    "LElbowRollCurrent",
    "LHipYawPitchCurrent",
    "LHipRollCurrent",
    "LHipPitchCurrent",
    "LKneePitchCurrent",
    "LAnklePitchCurrent",
    "LAnkleRollCurrent",
    "RHipRollCurrent",
    "RHipPitchCurrent",
    "RKneePitchCurrent",
    "RAnklePitchCurrent",
    "RAnkleRollCurrent",
    "RShoulderPitchCurrent",
    "RShoulderRollCurrent",
    "RElbowYawCurrent",
    "RElbowRollCurrent",
    "LWristYawCurrent",
    "LHandCurrent",
    "RWristYawCurrent",
    "RHandCurrent",
    //"!!END!!",
    // TEMPS
    "HeadYawTemp",
    "HeadPitchTemp",
    "LShoulderPitchTemp",
    "LShoulderRollTemp",
    "LElbowYawTemp",
    "LElbowRollTemp",
    "LHipYawPitchTemp",
    "LHipRollTemp",
    "LHipPitchTemp",
    "LKneePitchTemp",
    "LAnklePitchTemp",
    "LAnkleRollTemp",
    "RHipRollTemp",
    "RHipPitchTemp",
    "RKneePitchTemp",
    "RAnklePitchTemp",
    "RAnkleRollTemp",
    "RShoulderPitchTemp",
    "RShoulderRollTemp",
    "RElbowYawTemp",
    "RElbowRollTemp",
    "LWristYawTemp",
    "LHandTemp",
    "RWristYawTemp",
    "RHandTemp",
    //"!!END!!",
    //FSRS
    // left
    "FrontLeft",
    "FrontRight",
    "RearLeft",
    "RearRight",
    //"!!END!!",
    // right
    "FrontLeft",
    "FrontRight",
    "RearLeft",
    "RearRight",
    //"!!END!!",
    //INERTIALS
    "AccX",
    "AccY",
    "AccZ",
    "GyrX",
    "GyrY",
    "GyrZ",
    "AngleX",
    "AngleY",
    "AngleZ",
    //"!!END!!",
    //OTHERS
    "BatteryCharge",
    "BatteryCurrent",
    "USLeft",
    "USRight",
    "LFootBumperLeft",
    "LFootBumperRight",
    "RFootBumperLeft",
    "RFootBumperRight",
    "ChestboardButton",
    // STIFFNESSES
    "HeadPitchStiff",
    "HeadYawStiff",
    "LAnklePitchStiff",
    "LAnkleRollStiff",
    "LElbowRollStiff",
    "LElbowYawStiff",
    "LHandStiff",
    "LHipPitchStiff",
    "LHipRollStiff",
    "LHipYawPitchStiff",
    "LKneePitchStiff",
    "LShoulderPitchStiff",
    "LShoulderRollStiff",
    "LWristYawStiff",
    "RAnklePitchStiff",
    "RAnkleRollStiff",
    "RElbowRollStiff",
    "RElbowYawStiff",
    "RHandStiff",
    "RHipPitchStiff",
    "RHipRollStiff",
    "RKneePitchStiff",
    "RShoulderPitchStiff",
    "RShoulderRollStiff",
    "RWristYawStiff",
    "!!END!!"
};
} // namespace sensors
