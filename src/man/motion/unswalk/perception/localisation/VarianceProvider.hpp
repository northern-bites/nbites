/*
 * VarianceProvider.hpp
 *
 *  Created on: 15/12/2013
 *      Author: osushkov
 */

#pragma once

/**
 * Class that can be queried about the variance of observation measurements.
 * TODO: add the ability to get the variance of process updates using this class too.
 */
class VarianceProvider {
public:

   enum ObservationType {
      GOALPOST,
      BALL,
      CENTRE_CIRCLE,
      TEAMMATE_ROBOT,
   };

   struct Observation {
      const double distance;
      const double heading;

      Observation(double distance, double heading) :
         distance(distance), heading(heading) {}
   };

   
   /**
    * Singleton instance method. Returns an instance of the provider.
    */
   static const VarianceProvider& instance(void);

   /**
    * Gets the distance measurement variance for an observation of the given object type and with
    * the given observation parameters.
    */
   double getDistanceObservationVariance(
         ObservationType type, const Observation &observation) const;

   /**
    * Gets the heading measurement variance for an observation of the given object type and with
    * the given observation parameters.
    */
   double getHeadingObservationVariance(
         ObservationType type, const Observation &observation) const;
   
private:
   VarianceProvider();
   
   double getBallDistanceVariance(double distance) const;
   double getBallHeadingVariance(double distance) const;
   
   double getGoalpostDistanceVariance(double distance) const;
   double getGoalpostHeadingVariance(double distance) const;

   double getCentreCircleDistanceVariance(double distance) const;
   double getCentreCircleHeadingVariance(double distance) const;
   
   double getTeammateRobotDistanceVariance(double distance) const;
   double getTeammateRobotHeadingVariance(double distance) const;
};
