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
        }

        BallFilter::~BallFilter(){}

        void BallFilter::addObservation(BallObservation newObsv)
        {
            curEntry = (curEntry+1)%bufferSize;
            obsvBuffer[curEntry] = newObsv;
        }

        BallObservation BallFilter::getObsv(int which)
        {
            return obsvBuffer[curEntry + which];
        }

        BallObservation BallFilter::getEstimate()
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

    } //namespace balltrack
} //namespace man
