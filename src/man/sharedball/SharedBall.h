#pragma once

#include "RoboGrams.h"
#include "Common.h"

#include <iostream>

#include "WorldModel.pb.h"

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
    void incorporateWorldModel(messages::WorldModel newModel);

public:
    portals::InPortal<messages::WorldModel> worldModelIn[NUM_PLAYERS_PER_TEAM];

private:
    //Dumb filter, easier to make then KF to start
    float x;
    float y;
};

} // namespace man
} // namespace context
