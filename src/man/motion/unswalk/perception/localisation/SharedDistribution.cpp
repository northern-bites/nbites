/*
 * SharedDistribution.cpp
 *
 *  Created on: 22/04/2014
 *      Author: osushkov
 */

#include "SharedDistribution.hpp"
#include "LocalisationConstantsProvider.hpp"

static const LocalisationConstantsProvider& constantsProvider(
      LocalisationConstantsProvider::instance());

SharedDistribution::SharedDistribution() :
      ballSeenCount(0),
      framesCount(0),
      haveVisionUpdates(false),
      sharedGaussian(SimpleGaussian::createBaselineSharedGaussian()),
      sharedDx(0.0),
      sharedDy(0.0),
      sharedDh(0.0),
      sharedCovarianceDx(0.0),
      sharedCovarianceDy(0.0),
      sharedCovarianceDh(0.0) {}

SharedDistribution::~SharedDistribution() {
   delete sharedGaussian;
}
   
void SharedDistribution::processUpdate(
      const Odometry &odometry, const double dTimeSeconds, const bool canSeeBall) {
   OdometryUpdateResult odometryResult = sharedGaussian->processUpdate(odometry, dTimeSeconds, canSeeBall);

   sharedDx += odometryResult.dx;
   sharedDy += odometryResult.dy;
   sharedDh += odometryResult.dh;

   sharedCovarianceDx += odometryResult.covDx;
   sharedCovarianceDy += odometryResult.covDy;
   sharedCovarianceDh += odometryResult.covDh;
}

void SharedDistribution::visionUpdate(const StoredICPUpdate &icpUpdate, const UniModalVisionUpdate &vu) {
   framesCount++;
   if (vu.visionBundle.visibleBalls.size() > 0) {
      ballSeenCount++;
   }
   
   sharedGaussian->applyObservation(icpUpdate.updateDimension, icpUpdate.innovation, icpUpdate.jacobian,
         icpUpdate.observationVariance, false);
   int obsDim = sharedGaussian->uniModalVisionUpdate(vu);
   
   if (obsDim > 0 || icpUpdate.updateDimension > 0) {
      haveVisionUpdates = true;
   }
}

SharedLocalisationUpdateBundle SharedDistribution::getBroadcastData(void) const {
   double ballSeenFraction = 0.0;
   if (framesCount > 0) {
      ballSeenFraction = (double) ballSeenCount / (double) framesCount;
   }
   
   return SharedLocalisationUpdateBundle(
         ballSeenFraction,
         sharedGaussian->getMean(),
         sharedGaussian->getCovariance(),
         haveVisionUpdates,
         ballSeenCount > 0,
         sharedDx,
         sharedDy,
         sharedDh,
         sharedCovarianceDx,
         sharedCovarianceDy,
         sharedCovarianceDh);
}
   
void SharedDistribution::reset(const SimpleGaussian *topDistribution) {
   sharedGaussian->resetMean(topDistribution->getMean());
   
   double covarianceScaleFactor = constantsProvider.get(
         LocalisationConstantsProvider::SHARED_GAUSSIAN_RESET_UNCERTAINTY_FACTOR);
   sharedGaussian->resetCovariance(topDistribution->getCovariance() * covarianceScaleFactor);
   
   sharedDx = 0.0;
   sharedDy = 0.0;
   sharedDh = 0.0;
   
   sharedCovarianceDx = 0.0;
   sharedCovarianceDy = 0.0;
   sharedCovarianceDh = 0.0;
   
   haveVisionUpdates = false;
   
   ballSeenCount = 0;
   framesCount = 0;
}

void SharedDistribution::setReadyMode(bool enabled) {
   sharedGaussian->setReadyMode(enabled);
}
