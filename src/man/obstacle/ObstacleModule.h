/*
 * @class ObstacleModule
 * @author Lizzie Mamantov
 * @date June 2013
 *
 * A module that takes input from the various places that we get data about
 * objects around us and decides if there is an obstacle. Currently outputs
 * a single value: the position of the obstacle relative to the robot.
 * @see Obstacle.proto for actual names. This is what they mean:
 *
 *        -----------------------------
 *        |        |         |        |
 *        |  NW    |    N    |   NE   |
 *        |--------|---------|--------|
 *        |        |         |        |
 *        |   W    |  robot  |   E    |
 *        |--------|---------|--------|
 *        |        |         |        |
 *        |  SW    |    S    |   SE   |
 *        -----------------------------
 *
 * Clearly this is not very sophisticated--a more complete model with actual
 * distances might be helpful. However, since behavior just wants this info
 * to make a simple decision, I'm trying to give it the most straightforward
 * information possible.
 *
 * Right now the decision is made based on ARM CONTACT
 * (@see arms/ArmContactModule) and SONARS (from sensors). Arm contact info
 * is pretty well filtered and processed in its own module, so we trust
 * its values straight up. Sonars aren't, so we keep a running average here
 * and use that value instead of the unfiltered data straight from sensors.
 *
 * Basically, sonars catches us before we hit things, most of the time, but
 * arms tell us if we're walking into something from a direction that sonars
 * won't get. Arms can also provide backup, refinement for sonars.
 *
 * There is a lot more that you could do with this module (EKF anyone?) but
 * this is a start.
 */

#pragma once

#include "RoboGrams.h"

#include "ArmContactState.pb.h"
#include "VisionRobot.pb.h"
#include "SonarState.pb.h"

#include "Obstacle.pb.h"

#include <iostream>
#include <list>

namespace man {
namespace obstacle {

class ObstacleModule : public portals::Module
{
    // Parameters
    // How close do our sonar readings need to get for us to decide obstacle?
    static const float SONAR_THRESH = 0.4f;
    // How many frames do we consider in our average of sonar values?
    static const unsigned int SONAR_FRAMES_TO_BUFFER = 20;

public:
    ObstacleModule();

    portals::InPortal<messages::ArmContactState> armContactIn;
    // Not using vision right now. Potential TODO?
    //portals::InPortal<messages::VisionObstacle> visionIn;
    portals::InPortal<messages::SonarState> sonarIn;

    portals::OutPortal<messages::Obstacle> obstacleOut;

protected:
    virtual void run_();

    // Makes a decision based on arm contact
    messages::Obstacle::ObstaclePosition
    processArms(const messages::ArmContactState& input);

    // messages::Obstacle::ObstaclePosition
    // processVision(messages::VisionObstacle& input);

    // Makes a decision based on sonars
    messages::Obstacle::ObstaclePosition
    processSonar(const messages::SonarState& input);

    // Sonar value buffers
    std::list<float> rightSonars, leftSonars;
};

}
}
