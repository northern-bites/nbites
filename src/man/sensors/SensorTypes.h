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
	    // Head joints
	    HeadYaw = 0,
	    HeadPitch,

	    // Left Arm joints
	    LShoulderPitch,
	    LShoulderRoll,
	    LElbowYaw,
	    LElbowRoll,
	    LWristYaw,
	    LHand,

	    // Right Arm joints
	    RShoulderPitch,
	    RShoulderRoll,
	    RElbowYaw,
	    RElbowRoll,
	    RWristYaw,
	    RHand,

	    // Pelvis joints
	    LHipYawPitch,
	    RHipYawPitch,

	    // Left Leg joints
	    LHipRoll,
	    LHipPitch,
	    LKneePitch,
	    LAnklePitch,
	    LAnkleRoll,

	    // Right Leg joints
	    RHipRoll,
	    RHipPitch,
	    RKneePitch,
	    RAnklePitch,
	    RAnkleRoll,

	    // Force Sensitive Resistors (FSR)
	    // 4 on each foot
	    // (Left Foot)
	    LFsrFL, // front left
	    LFsrFR, // front right
	    LFsrRL, // rear left
	    LFsrRR, // rear right
	    // (Right Foot)
	    RFsrFL,
	    RFsrFR,
	    RFsrRL,
	    RFsrRR,

	    // Inertial Measurement Units (IMUs) or 
	    // Inertial Sensors (2 axis gyrometers,
	    // 1 3 axis accelerometer.)
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

	    // Ultrasonic (sonar) sensors.
	    USLeft,
	    USRight,

	    // Foot bumpers (push buttons).
	    // 2 on the left foot,
	    LFootBumperLeft,
	    LFootBumperRight,
	    // and 2 on the right foot.
	    RFootBumperLeft,
	    RFootBumperRight,

	    // The chest push-button.
	    ChestboardButton,

	    NUM_SENSOR_VALUES
	};

	static const std::string SensorNames[] =
	{
	    "HeadYaw",
	    "HeadPitch",
	    "LShoulderPitch",
	    "LShoulderRoll",
	    "LElbowYaw",
	    "LElbowRoll",
	    "LWristYaw",
	    "LHand",
	    "RShoulderPitch",
	    "RShoulderRoll",
	    "RElbowYaw",
	    "RElbowRoll",
	    "RWristYaw",
	    "RHand",
	    "LHipYawPitch",
	    "RHipYawPitch",
	    "LHipRoll",
	    "LHipPitch",
	    "LKneePitch",
	    "LAnklePitch",
	    "LAnkleRoll",
	    "RHipRoll",
	    "RHipPitch",
	    "RKneePitch",
	    "RAnklePitch",
	    "RAnkleRoll",
	    "LFsrFL", 
	    "LFsrFR", 
	    "LFsrRL", 
	    "LFsrRR", 
	    "RFsrFL",
	    "RFsrFR",
	    "RFsrRL",
	    "RFsrRR",
	    "AccX",
	    "AccY",
	    "AccZ",
	    "GyrX",
	    "GyrY",
	    "AngleX",
	    "AngleY",
	    "USLeft",
	    "USRight",
	    "LFootBumperLeft",
	    "LFootBumperRight",
	    "RFootBumperLeft",
	    "RFootBumperRight",
	    "ChestboardButton"
	};
    } // namespace sensors
} // namespace man
