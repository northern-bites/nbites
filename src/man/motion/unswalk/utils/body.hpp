/**
 * Body.hpp
 * Description: Contains Joint Codes, Limits, Motor Reductions and Limb
 *              lengths and weights
 * Modified: 2009-11-06 (for Naoqi 1.3.17)
 */

#pragma once

#include <iostream>
#include <cmath>
#include <string>
#include <utils/matrix_helpers.hpp>
#include "utils/angles.hpp"


namespace Joints {
   // Codes for each joint as used by NaoQi
   typedef enum JointCodesEnum {
      HeadYaw = 0,
      HeadPitch,
      LShoulderPitch,
      LShoulderRoll,
      LElbowYaw,
      LElbowRoll,
      LWristYaw,
      LHand,
      LHipYawPitch,
      LHipRoll,
      LHipPitch,
      LKneePitch,
      LAnklePitch,
      LAnkleRoll,
      RHipRoll,
      RHipPitch,
      RKneePitch,
      RAnklePitch,
      RAnkleRoll,
      RShoulderPitch,
      RShoulderRoll,
      RElbowYaw,
      RElbowRoll,
      RWristYaw,
      RHand,
      NUMBER_OF_JOINTS
   } JointCode;

   const JointCode jointCodes[] = {
      HeadYaw,
      HeadPitch,
      LShoulderPitch,
      LShoulderRoll,
      LElbowYaw,
      LElbowRoll,
      LWristYaw,
      LHand,
      LHipYawPitch,
      LHipRoll,
      LHipPitch,
      LKneePitch,
      LAnklePitch,
      LAnkleRoll,
      RHipRoll,
      RHipPitch,
      RKneePitch,
      RAnklePitch,
      RAnkleRoll,
      RShoulderPitch,
      RShoulderRoll,
      RElbowYaw,
      RElbowRoll,
      RWristYaw,
      RHand,
   };

   // Note: Functions are defined at the end of this file...
   const std::string jointNames[] = {
      "HeadYaw",
      "HeadPitch",
      "LShoulderPitch",
      "LShoulderRoll",
      "LElbowYaw",
      "LElbowRoll",
      "LWristYaw",
      "LHand",
      "LHipYawPitch",
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
      "RShoulderPitch",
      "RShoulderRoll",
      "RElbowYaw",
      "RElbowRoll",
      "RWristYaw",
      "RHand",
   };

   const std::string fliteJointNames[] = {
      "Head Yaw",
      "Head Pitch",
      "L Shoulder Pitch",
      "L Shoulder Roll",
      "L Elbow Yaw",
      "L Elbow Roll",
      "L Wrist Yaw",
      "L Hand",
      "L Hip Yaw Pitch",
      "L Hip Roll",
      "L Hip Pitch",
      "L Knee Pitch",
      "L Ankle Pitch",
      "L Ankle Roll",
      "R Hip Roll",
      "R Hip Pitch",
      "R Knee Pitch",
      "R Ankle Pitch",
      "R Ankle Roll",
      "R Shoulder Pitch",
      "R Shoulder Roll",
      "R Elbow Yaw",
      "R Elbow Roll",
      "R Wrist Yaw",
      "R Hand",
   };

   // TODO(Bel): It looks like this is used in a simulator. If it gets used, it might
   // need to be updated to match the new joints.
   const std::string simJointNames[] = {
      "headPan",
      "headTilt",
      "armLeft0",
      "armLeft1",
      "armLeft2",
      "armLeft3",
      "armLeft4",
      "legLeft0",
      "legLeft1",
      "legLeft2",
      "legLeft3",
      "legLeft4",
      "legLeft5",
      "legRight1",
      "legRight2",
      "legRight3",
      "legRight4",
      "legRight5",
      "armRight0",
      "armRight1",
      "armRight2",
      "armRight3",
      "armRight4"
   };

   // Limits for Joints in RADIANS
   namespace Radians {
      const float HeadYaw_Min          = DEG2RAD(-119.5);
      const float HeadYaw_Max          = DEG2RAD(119.5);
      const float HeadPitch_Min        = DEG2RAD(-38.5);
      const float HeadPitch_Max        = DEG2RAD(29.5);
      const float LShoulderPitch_Min   = DEG2RAD(-119.5);
      const float LShoulderPitch_Max   = DEG2RAD(119.5);
      const float LShoulderRoll_Max    = DEG2RAD(76);
      const float LShoulderRoll_Min    = DEG2RAD(-18);
      const float LElbowYaw_Min        = DEG2RAD(-119.5);
      const float LElbowYaw_Max        = DEG2RAD(119.5);
      const float LElbowRoll_Min       = DEG2RAD(-88.5);
      const float LElbowRoll_Max       = DEG2RAD(-2);
      const float LWristYaw_Min        = DEG2RAD(-104.5);
      const float LWristYaw_Max        = DEG2RAD(104.5);
      const float LHand_Min            = 0.0;
      const float LHand_Max            = 1.0;
      const float LHipYawPitch_Min     = DEG2RAD(-65.62);
      const float LHipYawPitch_Max     = DEG2RAD(42.44);
      const float LHipPitch_Min        = DEG2RAD(-101.63);
      const float LHipPitch_Max        = DEG2RAD(27.73);
      const float LHipRoll_Min         = DEG2RAD(-21.74);
      const float LHipRoll_Max         = DEG2RAD(45.29);
      const float LKneePitch_Min       = DEG2RAD(-5.29);
      const float LKneePitch_Max       = DEG2RAD(121.04);
      const float LAnklePitch_Min      = DEG2RAD(-68.15);
      const float LAnklePitch_Max      = DEG2RAD(52.86);
      const float LAnkleRoll_Min       = DEG2RAD(-22.79);
      const float LAnkleRoll_Max       = DEG2RAD(44.06);
      const float RHipPitch_Min        = DEG2RAD(-101.54);
      const float RHipPitch_Max        = DEG2RAD(27.82);
      const float RHipRoll_Min         = DEG2RAD(-42.30);
      const float RHipRoll_Max         = DEG2RAD(23.76);
      const float RKneePitch_Min       = DEG2RAD(-5.90);
      const float RKneePitch_Max       = DEG2RAD(121.47);
      const float RAnklePitch_Min      = DEG2RAD(-67.97);
      const float RAnklePitch_Max      = DEG2RAD(53.40);
      const float RAnkleRoll_Min       = DEG2RAD(-45.03);
      const float RAnkleRoll_Max       = DEG2RAD(22.27);
      const float RShoulderPitch_Min   = DEG2RAD(-119.5);
      const float RShoulderPitch_Max   = DEG2RAD(119.5);
      const float RShoulderRoll_Min    = DEG2RAD(-76);
      const float RShoulderRoll_Max    = DEG2RAD(18);
      const float RElbowYaw_Min        = DEG2RAD(-119.5);
      const float RElbowYaw_Max        = DEG2RAD(119.5);
      const float RElbowRoll_Min       = DEG2RAD(2);
      const float RElbowRoll_Max       = DEG2RAD(88.5);
      const float RWristYaw_Min        = DEG2RAD(-104.5);
      const float RWristYaw_Max        = DEG2RAD(104.5);
      const float RHand_Min            = 0.0;
      const float RHand_Max            = 1.0;

      // Array to hold the maximum and minimum boundaries
      const float MaxAngle[] = {
         HeadYaw_Max,
         HeadPitch_Max,
         LShoulderPitch_Max,
         LShoulderRoll_Max,
         LElbowYaw_Max,
         LElbowRoll_Max,
         LWristYaw_Max,
         LHand_Max,
         LHipYawPitch_Max,
         LHipRoll_Max,
         LHipPitch_Max,
         LKneePitch_Max,
         LAnklePitch_Max,
         LAnkleRoll_Max,
         RHipRoll_Max,
         RHipPitch_Max,
         RKneePitch_Max,
         RAnklePitch_Max,
         RAnkleRoll_Max,
         RShoulderPitch_Max,
         RShoulderRoll_Max,
         RElbowYaw_Max,
         RElbowRoll_Max,
         RWristYaw_Max,
         RHand_Max
      };

      const float MinAngle[] = {
         HeadYaw_Min,
         HeadPitch_Min,
         LShoulderPitch_Min,
         LShoulderRoll_Min,
         LElbowYaw_Min,
         LElbowRoll_Min,
         LWristYaw_Min,
         LHand_Min,
         LHipYawPitch_Min,
         LHipRoll_Min,
         LHipPitch_Min,
         LKneePitch_Min,
         LAnklePitch_Min,
         LAnkleRoll_Min,
         RHipRoll_Min,
         RHipPitch_Min,
         RKneePitch_Min,
         RAnklePitch_Min,
         RAnkleRoll_Min,
         RShoulderPitch_Min,
         RShoulderRoll_Min,
         RElbowYaw_Min,
         RElbowRoll_Min,
         RWristYaw_Min,
         RHand_Min
      };
   };


   // Motor speed reductions. These determine the maximum rotational
   // speed of the motors controlling the joint. It is 'highly' advised
   // that when controlling the motors these limits are taken into
   // consideration.  They have been placed in the same namespace as
   // their joint angle counter-parts as it makes sense given the
   // classification.
   // This is calculated as follows: No load speed / reduction ratio
   // / 60, then * 360 to get it into degrees as opposed to rotations.
   // Finally divide by 100 to get it into per 10ms. For example,
   // taking motor 1 type A, 
   // (https://community.aldebaran-robotics.com/doc/1-14/family/nao_h25/motors_h25.html)
   // we get 8300 / 201.3 / 60 * 360 / 100. Since it's not a good idea
   // to push the motors to their limits, (overheating issues etc), we
   // only take 90% of this number, ie 2.23 (to 2 decimal places).

   // Motor limits in Radians
   namespace Radians {
      // Maximum rotation speed (Radians) per cycle (10ms)
      const float MOTOR1_REDUCTIONA_RAD = DEG2RAD(2.23);
      const float MOTOR1_REDUCTIONB_RAD = DEG2RAD(3.43);
      const float MOTOR2_REDUCTIONA_RAD = DEG2RAD(8.96);
      const float MOTOR2_REDUCTIONB_RAD = DEG2RAD(12.52);
      const float MOTOR3_REDUCTIONA_RAD = DEG2RAD(3.85);
      const float MOTOR3_REDUCTIONB_RAD = DEG2RAD(3.34);

      const float HeadPitchSpeed       = MOTOR3_REDUCTIONB_RAD;
      const float HeadYawSpeed         = MOTOR3_REDUCTIONA_RAD;
      const float ShoulderPitchSpeed   = MOTOR3_REDUCTIONA_RAD;
      const float ShoulderRollSpeed    = MOTOR3_REDUCTIONB_RAD;
      const float ElbowYawSpeed        = MOTOR3_REDUCTIONA_RAD;
      const float ElbowRollSpeed       = MOTOR3_REDUCTIONB_RAD;
      const float WristYawSpeed        = MOTOR2_REDUCTIONA_RAD;
      const float HandSpeed            = MOTOR2_REDUCTIONB_RAD;
      const float HipYawPitchSpeed     = MOTOR1_REDUCTIONA_RAD;
      const float HipRollSpeed         = MOTOR1_REDUCTIONA_RAD;
      const float HipPitchSpeed        = MOTOR1_REDUCTIONB_RAD;
      const float KneePitchSpeed       = MOTOR1_REDUCTIONB_RAD;
      const float AnklePitchSpeed      = MOTOR1_REDUCTIONB_RAD;
      const float AnkleRollSpeed       = MOTOR1_REDUCTIONA_RAD;

      const float MaxSpeed[] = {
         HeadYawSpeed,
         HeadPitchSpeed,
         ShoulderPitchSpeed,  // Left arm
         ShoulderRollSpeed,
         ElbowYawSpeed,
         ElbowRollSpeed,
         WristYawSpeed,
         HandSpeed,
         HipYawPitchSpeed,    // Left leg
         HipRollSpeed,
         HipPitchSpeed,
         KneePitchSpeed,
         AnklePitchSpeed,
         AnkleRollSpeed,
         HipYawPitchSpeed,    // Right leg
         HipRollSpeed,
         HipPitchSpeed,
         KneePitchSpeed,
         AnklePitchSpeed,
         AnkleRollSpeed,
         ShoulderPitchSpeed,  // Right arm
         ShoulderRollSpeed,
         ElbowYawSpeed,
         ElbowRollSpeed,
         WristYawSpeed,
         HandSpeed
      };
   };


   /**
    * Given the specified joint, caps it at its limits if the angle exceeds the boundary
    * @param joint Joint to check the angle against
    * @param angle Angle to check (in RADIANS)
    */
   static inline float limitJointRadians(JointCode joint, const float &angle) {
      if (std::isnan(angle)) return 0.0;
      if (angle > Radians::MaxAngle[joint]) return Radians::MaxAngle[joint];
      if (angle < Radians::MinAngle[joint]) return Radians::MinAngle[joint];
      return angle;
   }
};

/**
 * Limb Masses and Lengths
 * Taken from the Naoqi documentation. Limb names as Naoqi uses.
 */

namespace Limbs {
   const float NeckOffsetZ = 126.50;
   const float ShoulderOffsetY = 98.00;
   const float ElbowOffsetY = 15.00;
   const float UpperArmLength = 105.00;
   const float LowerArmLength = 55.95;
   const float ShoulderOffsetZ = 100.00;
   const float HandOffsetX = 57.75;
   const float HandOffsetZ = 12.31;
   const float HipOffsetZ = 85.00;
   const float HipOffsetY = 50.00;
   const float ThighLength = 100.00;
   const float TibiaLength = 102.90;
   const float FootHeight = 45.19;

   const float Length[] = {
      NeckOffsetZ,
      ShoulderOffsetY,
      ElbowOffsetY,
      UpperArmLength,
      LowerArmLength,
      ShoulderOffsetZ,
      HandOffsetX,
      HandOffsetZ,
      HipOffsetZ,
      HipOffsetY,
      ThighLength,
      TibiaLength,
      FootHeight
   };

   const float TorsoMass = 1.04956;
   const float NeckMass = 0.06442; // Head Yaw
   const float HeadMass = 0.60533; // Head Pitch
   const float RightShoulderMass = 0.07504; // R Shoulder Pitch
   const float RightBicepMass = 0.15794; // R Shoulder Roll
   const float RightElbowMass = 0.06483; // R Elbow Yaw
   const float RightForearmMass = 0.07778; // R Elbow Roll
   const float RightHandMass = 0.18533; // R Wrist Yaw
   const float RightPelvisMass = 0.07118; // R Hip Yaw Pitch
   const float RightHipMass = 0.13053; // R Hip Roll
   const float RightThighMass = 0.38976; // R Hip Pitch
   const float RightTibiaMass = 0.29163; // R Knee Pitch
   const float RightAnkleMass = 0.13415; // R Ankle Pitch
   const float RightFootMass = 0.16171; // R Ankle Roll
   const float LeftShoulderMass = 0.07504; // L Shoulder Pitch
   const float LeftBicepMass = 0.15777; // L Shoulder Roll
   const float LeftElbowMass = 0.06483; // L Elbow Yaw
   const float LeftForearmMass = 0.07761; // L Elbow Roll
   const float LeftHandMass = 0.18533; // L Wrist Yaw
   const float LeftPelvisMass = 0.06981; // L Hip Yaw Pitch
   const float LeftHipMass = 0.13053; // L Hip Roll
   const float LeftThighMass = 0.38968; // L Hip Pitch
   const float LeftTibiaMass = 0.29142; // L Knee Pitch
   const float LeftAnkleMass = 0.13416; // L Ankle Pitch
   const float LeftFootMass = 0.16184; // L Ankle Roll

   const float Mass[] = {
      TorsoMass,
      NeckMass,
      HeadMass,
      RightShoulderMass,
      RightBicepMass,
      RightElbowMass,
      RightForearmMass,
      RightHandMass,
      RightPelvisMass,
      RightHipMass,
      RightThighMass,
      RightTibiaMass,
      RightAnkleMass,
      RightFootMass,
      LeftShoulderMass,
      LeftBicepMass,
      LeftElbowMass,
      LeftForearmMass,
      LeftHandMass,
      LeftPelvisMass,
      LeftHipMass,
      LeftThighMass,
      LeftTibiaMass,
      LeftAnkleMass,
      LeftFootMass,
   };

   // Location of centre of masses in mm. Note: these are converted from m
   // so that they match the unit of measurement of the lengths of the links
   // (see above).
   const float TorsoCoM[] = {-4.13, 0.09, 43.42, 1};
   const float NeckCoM[] = {-0.01, 0.14, -27.42, 1};
   const float HeadCoM[] = {-1.12, 0.03, 52.58, 1};
   const float RightShoulderCoM[] = {-1.65, 26.63, 0.14, 1};
   const float RightBicepCoM[] = {24.29, -9.52, 3.20, 1};
   const float RightElbowCoM[] = {-27.44, 0.00, -0.14, 1};
   const float RightForearmCoM[] = {25.52, -2.81, 0.90, 1};
   const float RightHandCoM[] = {34.34, -0.88, 3.08, 1};
   const float RightPelvisCoM[] = {-7.66, 12.00, 27.16, 1};
   const float RightHipCoM[] = {-15.49, -0.29, -5.16, 1};
   const float RightThighCoM[] = {1.39, -2.25, -53.74, 1};
   const float RightTibiaCoM[] = {3.94, -2.21, -49.38, 1};
   const float RightAnkleCoM[] = {0.45, -0.30, 6.84, 1};
   const float RightFootCoM[] = {25.40, -3.32, -32.39, 1};
   const float LeftShoulderCoM[] = {-1.65, -26.63, 0.14, 1};
   const float LeftBicepCoM[] = {24.55, 5.63, 3.30, 1};
   const float LeftElbowCoM[] = {-27.44, 0.00, -0.14, 1};
   const float LeftForearmCoM[] = {25.56, 2.81, 0.76, 1};
   const float LeftHandCoM[] = {34.34, -0.88, 3.08, 1};
   const float LeftPelvisCoM[] = {-7.81, -11.14, 26.61, 1};
   const float LeftHipCoM[] = {-15.49, 0.29, -5.15, 1};
   const float LeftThighCoM[] = {1.38, 2.21, -53.73, 1};
   const float LeftTibiaCoM[] = {4.53, 2.25, -49.36, 1};
   const float LeftAnkleCoM[] = {0.45, 0.29, 6.85, 1};
   const float LeftFootCoM[] = {25.42, 3.30, -32.39, 1};
};

namespace LEDs {
   enum LEDCode {
      LeftEar1 = 0,
      LeftEar2,
      LeftEar3,
      LeftEar4,
      LeftEar5,
      LeftEar6,
      LeftEar7,
      LeftEar8,
      LeftEar9,
      LeftEar10,
      RightEar1,
      RightEar2,
      RightEar3,
      RightEar4,
      RightEar5,
      RightEar6,
      RightEar7,
      RightEar8,
      RightEar9,
      RightEar10,
      LeftEyeRed1,
      LeftEyeRed2,
      LeftEyeRed3,
      LeftEyeRed4,
      LeftEyeRed5,
      LeftEyeRed6,
      LeftEyeRed7,
      LeftEyeRed8,
      LeftEyeGreen1,
      LeftEyeGreen2,
      LeftEyeGreen3,
      LeftEyeGreen4,
      LeftEyeGreen5,
      LeftEyeGreen6,
      LeftEyeGreen7,
      LeftEyeGreen8,
      LeftEyeBlue1,
      LeftEyeBlue2,
      LeftEyeBlue3,
      LeftEyeBlue4,
      LeftEyeBlue5,
      LeftEyeBlue6,
      LeftEyeBlue7,
      LeftEyeBlue8,
      RightEyeRed1,
      RightEyeRed2,
      RightEyeRed3,
      RightEyeRed4,
      RightEyeRed5,
      RightEyeRed6,
      RightEyeRed7,
      RightEyeRed8,
      RightEyeGreen1,
      RightEyeGreen2,
      RightEyeGreen3,
      RightEyeGreen4,
      RightEyeGreen5,
      RightEyeGreen6,
      RightEyeGreen7,
      RightEyeGreen8,
      RightEyeBlue1,
      RightEyeBlue2,
      RightEyeBlue3,
      RightEyeBlue4,
      RightEyeBlue5,
      RightEyeBlue6,
      RightEyeBlue7,
      RightEyeBlue8,
      ChestRed,
      ChestGreen,
      ChestBlue,
      LeftFootRed,
      LeftFootGreen,
      LeftFootBlue,
      RightFootRed,
      RightFootGreen,
      RightFootBlue,
      NUMBER_OF_LEDS
   };

   const std::string ledNames[NUMBER_OF_LEDS] = {
      "Ears/Led/Left/0Deg/Actuator/Value",
      "Ears/Led/Left/36Deg/Actuator/Value",
      "Ears/Led/Left/72Deg/Actuator/Value",
      "Ears/Led/Left/108Deg/Actuator/Value",
      "Ears/Led/Left/144Deg/Actuator/Value",
      "Ears/Led/Left/180Deg/Actuator/Value",
      "Ears/Led/Left/216Deg/Actuator/Value",
      "Ears/Led/Left/252Deg/Actuator/Value",
      "Ears/Led/Left/288Deg/Actuator/Value",
      "Ears/Led/Left/324Deg/Actuator/Value",
      "Ears/Led/Right/0Deg/Actuator/Value",
      "Ears/Led/Right/36Deg/Actuator/Value",
      "Ears/Led/Right/72Deg/Actuator/Value",
      "Ears/Led/Right/108Deg/Actuator/Value",
      "Ears/Led/Right/144Deg/Actuator/Value",
      "Ears/Led/Right/180Deg/Actuator/Value",
      "Ears/Led/Right/216Deg/Actuator/Value",
      "Ears/Led/Right/252Deg/Actuator/Value",
      "Ears/Led/Right/288Deg/Actuator/Value",
      "Ears/Led/Right/324Deg/Actuator/Value",
      "Face/Led/Red/Left/0Deg/Actuator/Value",
      "Face/Led/Red/Left/45Deg/Actuator/Value",
      "Face/Led/Red/Left/90Deg/Actuator/Value",
      "Face/Led/Red/Left/135Deg/Actuator/Value",
      "Face/Led/Red/Left/180Deg/Actuator/Value",
      "Face/Led/Red/Left/225Deg/Actuator/Value",
      "Face/Led/Red/Left/270Deg/Actuator/Value",
      "Face/Led/Red/Left/315Deg/Actuator/Value",
      "Face/Led/Green/Left/0Deg/Actuator/Value",
      "Face/Led/Green/Left/45Deg/Actuator/Value",
      "Face/Led/Green/Left/90Deg/Actuator/Value",
      "Face/Led/Green/Left/135Deg/Actuator/Value",
      "Face/Led/Green/Left/180Deg/Actuator/Value",
      "Face/Led/Green/Left/225Deg/Actuator/Value",
      "Face/Led/Green/Left/270Deg/Actuator/Value",
      "Face/Led/Green/Left/315Deg/Actuator/Value",
      "Face/Led/Blue/Left/0Deg/Actuator/Value",
      "Face/Led/Blue/Left/45Deg/Actuator/Value",
      "Face/Led/Blue/Left/90Deg/Actuator/Value",
      "Face/Led/Blue/Left/135Deg/Actuator/Value",
      "Face/Led/Blue/Left/180Deg/Actuator/Value",
      "Face/Led/Blue/Left/225Deg/Actuator/Value",
      "Face/Led/Blue/Left/270Deg/Actuator/Value",
      "Face/Led/Blue/Left/315Deg/Actuator/Value",
      "Face/Led/Red/Right/0Deg/Actuator/Value",
      "Face/Led/Red/Right/45Deg/Actuator/Value",
      "Face/Led/Red/Right/90Deg/Actuator/Value",
      "Face/Led/Red/Right/135Deg/Actuator/Value",
      "Face/Led/Red/Right/180Deg/Actuator/Value",
      "Face/Led/Red/Right/225Deg/Actuator/Value",
      "Face/Led/Red/Right/270Deg/Actuator/Value",
      "Face/Led/Red/Right/315Deg/Actuator/Value",
      "Face/Led/Green/Right/0Deg/Actuator/Value",
      "Face/Led/Green/Right/45Deg/Actuator/Value",
      "Face/Led/Green/Right/90Deg/Actuator/Value",
      "Face/Led/Green/Right/135Deg/Actuator/Value",
      "Face/Led/Green/Right/180Deg/Actuator/Value",
      "Face/Led/Green/Right/225Deg/Actuator/Value",
      "Face/Led/Green/Right/270Deg/Actuator/Value",
      "Face/Led/Green/Right/315Deg/Actuator/Value",
      "Face/Led/Blue/Right/0Deg/Actuator/Value",
      "Face/Led/Blue/Right/45Deg/Actuator/Value",
      "Face/Led/Blue/Right/90Deg/Actuator/Value",
      "Face/Led/Blue/Right/135Deg/Actuator/Value",
      "Face/Led/Blue/Right/180Deg/Actuator/Value",
      "Face/Led/Blue/Right/225Deg/Actuator/Value",
      "Face/Led/Blue/Right/270Deg/Actuator/Value",
      "Face/Led/Blue/Right/315Deg/Actuator/Value",
      "ChestBoard/Led/Red/Actuator/Value",
      "ChestBoard/Led/Green/Actuator/Value",
      "ChestBoard/Led/Blue/Actuator/Value",
      "LFoot/Led/Red/Actuator/Value",
      "LFoot/Led/Green/Actuator/Value",
      "LFoot/Led/Blue/Actuator/Value",
      "RFoot/Led/Red/Actuator/Value",
      "RFoot/Led/Green/Actuator/Value",
      "RFoot/Led/Blue/Actuator/Value",
   };
};

namespace Sensors {

   /**
    * foot FSR positions relative to ankle frame
    * 1st index specifies left or right, 2nd index specifies which of 4,
    * 3rd index specifies x or y axis
    */
   const float FSR_POS[2][4][2] = {
                                   { { 70.25, 29.9}, { 70.25,-23.1},
                                     {-30.25, 29.9}, {-29.65,-19.1} },
                                   { { 70.25, 23.1}, { 70.25,-29.9},
                                     {-30.25, 19.1}, {-29.65,-29.9} }
                                   };

   typedef enum SensorCodesEnum {
      InertialSensor_AccX = 0,
      InertialSensor_AccY,
      InertialSensor_AccZ,
      InertialSensor_GyrX,
      InertialSensor_GyrY,
      InertialSensor_GyrRef,
      InertialSensor_AngleX,
      InertialSensor_AngleY,

      LFoot_FSR_FrontLeft,
      LFoot_FSR_FrontRight,
      LFoot_FSR_RearLeft,
      LFoot_FSR_RearRight,
      LFoot_FSR_CenterOfPressure_X,
      LFoot_FSR_CenterOfPressure_Y,

      RFoot_FSR_FrontLeft,
      RFoot_FSR_FrontRight,
      RFoot_FSR_RearLeft,
      RFoot_FSR_RearRight,
      RFoot_FSR_CenterOfPressure_X,
      RFoot_FSR_CenterOfPressure_Y,

      LFoot_Bumper_Left,
      LFoot_Bumper_Right,
      RFoot_Bumper_Left,
      RFoot_Bumper_Right,

      ChestBoard_Button,
      Battery_Charge,
      Battery_Current,
      US,

      NUMBER_OF_SENSORS
   } SensorCode;

   const SensorCode sensorCodes[] = {
      InertialSensor_AccX,
      InertialSensor_AccY,
      InertialSensor_AccZ,
      InertialSensor_GyrX,
      InertialSensor_GyrY,
      InertialSensor_GyrRef,
      InertialSensor_AngleX,
      InertialSensor_AngleY,

      LFoot_FSR_FrontLeft,
      LFoot_FSR_FrontRight,
      LFoot_FSR_RearLeft,
      LFoot_FSR_RearRight,
      LFoot_FSR_CenterOfPressure_X,
      LFoot_FSR_CenterOfPressure_Y,

      RFoot_FSR_FrontLeft,
      RFoot_FSR_FrontRight,
      RFoot_FSR_RearLeft,
      RFoot_FSR_RearRight,
      RFoot_FSR_CenterOfPressure_X,
      RFoot_FSR_CenterOfPressure_Y,

      LFoot_Bumper_Left,
      LFoot_Bumper_Right,
      RFoot_Bumper_Left,
      RFoot_Bumper_Right,

      ChestBoard_Button,
      Battery_Charge,
      Battery_Current,
      US
   };

   const std::string sensorNames[] = {
      "InertialSensor/AccX",
      "InertialSensor/AccY",
      "InertialSensor/AccZ",
      "InertialSensor/GyrX",
      "InertialSensor/GyrY",
      "InertialSensor/GyrRef",
      "InertialSensor/AngleX",
      "InertialSensor/AngleY",

      "LFoot/FSR/FrontLeft",
      "LFoot/FSR/FrontRight",
      "LFoot/FSR/RearLeft",
      "LFoot/FSR/RearRight",
      "LFoot/FSR/CenterOfPressure/X",
      "LFoot/FSR/CenterOfPressure/Y",

      "RFoot/FSR/FrontLeft",
      "RFoot/FSR/FrontRight",
      "RFoot/FSR/RearLeft",
      "RFoot/FSR/RearRight",
      "RFoot/FSR/CenterOfPressure/X",
      "RFoot/FSR/CenterOfPressure/Y",

      "LFoot/Bumper/Left",
      "LFoot/Bumper/Right",
      "RFoot/Bumper/Left",
      "RFoot/Bumper/Right",

      "ChestBoard/Button",
      "Battery/Charge",
      "Battery/Current",
      "US"
   };
}

namespace Sonar {

   // From Naoqi docs. Typical use might be something like RBOTH + CONTINUOUS
   namespace Mode {
      const float NO_PING     = -1.f; // This is not from Naoqi docs, and is filtered out so that no command is given
      const float TLRL        = 0.0f; // transmit left, receive on left
      const float TLRR        = 1.0f; // transmit left, receive on right
      const float TRRL        = 2.0f; // transmit right, receive on left
      const float TRRR        = 3.0f; // transmit right, receive on right
      const float RBOTH       = 4.0f; // use both receivers
      const float TBOTH       = 8.0f; // use both transmitters
      const float CONTINUOUS  = 64.0f; // register for periodic capture, then can just read every 100ms
   }

   const float MIN = 0.25f; // min reliable reading  
   const float MAX = 2.5f; // max reliable reading
   const float INVALID = 2.55f;
   const float DISCARD = 10.f;

   const float RIGHT_START = DEG2RAD(-70.f); 
   const float RIGHT_END = DEG2RAD(10.f);
   const float MIDDLE_START = DEG2RAD(-20.f); 
   const float MIDDLE_END = DEG2RAD(20.f);
   const float LEFT_START = DEG2RAD(-10.f);
   const float LEFT_END = DEG2RAD(70.f);

   const int LEFT = 0;
   const int MIDDLE = 1;
   const int RIGHT = 2;
   const unsigned int SIZE = 3;

   const std::string actuatorName = "US/Actuator/Value";

   typedef enum ReadingCodesEnum {
      Left0,
      Left1,
      Left2,
      Left3,
      Left4,
      Left5,
      Left6,
      Left7,
      Left8,
      Left9,
      Right0,
      Right1,
      Right2,
      Right3,
      Right4,
      Right5,
      Right6,
      Right7,
      Right8,
      Right9,
      NUMBER_OF_READINGS
   } ReadingCode;

   const ReadingCode readingCodes[] = {
      Left0,
      Left1,
      Left2,
      Left3,
      Left4,
      Left5,
      Left6,
      Left7,
      Left8,
      Left9,
      Right0,
      Right1,
      Right2,
      Right3,
      Right4,
      Right5,
      Right6,
      Right7,
      Right8,
      Right9
   };

   const std::string readingNames[] = {
      "US/Left/Sensor/Value",
      "US/Left/Sensor/Value1",
      "US/Left/Sensor/Value2",
      "US/Left/Sensor/Value3",
      "US/Left/Sensor/Value4",
      "US/Left/Sensor/Value5",
      "US/Left/Sensor/Value6",
      "US/Left/Sensor/Value7",
      "US/Left/Sensor/Value8",
      "US/Left/Sensor/Value9",
      "US/Right/Sensor/Value",
      "US/Right/Sensor/Value1",
      "US/Right/Sensor/Value2",
      "US/Right/Sensor/Value3",
      "US/Right/Sensor/Value4",
      "US/Right/Sensor/Value5",
      "US/Right/Sensor/Value6",
      "US/Right/Sensor/Value7",
      "US/Right/Sensor/Value8",
      "US/Right/Sensor/Value9"
   };
}
