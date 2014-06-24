/**
* @file HeadMotionEngine.cpp
* This file implements a module that creates head joint angles from desired head motion.
* @author <a href="allli@informatik.uni-bremen.de">Alexander Härtl</a>
* @author Colin Graf
*/

#include <algorithm>
#include "HeadMotionEngine.h"
#include "Tools/Math/Common.h"
#include "Tools/Range.h"
#include "Tools/Debugging/DebugDrawings.h" // PLOT

MAKE_MODULE(HeadMotionEngine, Motion Control)

HeadMotionEngine::HeadMotionEngine()
{
  requestedPan = requestedTilt = JointDataBH::off;
  lastSpeed = Vector2BH<>();

  deathPoints[0] = Geometry::getCircle(
                     Vector2BH<int>(45, -25), Vector2BH<int>(120, -20), Vector2BH<int>(90, -17));
  deathPoints[1] = Geometry::getCircle(
                     Vector2BH<int>(-45, -25), Vector2BH<int>(-120, -20), Vector2BH<int>(-90, -17));
  deathPoints[2] = Geometry::getCircle(
                     Vector2BH<int>(17, 46), Vector2BH<int>(85, 23), Vector2BH<int>(120, 27));
  deathPoints[3] = Geometry::getCircle(
                     Vector2BH<int>(-17, 46), Vector2BH<int>(-85, 23), Vector2BH<int>(-120, 27));

  for(int i = 0; i < 4; ++i)
  {
    deathPoints[i].center.x = fromDegrees(deathPoints[i].center.x);
    deathPoints[i].center.y = fromDegrees(deathPoints[i].center.y);
    deathPoints[i].radius = fromDegrees(deathPoints[i].radius) * 1.015f;
  }
}

void HeadMotionEngine::update(HeadJointRequestBH& headJointRequest)
{
  // update requested angles
  requestedPan = theHeadAngleRequestBH.pan;
  requestedTilt = theHeadAngleRequestBH.tilt;

  //
  float maxAcc = theGroundContactStateBH.contact ? 10.f : 1.f; // arbitrary value that seems to be good...
  MODIFY("module:HeadMotionEngine:maxAcceleration", maxAcc);

  float pan = requestedPan == JointDataBH::off ? JointDataBH::off : RangeBH<>(theJointCalibrationBH.joints[JointDataBH::HeadYaw].minAngle, theJointCalibrationBH.joints[JointDataBH::HeadYaw].maxAngle).limit(requestedPan);
  float tilt = requestedTilt == JointDataBH::off ? JointDataBH::off : RangeBH<>(theJointCalibrationBH.joints[JointDataBH::HeadPitch].minAngle, theJointCalibrationBH.joints[JointDataBH::HeadPitch].maxAngle).limit(requestedTilt);

  const float deltaTime = theFrameInfoBH.cycleTime;
  const Vector2BH<> position(headJointRequest.pan == JointDataBH::off ? theFilteredJointDataBH.angles[JointDataBH::HeadYaw] : headJointRequest.pan,
                           headJointRequest.tilt == JointDataBH::off ? theFilteredJointDataBH.angles[JointDataBH::HeadPitch] : headJointRequest.tilt);
  const Vector2BH<> target(pan == JointDataBH::off ? 0 : pan, tilt == JointDataBH::off ? 0 : tilt);
  Vector2BH<> offset(target - position);
  const float distanceToTarget = offset.abs();

  // calculate max speed
  const float maxSpeedForDistance = std::sqrt(2.f * distanceToTarget * maxAcc * 0.8f);
  const float maxSpeed = std::min(maxSpeedForDistance, theHeadAngleRequestBH.speed);

  // max speed clipping
  if(distanceToTarget / deltaTime > maxSpeed)
    offset *= maxSpeed * deltaTime / distanceToTarget; //<=> offset.normalizeBH(maxSpeed * deltaTime);

  // max acceleration clipping
  Vector2BH<> speed(offset / deltaTime);
  Vector2BH<> acc((speed - lastSpeed) / deltaTime);
  const float accSquareAbs = acc.squareAbs();
  if(accSquareAbs > maxAcc * maxAcc)
  {
    acc *= maxAcc * deltaTime / std::sqrt(accSquareAbs);
    speed = acc + lastSpeed;
    offset = speed * deltaTime;
  }
  /* <=>
  Vector2BH<> speed(offset / deltaTime);
  Vector2BH<> acc((speed - lastSpeed) / deltaTime);
  if(acc.squareAbs() > maxAcc * maxAcc)
  {
    speed = acc.normalizeBH(maxAcc * deltaTime) + lastSpeed;
    offset = speed * deltaTime;
  }
  */
  PLOT("module:HeadMotionEngine:speed", toDegrees(speed.abs()));

  // calculate new position
  Vector2BH<> newPosition(position + offset);

  // make sure we don't get to close to the evil points of death
  if(pan != JointDataBH::off && tilt != JointDataBH::off)
    for(int i = 0; i < 4; ++i)
    {
      Vector2BH<> deathPointToPosition(newPosition - deathPoints[i].center);
      const float deathPointToPositionSquareAbs = deathPointToPosition.squareAbs();
      if(deathPointToPositionSquareAbs != 0.f && deathPointToPositionSquareAbs < sqrBH(deathPoints[i].radius))
      {
        const float deathPointToPositionAbs = std::sqrt(deathPointToPositionSquareAbs);
        deathPointToPosition *= (deathPoints[i].radius - deathPointToPositionAbs) / deathPointToPositionAbs;
        newPosition += deathPointToPosition;
      }
    }

  // set new position
  headJointRequest.pan = pan == JointDataBH::off ? JointDataBH::off : newPosition.x;
  headJointRequest.tilt = tilt == JointDataBH::off ? JointDataBH::off : newPosition.y;
  headJointRequest.moving = pan != JointDataBH::off && tilt != JointDataBH::off && ((newPosition - position) / deltaTime).squareAbs() > sqrBH(maxAcc * deltaTime * 0.5f);

  // check reachability
  headJointRequest.reachable = true;
  if(pan != requestedPan || tilt != requestedTilt)
    headJointRequest.reachable = false;
  else
    for(int i = 0; i < 4; ++i)
      if((target - deathPoints[i].center).squareAbs() < sqrBH(deathPoints[i].radius))
        headJointRequest.reachable = false;

  // store some values for the next iteration
  lastSpeed = speed;
}
