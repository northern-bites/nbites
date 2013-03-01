/*
 * @brief  This is a FAKE motion module for testing the loc module
 * @author EJ Googins <egoogins@bowdoin.edu>
 * @date   Febraury 2013
 *
 */
#pragma once

#include "RoboGrams.h"
#include "Motion.pb.h"
#include "Vision.pb.h"
#include "Common.pb.h"

#include "NBMath.h"

#include <iostream>
#include <fstream>

#include <boost/random.hpp>

namespace man
{
    namespace fakeInput
    {
        static const float INITIAL_X = 100.f;
        static const float INITIAL_Y = 100.f;
        static const float INITIAL_H = 0.f;

        static const float FINAL_X = 150.f;
        static const float FINAL_Y = 150.f;
        static const float FINAL_H = 1.3f;

        static const float NUM_FRAMES = 319;

        static const float MIN_OBS_DIST = 50;
        static const float MAX_OBS_DIST = 150;
        static const float DIST_STD_DEV = 10;
        static const float BEAR_STD_DEV = .174f; //~10 degrees

        static const float NUM_CORNER_OBS = 2;
        static const float NUM_GOAL_OBS = 1;
        static const bool CROSS_OBS = true;


        class FakeLocInputModule : public portals::Module
        {
        public:
            FakeLocInputModule();
            ~FakeLocInputModule();

            portals::OutPortal<messages::Motion> fMotionOutput;
            portals::OutPortal<messages::PVisionField> fVisionOutput;

            boost::mt19937 rng;

        protected:
            void run_();

        private:
            void calcDeltaMotion();
            void genNoisyOdometry();

            void addCornerObservation();
            void addGoalObservation(bool rightGoal);
            void addCrossObservation();

            void genNoisyVision();
            void genVisualDetection(messages::PVisualDetection &visualObservation);

            float timestamp;
            float frames;

            messages::RobotLocation initialLocation;
            messages::RobotLocation currentLocation;
            messages::RobotLocation finalLocation;

            messages::RobotLocation odometry;
            messages::RobotLocation noisyOdometry;

            messages::PVisionField  noisyVision;
            messages::Motion        noisyMotion;




        };
    } //namespace fakeInput
} //namespace man
