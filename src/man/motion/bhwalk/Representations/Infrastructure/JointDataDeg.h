/**
* @file Representations/Infrastructure/JointDataDeg.h
* This file declares a class to represent the joint angles in degrees.
*/

#pragma once

#include "JointData.h"
#include "Tools/Math/Common.h"
#include "Platform/BHAssert.h"

/**
* @class JointDataDeg
* A class that wraps joint data to be transmitted in degrees.
*/
class JointDataDeg : public Streamable
{
private:
  virtual void serialize(In* in, Out* out)
  {
    STREAM_REGISTER_BEGIN;
    float angles[JointDataBH::numOfJoints];
    unsigned& timeStamp = jointData.timeStamp;
    if(out)
      for(int i = 0; i < JointDataBH::numOfJoints; ++i)
        angles[i] = jointData.angles[i] == JointDataBH::off ? JointDataBH::off
                    : std::floor(toDegrees(jointData.angles[i]) * 10.0f + 0.5f) / 10.0f;
    STREAM(angles);
    STREAM(timeStamp);
    if(in)
      for(int i = 0; i < JointDataBH::numOfJoints; ++i)
        jointData.angles[i] = angles[i] == JointDataBH::off ? JointDataBH::off : fromDegrees(angles[i]);
    STREAM_REGISTER_FINISH;
  }

  JointDataBH& jointData; /**< The joint data that is wrapped. */

public:
  /**
  * Constructor.
  * @param jointData The joint data that is wrapped.
  */
  JointDataDeg(JointDataBH& jointData) : jointData(jointData) {}
};
