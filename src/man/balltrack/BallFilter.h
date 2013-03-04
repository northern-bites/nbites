/**
 * @brief  A naive implementation of some ball trackers for shits
 *         Implement a ring buffer to take obsv, average for estimate
 *         Wipe if threshold observations rejected in a row
 *
 * @author EJ Googins <egoogins@bowdoin.edu>
 * @date   March 2013
 */

#pragma once

namespace man
{
    namespace balltrack
    {
        static const int DEFAULT_BUFFER_SIZE = 30;
        static const float ALPHA = .6f;

        /*
         * @brief Struct to take observations from vision
         */
        struct BallObservation
        {
            BallObservation(float dist_, float bear_) : dist(dist_), bear(bear) {}
            BallObservation() {}
            float dist;
            float bear;
        };

        class BallFilter
        {
        public:
            BallFilter(int bufferSize_ = DEFAULT_BUFFER_SIZE);
            ~BallFilter();

            void addObservation(BallObservation newObsv);
            BallObservation getObsv(int which);

            BallObservation getNaiveEstimate();
            BallObservation getExponentialEstimate();
            BallObservation getWeightedNaiveEstimate();

        private:
            BallObservation *obsvBuffer;
            int curEntry;
            int bufferSize;

            BallObservation curExpEstimate;
        };
    } //namespace balltrack
} //namespace man
