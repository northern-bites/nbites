/**
 * @brief  Contains useful sensor structs, definitions, and types.
 * @author Ellis Ratner <eratner@bowdoin.edu>
 * @date   January 2013 (modified)
 */
#pragma once

namespace man
{
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
        RHipYawPitch,
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

        END_JOINTS,

        /** Temperatures **/

        // Head joints
        HeadYawTemp,
        HeadPitchTemp,

        // Left Arm joints
        LShoulderPitchTemp,
        LShoulderRollTemp,
        LElbowYawTemp,
        LElbowRollTemp,
        LWristYawTemp,
        LHandTemp,

        // Right Arm joints
        RShoulderPitchTemp,
        RShoulderRollTemp,
        RElbowYawTemp,
        RElbowRollTemp,
        RWristYawTemp,
        RHandTemp,

        // Pelvis joints
        LHipYawPitchTemp,
        RHipYawPitchTemp,

        // Left Leg joints
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

        END_TEMPERATURES,

        /** Force Sensitive Resistors (FSR) **/
        // 4 on each foot
        // (Left Foot)
        LFsrFL, // front left
        LFsrFR, // front right
        LFsrRL, // rear left
        LFsrRR, // rear right

        END_FSRS_LEFT,

        // (Right Foot)
        RFsrFL,
        RFsrFR,
        RFsrRL,
        RFsrRR,

        END_FSRS_RIGHT,

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
        // Filtered angle data
        AngleX,
        AngleY,

        END_INTERTIALS,

        /* Past here, unneccessary to have END_VARS because
           the string keys are too unique to loop over.     */

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

        // The battery charge.
        BatteryCharge,

        NUM_SENSOR_VALUES
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
        "RHipYawPitch",
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
        "!!END!!",
        // TEMPS
        "HeadYawTemp",
        "HeadPitchTemp",
        "LShoulderPitchTemp",
        "LShoulderRollTemp",
        "LElbowYawTemp",
        "LElbowRollTemp",
        "LWristYawTemp",
        "LHandTemp",
        "RShoulderPitchTemp",
        "RShoulderRollTemp",
        "RElbowYawTemp",
        "RElbowRollTemp",
        "RWristYawTemp",
        "RHandTemp",
        "LHipYawPitchTemp",
        "RHipYawPitchTemp",
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
        "!!END!!",
        //FSRS
        // left
        "LFsrFL",
        "LFsrFR",
        "LFsrRL",
        "LFsrRR",
        "!!END!!",
        // right
        "RFsrFL",
        "RFsrFR",
        "RFsrRL",
        "RFsrRR",
        "!!END!!",
        //INERTIALS
        "AccX",
        "AccY",
        "AccZ",
        "GyrX",
        "GyrY",
        "AngleX",
        "AngleY",
        "!!END!!",
        //OTHERS
        "USLeft",
        "USRight",
        "LFootBumperLeft",
        "LFootBumperRight",
        "RFootBumperLeft",
        "RFootBumperRight",
        "ChestboardButton",
        "BatteryCharge",
        "!!END!!"
    };
    } // namespace sensors
} // namespace man
