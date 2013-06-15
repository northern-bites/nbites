#pragma once

#include "RoboGrams.h"
#include "PMotion.pb.h"
#include "ArmContactState.pb.h"
#include "HandSpeeds.pb.h"

#include <queue>
#include <list>
#include <vector>
#include <math.h>
#include <iostream>

namespace man {
namespace arms {

static const unsigned int FRAMES_DELAY = 5;
static const float DISPLACEMENT_THRESH = 0.02f;
static const unsigned int FRAMES_TO_BUFFER = 50;
static const float SPEED_BASED_ERROR_REDUCTION = 600.f;

enum Joint
{
    PITCH = 0,
    ROLL = 1
};

typedef std::vector<float> Displacement;
typedef std::list<Displacement> DisplacementBuffer;

Displacement getAverageDisplacement(DisplacementBuffer& buf);

class ArmContactModule : public portals::Module
{
public:
    ArmContactModule();

    portals::InPortal<messages::JointAngles> actualJointsIn;
    portals::InPortal<messages::JointAngles> expectedJointsIn;
    portals::InPortal<messages::HandSpeeds> handSpeedsIn;

    portals::OutPortal<messages::ArmContactState> contactOut;

protected:
    virtual void run_();
    void determineContactState();

    std::queue<messages::JointAngles> expectedJoints;
    messages::JointAngles* jointsWithDelay;

    messages::ArmContactState::PushDirection rightArm;
    messages::ArmContactState::PushDirection leftArm;

    DisplacementBuffer right, left;
};

}
}
