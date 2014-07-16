/*
 * @class ObstacleModule
 * @author Lizzie Mamantov
 * @date June 2013
 *
 * @updated Megan Maher
 * @date July 2014
 *
 * A module that takes input from the various places that we get data about
 * objects around us and decides if there is an obstacle. Currently outputs
 * a : the position of the obstacle relative to the robot.
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
#include "NBMath.h"

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
    // How many possible directions are there?
    static const int NUM_DIRECTIONS = 9;

    // How close do our sonar readings need to get for us to decide obstacle?
    static constexpr float SONAR_THRESH = 0.4f;
    // How many frames do we consider in our average of sonar values?
    static const unsigned int SONAR_FRAMES_TO_BUFFER = 20;

    // How do we divide up the directions of the robot's field of vision?
    static constexpr float ZONE_WIDTH = (3.14159f) / (2.f * 4.f); //4 dir per pi
    // How far away should we consider something to be an obstacle?
    static constexpr float VISION_MAX_DIST = 150.f; // 1.5 meters
    // How many frames do we consider in our average of obstacle distances?
    static const unsigned int VISION_FRAMES_TO_BUFFER = 20;
    // After we see an obstacle, how long should we say it's still there?
    // We say the time it takes to do a full pan: 3-4 seconds
    static const int VISION_FRAMES_TO_HAVE_OBSTACLE = 30;

public:
    ObstacleModule();

    portals::InPortal<messages::ArmContactState> armContactIn;
    portals::InPortal<messages::VisionObstacle> visionIn;
    // We don't trust sonars right now! So we won't actually use them.
    portals::InPortal<messages::SonarState> sonarIn;

    portals::OutPortal<messages::FieldObstacles> obstacleOut;

protected:
    virtual void run_();

    // Makes a decision based on arm contact
    messages::FieldObstacles::Obstacle::ObstaclePosition
    processArms(const messages::ArmContactState& input);

    // Makes a decision based on vision
    messages::FieldObstacles::Obstacle::ObstaclePosition
    processVision(float distance, float bearing);

    // Makes a decision based on sonars, not using right now
    messages::FieldObstacles::Obstacle::ObstaclePosition
    processSonar(const messages::SonarState& input);

    // Updates vision buffer with info from last frame of vision
    void updateObstacleBuffer
    (messages::FieldObstacles::Obstacle::ObstaclePosition visionL,
     messages::FieldObstacles::Obstacle::ObstaclePosition visionM,
     messages::FieldObstacles::Obstacle::ObstaclePosition visionR);

    // Checks average of the appropriate buffer and acts accordingly
    messages::FieldObstacles::Obstacle::ObstaclePosition
    checkAverage(messages::FieldObstacles::Obstacle::ObstaclePosition direction,
                 std::list<float> distances);

private:
    // Sonar value buffers
    std::list<float> rightSonars, leftSonars;

    // Vision buffers for all directions robot can see
    std::list<float> SWDists, WDists, NWDists, NDists, NEDists, EDists, SEDists;
    // std::list<float> WBearings, NWBearings, NBearings, NEBearings, EBearings;

    // Global buffer that is updated every frame to show all obstacles
    int obstacleBuffer[NUM_DIRECTIONS];

    // Global array that keeps avg distance of obstacle in given direction
    float obstacleDistances[NUM_DIRECTIONS];

    // Keeps a list of the obstacle message type locations
    messages::FieldObstacles::Obstacle::ObstaclePosition
    obstaclesList[NUM_DIRECTIONS];
};

}
}
