/**
* @file RobotsModelBH.cpp
* Debug drawings for the RobotsModelBH.
* @author <a href="mailto:afabisch@tzi.de">Alexander Fabisch</a>
*/

#include "RobotsModel.h"
#include "Platform/BHAssert.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/Math/Covariance.h"

RobotsModelBH::Robot::Robot(const Vector2BH<>& relPosOnField, bool teamRed, bool standing,
                          const Matrix2x2BH<>& covariance, unsigned timeStamp)
: relPosOnField(relPosOnField),
  teamRed(teamRed),
  standing(standing),
  covariance(covariance),
  timeStamp(timeStamp) {}

void RobotsModelBH::draw() const
{
  DECLARE_DEBUG_DRAWING("representation:RobotsModelBH:robots", "drawingOnField");
  DECLARE_DEBUG_DRAWING("representation:RobotsModelBH:covariance", "drawingOnField");

  COMPLEX_DRAWING("representation:RobotsModelBH:covariance",
  {
    for(RCIt r(robots.begin()); r != robots.end(); r++)
    {
      float s1 = 0.0, s2 = 0.0, theta = 0.0;
      Covariance::errorEllipse(r->covariance, s1, s2, theta);
      ELLIPSE("representation:RobotsModelBH:covariance", r->relPosOnField, sqrt(3.0f)*s1, sqrt(3.0f)*s2, theta,
              10, Drawings::ps_solid, ColorRGBA(100, 100, 255, 100), Drawings::bs_solid, ColorRGBA(100, 100, 255, 100));
      ELLIPSE("representation:RobotsModelBH:covariance", r->relPosOnField, sqrt(2.0f)*s1, sqrt(2.0f)*s2, theta,
              10, Drawings::ps_solid, ColorRGBA(150, 150, 100, 100), Drawings::bs_solid, ColorRGBA(150, 150, 100, 100));
      ELLIPSE("representation:RobotsModelBH:covariance", r->relPosOnField, s1, s2, theta,
              10, Drawings::ps_solid, ColorRGBA(255, 100, 100, 100), Drawings::bs_solid, ColorRGBA(255, 100, 100, 100));
    }
  });

  COMPLEX_DRAWING("representation:RobotsModelBH:robots",
  {
    for(RCIt r(robots.begin()); r != robots.end(); r++)
    {
      ColorClasses::Color color(r->teamRed ? ColorClasses::red : ColorClasses::blue);
      CROSS("representation:RobotsModelBH:robots", r->relPosOnField.x, r->relPosOnField.y,
            50, 20, Drawings::ps_solid, color);
    }
  });
}

RobotsModelCompressed::Robot::Robot(const RobotsModelBH::Robot& robot)
: relPosOnField(robot.relPosOnField),
  teamRed(robot.teamRed),
  standing(robot.standing),
  covXX(robot.covariance[0][0]),
  covYY(robot.covariance[1][1]),
  covXY(robot.covariance[0][1]),
  timeStamp(robot.timeStamp) {}

RobotsModelCompressed::Robot::operator RobotsModelBH::Robot() const
{
  return RobotsModelBH::Robot(Vector2BH<>(relPosOnField),
                            teamRed,
                            standing,
                            Matrix2x2BH<>(covXX, covXY, covXY, covYY),
                            timeStamp);
}

RobotsModelCompressed::RobotsModelCompressed(const RobotsModelBH& robotsModel, unsigned int maxNumberOfRobots)
{
  unsigned int offset = 0;
  const unsigned int numOfInputRobots = robotsModel.robots.size();
  unsigned int numOfUsedRobots = numOfInputRobots;
  if(numOfUsedRobots > maxNumberOfRobots)
  {
    numOfUsedRobots = maxNumberOfRobots;
    offset = static_cast<unsigned int>(random(static_cast<int>(numOfInputRobots)));
  }
  robots.reserve(numOfUsedRobots);
  for(size_t i = 0; i < numOfUsedRobots; i++)
    robots.push_back(Robot(robotsModel.robots[(offset + i) % numOfInputRobots]));
}

RobotsModelCompressed::operator RobotsModelBH() const
{
  RobotsModelBH robotsModel;
  robotsModel.robots.reserve(robots.size());
  for(size_t i = 0; i < robots.size(); i++)
  {
    robotsModel.robots.push_back(robots[i]);
  }
  return robotsModel;
}
