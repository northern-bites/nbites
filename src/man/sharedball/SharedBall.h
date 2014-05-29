#pragma once

#include "RoboGrams.h"
#include "Common.h"

#include "FieldConstants.h"

#include <iostream>

#include "WorldModel.pb.h"
#include "BallModel.pb.h"

/**
 *
 * @brief Class to control a global ball position estimate
 *
 */

namespace man {
namespace context {

class SharedBallModule : public portals::Module
{
public:
    SharedBallModule();
    virtual ~SharedBallModule();

    virtual void run_();

private:
//determines where a robot thinks the ball is and gives it a weight
    void incorporateWorldModel(messages::WorldModel newModel);

//Not being used but same as previous method but assumes goalie's fixed position
    // void incorporateGoalieWorldModel(messages::WorldModel newModel);

//calculates a weighted average of the robot's ball locations
//weight is determined by distance to ball and uncertainty
    void weightedavg();

public:
    portals::InPortal<messages::WorldModel> worldModelIn[NUM_PLAYERS_PER_TEAM];
    portals::OutPortal<messages::SharedBall> sharedBallOutput;

private:
    float x;            //ball x location for a given robot
    float y;            //ball y location for a given robot
    float weight;       //determined by distance to ball and uncertainty
    float numx;         //numerator of weighted average of x
    float numy;         //numerator of weighted average of y
    float sumweight;    //denominator of weighted average is sum of weights

    bool ball_on;
};

} // namespace man
} // namespace context
