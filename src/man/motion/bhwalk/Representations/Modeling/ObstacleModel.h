/**
* @file ObstacleModel.h
*
* Declaration of class ObstacleModelBH
*
* @author <a href="mailto:Tim.Laue@dfki.de">Tim Laue</a>
*/

#pragma once

#include "Tools/Math/Vector2.h"
#include "Tools/Math/Matrix2x2.h"
#include "Tools/Math/Random.h"
#include "Tools/Enum.h"
#include "Tools/Streams/AutoStreamable.h"
#include "Platform/BHAssert.h"
#include <vector>

class Pose3DBH;

/**
* @class ObstacleModelBH
*
* A class that represents the current state of the robot's environment
*/
STREAMABLE(ObstacleModelBH,
{
public:
  /** A single obstacle */
  STREAMABLE(Obstacle,
  {
  public:
    ENUM(Type, US, ROBOT, ARM, FOOT); /**< Different obstacle type */

    /** Constructor */
    Obstacle(const Vector2BH<>& leftCorner, const Vector2BH<>& rightCorner,
             const Vector2BH<>& center, const Vector2BH<>& closestPoint, const Matrix2x2BH<>& covariance, Type type = US),

    (Vector2BH<>) leftCorner,      /**< Leftmost point of the obstacle */
    (Vector2BH<>) rightCorner,     /**< Rightmost point of the obstacle */
    (Vector2BH<>) center,          /**< Center of mass of obstacle */
    (Vector2BH<>) closestPoint,    /**< Point of obstacle that is closest to the robot */
    (Matrix2x2BH<>) covariance,
    (Type)(US) type,             /**< The type of an obstacle */
  });

  /** Function for drawing */
  void draw() const;

  void draw3D(const Pose3DBH& torsoMatrix) const,

  /** A list of obstacles */
  (std::vector<Obstacle>) obstacles,
});

class USObstacleModelBH : public ObstacleModelBH {};

/**
 * A compressed version of the obstacleModel.
 */
STREAMABLE(ObstacleModelCompressed,
{
public:
  /**
   * private obstacle with compressed streaming.
   */
  STREAMABLE(Obstacle,
  {
  public:
    Obstacle(const ObstacleModelBH::Obstacle& other),

    (Vector2BH<short>) leftCorner,   /**< Leftmost point of the obstacle */
    (Vector2BH<short>) rightCorner,  /**< Rightmost point of the obstacle */
    (Vector2BH<short>) center,       /**< Center of mass of obstacle */
    (Vector2BH<short>) closestPoint, /**< Point of obstacle that is closest to the robot */

    /**
    * The covariance is a 2x2 matrix. however x12 and x21 are always identical.
    * Therefore we only store 3 floats instead of 4: x11, x12 and x22
    * |x11  x12|
    * |        |
    * |x21  x22|
    */
    (float) x11,
    (float) x12,
    (float) x22,

    (ObstacleModelBH::Obstacle, Type)(US) type, /**< The type of an obstacle */
  });

  ObstacleModelCompressed(const ObstacleModelBH& other, unsigned int maxNumberOfObstacles);

  operator ObstacleModelBH () const,

  (std::vector<Obstacle>) obstacles,
});

