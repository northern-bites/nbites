/*
 * @brief  Defines a module for tracking and predicting the ball
 *
 * @author EJ Googins <egoogins@bowdoin.edu>
 * @date   March 2013
 */
#pragma once

#include "RoboGrams.h"
#include "Common.pb.h"

namespace man
{
    namespace balltrack
    {

    class BallTrackModule : public portals::Module
    {
    public:
        BallTrackModule();
        ~BallTrackModule();

        portals::OutPortal<messages::RobotLocation> ballLocationOutput;

    protected:
        /**
         * @brief called through diagrams
         */
        void run_();

    private:
        float ballX;
        float ballY;
    };

    } //namespace balltrack
} //namespace man
