/**
* @file BallModelBH.cpp
* Implementation of the BallModelBH's drawing functions
*/

#include "BallModel.h"
#include "Tools/Math/Pose2D.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/Debugging/DebugDrawings3D.h"

void BallModelBH::draw() const
{
  // drawing of the ball model in the field view
  DECLARE_DEBUG_DRAWING("representation:BallModelBH", "drawingOnField",
    const Vector2BH<>& position(estimate.position);
    const Vector2BH<>& velocity(estimate.velocity);
    CIRCLE("representation:BallModelBH",
            position.x,
            position.y,
            45,
            0, // pen width
            Drawings::ps_solid,
            ColorClasses::red,
            Drawings::bs_solid,
            ColorClasses::red);
    ARROW("representation:BallModelBH", position.x, position.y,
          position.x + velocity.x, position.y + velocity.y, 5, 1, ColorClasses::red);
  );
}

void BallModelBH::draw3D(const Pose2DBH& robotPose) const
{
  // drawing og the ball model in the scene
  DECLARE_DEBUG_DRAWING3D("representation:BallModelBH", "field",
  {
    if(SystemCall::getTimeSince(timeWhenLastSeen) < 5000 && SystemCall::getTimeSince(timeWhenDisappeared) < 1000)
    {
      Vector2BH<> ballRelToWorld = robotPose * estimate.position;
      SPHERE3D("representation:BallModelBH", ballRelToWorld.x, ballRelToWorld.y, 35.f, 35.f, ColorClasses::orange);
      LINE3D("representation:BallModelBH", robotPose.translation.x, robotPose.translation.y, 1.f, ballRelToWorld.x, ballRelToWorld.y, 1.f, 5.f, ColorClasses::orange);
    }
  });
}


void BallModelBH::drawEndPosition(float ballFriction) const
{
  // drawing of the end position
  DECLARE_DEBUG_DRAWING("representation:BallModelBH:endPosition", "drawingOnField",
    Vector2BH<> position = estimate.getEndPosition(ballFriction);
    CIRCLE("representation:BallModelBH:endPosition",
            position.x,
            position.y,
            45,
            0, // pen width
            Drawings::ps_solid,
            ColorClasses::black,
            Drawings::bs_solid,
            ColorRGBA(168, 25, 99, 220));
  );
}

void GroundTruthBallModelBH::draw() const
{
  DECLARE_DEBUG_DRAWING("representation:GroundTruthBallModelBH", "drawingOnField",
    const Vector2BH<>& position(estimate.position);
    const Vector2BH<>& velocity(estimate.velocity);
    CIRCLE("representation:GroundTruthBallModelBH",
            position.x,
            position.y,
            45,
            0, // pen width
            Drawings::ps_solid,
            ColorRGBA(255, 128, 0, 192),
            Drawings::bs_solid,
            ColorRGBA(255, 128, 0, 192));
    ARROW("representation:GroundTruthBallModelBH", position.x, position.y,
          position.x + velocity.x, position.y + velocity.y, 5, 1, ColorRGBA(255, 128, 0, 192));
  );
}

BallModelCompressed::BallModelCompressed(const BallModelBH& ballModel)
: lastPerception(ballModel.lastPerception),
  position(ballModel.estimate.position),
  velocity(ballModel.estimate.velocity),
  timeWhenLastSeen(ballModel.timeWhenLastSeen),
timeWhenDisappeared(ballModel.timeWhenDisappeared) {}

BallModelCompressed::operator BallModelBH() const
{
  BallModelBH ballModel;
  ballModel.lastPerception = Vector2BH<>(lastPerception);
  ballModel.estimate.position = Vector2BH<>(position);
  ballModel.estimate.velocity = Vector2BH<>(velocity);
  ballModel.timeWhenLastSeen = timeWhenLastSeen;
  ballModel.timeWhenDisappeared = timeWhenDisappeared;
  return ballModel;
}

