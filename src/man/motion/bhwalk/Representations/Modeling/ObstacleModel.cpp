/**
* @file ObstacleModelBH.cpp
* Implementation of class ObstacleModelBH
* @author <a href="mailto:Tim.Laue@dfki.de">Tim Laue</a>
*/

#include "ObstacleModel.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/Debugging/DebugDrawings3D.h"
#include "Tools/Math/Pose3D.h"

ObstacleModelBH::Obstacle::Obstacle(const Vector2BH<>& leftCorner, const Vector2BH<>& rightCorner,
                                  const Vector2BH<>& center, const Vector2BH<>& closestPoint,
                                  const Matrix2x2BH<>& covariance, Type type)
: leftCorner(leftCorner),
  rightCorner(rightCorner),
  center(center),
  closestPoint(closestPoint),
  covariance(covariance), type(type) {}

void ObstacleModelBH::draw() const
{
  DECLARE_DEBUG_DRAWING("representation:ObstacleModelBH", "drawingOnField");
  DECLARE_DEBUG_DRAWING("representation:ObstacleModelReduced", "drawingOnField");

  COMPLEX_DRAWING("representation:ObstacleModelBH",
  {
    for(std::vector<Obstacle>::const_iterator it = obstacles.begin(), end = obstacles.end(); it != end; ++it)
    {
      const Obstacle& obstacle = *it;
      ColorClasses::Color color = obstacle.type == Obstacle::US ? ColorClasses::blue : obstacle.type == Obstacle::ARM ? ColorClasses::yellow : obstacle.type == Obstacle::FOOT ? ColorClasses::green : ColorClasses::black;
      const Vector2BH<>& left = obstacle.leftCorner;
      Vector2BH<> right = obstacle.rightCorner;
      const float leftLen = left.abs();
      const float rightLen = right.abs();

      Vector2BH<> expanded = left;
      expanded.normalizeBH(leftLen + 500.f);
      LINE("representation:ObstacleModelBH", left.x, left.y, expanded.x, expanded.y, 30, Drawings::ps_solid, color);
      expanded = right;
      expanded.normalizeBH(rightLen + 500.f);
      LINE("representation:ObstacleModelBH", right.x, right.y, expanded.x, expanded.y, 30, Drawings::ps_solid, color);

      Vector2BH<> rightNorm = right;
      rightNorm.normalizeBH();
      Vector2BH<> leftRotated = Vector2BH<>(rightNorm.x, -rightNorm.y) * left.x + Vector2BH<>(rightNorm.y, rightNorm.x) * left.y;
      float angle = std::atan2(leftRotated.y, leftRotated.x);
      if(angle < 0.f)
        angle += pi2;

      int segments = ((int) floor(angle / fromDegrees(30.f))) + 1;
      float segmentAngle = angle / (float) segments;
      Vector2BH<> newRight;
      for(int i = 1; i < segments; ++i)
      {
        newRight = right;
        newRight.rotate(segmentAngle);
        newRight.normalizeBH(rightLen + (leftLen - rightLen) * i / (float) segments);
        LINE("representation:ObstacleModelBH", right.x, right.y, newRight.x, newRight.y, 30, Drawings::ps_solid, color);
        right = newRight;
      }
      LINE("representation:ObstacleModelBH", left.x, left.y, right.x, right.y, 30, Drawings::ps_solid, color);

      CROSS("representation:ObstacleModelBH", obstacle.center.x, obstacle.center.y, 100, 20, Drawings::ps_solid, ColorClasses::blue);
      CROSS("representation:ObstacleModelBH", obstacle.closestPoint.x, obstacle.closestPoint.y, 100, 20, Drawings::ps_solid, ColorClasses::red);
    }
  });

  COMPLEX_DRAWING("representation:ObstacleModelReduced",
  {
    for(std::vector<Obstacle>::const_iterator it = obstacles.begin(), end = obstacles.end(); it != end; ++it)
    {
      const Obstacle& obstacle = *it;
      ColorClasses::Color color = obstacle.type == Obstacle::US ? ColorClasses::blue : obstacle.type == Obstacle::ARM ? ColorClasses::yellow : ColorClasses::black;
      const Vector2BH<>& left = obstacle.leftCorner;
      Vector2BH<> right = obstacle.rightCorner;
      const float leftLen = left.abs();
      const float rightLen = right.abs();

      Vector2BH<> expanded = left;
      expanded.normalizeBH(leftLen + 500.f);
      LINE("representation:ObstacleModelReduced", left.x, left.y, expanded.x, expanded.y, 30, Drawings::ps_solid, color);
      expanded = right;
      expanded.normalizeBH(rightLen + 500.f);
      LINE("representation:ObstacleModelReduced", right.x, right.y, expanded.x, expanded.y, 30, Drawings::ps_solid, color);

      Vector2BH<> rightNorm = right;
      rightNorm.normalizeBH();
      Vector2BH<> leftRotated = Vector2BH<>(rightNorm.x, -rightNorm.y) * left.x + Vector2BH<>(rightNorm.y, rightNorm.x) * left.y;
      float angle = std::atan2(leftRotated.y, leftRotated.x);
      if(angle < 0.f)
        angle += pi2;

      int segments = ((int) floor(angle / fromDegrees(30.f))) + 1;
      float segmentAngle = angle / (float) segments;
      Vector2BH<> newRight;
      for(int i = 1; i < segments; ++i)
      {
        newRight = right;
        newRight.rotate(segmentAngle);
        newRight.normalizeBH(rightLen + (leftLen - rightLen) * i / (float) segments);
        LINE("representation:ObstacleModelReduced", right.x, right.y, newRight.x, newRight.y, 30, Drawings::ps_solid, color);
        right = newRight;
      }
      LINE("representation:ObstacleModelReduced", left.x, left.y, right.x, right.y, 30, Drawings::ps_solid, color);
    }
  });
}

void ObstacleModelBH::draw3D(const Pose3DBH& torsoMatrix) const
{

#define LINE3D_REL_ODOMETRY_ORIGIN(id, torsoMatrix, point1, point2, size, color) \
  { \
    Pose3DBH torsoMatrixInv = torsoMatrix.invert(); \
    Vector3BH<> point1Rel = torsoMatrixInv * Vector3BH<>(point1.x, point1.y, 0.f); \
    Vector3BH<> point2Rel = torsoMatrixInv * Vector3BH<>(point2.x, point2.y, 0.f); \
    LINE3D(id, point1Rel.x, point1Rel.y, 0.f, point2Rel.x, point2Rel.y, 0.f, size, color); \
  }

  DECLARE_DEBUG_DRAWING3D("representation:ObstacleModelBH", "robot",
    for(std::vector<Obstacle>::const_iterator it = obstacles.begin(), end = obstacles.end(); it != end; ++it)
    {
      const Obstacle& obstacle = *it;
      ColorClasses::Color color = obstacle.type == Obstacle::US ? ColorClasses::blue : obstacle.type == Obstacle::ARM ? ColorClasses::yellow : ColorClasses::black;
      const Vector2BH<>& left = obstacle.leftCorner;
      Vector2BH<> right = obstacle.rightCorner;
      const float leftLen = left.abs();
      const float rightLen = right.abs();

      Vector2BH<> expanded = left;
      expanded.normalizeBH(leftLen + 500.f);
      LINE3D_REL_ODOMETRY_ORIGIN("representation:ObstacleModelBH", torsoMatrix, left, expanded, 4, color);
      expanded = right;
      expanded.normalizeBH(rightLen + 500.f);
      LINE3D_REL_ODOMETRY_ORIGIN("representation:ObstacleModelBH", torsoMatrix, right, expanded, 4, color);

      Vector2BH<> rightNorm = right;
      rightNorm.normalizeBH();
      Vector2BH<> leftRotated = Vector2BH<>(rightNorm.x, -rightNorm.y) * left.x + Vector2BH<>(rightNorm.y, rightNorm.x) * left.y;
      float angle = std::atan2(leftRotated.y, leftRotated.x);
      if(angle < 0.f)
        angle += pi2;

      int segments = ((int) floor(angle / fromDegrees(30.f))) + 1;
      float segmentAngle = angle / (float) segments;
      Vector2BH<> newRight;
      for(int i = 1; i < segments; ++i)
      {
        newRight = right;
        newRight.rotate(segmentAngle);
        newRight.normalizeBH(rightLen + (leftLen - rightLen) * i / (float) segments);
        LINE3D_REL_ODOMETRY_ORIGIN("representation:ObstacleModelBH", torsoMatrix, right, newRight, 4, color);
        right = newRight;
      }
      LINE3D_REL_ODOMETRY_ORIGIN("representation:ObstacleModelBH", torsoMatrix, left, right, 4, color);
    }
  );
}

ObstacleModelCompressed::Obstacle::Obstacle(const ObstacleModelBH::Obstacle& other)
: leftCorner(other.leftCorner),
  rightCorner(other.rightCorner),
  center(other.center),
  closestPoint(other.closestPoint),
  x11(other.covariance.c[0].x),
  x12(other.covariance.c[1].x),x22(other.covariance.c[1].y),
  type(other.type) {}

ObstacleModelCompressed::ObstacleModelCompressed(const ObstacleModelBH& other, unsigned int maxNumberOfObstacles)
{
  unsigned int offset = 0;
  const unsigned int numOfInputObstacles = other.obstacles.size();
  unsigned int numOfUsedObstacles = numOfInputObstacles;
  if(numOfUsedObstacles > maxNumberOfObstacles)
  {
    numOfUsedObstacles = maxNumberOfObstacles;
    offset = static_cast<unsigned int>(random(static_cast<int>(numOfInputObstacles)));
  }
  obstacles.reserve(numOfUsedObstacles);
  for(size_t i = 0; i < numOfUsedObstacles; i++)
    obstacles.push_back(Obstacle(other.obstacles[(offset + i) % numOfInputObstacles]));
}

ObstacleModelCompressed::operator ObstacleModelBH() const
{
  ObstacleModelBH o;
  for(std::vector<Obstacle>::const_iterator it = obstacles.begin(); it != obstacles.end(); it++)
  {
    const Obstacle& compressed = *it;

    //x12 and x21 are identical, therefore only x12 is streamed.
    Matrix2x2BH<> uncompressedCovariance(compressed.x11, compressed.x12,
                                       compressed.x12, compressed.x22);

    ObstacleModelBH::Obstacle uncompressed(Vector2BH<>(compressed.leftCorner),
                                         Vector2BH<>(compressed.rightCorner),
                                         Vector2BH<>(compressed.center),
                                         Vector2BH<>(compressed.closestPoint),
                                         uncompressedCovariance,
                                         compressed.type);
    o.obstacles.push_back(uncompressed);
  }
  return o;
}


