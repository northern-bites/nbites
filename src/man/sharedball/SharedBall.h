#pragma once

#include "DebugConfig.h"

#include "RoboGrams.h"
#include "Common.h"

#include "FieldConstants.h"

#include <iostream>

#include "WorldModel.pb.h"
#include "BallModel.pb.h"
#include "RobotLocation.pb.h"
#include "BallModel.pb.h"

/**
 *****DOES NOT WORK WITH WORLDVIEW OR SHARED VIEWER. MAJOR TODO.****
 *
 * A module that creates a global ball position. How it works:
 *
 * We go through each robot to count the number of robots (including itself)
 * that agree with its ball estimate, i.e. who have a ball estimate within
 * a "CONSENSUS_THRESHOLD" distance away.  This tells us how many robots would
 * be a part of its sharedball.  If there is a clear robot that has more robots
 * that agree with it, we say this robot is correct and calculate the shared
 * ball with it, and any robot that agrees with it.
 *
 * If there is a tie: i.e. there are two or more groups of robots that have
 * have the same number of agreements, then we use the goalie to break the
 * tie! If the goalie is part of one of these agreements, then we say the
 * goalie is right and calculate the shared ball with all robots that agree
 * with the goalie.
 *
 * To caluclate the shared ball, we take a weighted average of all robots
 * involved, depending on distance away from the ball.
 *
 * After the shared ball is calculated, if one of the robots that didn't agree
 * has a ball estimate that would agree if he were flipped... we flip that
 * robot. (As long as he is not the goalie)
 *
 * The module returns 2 messages:
 *     SharedBallMessage -> Gives coordinates of shared ball, if ball is on, and
 *                          a reliability.  The reliability is on a scale from
 *                          0 - 6, where each robot who sees the ball counts as
 *                          1, except for the goalie who counts for 2.
 *     Flip Message ------> Gives a timestamp that is used to tell if a flip
 *                          should occur at all, and a reset x, y, h.
 *                          While using the shared ball tool, the uncert is set
 *                          to which player we want to flip.
 *
 * NOTE: To test this module with the shared ball tool ("SharedViewer"),
 *       uncomment the lines in .cpp that are specified by the comments "TOOL".
 *
 */

namespace man {
namespace context {

const float CONSENSUS_THRESHOLD = 200.f;
const float DISTANCE_FOR_FLIP = 100.f;
const float TOO_CLOSE_TO_MIDFIELD_X = 120.f;
const float TOO_CLOSE_TO_MIDFIELD_Y = FIELD_GREEN_HEIGHT / 3.f;

class SharedBallModule : public portals::Module
{
public:
    SharedBallModule(int playerNumber);
    virtual ~SharedBallModule();

    virtual void run_();

    portals::InPortal<messages::WorldModel> worldModelIn[NUM_PLAYERS_PER_TEAM];
    portals::InPortal<messages::RobotLocation> locIn;
    portals::InPortal<messages::FilteredBall> ballIn;


    portals::OutPortal<messages::SharedBall> sharedBallOutput;
    portals::OutPortal<messages::RobotLocation> sharedBallReset;

private:
//not used, but assumes goalie is in fixed position
    //void incorporateGoalieWorldModel(messages::WorldModel newModel);

    void chooseRobots();
    void weightedavg();
    void checkForPlayerFlip();
    void calculateBallCoords(int i);
    float getBallDistanceSquared(int i, int j);
    int getQuadrantNumber(int i);
    bool inGoalieBox(float x, float y);

    messages::WorldModel worldMessages[NUM_PLAYERS_PER_TEAM];
    messages::FilteredBall myBall;

    float x;            //ball x location for a given robot
    float y;            //ball y location for a given robot

    int my_num;               // my player number
    float myX;
    float myY;
    float myH;

    // used for flipping robots
    float resetx;
    float resety;
    float reseth;
    int timestamp;

    int numRobotsOn;    // number of robots that see the ball
    int reliability;    // spans from 0 to NUM_ROBOTS + 1 (goalie counts double)
    float ballX[NUM_PLAYERS_PER_TEAM];
    float ballY[NUM_PLAYERS_PER_TEAM];
    int ignoreRobot[NUM_PLAYERS_PER_TEAM];
    float flippedRobot;

    bool ballOn;       //set to true if at least one robot sees the ball
};

} // namespace man
} // namespace context
