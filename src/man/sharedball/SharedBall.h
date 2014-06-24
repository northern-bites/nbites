#pragma once

#include "RoboGrams.h"
#include "Common.h"

#include "FieldConstants.h"

#include <iostream>

#include "WorldModel.pb.h"
#include "BallModel.pb.h"
#include "RobotLocation.pb.h"

/**
 *
 * @brief Class to control a global ball position estimate
 *
 */

namespace man {
namespace context {

const float DISTANCE_TOO_FAR = 20.f;
const float DISTANCE_SQUARED = DISTANCE_TOO_FAR * DISTANCE_TOO_FAR;
const float DISTANCE_FOR_FLIP = 5.f;

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

    // used for flipping robots
    float resetx;
    float resety;
    float reseth;
    int timestamp;

    int numRobotsOn;    // number of robots that see the ball
    int robotToIgnore;  // will be bad robot
    float ballX[NUM_PLAYERS_PER_TEAM];
    float ballY[NUM_PLAYERS_PER_TEAM];

    bool ballOn;       //set to true if at least one robot sees the ball
};

} // namespace man
} // namespace context
