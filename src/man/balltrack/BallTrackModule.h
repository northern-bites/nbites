/*
 * @brief  Defines a module for tracking and predicting the ball
 *
 * @author EJ Googins <egoogins@bowdoin.edu>
 * @date   March 2013
 */
#pragma once

#include "BallFilter.h"

#include "RoboGrams.h"
#include "BallModel.pb.h"

#include <boost/shared_ptr.hpp>

namespace man
{
    namespace balltrack
    {

    class BallTrackModule : public portals::Module
    {
    public:
        BallTrackModule();
        ~BallTrackModule();

        portals::InPortal<messages::VisionBall> visionBallInput;
        portals::OutPortal<messages::FilteredBall> ballLocationOutput;

    protected:
        /**
         * @brief called through diagrams
         */
        void run_();

    private:
//        boost::shared_ptr<BallFilter> ballFilter;
        BallFilter *ballFilter;

        float ballX;
        float ballY;
    };

    } //namespace balltrack
} //namespace man
