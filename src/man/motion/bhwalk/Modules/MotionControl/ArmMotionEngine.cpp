#include "ArmMotionEngine.h"

MAKE_MODULE(ArmMotionEngine, Motion Control)

ArmMotionEngine::ArmMotionEngine() : ArmMotionEngineBase()
{
  arms[ArmMotionRequestBH::left] = Arm(ArmMotionRequestBH::left, JointDataBH::LShoulderPitch);
  arms[ArmMotionRequestBH::right] = Arm(ArmMotionRequestBH::right, JointDataBH::RShoulderPitch);

  ASSERT(allMotions[ArmMotionRequestBH::useDefault].states.size() == 1);
  defaultPos = allMotions[ArmMotionRequestBH::useDefault].states[0];
}

void ArmMotionEngine::update(ArmMotionEngineOutputBH& armMotionEngineOutput)
{
  updateArm(arms[ArmMotionRequestBH::left], armMotionEngineOutput);
  updateArm(arms[ArmMotionRequestBH::right], armMotionEngineOutput);
}

bool ArmMotionEngine::newMotionPossible(Arm& arm)
{
  return !arm.isMotionActive &&
    (theGameInfoBH.state == STATE_PLAYING || theGameInfoBH.state == STATE_READY) &&
    (theMotionInfoBH.motion == MotionInfoBH::walk || theMotionInfoBH.motion == MotionInfoBH::stand) &&
    theGroundContactStateBH.contact;
}

void ArmMotionEngine::updateArm(Arm& arm, ArmMotionEngineOutputBH& armMotionEngineOutput)
{
  // assume arm should not be moved.
  armMotionEngineOutput.arms[arm.id].move = false;

  // make sure to not interfere with other motion engines
  if(theFallDownStateBH.state == FallDownStateBH::onGround ||
    theMotionInfoBH.motion == MotionInfoBH::getUp)
  {
    arm.isMotionActive = false;
    return;
  }

  if(newMotionPossible(arm))
  {
    // no motion is active, so decide what to do.
    // behavior overrides contact triggered motions
    if(theArmMotionRequestBH.motion[arm.id] != ArmMotionRequestBH::useDefault)
    {
      ArmMotion nextMotion = allMotions[theArmMotionRequestBH.motion[arm.id]];
      arm.contactTriggered = false;
      arm.startMotion(nextMotion,
        theArmMotionRequestBH.fast[arm.id],
        theArmMotionRequestBH.autoReverse[arm.id],
        theArmMotionRequestBH.autoReverseTime[arm.id],
        theFilteredJointDataBH);

    } else if(theFrameInfoBH.getTimeSince(arm.lastContactAction) > actionDelay &&
      (arm.id == ArmMotionRequestBH::left ? theArmContactModelBH.contactLeft : theArmContactModelBH.contactRight))
    {
#ifdef TARGET_ROBOT
      // check for armcontact
      ArmContactModelBH::PushDirection dir = (arm.id == ArmMotionRequestBH::left)
        ? theArmContactModelBH.pushDirectionLeft
        : theArmContactModelBH.pushDirectionRight;

      switch(dir)
      {
      case ArmContactModelBH::S:
      case ArmContactModelBH::SW:
      case ArmContactModelBH::SE:
        arm.lastContactAction = theFrameInfoBH.time;
        arm.contactTriggered = true;
        arm.startMotion(allMotions[ArmMotionRequestBH::back], false, true, targetTime, theFilteredJointDataBH);
        break;
      default:
        break;
      }
#endif
    }
  }

  // when falling, try to set emergency mode fast
  if(theFallDownStateBH.state == FallDownStateBH::falling && arm.isMotionActive && arm.currentMotion.id != ArmMotionRequestBH::falling)
  {
    arm.startMotion(allMotions[ArmMotionRequestBH::falling], true, false, 0, theFilteredJointDataBH);
  }
  else if((theFallDownStateBH.state == FallDownStateBH::falling || theFallDownStateBH.state == FallDownStateBH::onGround || !theGroundContactStateBH.contact) &&
    !arm.isMotionActive)
  {
    // fallen
    return;
  }
  else if(theMotionInfoBH.motion == MotionInfoBH::bike && arm.isMotionActive && arm.currentMotion.id != ArmMotionRequestBH::useDefault)
  {
    arm.startMotion(allMotions[ArmMotionRequestBH::useDefault], true, false, 0, theFilteredJointDataBH);
  }

  // test whether a motion is active now
  if(arm.isMotionActive)
  {
    // a motion is active, so decide what to do

    if(arm.stateIndex == arm.currentMotion.states.size())
    {
      // arm reached its motion target
      ++arm.targetTime;

      if(arm.currentMotion.id == ArmMotionRequestBH::useDefault)
      {
        // arm is in default position, so ARME won't output any angles
        armMotionEngineOutput.arms[arm.id].move = false;
        arm.isMotionActive = false;
      }
      else if((arm.autoReverse && arm.targetTime == arm.autoReverseTime) || // target time ran out
        (!theGroundContactStateBH.contact) ||  // ground contact lost
        (theArmMotionRequestBH.motion[arm.id] != arm.currentMotion.id && !arm.contactTriggered))  // different motion requested
      {
        // start a reversed motion to reach the default position
        arm.startMotion(arm.currentMotion.reverse(defaultPos),
          theArmMotionRequestBH.fast[arm.id], false, 0, theFilteredJointDataBH);
      }
      else
      {
        // stay in target position
        updateOutput(arm, armMotionEngineOutput, arm.currentMotion.getTargetState());
      }
    }
    else
    {
      // target not yet reached, so interpolate between the states
      ArmMotion::ArmAngles nextState = arm.currentMotion.states[arm.stateIndex];
      if(!arm.fast)
      {
        ArmMotion::ArmAngles result;
        createOutput(arm, nextState, arm.interpolationTime, result);
        updateOutput(arm, armMotionEngineOutput, result);

        if(arm.interpolationTime >= nextState.steps)
        {
          ++arm.stateIndex;
          arm.interpolationTime = 1;
          arm.interpolationStart = nextState;
        }
      }
      else
      {
        // no interpolation
        updateOutput(arm, armMotionEngineOutput, nextState);
        ++arm.stateIndex;
        arm.interpolationTime = 1;
        arm.interpolationStart = nextState;
      }
    }
  }
}

void ArmMotionEngine::createOutput(Arm& arm, ArmMotion::ArmAngles target, int& time, ArmMotion::ArmAngles& result)
{
  // interpolate angles and set hardness
  const ArmMotion::ArmAngles from = arm.interpolationStart;
  for(unsigned i = 0; i < from.angles.size(); ++i)
  {
      const float offset = target.angles[i] - from.angles[i];
      const float speed = (float) time / (float) target.steps;
      result.angles[i] = (from.angles[i] + offset * speed);
      result.hardness[i] = target.hardness[i] == HardnessData::useDefault
        ? theHardnessSettings.hardness[arm.firstJoint + i]
        : target.hardness[i];
  }
  ++time;
}

void ArmMotionEngine::updateOutput(Arm& arm, ArmMotionEngineOutputBH& armMotionEngineOutput, ArmMotion::ArmAngles& values)
{
  for(unsigned i = 0; i < values.angles.size(); ++i)
  {
    armMotionEngineOutput.arms[arm.id].angles[i] = values.angles[i];
    armMotionEngineOutput.arms[arm.id].hardness[i] = values.hardness[i];
  }
  armMotionEngineOutput.arms[arm.id].motion = arm.currentMotion.id;
  armMotionEngineOutput.arms[arm.id].move = true;
  armMotionEngineOutput.arms[arm.id].lastMovement = theFrameInfoBH.time;
}
