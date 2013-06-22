#pragma once

#include "RoboGrams.h"
#include "Common.h"

#include "FieldConstants.h"

#include <iostream>

#include "WorldModel.pb.h"

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

private:
    // Exponential filter on global x,y
    float x;
    float y;
};

} // namespace man
} // namespace context
