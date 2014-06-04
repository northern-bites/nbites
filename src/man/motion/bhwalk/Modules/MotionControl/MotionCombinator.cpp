/**
* @file Modules/MotionControl/MotionCombinator.cpp
* This file implements a module that combines the motions created by the different modules.
* @author <A href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</A>
*/

#include "MotionCombinator.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/Streams/InStreams.h"
#include "Representations/Infrastructure/JointDataDeg.h"

using std::abs;

MotionCombinator::MotionCombinator()
{
  currentRecoveryTime = recoveryTime + 1;
  headJawInSavePosition = false;
  headPitchInSavePosition = false;
}

void MotionCombinator::update(JointRequestBH& jointRequest)
{
  specialActionOdometry += theSpecialActionsOutputBH.odometryOffset;

  const JointRequestBH* jointRequests[MotionRequestBH::numOfMotions];
  jointRequests[MotionRequestBH::walk] = &theWalkingEngineOutputBH;
  jointRequests[MotionRequestBH::bike] = &theBikeEngineOutputBH;
  jointRequests[MotionRequestBH::indykick] = &theIndykickEngineOutputBH;
  jointRequests[MotionRequestBH::specialAction] = &theSpecialActionsOutputBH;
  jointRequests[MotionRequestBH::stand] = &theWalkingEngineStandOutputBH;
  jointRequests[MotionRequestBH::getUp] = &theGetUpEngineOutputBH;
  jointRequests[MotionRequestBH::takeBall] = &theBallTakingOutputBH;

  jointRequest.angles[JointDataBH::HeadYaw] = theHeadJointRequestBH.pan;
  jointRequest.angles[JointDataBH::HeadPitch] = theHeadJointRequestBH.tilt;

  copy(*jointRequests[theMotionSelectionBH.targetMotion], jointRequest);

  int i;
  for(i = 0; i < MotionRequestBH::numOfMotions; ++i)
    if(theMotionSelectionBH.ratios[i] == 1.f)
    {
      // default values
      motionInfo.motion = MotionRequestBH::Motion(i);
      motionInfo.isMotionStable = true;
      motionInfo.upcomingOdometryOffset = Pose2DBH();

      lastJointData = theFilteredJointDataBH;

      if(theMotionSelectionBH.ratios[MotionRequestBH::walk] == 1.f)
      {
        odometryData += theWalkingEngineOutputBH.odometryOffset;
        motionInfo.walkRequest = theWalkingEngineOutputBH.executedWalk;
        motionInfo.upcomingOdometryOffset = theWalkingEngineOutputBH.upcomingOdometryOffset;
      }
      else if(theMotionSelectionBH.ratios[MotionRequestBH::bike] == 1.f)
      {
        odometryData += theBikeEngineOutputBH.odometryOffset;
        motionInfo.bikeRequest = theBikeEngineOutputBH.executedBikeRequest;
        motionInfo.isMotionStable = theBikeEngineOutputBH.isStable;
      }
      else if(theMotionSelectionBH.ratios[MotionRequestBH::indykick] == 1.0f)
      {
        motionInfo.indykickRequest = theIndykickEngineOutputBH.executedIndykickRequest;
      }
      else if(theMotionSelectionBH.ratios[MotionRequestBH::specialAction] == 1.0f)
      {
        odometryData += specialActionOdometry;
        specialActionOdometry = Pose2DBH();
        motionInfo.specialActionRequest = theSpecialActionsOutputBH.executedSpecialAction;
        motionInfo.isMotionStable = theSpecialActionsOutputBH.isMotionStable;
      }
      else if(theMotionSelectionBH.ratios[MotionRequestBH::stand] == 1.f)
      {
        motionInfo.motion = MotionRequestBH::stand;
      }
      else if(theMotionSelectionBH.ratios[MotionRequestBH::getUp] == 1.f)
      {
        motionInfo.motion = MotionRequestBH::getUp;
        motionInfo.isMotionStable = false;
        odometryData += theGetUpEngineOutputBH.odometryOffset;
      }
      else if(theMotionSelectionBH.ratios[MotionRequestBH::takeBall] == 1.f)
      {
        motionInfo.motion = MotionRequestBH::takeBall;
        motionInfo.isMotionStable = true;
        odometryData += theBallTakingOutputBH.odometryOffset;
      }
      break;
    }

  if(i == MotionRequestBH::numOfMotions)
  {
    for(i = 0; i < MotionRequestBH::numOfMotions; ++i)
      if(i != theMotionSelectionBH.targetMotion && theMotionSelectionBH.ratios[i] > 0.)
      {
        bool interpolateHardness = !(i == MotionRequestBH::specialAction && theMotionSelectionBH.specialActionRequest.specialAction == SpecialActionRequest::playDead); // do not interpolate from play_dead
        interpolate(*jointRequests[i], *jointRequests[theMotionSelectionBH.targetMotion], theMotionSelectionBH.ratios[i], jointRequest, interpolateHardness);
      }
  }

  if(emergencyOffEnabled)
  {
    if(theFallDownStateBH.state == FallDownStateBH::falling && motionInfo.motion != MotionRequestBH::specialAction)
    {
      saveFall(jointRequest);
      centerHead(jointRequest);
      currentRecoveryTime = 0;
    }
    else if((theFallDownStateBH.state == FallDownStateBH::staggering || theFallDownStateBH.state == FallDownStateBH::onGround) && (motionInfo.motion != MotionRequestBH::specialAction))
    {
      centerHead(jointRequest);
    }
    else
    {
      if(theFallDownStateBH.state == FallDownStateBH::upright)
      {
        headJawInSavePosition = false;
        headPitchInSavePosition = false;
      }

      if(currentRecoveryTime < recoveryTime)
      {
        currentRecoveryTime += 1;
        float ratio = (1.f / float(recoveryTime)) * currentRecoveryTime;
        for(int i = 0; i < JointDataBH::numOfJoints; i ++)
        {
          jointRequest.jointHardness.hardness[i] = 30 + int (ratio * float(jointRequest.jointHardness.hardness[i] - 30));
        }
      }
    }
  }

#ifndef RELEASE
  float sum(0);
  int count(0);
  for(int i = JointDataBH::LHipYawPitch; i < JointDataBH::numOfJoints; i++)
  {
    if(jointRequest.angles[i] != JointDataBH::off && jointRequest.angles[i] != JointDataBH::ignore && lastJointRequest.angles[i] != JointDataBH::off && lastJointRequest.angles[i] != JointDataBH::ignore)
    {
      sum += abs(jointRequest.angles[i] - lastJointRequest.angles[i]);
      count++;
    }
  }
  PLOT("module:MotionCombinator:deviations:JointRequestBH:legsOnly", sum / count);
  for(int i = 0; i < JointDataBH::LHipYawPitch; i++)
  {
    if(jointRequest.angles[i] != JointDataBH::off && jointRequest.angles[i] != JointDataBH::ignore && lastJointRequest.angles[i] != JointDataBH::off && lastJointRequest.angles[i] != JointDataBH::ignore)
    {
      sum += abs(jointRequest.angles[i] - lastJointRequest.angles[i]);
      count++;
    }
  }
  PLOT("module:MotionCombinator:deviations:JointRequestBH:all", sum / count);

  sum = 0;
  count = 0;
  for(int i = JointDataBH::LHipYawPitch; i < JointDataBH::numOfJoints; i++)
  {
    if(lastJointRequest.angles[i] != JointDataBH::off && lastJointRequest.angles[i] != JointDataBH::ignore)
    {
      sum += abs(lastJointRequest.angles[i] - theFilteredJointDataBH.angles[i]);
      count++;
    }
  }
  PLOT("module:MotionCombinator:differenceToJointData:legsOnly", sum / count);

  for(int i = 0; i < JointDataBH::LHipYawPitch; i++)
  {
    if(lastJointRequest.angles[i] != JointDataBH::off && lastJointRequest.angles[i] != JointDataBH::ignore)
    {
      sum += abs(lastJointRequest.angles[i] - theFilteredJointDataBH.angles[i]);
      count++;
    }
  }
  lastJointRequest = jointRequest;
  PLOT("module:MotionCombinator:differenceToJointData:all", sum / count);
#endif

#ifndef NDEBUG
  if(!jointRequest.isValid())
  {
    {
      OutMapFile stream("jointRequest.log");
      stream << jointRequest;
      OutMapFile stream2("motionSelection.log");
      stream2 << theMotionSelectionBH;
    }
    ASSERT(false);
  }
#endif

#ifndef RELEASE
  JointDataDeg jointRequestDeg(jointRequest);
#endif
  MODIFY("representation:JointRequestDeg", jointRequestDeg);
}

void MotionCombinator::copy(const JointRequestBH& source, JointRequestBH& target) const
{
  for(int i = 0; i < JointDataBH::numOfJoints; ++i)
  {
    if(source.angles[i] != JointDataBH::ignore)
      target.angles[i] = source.angles[i];
    target.jointHardness.hardness[i] = source.angles[i] != JointDataBH::off ? source.jointHardness.hardness[i] : 0;
    if(target.jointHardness.hardness[i] == HardnessData::useDefault)
      target.jointHardness.hardness[i] = theHardnessSettings.hardness[i];
  }
}

void MotionCombinator::interpolate(const JointRequestBH& from, const JointRequestBH& to,
                                   float fromRatio, JointRequestBH& target, bool interpolateHardness) const
{
  for(int i = 0; i < JointDataBH::numOfJoints; ++i)
  {
    float f = from.angles[i];
    float t = to.angles[i];

    if(t == JointDataBH::ignore && f == JointDataBH::ignore)
      continue;

    if(t == JointDataBH::ignore)
      t = target.angles[i];
    if(f == JointDataBH::ignore)
      f = target.angles[i];

    int fHardness = f != JointDataBH::off ? from.jointHardness.hardness[i] : 0;
    int tHardness = t != JointDataBH::off ? to.jointHardness.hardness[i] : 0;
    if(fHardness == HardnessData::useDefault)
      fHardness = theHardnessSettings.hardness[i];
    if(tHardness == HardnessData::useDefault)
      tHardness = theHardnessSettings.hardness[i];

    if(t == JointDataBH::off || t == JointDataBH::ignore)
      t = lastJointData.angles[i];
    if(f == JointDataBH::off || f == JointDataBH::ignore)
      f = lastJointData.angles[i];
    if(target.angles[i] == JointDataBH::off || target.angles[i] == JointDataBH::ignore)
      target.angles[i] = lastJointData.angles[i];

    ASSERT(target.angles[i] != JointDataBH::off && target.angles[i] != JointDataBH::ignore);
    ASSERT(t != JointDataBH::off && t != JointDataBH::ignore);
    ASSERT(f != JointDataBH::off && f != JointDataBH::ignore);

    target.angles[i] += -fromRatio * t + fromRatio * f;
    if(interpolateHardness)
      target.jointHardness.hardness[i] += int(-fromRatio * float(tHardness) + fromRatio * float(fHardness));
    else
      target.jointHardness.hardness[i] = tHardness;
  }
}

void MotionCombinator::update(OdometryDataBH& odometryData)
{
  this->odometryData.rotation += theFallDownStateBH.odometryRotationOffset;
  this->odometryData.rotation = normalizeBH(this->odometryData.rotation);

  odometryData = this->odometryData;

#ifndef RELEASE
  Pose2DBH odometryOffset(odometryData);
  odometryOffset -= lastOdometryData;
  PLOT("module:MotionCombinator:odometryOffsetX", odometryOffset.translation.x);
  PLOT("module:MotionCombinator:odometryOffsetY", odometryOffset.translation.y);
  PLOT("module:MotionCombinator:odometryOffsetRotation", toDegrees(odometryOffset.rotation));
  lastOdometryData = odometryData;
#endif
}

void MotionCombinator::saveFall(JointRequestBH& jointRequest)
{
  for(int i = 0; i < JointDataBH::numOfJoints; i++)
    jointRequest.jointHardness.hardness[i] = 30;

  // if ARME is moving the arms, do not lower hardness to ensure motion can be finished
  if (theArmMotionEngineOutputBH.arms[ArmMotionRequestBH::left].move)
  {
    for(int i = 0; i < 4; ++i)
      jointRequest.jointHardness.hardness[JointDataBH::LShoulderPitch + i] = theArmMotionEngineOutputBH.arms[ArmMotionRequestBH::left].hardness[i];
  }
  if (theArmMotionEngineOutputBH.arms[ArmMotionRequestBH::right].move)
  {
    for(int i = 0; i < 4; ++i)
      jointRequest.jointHardness.hardness[JointDataBH::RShoulderPitch + i] = theArmMotionEngineOutputBH.arms[ArmMotionRequestBH::right].hardness[i];
  }
}

void MotionCombinator::centerHead(JointRequestBH& jointRequest)
{
  jointRequest.angles[JointDataBH::HeadYaw] = 0;
  jointRequest.angles[JointDataBH::HeadPitch] = 0;
  if(theFallDownStateBH.direction == FallDownStateBH::front)
    jointRequest.angles[JointDataBH::HeadPitch] = 0.4f;
  else if(theFallDownStateBH.direction == FallDownStateBH::back)
    jointRequest.angles[JointDataBH::HeadPitch] = -0.3f;
  if(abs(theFilteredJointDataBH.angles[JointDataBH::HeadYaw]) > 0.1f && !headJawInSavePosition)
    jointRequest.jointHardness.hardness[JointDataBH::HeadYaw] = 100;
  else
  {
    headJawInSavePosition = true;
    jointRequest.jointHardness.hardness[JointDataBH::HeadYaw] = 25;
  }
  if(abs(theFilteredJointDataBH.angles[JointDataBH::HeadPitch] - jointRequest.angles[JointDataBH::HeadPitch]) > 0.1f && !headPitchInSavePosition)
    jointRequest.jointHardness.hardness[JointDataBH::HeadPitch] = 100;
  else
  {
    headPitchInSavePosition = true;
    jointRequest.jointHardness.hardness[JointDataBH::HeadPitch] = 25;
  }
}

MAKE_MODULE(MotionCombinator, Motion Control)
