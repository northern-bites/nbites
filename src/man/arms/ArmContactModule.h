#pragma once

#include "RoboGrams.h"
#include "PMotion.pb.h"
#include "ArmContactState.pb.h"

#include <queue>
#include <math.h>
#include <iostream>

namespace man {
namespace arms {

static const unsigned int FRAMES_DELAY = 5;
static const float DISPLACEMENT_THRESH = 0.04f;

class ArmContactModule : public portals::Module
{
public:
    ArmContactModule();

    portals::InPortal<messages::JointAngles> actualJointsIn;
    portals::InPortal<messages::JointAngles> expectedJointsIn;
    portals::OutPortal<messages::ArmContactState> contactOut;

protected:
    virtual void run_();
    void determineContactState();

    std::queue<messages::JointAngles> expectedJoints;
    messages::JointAngles* jointsWithDelay;

    messages::ArmContactState::PushDirection rightArm;
    messages::ArmContactState::PushDirection leftArm;
};

}
}
