/*
 * @class ArmContactStateModule
 * @author Lizzie Mamantov
 * @date June 2013
 *
 * Module that compares the desired and actual joint angles for the robot's
 * arms and decides whether the arm is being pushed, and in what direction.
 * The directions are (@see ArmContactState.proto) :
 *        -----------------------------
 *        |        |         |        |
 *        |  NW    |    N    |   NE   |
 *        |--------|---------|--------|
 *        |        |         |        |
 *        |   W    |   arm   |   E    |
 *        |--------|---------|--------|
 *        |        |         |        |
 *        |  SW    |    S    |   SE   |
 *        -----------------------------
 *
 * This "direction" means the direction of displacement of the arm from its
 * expected position. We decide this direction based on the robot's shoulder
 * pitch and roll. If the grid above is the x-y plane, the shoulder roll gives
 * us info about x displacement and pitch about y displacement.
 *
 * Since this difference can be noisy, we keep a running average and only
 * decide actual pushes based on that average. There are other corrections
 * as well, described in the code below.
 */

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

// PARAMETERS
// Joints take a few frames to get to their requested position, so we compare
// expected joints from FRAMES_DELAY frames ago instead of the current frame
static const unsigned int FRAMES_DELAY = 5;
// How big is a valid displacement?
static const float DISPLACEMENT_THRESH = 0.03f;
// How many frames to average, to reduce noisy joints
static const unsigned int FRAMES_TO_BUFFER = 50;
// How much do we reduce how much we trust a value if the hands are moving
// quickly? See BHuman's 2012 team report for how this works.
static const float SPEED_BASED_ERROR_REDUCTION = 600.f;
// After how many frames do we assume that an arm is just stuck?
static const int STUCK_THRESH = 300;

enum Joint
{
    PITCH = 0,
    ROLL = 1
};

// A Displacement is sort of an x, y (or roll, pitch) vector
typedef std::vector<float> Displacement;
typedef std::list<Displacement> DisplacementBuffer;

// Helper method to average a displacement buffer
Displacement getAverageDisplacement(DisplacementBuffer& buf);

class ArmContactModule : public portals::Module
{
public:
    ArmContactModule();

    portals::InPortal<messages::JointAngles> actualJointsIn;
    portals::InPortal<messages::JointAngles> expectedJointsIn;
    // The hand speed is calculated in the walk from BHuman odometry and
    // passed in here
    portals::InPortal<messages::HandSpeeds> handSpeedsIn;

    portals::OutPortal<messages::ArmContactState> contactOut;

protected:
    virtual void run_();
    void determineContactState();

    std::queue<messages::JointAngles> expectedJoints;
    messages::JointAngles* jointsWithDelay;

    messages::ArmContactState::PushDirection rightArm, leftArm;
    messages::ArmContactState::PushDirection lastDetectedRight,
                                             lastDetectedLeft;
    int rightStuckCounter, leftStuckCounter;

    DisplacementBuffer right, left;
};

}
}
