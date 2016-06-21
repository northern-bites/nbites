/*
 * VarianceProvider.cpp
 *
 *  Created on: 15/12/2013
 *      Author: osushkov
 */

#include "VarianceProvider.hpp"
#include "LocalisationConstantsProvider.hpp"
#include "utils/angles.hpp"
#include "utils/basic_maths.hpp"
#include <cmath>

#define LARGE_VARIANCE SQUARE(10000.0)

static const LocalisationConstantsProvider& constantsProvider(
      LocalisationConstantsProvider::instance());

const VarianceProvider& VarianceProvider::instance(void) {
   static const VarianceProvider provider;
   return provider;
}

VarianceProvider::VarianceProvider() {}

double VarianceProvider::getDistanceObservationVariance(
      ObservationType type, const Observation &observation) const {
   switch (type) {
   case GOALPOST:
      return getGoalpostDistanceVariance(observation.distance);
   case BALL:
      return getBallDistanceVariance(observation.distance);
   case CENTRE_CIRCLE:
      return getCentreCircleDistanceVariance(observation.distance);
   case TEAMMATE_ROBOT:
      return getTeammateRobotDistanceVariance(observation.distance);
   default:
      return LARGE_VARIANCE; // This should neven happen but just in case. 
   }
}

double VarianceProvider::getHeadingObservationVariance(
      ObservationType type, const Observation &observation) const {
   switch (type) {
   case GOALPOST:
      return getGoalpostHeadingVariance(observation.distance);
   case BALL:
      return getBallHeadingVariance(observation.distance);
   case CENTRE_CIRCLE:
      return getCentreCircleHeadingVariance(observation.distance);
   case TEAMMATE_ROBOT:
      return getTeammateRobotHeadingVariance(observation.distance);
   default:
      return LARGE_VARIANCE; // This should neven happen but just in case.
   }
}

double VarianceProvider::getBallDistanceVariance(double distance) const {
   // TODO: this needs much more investigation, same with all the rest.
   double b = constantsProvider.get(LocalisationConstantsProvider::DISTANCE_VARIANCE_B);
   double c = constantsProvider.get(LocalisationConstantsProvider::DISTANCE_VARIANCE_C);
   double result = exp(b * distance + c); 
   if (result > 10000.0 * 10000.0) {
      return 10000.0 * 10000.0;
   } else {
      return result;
   }
}

double VarianceProvider::getBallHeadingVariance(double distance) const {
   return constantsProvider.get(LocalisationConstantsProvider::HEADING_VARIANCE);
}
   
double VarianceProvider::getGoalpostDistanceVariance(double distance) const {
   return getBallDistanceVariance(distance);
}

double VarianceProvider::getGoalpostHeadingVariance(double distance) const {
   return constantsProvider.get(LocalisationConstantsProvider::HEADING_VARIANCE);
}

double VarianceProvider::getCentreCircleDistanceVariance(double distance) const {
   return getBallDistanceVariance(distance);
}

double VarianceProvider::getCentreCircleHeadingVariance(double distance) const {
   return constantsProvider.get(LocalisationConstantsProvider::HEADING_VARIANCE);
}

double VarianceProvider::getTeammateRobotDistanceVariance(double distance) const {
   return getBallDistanceVariance(distance) * 
         constantsProvider.get(LocalisationConstantsProvider::TEAMMATE_ROBOT_DISTANCE_VARIANCE_SCALE);
}

double VarianceProvider::getTeammateRobotHeadingVariance(double distance) const {
   return constantsProvider.get(LocalisationConstantsProvider::TEAMMATE_ROBOT_HEADING_VARIANCE);
}

