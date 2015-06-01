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
namespace naive {

const float ALPHA = .7f;
const int NUM_FRAMES = 30;
const int MAX_FRAMES_OFF = 5;
const int AVGING_FRAMES = 5; // Number of frames to take an avg position estimate from
// Magic numbers...
// TODO how many frames per second?

class NaiveBallModule : public portals::Module
{
public:
    struct BallState;
    NaiveBallModule();
    virtual ~NaiveBallModule();

    virtual void run_();

    // portals::InPortal<messages::WorldModel> worldModelIn[NUM_PLAYERS_PER_TEAM];
    // portals::InPortal<messages::RobotLocation> locIn;
    portals::InPortal<messages::FilteredBall> ballIn;

    portals::OutPortal<messages::NaiveBall> naiveBallOutput;

private:
    float calcSumSquaresSQRT(float a, float b);
    void updateBuffer();
    void clearBuffer();
    void naiveCheck();
    BallState avgFrames(int startingIndex);
    void print();
    void printBallState(BallState x);
    void printBuffer();

    BallState *ballStateBuffer;
    int currentIndex;
    int buffSize;
    int frameOffCount;
    int count;
    float velocityEst;
    bool bufferFull;
    messages::WorldModel worldMessages[NUM_PLAYERS_PER_TEAM];
    messages::FilteredBall myBall;

};

}
}
