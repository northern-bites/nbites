/**
 * @file HeadLimitsBH.cpp
 * Implementation of the methods of a class for representing the limits of the head joints.
 * @author Felix Wenk
 */

#include <algorithm>
#include "Representations/Infrastructure/JointData.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Tools/Math/Vector3.h"
#include "Tools/Debugging/DebugDrawings3D.h"
#include "HeadLimits.h"

Vector2BH<> HeadLimitsBH::getTiltBound(float pan) const
{
  const std::vector<float>::const_iterator begin = intervals.begin();
  const std::vector<float>::const_iterator end = intervals.end();
  const std::vector<float>::const_iterator it = std::lower_bound(begin, end, pan);

  if(it == end)
    return Vector2BH<>(JointDataBH::off, JointDataBH::off); // Unreachable pan angle

  const int index = it - begin;
  const float xe = intervals[index];   // Interval end
  const float le = lowerBounds[index]; // Lower bound at interval end
  const float ue = upperBounds[index]; // Upper bound at interval end
  if(pan == xe)
    return Vector2BH<>(ue, le);

  if(index == 0)
    return Vector2BH<>(JointDataBH::off, JointDataBH::off); // Unreachable pan angle (smaller than begin of first interval)

  const float xs = intervals[index - 1];   // Interval start
  const float ls = lowerBounds[index - 1]; // Lower bound at interval start.
  const float us = upperBounds[index - 1]; // Upper bound at interval start

  const float lowerSlope = (le - ls) / (xe - xs);
  const float upperSlope = (ue - us) / (xe - xs);
  return Vector2BH<>(us + upperSlope * (pan - xs),
                   ls + lowerSlope * (pan - xs));
}

bool HeadLimitsBH::imageCenterHiddenByShoulder(const RobotCameraMatrixBH& robotCameraMatrix,
                                             const Vector3BH<>& shoulderInOrigin,
                                             const float imageTilt, const float hysteresis) const
{
  Vector3BH<> intersection;
  if(!intersectionWithShoulderPlane(robotCameraMatrix, shoulderInOrigin, imageTilt, intersection))
    return false; // No intersection with shoulder plane and therefore no intersection with circle.
  return intersection.abs() <= shoulderRadius + hysteresis;
}

bool HeadLimitsBH::intersectionWithShoulderEdge(const RobotCameraMatrixBH& robotCameraMatrix,
                                              const Vector3BH<>& shoulderInOrigin,
                                              Vector3BH<>& intersection) const
{
  if(!intersectionWithShoulderPlane(robotCameraMatrix, shoulderInOrigin, 0.0f, intersection))
    return false; // No intersection with the plane.
  if(std::abs(intersection.x) > shoulderRadius)
    return false; // No intersection with the shoulder circle.
  intersection.z = shoulderRadius * std::sin(std::acos(intersection.x / shoulderRadius));
  intersection += shoulderInOrigin;
  return true;
}

bool HeadLimitsBH::intersectionWithShoulderPlane(const RobotCameraMatrixBH& robotCameraMatrix,
                                               const Vector3BH<>& shoulderInOrigin,
                                               const float imageTilt, Vector3BH<>& intersection) const
{
  static const Vector3BH<> normal(0.0, 1.0f, 0.0f);
  Pose3DBH camera2Shoulder(-shoulderInOrigin);
  camera2Shoulder.conc(robotCameraMatrix);
  Vector3BH<> line(std::cos(-imageTilt), 0.0f, std::sin(-imageTilt));
  line = camera2Shoulder * line - camera2Shoulder.translation;
  const float denominator = normal * line;
  if(denominator == 0.0f)
    return false; // Line is parallel to the shoulder plane
  const float scale = (normal * camera2Shoulder.translation) / denominator;
  intersection = camera2Shoulder.translation - line * scale;
  return true;
}

void HeadLimitsBH::draw() const
{
  DECLARE_DEBUG_DRAWING3D("representation:HeadLimitsBH:left", "LShoulderPitch");
  DECLARE_DEBUG_DRAWING3D("representation:HeadLimitsBH:right", "RShoulderPitch");
  SPHERE3D("representation:HeadLimitsBH:left", 0, 0, 0, shoulderRadius, ColorClasses::orange);
  SPHERE3D("representation:HeadLimitsBH:right", 0, 0, 0, shoulderRadius, ColorClasses::orange);
}
