/**
* @file Representations/Infrastructure/JointData.h
*
* This file declares a classes to represent the joint angles.
*
* @author <A href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</A>
*/

#pragma once

#include "Tools/Math/Common.h"
#include "Tools/Streams/AutoStreamable.h"
#include "Tools/Enum.h"

/**
* @class JointDataBH
* A class to represent the joint angles sent to the robot.
*/
STREAMABLE(JointDataBH,
{
public:
  ENUM(Joint,
    HeadYaw,
    HeadPitch,
    LShoulderPitch,
    LShoulderRoll,
    LElbowYaw,
    LElbowRoll,
    RShoulderPitch,
    RShoulderRoll,
    RElbowYaw,
    RElbowRoll,
    LHipYawPitch,
    LHipRoll,
    LHipPitch,
    LKneePitch,
    LAnklePitch,
    LAnkleRoll,
    RHipYawPitch,
    RHipRoll,
    RHipPitch,
    RKneePitch,
    RAnklePitch,
    RAnkleRoll
  );

  // If you change those values be sure to change them in MofCompiler.cpp too. (Line ~280)
  enum {off = 1000}; /**< Special angle for switching off a joint. */
  enum {ignore = 2000}; /**< Special angle for not overwriting the previous setting. */

  /**
  * The method returns the angle of the mirror (left/right) of the given joint.
  * @param joint The joint the mirror of which is returned.
  * @return The angle of the mirrored joint.
  */
  float mirror(Joint joint) const
  {
    switch(joint)
    {
      // don't mirror an invalid joint value (!)
    case HeadYaw:
      return angles[HeadYaw] == off || angles[HeadYaw] == ignore ? angles[HeadYaw] : -angles[HeadYaw];
    case LShoulderPitch:
      return angles[RShoulderPitch];
    case LShoulderRoll:
      return angles[RShoulderRoll];
    case LElbowYaw:
      return angles[RElbowYaw];
    case LElbowRoll:
      return angles[RElbowRoll];
    case RShoulderPitch:
      return angles[LShoulderPitch];
    case RShoulderRoll:
      return angles[LShoulderRoll];
    case RElbowYaw:
      return angles[LElbowYaw];
    case RElbowRoll:
      return angles[LElbowRoll];
    case LHipYawPitch:
      return angles[RHipYawPitch];
    case LHipRoll:
      return angles[RHipRoll];
    case LHipPitch:
      return angles[RHipPitch];
    case LKneePitch:
      return angles[RKneePitch];
    case LAnklePitch:
      return angles[RAnklePitch];
    case LAnkleRoll:
      return angles[RAnkleRoll];
    case RHipYawPitch:
      return angles[LHipYawPitch];
    case RHipRoll:
      return angles[LHipRoll];
    case RHipPitch:
      return angles[LHipPitch];
    case RKneePitch:
      return angles[LKneePitch];
    case RAnklePitch:
      return angles[LAnklePitch];
    case RAnkleRoll:
      return angles[LAnkleRoll];
    default:
      return angles[joint];
    }
  }

  /**
  * The method initializes the joint angles as a mirror of a set of other joint angles.
  * @param other The set of joint angles that are mirrored.
  */
  void mirror(const JointDataBH& other)
  {
    for(int i = 0; i < numOfJoints; ++i)
      angles[i] = other.mirror((Joint) i);
    timeStamp = other.timeStamp;
  },

  (float[numOfJoints]) angles, /**< The angles of all joints. */
  (unsigned)(0) timeStamp, /**< The time when these angles were received. */

  // Initialization
  for(int i = 0; i < numOfJoints; ++i)
    angles[i] = off;
});


/**
 * @class HardnessData
 * This class represents the joint hardness in a jointRequest.
 * It loads the default hardness values from hardnessSettings.cfg.
 */
STREAMABLE(HardnessData,
{
public:
  enum {useDefault = -1};

  /**
  * The method returns the hardness of the mirror (left/right) of the given joint.
  * @param joint The joint the mirror of which is returned.
  * @return The output hardness of the mirrored joint.
  */
  int mirror(const JointDataBH::Joint joint) const
  {
    switch(joint)
    {
    case JointDataBH::HeadYaw:
      return hardness[JointDataBH::HeadYaw];
    case JointDataBH::LShoulderPitch:
      return hardness[JointDataBH::RShoulderPitch];
    case JointDataBH::LShoulderRoll:
      return hardness[JointDataBH::RShoulderRoll];
    case JointDataBH::LElbowYaw:
      return hardness[JointDataBH::RElbowYaw];
    case JointDataBH::LElbowRoll:
      return hardness[JointDataBH::RElbowRoll];
    case JointDataBH::RShoulderPitch:
      return hardness[JointDataBH::LShoulderPitch];
    case JointDataBH::RShoulderRoll:
      return hardness[JointDataBH::LShoulderRoll];
    case JointDataBH::RElbowYaw:
      return hardness[JointDataBH::LElbowYaw];
    case JointDataBH::RElbowRoll:
      return hardness[JointDataBH::LElbowRoll];
    case JointDataBH::LHipYawPitch:
      return hardness[JointDataBH::RHipYawPitch];
    case JointDataBH::LHipRoll:
      return hardness[JointDataBH::RHipRoll];
    case JointDataBH::LHipPitch:
      return hardness[JointDataBH::RHipPitch];
    case JointDataBH::LKneePitch:
      return hardness[JointDataBH::RKneePitch];
    case JointDataBH::LAnklePitch:
      return hardness[JointDataBH::RAnklePitch];
    case JointDataBH::LAnkleRoll:
      return hardness[JointDataBH::RAnkleRoll];
    case JointDataBH::RHipYawPitch:
      return hardness[JointDataBH::LHipYawPitch];
    case JointDataBH::RHipRoll:
      return hardness[JointDataBH::LHipRoll];
    case JointDataBH::RHipPitch:
      return hardness[JointDataBH::LHipPitch];
    case JointDataBH::RKneePitch:
      return hardness[JointDataBH::LKneePitch];
    case JointDataBH::RAnklePitch:
      return hardness[JointDataBH::LAnklePitch];
    case JointDataBH::RAnkleRoll:
      return hardness[JointDataBH::LAnkleRoll];
    default:
      return hardness[joint];
    }
  }

  /**
   * initializes this instance with the mirrored values of other
   * @param other the HardnessData to be mirrored
   */
  void mirror(const HardnessData& other)
  {
    for(int i = 0; i < JointDataBH::numOfJoints; ++i)
      hardness[i] = other.mirror((JointDataBH::Joint)i);
  }

  /**
   * This function resets the hardness for all joints to the default value.
   */
  inline void resetToDefault()
  {
    for(int i = 0; i < JointDataBH::numOfJoints; ++i)
      hardness[i] = useDefault;
  },

  (int[JointDataBH::numOfJoints]) hardness, /**< the custom hardness for each joint */

  // Initialization
  resetToDefault();
});

class HardnessSettingsBH : public HardnessData {};

/**
 * @class JointRequestBH
 */
STREAMABLE_WITH_BASE(JointRequestBH, JointDataBH,
{
public:
  /**
   * Initializes this instance with the mirrored data from a other JointRequestBH
   * @param other the JointRequestBH to be mirrored
   */
  void mirror(const JointRequestBH& other)
  {
    JointDataBH::mirror(other);
    jointHardness.mirror(other.jointHardness);
  }

  /**
   * Returns the mirrored angle of joint
   * @param joint the joint to be mirrored
   */
  float mirror(const JointDataBH::Joint joint)
  {
    return JointDataBH::mirror(joint);
  }

  bool isValid() const
  {
    for(int i = 0; i < numOfJoints; ++i)
      if(isnan(angles[i]) || jointHardness.hardness[i] < 0 || jointHardness.hardness[i] > 100)
        return false;
    return true;
  },

  (HardnessData) jointHardness, /**< the hardness for all joints */
});

class FilteredJointDataBH : public JointDataBH {};
