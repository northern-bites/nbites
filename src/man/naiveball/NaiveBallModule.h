#pragma once

#include "DebugConfig.h"

#include "RoboGrams.h"
#include "Common.h"
#include <math.h>
#include "FieldConstants.h"

#include <iostream>

#include "WorldModel.pb.h"
#include "BallModel.pb.h"
#include "RobotLocation.pb.h"

/*
 * Module
 *
 */

namespace man {
namespace context {

const float ALPHA = .7f;
const int NUM_FRAMES = 30;
const int MAX_FRAMES_OFF = 5;
const int AVGING_FRAMES = 5; // Number of frames to take an avg position estimate from
// Magic numbers...
// TODO how many frames per second?

class NaiveBallModule : public portals::Module
{
public:
    NaiveBallModule();
    virtual ~NaiveBallModule();

    virtual void run_();

    // portals::InPortal<messages::WorldModel> worldModelIn[NUM_PLAYERS_PER_TEAM];
    // portals::InPortal<messages::RobotLocation> locIn;
    portals::InPortal<messages::FilteredBall> ballIn;

    portals::OutPortal<messages::NaiveBall> naiveBallOutput;

private:
    struct BallState{
        BallState(float rel_x_, float rel_y_, float distance_, float bearing_) : rel_x(rel_x_), rel_y(rel_y_), distance(distance_), bearing(bearing_) {}
        float rel_x;
        float rel_y;
        float distance;
        float bearing;
    };

    void updateBuffer();
    void clearBuffer();
    void naiveCheck();

    BallState *ballStateBuffer;
    int currentIndex;
    int buffSize;
    int frameOffCount;
    float velocityEst;
    bool buffFull;

    messages::WorldModel worldMessages[NUM_PLAYERS_PER_TEAM];
    messages::FilteredBall myBall;

};

}
}
