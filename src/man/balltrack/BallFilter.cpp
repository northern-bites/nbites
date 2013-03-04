#include "BallFilter.h"

namespace man
{
    namespace balltrack
    {
        BallFilter::BallFilter(int bufferSize_)
        {
            bufferSize = bufferSize_;
            obsvBuffer = new BallObservation[bufferSize];
            curEntry = 0;
            curExpEstimate.dist = 0;
            curExpEstimate.bear = 0;
        }

        BallFilter::~BallFilter(){}

        void BallFilter::addObservation(BallObservation newObsv)
        {
            curEntry = (curEntry+1)%bufferSize;
            obsvBuffer[curEntry] = newObsv;

            curExpEstimate.dist = newObsv.dist * ALPHA +
                                      curExpEstimate.dist * (1 - ALPHA);
            curExpEstimate.bear = newObsv.bear * ALPHA +
                                      curExpEstimate.bear * (1 - ALPHA);
        }

        BallObservation BallFilter::getObsv(int which)
        {
            return obsvBuffer[curEntry + which];
        }

        BallObservation BallFilter::getNaiveEstimate()
        {
            float distSum = 0;
            float bearSum = 0;
            for(int i=0; i<bufferSize; i++)
            {
                distSum += getObsv(i).dist;
                bearSum += getObsv(i).bear;
            }

            return BallObservation(distSum/(float)bufferSize,
                                   bearSum/(float)bufferSize);
        }

        BallObservation BallFilter::getWeightedNaiveEstimate()
        {
            float distSum = 0;
            float bearSum = 0;
            for(int i=0; i<bufferSize; i++)
            {
                distSum += getObsv(i).dist * (float)(bufferSize - i);
                bearSum += getObsv(i).bear * (float)(bufferSize - i);
            }

            float totalWeights = (float)(bufferSize*(bufferSize-1))/2;

            return BallObservation(distSum/(float)totalWeights,
                                   bearSum/(float)totalWeights);
        }

        BallObservation BallFilter::getExponentialEstimate()
        {
            return curExpEstimate;
        }

    } //namespace balltrack
} //namespace man
