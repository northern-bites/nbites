#pragma once

#include "RoboGrams.h"
#include "Common.h"

#include "FieldConstants.h"
#include "RobotConfig.h"

#include <iostream>

#include "WorldModel.pb.h"
#include "BallModel.pb.h"
#include "RobotLocation.pb.h"

/**
 *
 * @brief Class to control a global ball position:
 * @      - Takes weighted average of each player that sees the ball
 * @      and weighs the goalie twice as heavily as other players.
 * @      - If a player's ball estimate does not agree with other ball
 * @      estimates on field, the player's estimate is not used to
 * @      calculate the shared ball. If this same player's ball estimate
 * @      is close to where the shared ball would be if it were flipped,
 * @      and the player is not too close to the midfield, the player is
 * @      flipped.
 *
 *
 * NOTE: To test this module with the shared ball tool ("SharedViewer"),
 *       uncomment the lines in .cpp that are specified by the comments "TOOL".
 *
 */

namespace man {
namespace context {

const float DISTANCE_TOO_FAR = 250.f;
const float DISTANCE_SQUARED = DISTANCE_TOO_FAR * DISTANCE_TOO_FAR;
const float DISTANCE_FOR_FLIP = 100.f;
const float TOO_CLOSE_TO_MIDFIELD = 50.f;

class SharedBallModule : public portals::Module
{
public:
    SharedBallModule();
    virtual ~SharedBallModule();

    virtual void run_();

    portals::InPortal<messages::WorldModel> worldModelIn[NUM_PLAYERS_PER_TEAM];
    portals::OutPortal<messages::SharedBall> sharedBallOutput;
    portals::OutPortal<messages::RobotLocation> sharedBallReset;

private:
//not used, but assumes goalie is in fixed position
    //void incorporateGoalieWorldModel(messages::WorldModel newModel);

    void eliminateBadRobots();
    void weightedavg();
    void checkForPlayerFlip();
    void calculateBallCoords(int i);
    float getBallDistanceSquared(int i, int j);

    messages::WorldModel messages[NUM_PLAYERS_PER_TEAM];

    float x;            //ball x location for a given robot
    float y;            //ball y location for a given robot
    int my_num;         // my player number

    // used for flipping robots
    float resetx;
    float resety;
    float reseth;
    int timestamp;

    int numRobotsOn;    // number of robots that see the ball
    int robotToIgnore;  // will be bad robot - 1, i.e. which index in the array
    float ballX[NUM_PLAYERS_PER_TEAM];
    float ballY[NUM_PLAYERS_PER_TEAM];

    bool ballOn;       //set to true if at least one robot sees the ball
};

} // namespace man
} // namespace context
