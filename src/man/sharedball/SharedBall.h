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

const float ALPHA = .7f; // For exponential filter

class SharedBallModule : public portals::Module
{
public:
    SharedBallModule();
    virtual ~SharedBallModule();

    virtual void run_();

private:
    void incorporateWorldModel(messages::WorldModel newModel);
    void incorporateGoalieWorldModel(messages::WorldModel newModel);

public:
    portals::InPortal<messages::WorldModel> worldModelIn[NUM_PLAYERS_PER_TEAM];
    portals::OutPortal<messages::SharedBall> sharedBallOutput;

private:
    // Exponential filter on global x,y
    float x;
    float y;

    int framesSinceUpdate;
    bool updatedThisFrame;
};

} // namespace man
} // namespace context
