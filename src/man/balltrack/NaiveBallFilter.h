/**
 * @brief  A naive implementation of some ball trackers for shits
 *         Implement a ring buffer to take obsv, average for estimate
 *         Wipe if threshold observations rejected in a row
 *
 * @author EJ Googins <egoogins@bowdoin.edu>
 * @date   March 2013
 */

#pragma once

#include "NBMath.h"
#include "BallTrackStructs.h"

#include "BallModel.pb.h"
#include "Motion.pb.h"

namespace man
{
    namespace balltrack
    {
        static const int DEFAULT_BUFFER_SIZE = 30;
        static const float ALPHA = .7f;

        class NaiveBallFilter
        {
        public:
            NaiveBallFilter(int bufferSize_ = DEFAULT_BUFFER_SIZE);
            ~NaiveBallFilter();

            void update(messages::VisionBall visionBall,
                        messages::Motion odometry);

            void addObservation(BallObservation newObsv);
            BallObservation getObsv(int which);

            BallObservation getNaiveEstimate();
            CartesianBallEstimate getCartesianNaiveEstimate();
            BallObservation getExponentialEstimate();
            CartesianBallEstimate getCartesianExponentialEstimate();
            BallObservation getWeightedNaiveEstimate();
            CartesianBallEstimate getCartesianWeightedNaiveEstimate();

            CartesianBallEstimate getCartesianRep(BallObservation obsv);

            int getBufferSize(){return bufferSize;};

        private:
            BallObservation *obsvBuffer;
            int curEntry;
            int bufferSize;

            BallObservation curExpEstimate;
        };
    } //namespace balltrack
} //namespace man
