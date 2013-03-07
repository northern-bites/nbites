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

        void BallFilter::update(messages::VisionBall visionBall)
        {
            addObservation(BallObservation(visionBall.distance(),
                                           visionBall.bearing()));
        }

        void BallFilter::addObservation(BallObservation newObsv)
        {
            curEntry = (curEntry+1)%bufferSize;
            obsvBuffer[curEntry] = newObsv;

            curExpEstimate.dist = newObsv.dist * ALPHA +
                                      curExpEstimate.dist * (1 - ALPHA);
            curExpEstimate.bear = newObsv.bear * ALPHA +
                                      curExpEstimate.bear * (1 - ALPHA);
        }

        CartesianBallEstimate BallFilter::getCartesianRep(BallObservation obsv)
        {
            float sinD, cosD;
            sincosf(obsv.bear, &sinD, &cosD);
            return CartesianBallEstimate(obsv.dist*cosD,
                                         obsv.dist*sinD);
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

        CartesianBallEstimate BallFilter::getCartesianNaiveEstimate()
        {
            return getCartesianRep(getNaiveEstimate());
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

        CartesianBallEstimate BallFilter::getCartesianWeightedNaiveEstimate()
        {
            return getCartesianRep(getWeightedNaiveEstimate());
        }

        BallObservation BallFilter::getExponentialEstimate()
        {
            return curExpEstimate;
        }

        CartesianBallEstimate BallFilter::getCartesianExponentialEstimate()
        {
            return getCartesianRep(getExponentialEstimate());
        }

    } //namespace balltrack
} //namespace man
