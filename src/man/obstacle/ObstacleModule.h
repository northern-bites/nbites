#pragma once

#include "RoboGrams.h"

#include "ArmContactState.pb.h"
#include "VisionRobot.pb.h"
#include "ButtonState.pb.h"
#include "SonarState.pb.h"

#include "Obstacle.pb.h"

#include <iostream>

namespace man {
namespace obstacle {

class ObstacleModule : public portals::Module
{
public:
    ObstacleModule();

    portals::InPortal<messages::ArmContactState> armContactIn;
    portals::InPortal<messages::VisionObstacle> visionIn;
    portals::InPortal<messages::FootBumperState> footBumperIn;
    portals::InPortal<messages::SonarState> sonarIn;

    portals::OutPortal<messages::Obstacle> obstacleOut;

protected:
    virtual void run_();

    messages::Obstacle::ObstaclePosition
    processArms(messages::ArmContactState& input);

    // messages::Obstacle::ObstaclePosition
    // processVision(messages::VisionObstacle& input);

    // messages::Obstacle::ObstaclePosition
    // processFeet(messages::FootBumperState& input);

    // messages::Obstacle::ObstaclePosition
    // processSonar(messages::Sonartate& input);
};

}
}
