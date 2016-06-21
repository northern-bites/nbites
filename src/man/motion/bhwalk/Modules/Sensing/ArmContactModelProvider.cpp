/**
* @file ArmContactModelProvider.h
*
* Implementation of class ArmContactModelProvider.
* @author <a href="mailto:fynn@informatik.uni-bremen.de">Fynn Feldpausch</a>
* @author <a href="mailto:simont@informatik.uni-bremen.de">Simon Taddiken</a>
* @author <a href="mailto:arneboe@informatik.uni-bremen.de">Arne Böckmann</a>
*/

#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/Math/Common.h"
#include "ArmContactModelProvider.h"
#include <sstream>
#include <cmath>
#include <algorithm>

/** Scale factor for debug drawings */
#define SCALE 20

MAKE_MODULE(ArmContactModelProvider, Sensing);

ArmContactModelProvider::ArmContactModelProvider()
: soundDelay(1000), lastSoundTime(0), lastGameState(STATE_INITIAL)
{
}

void ArmContactModelProvider::checkArm(bool left, float factor)
{
  Vector2f retVal;
  /* Calculate arm diffs */
  struct ArmAngles angles = angleBuffer[frameDelay];
  retVal.x = left
                  ? theFilteredJointDataBH.angles[JointDataBH::LShoulderPitch] - angles.leftX
                  : theFilteredJointDataBH.angles[JointDataBH::RShoulderPitch] - angles.rightX;
  retVal.y = left
                  ? theFilteredJointDataBH.angles[JointDataBH::LShoulderRoll] - angles.leftY
                  :  theFilteredJointDataBH.angles[JointDataBH::RShoulderRoll] - angles.rightY;
  retVal *= factor;

  if(left)
  {
    ARROW("module:ArmContactModelProvider:armContact", 0, 0, -(toDegrees(retVal.y) * SCALE), toDegrees(retVal.x) * SCALE, 20, Drawings::ps_solid, ColorClasses::blue);
    leftErrorBuffer.add(retVal);
  }
  else
  {
    ARROW("module:ArmContactModelProvider:armContact", 0, 0, (toDegrees(retVal.y) * SCALE), toDegrees(retVal.x) * SCALE, 20, Drawings::ps_solid, ColorClasses::blue);
    rightErrorBuffer.add(retVal);
  }
}

void ArmContactModelProvider::resetAll(ArmContactModelBH& model)
{
  model.contactLeft = false;
  model.contactRight = false;
  angleBuffer.init();
  leftErrorBuffer.init();
  rightErrorBuffer.init();
}

void ArmContactModelProvider::update(ArmContactModelBH& model)
{
  DECLARE_PLOT("module:ArmContactModelProvider:errorLeftX");
  DECLARE_PLOT("module:ArmContactModelProvider:errorRightX");
  DECLARE_PLOT("module:ArmContactModelProvider:errorLeftY");
  DECLARE_PLOT("module:ArmContactModelProvider:errorRightY");
  DECLARE_PLOT("module:ArmContactModelProvider:errorDurationLeft");
  DECLARE_PLOT("module:ArmContactModelProvider:errorDurationRight");
  DECLARE_PLOT("module:ArmContactModelProvider:errorYThreshold");
  DECLARE_PLOT("module:ArmContactModelProvider:errorXThreshold");
  DECLARE_PLOT("module:ArmContactModelProvider:contactLeft");
  DECLARE_PLOT("module:ArmContactModelProvider:contactRight");

  DECLARE_DEBUG_DRAWING("module:ArmContactModelProvider:armContact", "drawingOnField");

	// If in INITIAL or FINISHED, or we are penalized, we reset all buffered errors and detect no arm contacts
	if (theGameInfoBH.state == STATE_INITIAL || theGameInfoBH.state == STATE_FINISHED ||
    theFallDownStateBH.state == FallDownStateBH::onGround ||
    theFallDownStateBH.state == FallDownStateBH::falling ||
    theMotionInfoBH.motion == MotionRequestBH::getUp ||
    theMotionInfoBH.motion == MotionRequestBH::specialAction ||
    theMotionInfoBH.motion == MotionRequestBH::bike ||
    !theGroundContactStateBH.contact ||
    (theRobotInfoBH.penalty != PENALTY_NONE && !detectWhilePenalized))
  {
    resetAll(model);
    return;
  }

  // check if any arm just finished moving, if so reset its error buffer
  if(leftMovingLastFrame && !theArmMotionEngineOutputBH.arms[LEFT].move)
  {
    model.contactLeft = false;
    leftErrorBuffer.init();
  }

  if(rightMovingLastFrame && !theArmMotionEngineOutputBH.arms[RIGHT].move)
  {
    model.contactLeft = false;
    rightErrorBuffer.init();
  }

  leftMovingLastFrame = theArmMotionEngineOutputBH.arms[LEFT].move;
  rightMovingLastFrame = theArmMotionEngineOutputBH.arms[RIGHT].move;



  // clear on game state changes
  if (lastGameState != theGameInfoBH.state)
  {
    resetAll(model);
  }
  lastGameState = theGameInfoBH.state;

  CIRCLE("module:ArmContactModelProvider:armContact", 0, 0, 200, 30, Drawings::ps_solid, ColorClasses::blue, Drawings::ps_null, ColorClasses::blue);
  CIRCLE("module:ArmContactModelProvider:armContact", 0, 0, 400, 30, Drawings::ps_solid, ColorClasses::blue, Drawings::ps_null, ColorClasses::blue);
  CIRCLE("module:ArmContactModelProvider:armContact", 0, 0, 600, 30, Drawings::ps_solid, ColorClasses::blue, Drawings::ps_null, ColorClasses::blue);
  CIRCLE("module:ArmContactModelProvider:armContact", 0, 0, 800, 30, Drawings::ps_solid, ColorClasses::blue, Drawings::ps_null, ColorClasses::blue);
  CIRCLE("module:ArmContactModelProvider:armContact", 0, 0, 1000, 30, Drawings::ps_solid, ColorClasses::blue, Drawings::ps_null, ColorClasses::blue);
  CIRCLE("module:ArmContactModelProvider:armContact", 0, 0, 1200, 30, Drawings::ps_solid, ColorClasses::blue, Drawings::ps_null, ColorClasses::blue);


  /* Buffer arm angles */
  struct ArmAngles angles;
  angles.leftX = theJointRequestBH.angles[JointDataBH::LShoulderPitch];
  angles.leftY = theJointRequestBH.angles[JointDataBH::LShoulderRoll];
  angles.rightX = theJointRequestBH.angles[JointDataBH::RShoulderPitch];
  angles.rightY = theJointRequestBH.angles[JointDataBH::RShoulderRoll];
  angleBuffer.add(angles);

  Pose2DBH odometryOffset = theOdometryDataBH - lastOdometry;
  lastOdometry = theOdometryDataBH;

  /* Check for arm contact */
  // motion types to take into account: stand, walk (if the robot is upright)
  if((theMotionInfoBH.motion == MotionInfoBH::stand || theMotionInfoBH.motion == MotionInfoBH::walk) &&
     (theFallDownStateBH.state == FallDownStateBH::upright || theFallDownStateBH.state == FallDownStateBH::staggering) &&
     angleBuffer.isFilled())
  {

    const float leftFactor = calculateCorrectionFactor(theRobotModelBH.limbs[MassCalibrationBH::foreArmLeft], odometryOffset, lastLeftHandPos);
    const float rightFactor = calculateCorrectionFactor(theRobotModelBH.limbs[MassCalibrationBH::foreArmRight], odometryOffset, lastRightHandPos);

    // determine if arm is not moving and time passed since last arm motion for each arm
    // HINT: both conditions should be equivalent as long as waitAfterMovement is > 0
    const bool leftValid = !leftMovingLastFrame &&
      theFrameInfoBH.getTimeSince(theArmMotionEngineOutputBH.arms[LEFT].lastMovement) > waitAfterMovement;
    const bool rightValid = !rightMovingLastFrame &&
      theFrameInfoBH.getTimeSince(theArmMotionEngineOutputBH.arms[RIGHT].lastMovement) > waitAfterMovement;

    // only integrate new measurement if arm not moving and time passed > waitAfterMovement
    if(leftValid)
      checkArm(LEFT, leftFactor);
    if(rightValid)
      checkArm(RIGHT, rightFactor);

    const Vector2f left = leftErrorBuffer.getAverageFloat();
    const Vector2f right = rightErrorBuffer.getAverageFloat();


    //Determine if we are being pushed or not. Can only be true if last arm movement long enough ago
    bool leftX  = leftValid && fabs(left.x) > fromDegrees(errorXThreshold);
    bool leftY  = leftValid && fabs(left.y) > fromDegrees(errorYThreshold);
    bool rightX = rightValid && fabs(right.x)> fromDegrees(errorXThreshold);
    bool rightY = rightValid && fabs(right.y)> fromDegrees(errorYThreshold);

    // update the model
    model.contactLeft  = (leftX || leftY)  &&
      leftErrorBuffer.isFilled();

    model.contactRight = (rightX || rightY) &&
      rightErrorBuffer.isFilled();

    // There should be no contact for an arm that is currently being moved
    ASSERT(model.contactLeft ? !theArmMotionEngineOutputBH.arms[LEFT].move : true);
    ASSERT(model.contactRight ? !theArmMotionEngineOutputBH.arms[RIGHT].move : true);

    // The duration of the contact is counted upwards as long as the error
    // remains. Otherwise it is reseted to 0.
    model.durationLeft  = model.contactLeft  ? model.durationLeft + 1 : 0;
    model.durationRight = model.contactRight ? model.durationRight + 1 : 0;

    model.contactLeft &= model.durationLeft < malfunctionThreshold;
    model.contactRight &= model.durationRight < malfunctionThreshold;


    if(model.contactLeft)
    {
      model.timeOfLastContactLeft = theFrameInfoBH.time;
    }
    if(model.contactRight)
    {
      model.timeOfLastContactRight = theFrameInfoBH.time;
    }

    model.pushDirectionLeft = getDirection(LEFT, leftX, leftY, left);
    model.pushDirectionRight = getDirection(RIGHT, rightX, rightY, right);

    model.lastPushDirectionLeft = model.pushDirectionLeft != ArmContactModelBH::NONE ? model.pushDirectionLeft : model.lastPushDirectionLeft;
    model.lastPushDirectionRight = model.pushDirectionRight != ArmContactModelBH::NONE ? model.pushDirectionRight : model.lastPushDirectionRight;

    PLOT("module:ArmContactModelProvider:errorLeftX",         toDegrees(left.x));
    PLOT("module:ArmContactModelProvider:errorRightX",        toDegrees(right.x));
    PLOT("module:ArmContactModelProvider:errorLeftY",         toDegrees(left.y));
    PLOT("module:ArmContactModelProvider:errorRightY",        toDegrees(right.y));
    PLOT("module:ArmContactModelProvider:errorDurationLeft",  model.durationLeft);
    PLOT("module:ArmContactModelProvider:errorDurationRight", model.durationRight);
    PLOT("module:ArmContactModelProvider:errorYThreshold",    errorYThreshold);
    PLOT("module:ArmContactModelProvider:errorXThreshold",    errorXThreshold);
    PLOT("module:ArmContactModelProvider:contactLeft",        model.contactLeft ? 10.0 : 0.0);
    PLOT("module:ArmContactModelProvider:contactRight",       model.contactRight ? 10.0 : 0.0);

    ARROW("module:ArmContactModelProvider:armContact", 0, 0, -(toDegrees(left.y) * SCALE), toDegrees(left.x) * SCALE, 20, Drawings::ps_solid, ColorClasses::green);
    ARROW("module:ArmContactModelProvider:armContact", 0, 0, toDegrees(right.y) * SCALE, toDegrees(right.x) * SCALE, 20, Drawings::ps_solid, ColorClasses::red);

    COMPLEX_DRAWING("module:ArmContactModelProvider:armContact",
    {
      DRAWTEXT("module:ArmContactModelProvider:armContact", -2300, 1300, 200, ColorClasses::black, "LEFT");
      DRAWTEXT("module:ArmContactModelProvider:armContact", -2300, 1100, 200, ColorClasses::black, "ErrorX: " << toDegrees(left.x));
      DRAWTEXT("module:ArmContactModelProvider:armContact", -2300, 900, 200, ColorClasses::black,  "ErrorY: " << toDegrees(left.y));
      DRAWTEXT("module:ArmContactModelProvider:armContact", -2300, 500, 200, ColorClasses::black,  ArmContactModelBH::getName(model.pushDirectionLeft));
      DRAWTEXT("module:ArmContactModelProvider:armContact", -2300, 300, 200, ColorClasses::black,  "Time: " << model.timeOfLastContactLeft);

      DRAWTEXT("module:ArmContactModelProvider:armContact", 1300, 1300, 200, ColorClasses::black, "RIGHT");
      DRAWTEXT("module:ArmContactModelProvider:armContact", 1300, 1100, 200, ColorClasses::black, "ErrorX: " << toDegrees(right.x));
      DRAWTEXT("module:ArmContactModelProvider:armContact", 1300, 900, 200, ColorClasses::black,  "ErrorY: " << toDegrees(right.y));
      DRAWTEXT("module:ArmContactModelProvider:armContact", 1300, 500, 200, ColorClasses::black,  ArmContactModelBH::getName(model.pushDirectionRight));
      DRAWTEXT("module:ArmContactModelProvider:armContact", 1300, 300, 200, ColorClasses::black,  "Time: " << model.timeOfLastContactRight);

      if (model.contactLeft)
      {
        CROSS("module:ArmContactModelProvider:armContact", -2000, 0, 100, 20, Drawings::ps_solid, ColorClasses::red);
      }
      if (model.contactRight)
      {
        CROSS("module:ArmContactModelProvider:armContact", 2000, 0, 100, 20, Drawings::ps_solid, ColorClasses::red);
      }
  });


    if(debugMode && theFrameInfoBH.getTimeSince(lastSoundTime) > soundDelay &&
      (model.contactLeft || model.contactRight))
    {
      lastSoundTime = theFrameInfoBH.time;
#ifdef TARGET_ROBOT
      SystemCall::playSound("arm.wav");
#else
      OUTPUT(idText, text, (model.contactLeft ? "Left" : "") << (model.contactRight ? "Right" : "") << " arm!");
#endif
    }

  }
}


ArmContactModelBH::PushDirection ArmContactModelProvider::getDirection(bool left, bool contactX, bool contactY, Vector2f error)
{
  // for the left arm, y directions are mirrored!
  if (left)
  {
    error = Vector2f(error.x, -error.y);
  }

  ArmContactModelBH::PushDirection result = ArmContactModelBH::NONE;
  if (contactX && contactY)
  {
    if (error.x > 0.0 && error.y < 0.0f)
    {
      result = ArmContactModelBH::NW;
    }
    else if (error.x > 0.0 && error.y > 0.0)
    {
      result = ArmContactModelBH::NE;
    }
    if (error.x < 0.0 && error.y < 0.0f)
    {
      result = ArmContactModelBH::SW;
    }
    else if (error.x < 0.0 && error.y > 0.0)
    {
      result = ArmContactModelBH::SE;
    }
  }
  else if (contactX)
  {
    if (error.x < 0.0)
    {
      result = ArmContactModelBH::S;
    }
    else
    {
      result = ArmContactModelBH::N;
    }
  }
  else if (contactY)
  {
    if (error.y < 0.0)
    {
      result = ArmContactModelBH::W;
    }
    else
    {
      result = ArmContactModelBH::E;
    }
  }
  else
  {
    result = ArmContactModelBH::NONE;
  }

  return result;
}



float ArmContactModelProvider::calculateCorrectionFactor(const Pose3DBH foreArm, const Pose2DBH odometryOffset, Vector2BH<> &lastArmPos)
{
  const Vector3BH<>& handPos3D = foreArm.translation;
  Vector2BH<> handPos(handPos3D.x, handPos3D.y);
  Vector2BH<> handSpeed = (odometryOffset + Pose2DBH(handPos) - Pose2DBH(lastArmPos)).translation / theFrameInfoBH.cycleTime;
  float factor = std::max(0.f, 1.f - handSpeed.abs() / speedBasedErrorReduction);
  lastArmPos = handPos;
  return factor;
}
