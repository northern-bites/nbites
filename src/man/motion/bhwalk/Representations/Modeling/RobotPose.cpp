/**
* @file RobotPoseBH.cpp
*
* contains the implementation of the streaming operators
* for the class RobotPoseBH
*/

#include "RobotPose.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/Debugging/DebugDrawings3D.h"

void RobotPoseBH::draw(bool teamRed)
{
  DECLARE_DEBUG_DRAWING("representation:RobotPoseBH", "drawingOnField");
  Vector2BH<> bodyPoints[4] = {Vector2BH<>(55, 90),
                             Vector2BH<>(-55, 90),
                             Vector2BH<>(-55, -90),
                             Vector2BH<>(55, -90)
                            };
  for(int i = 0; i < 4; i++)
    bodyPoints[i] = *this * bodyPoints[i];
  Vector2BH<> dirVec(200, 0);
  dirVec = *this * dirVec;
  const ColorRGBA ownTeamColorForDrawing = teamRed ? ColorRGBA(255, 0, 0) : ColorRGBA(0, 0, 255);
  LINE("representation:RobotPoseBH", translation.x, translation.y, dirVec.x, dirVec.y,
       20, Drawings::ps_solid, ColorClasses::white);
  POLYGON("representation:RobotPoseBH", 4, bodyPoints, 20, Drawings::ps_solid,
          ownTeamColorForDrawing, Drawings::bs_solid, ColorClasses::white);
  CIRCLE("representation:RobotPoseBH", translation.x, translation.y, 42, 0,
         Drawings::ps_solid, ownTeamColorForDrawing, Drawings::bs_solid, ownTeamColorForDrawing);

  DECLARE_DEBUG_DRAWING("representation:RobotPoseBH:deviation", "drawingOnField");
  if(deviation < 100000.f)
    DRAWTEXT("representation:RobotPoseBH:deviation", -3000, -2300, 100, ColorRGBA(0xff, 0xff, 0xff), "pose deviation: " << deviation);
  else
    DRAWTEXT("representation:RobotPoseBH:deviation", -3000, -2300, 100, ColorRGBA(0xff, 0xff, 0xff), "pose deviation: unknown");

  DECLARE_DEBUG_DRAWING3D("representation:RobotPoseBH", "field",
  {
    LINE3D("representation:RobotPoseBH", translation.x, translation.y, 10,
           dirVec.x, dirVec.y, 10, 1, ownTeamColorForDrawing);
    for(int i = 0; i < 4; ++i)
    {
      const Vector2BH<> p1 = bodyPoints[i];
      const Vector2BH<> p2 = bodyPoints[(i + 1) & 3];
      LINE3D("representation:RobotPoseBH", p1.x, p1.y, 10,
             p2.x, p2.y, 10, 1, ownTeamColorForDrawing);
    }
  });

  DECLARE_DEBUG_DRAWING("origin:RobotPoseBH", "drawingOnField"); // Set the origin to the robot's current position
  DECLARE_DEBUG_DRAWING("origin:RobotPoseWithoutRotation", "drawingOnField");
  ORIGIN("origin:RobotPoseBH", translation.x, translation.y, rotation);
  ORIGIN("origin:RobotPoseWithoutRotation", translation.x, translation.y, 0);
}

void GroundTruthRobotPoseBH::draw() const
{
  DECLARE_DEBUG_DRAWING("representation:GroundTruthRobotPoseBH", "drawingOnField");
  ColorRGBA transparentWhite(ColorClasses::white);
  transparentWhite.a = 128;
  Vector2BH<> bodyPoints[4] = {Vector2BH<>(55, 90),
                             Vector2BH<>(-55, 90),
                             Vector2BH<>(-55, -90),
                             Vector2BH<>(55, -90)
                            };
  for(int i = 0; i < 4; i++)
    bodyPoints[i] = *this * bodyPoints[i];
  Vector2BH<> dirVec(200, 0);
  dirVec = *this * dirVec;
  const ColorRGBA ownTeamColorForDrawing(0, 0, 0, 128);
  LINE("representation:GroundTruthRobotPoseBH", translation.x, translation.y, dirVec.x, dirVec.y,
       20, Drawings::ps_solid, transparentWhite);
  POLYGON("representation:GroundTruthRobotPoseBH", 4, bodyPoints, 20, Drawings::ps_solid,
          ownTeamColorForDrawing, Drawings::bs_solid, transparentWhite);
  CIRCLE("representation:GroundTruthRobotPoseBH", translation.x, translation.y, 42, 0,
         Drawings::ps_solid, ownTeamColorForDrawing, Drawings::bs_solid, ownTeamColorForDrawing);

  DECLARE_DEBUG_DRAWING("origin:GroundTruthRobotPoseBH", "drawingOnField"); // Set the origin to the robot's ground truth position
  DECLARE_DEBUG_DRAWING("origin:GroundTruthRobotPoseWithoutRotation", "drawingOnField");
  ORIGIN("origin:GroundTruthRobotPoseBH", translation.x, translation.y, rotation);
  ORIGIN("origin:GroundTruthRobotPoseWithoutRotation", translation.x, translation.y, 0);
}

RobotPoseCompressed::RobotPoseCompressed(const RobotPoseBH& robotPose)
: translation(robotPose.translation),
  deviation(robotPose.deviation)
{
  float normalizedAngle = normalizeBH(robotPose.rotation);
  int discretizedAngle = (int)(normalizedAngle * 128.0f / pi);
  if(discretizedAngle > 127)
    discretizedAngle = -128;
  rotation = (char) discretizedAngle;
  validity = (unsigned char) (robotPose.validity * 255.f);
}

RobotPoseCompressed::operator RobotPoseBH() const
{
  RobotPoseBH robotPose;
  robotPose.translation = Vector2BH<>(translation);
  robotPose.rotation = (float) rotation * pi / 128.f;
  robotPose.validity = (float) validity / 255.f;
  robotPose.deviation = deviation;
  return robotPose;
}
