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
        static const float FINAL_H = 1.f;

        static const float NUM_FRAMES = 60;


        class FakeLocInputModule : public portals::Module
        {
        public:
            FakeLocInputModule();
            ~FakeLocInputModule();

            portals::OutPortal<messages::Motion> fMotionOutput;
            portals::OutPortal<messages::PVisionField> fVisionOutput;

        protected:
            void run_();

        private:

            void calcDeltaMotion();
            void genNoisyOdometry();

            float timestamp;
            float frames;

            messages::RobotLocation initialLocation;
            messages::RobotLocation currentLocation;
            messages::RobotLocation finalLocation;

            messages::RobotLocation odometry;
            messages::RobotLocation noisyOdometry;
            messages::Motion        noisyMotion;

        };
    } //namespace fakeInput
} //namespace man
