/**
 * @file RobotPose.h
 *
 * The file contains the definition of the class RobotPoseBH.
 *
 * @author <A href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</A>
 */

#pragma once

#include "Tools/Math/Pose2D.h"
#include "Tools/Math/Matrix3x3.h"
#include "Tools/Streams/AutoStreamable.h"

/**
* @class RobotPoseBH
* The pose of the robot with additional information
*/
STREAMABLE_WITH_BASE(RobotPoseBH, Pose2DBH,
{
public:
  enum {unknownDeviation = 100000};

  /** Assignment operator for Pose2DBH objects
  * @param other A Pose2DBH object
  * @return A reference to the object after the assignment
  */
  const RobotPoseBH& operator=(const Pose2DBH& other)
  {
    (Pose2DBH&) *this = other;
    // validity and co are not set
    return *this;
  }

  /** Draws the robot pose in the color of the team to the field view*/
  void draw(bool teamRed),

  (float)(0) validity,                 /**< The validity of the robot pose. (0 = invalid, 1 = perfect) */
  (float)(unknownDeviation) deviation, /**< The deviation of the robot pose. */
  (Matrix3x3BH<>) covariance,            /**< The covariance matrix of the estimated robot pose. */
});

/**
* @class GroundTruthRobotPoseBH
* The same as the RobotPoseBH, but - in general - provided by an external
* source that has ground truth quality
*/
STREAMABLE_WITH_BASE(GroundTruthRobotPoseBH, RobotPoseBH,
{
public:
  /** Draws the robot pose to the field view*/
  void draw() const,

  (unsigned)(0) timestamp,
});

/**
* @class RobotPoseCompressed
* A compressed version of RobotPoseBH used in team communication
*/
STREAMABLE(RobotPoseCompressed,
{
public:
  RobotPoseCompressed(const RobotPoseBH& robotPose);
  operator RobotPoseBH() const,

  (Vector2BH<short>) translation,
  (char) rotation,
  (unsigned char) validity,
  (float) deviation,
});
