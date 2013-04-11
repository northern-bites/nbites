/*
 * @brief  This is a FAKE module for providing information
 *         to the ball track modules for testing
 * @author EJ Googins <egoogins@bowdoin.edu>
 * @date   March 2013
 */

#pragma once

#include "RoboGrams.h"
#include "Motion.pb.h"
#include "BallModel.pb.h"

#include <boost/random.hpp>

namespace man
{
    namespace balltrack
    {

    static const float INITIAL_X = 100.f;
    static const float INITIAL_Y = 100.f;
    static const float INITIAL_H = 0.f;

    static const float FINAL_X = 100.f;
    static const float FINAL_Y = 100.f;
    static const float FINAL_H = 0.f;

    static const float MIN_OBS_DIST = 50;
    static const float MAX_OBS_DIST = 150;
    static const float MIN_OBS_BEAR = -.785398f;
    static const float MAX_OBS_BEAR = .785398f;

    static const float INITIAL_BALL_DIST = 75.f;
    static const float INITIAL_BALL_BEAR = 0.f;

    static const float DIST_STD_DEV = 10.f;
    static const float BEAR_STD_DEV = .174f;

    class FakeBallInputModule : public portals::Module
    {
    public:
        FakeBallInputModule();
        ~FakeBallInputModule();

        portals::OutPortal<messages::VisionBall> fVisionBallOutput;
        portals::OutPortal<messages::Motion> fMotionOutput;

        boost::mt19937 rng;

    protected:
        void run_();

    private:
        void updateNoisyMotion();
        void updateNoisyVision();

        float timestamp;

        messages::RobotLocation currentLocation;
        messages::RobotLocation currentOdometry;

        float curBallDist;
        float curBallBear;
        messages::VisionBall    currentBallObsv;

        messages::VisionBall    noisyVision;
        messages::Motion        noisyMotion;
    };

    } //namespace balltrack
} // namespace man
