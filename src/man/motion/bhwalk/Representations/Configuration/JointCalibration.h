/**
* @file JointCalibration.h
* Declaration of a class for representing the calibration values of joints.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#pragma once

#include "Representations/Infrastructure/JointData.h"
#include "Tools/Math/Common.h"

class JointCalibrationBH : public Streamable
{
private:
  virtual void serialize(In* in, Out* out)
  {
    JointInfo& headYaw = joints[JointDataBH::HeadYaw];
    JointInfo& headPitch = joints[JointDataBH::HeadPitch];
    JointInfo& lShoulderPitch = joints[JointDataBH::LShoulderPitch];
    JointInfo& lShoulderRoll = joints[JointDataBH::LShoulderRoll];
    JointInfo& lElbowYaw = joints[JointDataBH::LElbowYaw];
    JointInfo& lElbowRoll = joints[JointDataBH::LElbowRoll];
    JointInfo& rShoulderPitch = joints[JointDataBH::RShoulderPitch];
    JointInfo& rShoulderRoll = joints[JointDataBH::RShoulderRoll];
    JointInfo& rElbowYaw = joints[JointDataBH::RElbowYaw];
    JointInfo& rElbowRoll = joints[JointDataBH::RElbowRoll];
    JointInfo& lHipYawPitch = joints[JointDataBH::LHipYawPitch];
    JointInfo& lHipRoll = joints[JointDataBH::LHipRoll];
    JointInfo& lHipPitch = joints[JointDataBH::LHipPitch];
    JointInfo& lKneePitch = joints[JointDataBH::LKneePitch];
    JointInfo& lAnklePitch = joints[JointDataBH::LAnklePitch];
    JointInfo& lAnkleRoll = joints[JointDataBH::LAnkleRoll];
    JointInfo& rHipYawPitch = joints[JointDataBH::RHipYawPitch];
    JointInfo& rHipRoll = joints[JointDataBH::RHipRoll];
    JointInfo& rHipPitch = joints[JointDataBH::RHipPitch];
    JointInfo& rKneePitch = joints[JointDataBH::RKneePitch];
    JointInfo& rAnklePitch = joints[JointDataBH::RAnklePitch];
    JointInfo& rAnkleRoll = joints[JointDataBH::RAnkleRoll];

    STREAM_REGISTER_BEGIN;
    STREAM(headYaw)
    STREAM(headPitch)
    STREAM(lShoulderPitch)
    STREAM(lShoulderRoll)
    STREAM(lElbowYaw)
    STREAM(lElbowRoll)
    STREAM(rShoulderPitch)
    STREAM(rShoulderRoll)
    STREAM(rElbowYaw)
    STREAM(rElbowRoll)
    STREAM(lHipYawPitch)
    STREAM(lHipRoll)
    STREAM(lHipPitch)
    STREAM(lKneePitch)
    STREAM(lAnklePitch)
    STREAM(lAnkleRoll)
    STREAM(rHipYawPitch)
    STREAM(rHipRoll)
    STREAM(rHipPitch)
    STREAM(rKneePitch)
    STREAM(rAnklePitch)
    STREAM(rAnkleRoll)
    STREAM_REGISTER_FINISH
  }

public:
  class JointInfo : public Streamable
  {
  private:
    virtual void serialize(In* in, Out* out)
    {
      float offset = toDegrees(this->offset);
      float maxAngle = toDegrees(this->maxAngle);
      float minAngle = toDegrees(this->minAngle);

      STREAM_REGISTER_BEGIN;
      STREAM(offset);
      STREAM(sign);
      STREAM(minAngle);
      STREAM(maxAngle);
      STREAM_REGISTER_FINISH

      if(in)
      {
        this->offset = fromDegrees(offset);
        this->minAngle = fromDegrees(minAngle);
        this->maxAngle = fromDegrees(maxAngle);
      }
    }

  public:
    float offset; /**< An offset added to the angle. */
    short sign; /**< A multiplier for the angle (1 or -1). */
    float maxAngle; /** the maximal angle in radians */
    float minAngle;  /** the minmal angle in radians */

    /**
    * Default constructor.
    */
    JointInfo() : offset(0), sign(1), maxAngle(2.618f), minAngle(-2.618f) {}
  };

  JointInfo joints[JointDataBH::numOfJoints]; /**< Information on the calibration of all joints. */
};
