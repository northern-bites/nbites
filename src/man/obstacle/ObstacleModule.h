#pragma once

#include "RoboGrams.h"

#include "ArmContactState.pb.h"
#include "VisionRobot.pb.h"
#include "ButtonState.pb.h"
#include "SonarState.pb.h"

#include "Obstacle.pb.h"

#include <iostream>
#include <list>

namespace man {
namespace obstacle {

class ObstacleModule : public portals::Module
{
    static const float SONAR_THRESH = 0.5f;
    static const unsigned int SONAR_FRAMES_TO_BUFFER = 20;

    static const unsigned int FEET_FRAMES_TO_BUFFER = 30;

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
    processArms(const messages::ArmContactState& input);

    // messages::Obstacle::ObstaclePosition
    // processVision(messages::VisionObstacle& input);

    messages::Obstacle::ObstaclePosition
    processFeet(const messages::FootBumperState& input);

    messages::Obstacle::ObstaclePosition
    processSonar(const messages::SonarState& input);

    std::list<float> rightSonars, leftSonars;
    std::list<bool> rightFeet, leftFeet;
};

}
}
