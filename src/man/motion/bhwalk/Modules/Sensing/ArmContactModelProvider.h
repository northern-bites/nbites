/**
* @file ArmContactModelProvider.h
*
* This module detects whether the robot touches a potential obstacle with an arm.
* This is done with comparing the current arm position and the position where the arm should be.
* The difference of those values is then stored into a buffer for each arm and if that buffer represents
* an error large enough, a positive arm contact for that arm is reported.
*
* If the robot fell down, all error buffers are resetted. Additionally, arm contact may only be reported
* if the robot is standing or walking and not penalized.
*
* Declaration of class ArmContactModelProvider.
* @author <a href="mailto:fynn@informatik.uni-bremen.de">Fynn Feldpausch</a>
* @author <a href="mailto:simont@informatik.uni-bremen.de">Simon Taddiken</a>
* @author <a href="mailto:arneboe@informatik.uni-bremen.de">Arne Böckmann</a>
*/

#pragma once

#include "Tools/RingBuffer.h"
#include "Tools/RingBufferWithSum.h"
#include "Tools/Module/Module.h"
#include "Representations/Sensing/FallDownState.h"
#include "Representations/Sensing/GroundContactState.h"
#include "Representations/Sensing/ArmContactModel.h"
#include "Representations/Infrastructure/JointData.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/GameInfo.h"
#include "Representations/Infrastructure/RobotInfo.h"
#include "Representations/Infrastructure/RoboCupGameControlData.h"
#include "Representations/MotionControl/MotionInfo.h"
#include "Representations/MotionControl/OdometryData.h"
#include "Representations/Sensing/RobotModel.h"
#include "Representations/MotionControl/ArmMotionEngineOutput.h"
#include "Tools/Math/Vector.h"


/** maximum numbers of frames to buffer */
#define FRAME_BUFFER_SIZE 5

/** number of angle differences to buffer */
#define ERROR_BUFFER_SIZE 100

/** serves as input for the checkArms method if checking the left arm */
#define LEFT true

/** serves as input for the checkArms method if checking the right arm */
#define RIGHT false

MODULE(ArmContactModelProvider)
  REQUIRES(FilteredJointDataBH)
  USES(MotionInfoBH)
  USES(JointRequestBH)
  USES(ArmMotionEngineOutputBH)
  USES(OdometryDataBH)
  REQUIRES(GroundContactStateBH)
  REQUIRES(GameInfoBH)
  REQUIRES(RobotInfoBH)
  REQUIRES(FrameInfoBH)
  REQUIRES(FallDownStateBH)
  REQUIRES(RobotModelBH)
  PROVIDES_WITH_MODIFY(ArmContactModelBH)
  LOADS_PARAMETER(float, errorXThreshold)           /**< Maximum divergence of arm angleX (in degrees) that is not treated as an obstacle detection */
  LOADS_PARAMETER(float, errorYThreshold)           /**< Maximum divergence of arm angleY (in degrees) that is not treated as an obstacle detection */
  LOADS_PARAMETER(unsigned, malfunctionThreshold)   /**< Duration of contact in frames after a contact is ignored */
  LOADS_PARAMETER(unsigned, frameDelay)             /**< The size of the delay in frames */
  LOADS_PARAMETER(bool, debugMode)                  /**< Enable debug mode */
  LOADS_PARAMETER(float, speedBasedErrorReduction)  /**< At this translational hand speed, the angular error will be ignored (in mm/s). */
  LOADS_PARAMETER(int, waitAfterMovement)           /**< wait this amount of time (in ms) after ARME moved an arm before contact can be detected again */
  LOADS_PARAMETER(bool, detectWhilePenalized)
END_MODULE


/**
* @class ArmContactModelProvider
*
*/
class ArmContactModelProvider: public ArmContactModelProviderBase
{
public:
  /** Constructor */
  ArmContactModelProvider();

private:
  struct ArmAngles
  {
    float leftX,             /**< X angle of left arm */
          leftY,             /**< Y angle of left arm */
          rightX,            /**< X angle of right arm */
          rightY;            /**< Y angle of right arm */
  };

  RingBufferBH<ArmAngles, FRAME_BUFFER_SIZE> angleBuffer;               /**< Buffered arm angles to eliminate delay */
  /**
  * Error vector:
  * y-Component: error of shoulder roll
  * x-Component: error of shoulder pitch
  */
  RingBufferWithSumBH<Vector2f, ERROR_BUFFER_SIZE> leftErrorBuffer;     /**< Buffered error over ERROR_BUFFER_SIZE frames */
  RingBufferWithSumBH<Vector2f, ERROR_BUFFER_SIZE> rightErrorBuffer;

  Vector2f errorLeft;
  Vector2f errorRight;

  bool leftMovingLastFrame;     /**< Whether left arm was moving last frame */
  bool rightMovingLastFrame;    /**< Whether right arm was moving last frame */
  const int soundDelay;         /**< Length of debug sound */
  unsigned int lastSoundTime;   /**< Time of last debug sound */
  Vector2BH<> lastLeftHandPos;    /**< Last 2-D position of the left hand */
  Vector2BH<> lastRightHandPos;   /**< Last 2-D position of the right hand */
  Pose2DBH lastOdometry;          /**< Odometry inthe previous frame. */
  int lastGameState;            /**< Game state in previous frame. */

  /**
  * Resets the arm contact model to default values and clears the error buffers.
  * @param armContactModel The model to reset.
  */
  void resetAll(ArmContactModelBH& armContactModel);


  /** Fills the error buffers with differences of current and requested
  * joint angles
  * @param left Check the left or the right arm for collisions?
  * @param factor A factor used to reduce the error entered into the buffer.
  */
  void checkArm(bool left, float factor);


  /** Executes this module.
  * @param ArmContactModelBH The data structure that is filled by this module.
  */
  void update(ArmContactModelBH& armContactModel);


  /** Calculates the angular speed of the robots hands.
  *
  * @param foreArm The arm which hand should be checked.
  * @param odometryOffset current odometry offset.
  * @param The speed of the hand calculated in the previous frame. This will be updated at the end of this method.
  */
  float calculateCorrectionFactor(const Pose3DBH foreArm, const Pose2DBH odometryOffset, Vector2BH<> &lastArmPos);


  /** Dertemines the push direction for an arm. That is, the direction in which the specified arm is being
  * pushed. That means that the obstacle causing the contact is located on the opposite direction of
  * the push direction.
  * The directions are given as compass directions with NORTH being the direction in which the robot
  * currently looks.
  *
  * @param left Check the left arm (true) or the right (false)
  * @param Is error.x above the x-axis threshold?
  * @param Is error.y above the y-axis threshold?
  * @param Current error vector.
  * @return The direction in which the specified arm is being pushed.
  */
  ArmContactModelBH::PushDirection getDirection(bool left, bool contactX, bool contactY, Vector2f error);
};
