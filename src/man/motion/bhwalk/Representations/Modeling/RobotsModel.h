/**
 * @file RobotsModel.h
 *
 * @author <a href="mailto:afabisch@tzi.de>Alexander Fabisch</a>
 */

#pragma once

#include "Tools/Math/Matrix2x2.h"
#include "Tools/Math/Random.h"
#include <vector>
#include "Tools/Streams/AutoStreamable.h"

/**
 * @class RobotsModelBH
 * This representation contains all percepted and smoothed robots in relative
 * field coordinates.
 */
STREAMABLE(RobotsModelBH,
{
public:
  /**
   * @class Robot
   * A robot on the field.
   */
  STREAMABLE(Robot,
  {
  public:
    Robot(const Vector2BH<>& relPosOnField, bool teamRed, bool standing,
          const Matrix2x2BH<>& covariance, unsigned timeStamp),

    (Vector2BH<>) relPosOnField, /**< Relative position of the robot on the field. */
    (bool) teamRed,            /**< Red team or blue team? */
    (bool) standing,           /**< Is this robot standing or horizontal? */
    (Matrix2x2BH<>) covariance,  /**< covariance of a seen robot */
    (unsigned) timeStamp,      /**< Timestamp of the last update. */
  });

  typedef std::vector<Robot>::const_iterator RCIt;
  typedef std::vector<Robot>::iterator RIt;

  void draw() const,

  (std::vector<Robot>) robots,
});

/**
 * @class GroundTruthRobotsModelBH
 * The class contains the true RobotsModelBH.
 */
class GroundTruthRobotsModelBH : public RobotsModelBH {};

/**
 * @class RobotsModelCompressed
 * This class contains a compressed RobotsModelBH for team communication.
 */
STREAMABLE(RobotsModelCompressed,
{
public:
  STREAMABLE(Robot,
  {
  public:
    Robot(const RobotsModelBH::Robot& robot);
    operator RobotsModelBH::Robot() const,

    // 36 Bytes -> 22 Bytes
    (Vector2BH<short>) relPosOnField,
    (bool) teamRed,
    (bool) standing,
    (float) covXX,
    (float) covYY,
    (float) covXY, // == covYX
    (unsigned) timeStamp,
  });

  RobotsModelCompressed(const RobotsModelBH& robotsModel, unsigned int maxNumberOfRobots);
  operator RobotsModelBH() const,

  (std::vector<Robot>) robots,
});
