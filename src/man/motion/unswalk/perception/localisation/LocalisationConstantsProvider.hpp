/*
Copyright 2013 The University of New South Wales (UNSW).

This file is part of the 2010 team rUNSWift RoboCup entry. You may
redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version as
modified below. As the original licensors, we add the following
conditions to that license:

In paragraph 2.b), the phrase "distribute or publish" should be
interpreted to include entry into a competition, and hence the source
of any derived work entered into a competition must be made available
to all parties involved in that competition under the terms of this
license.

In addition, if the authors of a derived work publish any conference
proceedings, journal articles or other academic papers describing that
derived work, then appropriate academic citations to the original work
must be included in that publication.

This rUNSWift source is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License along
with this source code; if not, write to the Free Software Foundation,
Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#pragma once

#include <vector>

/**
 * A provider for constants used in the localisation code. This is so we dont have to use magic numbers
 * in the code and can easily later swap out values to test how well they work automatically.
 */
class LocalisationConstantsProvider {
public:
   /**
    * Enum for the various constants that are used by the localisation code. These are used for
    * querying the LocalisationConstantsProvider.
    */
   enum LocalisationConstant {
      // TODO: add description for each key
      
      // When deciding whether to merge two modes, we compare their mean vectors. If their
      // mean vectors differ by less than a threshold amount, we can merge the two modes.
      MEAN_DIST_SIMILARITY_THRESHOLD, // The x,y of the robot pose estimate must be within this threshold to merge.
      
      // NOTE: this param is part of a hack to address a overzealous merging near goals, This threshold is much lower
      // and should be used when the robot is near his or opponent goals.
      MEAN_DIST_SIMILARITY_THRESHOLD_NEAR_GOALS, 
      
      // The robot heading component of the mean vector must be within this threshold amount to merge two modes.
      MEAN_HEADING_SIMILARITY_THRESHOLD,
      // NOTE: same as above, this is part of a hack and is a much lower value than the normal heading threshold.
      MEAN_HEADING_SIMILARITY_THRESHOLD_NEAR_GOALS,
      
      // When merging modes we also check that their covariance matrices are within a threshold amount.
      COVARIANCE_DIST_SIMILARITY_THRESHOLD,
      
      
      // When updating the ball's velocity during the process update step, we decay the velocity by this amount.
      // This is a "per-second" decay factor.
      BALL_FRICTION,
      
      
      // When we do a motion update of the covariance matrix we add uncertainty.
      // The amount of uncertainty is dependent on the motion performed by the robot.
      // We relate the uncertainty to the motion using a linear function, with the
      // parameters A and C, so additional_uncertainty = A*dx + C*dt.
      ROBOT_POS_MOTION_UPDATE_COVARIANCE_A,
      ROBOT_POS_MOTION_UPDATE_COVARIANCE_C, // per second
      ROBOT_HEADING_MOTION_UPDATE_COVARIANCE_A,
      ROBOT_HEADING_MOTION_UPDATE_COVARIANCE_C, // per second
      
      // During the process update, we increase the ball's position uncertainty by this amount.
      BALL_POS_MOTION_UPDATE_COVARIANCE_C, // per second
      BALL_UNSEEN_POS_MOTION_UPDATE_COVARIANCE_C, // per second (this is used when we cant see the ball).
      BALL_POS_MOTION_UPDATE_LINE_UP_COVARIANCE_C, // use this when lining up to kick
      
      // During the process update, we increase the ball's velocity uncertainty by this amount.
      BALL_VEL_MOTION_UPDATE_COVARIANCE_C, // per second
      
      // When we receive a remote update from a teammate, we incorporate it into our state filter.
      // However, we treat teammate's observations as less reliable than our own.
      
      // Increase the variance of teammate's observations by this factor.
      REMOTE_OBSERVATION_UNCERTAINTY_FACTOR, 
      // When resettings our shared update gaussian, multiply its covariance by this factor.
      SHARED_GAUSSIAN_RESET_UNCERTAINTY_FACTOR, 
      // When updating the uncertainty of a teammate's pose in our filter, we scale the uncertainty
      // increase by this factor.
      TEAMMATE_ODOMETRY_UNCERTAINTY_SCALE, 

      
      // The factor by which we scale the weight of a mode when we split it in two. One has the
      // observation applied, the other doesn't. The mode which doesnt have the observation
      // applied is scaled by this factor.
      INVALID_OBSERVATION_PROBABILITY,
      // ICP update's weight must be above this amount to be considered "valid"
      INVALID_ICP_PROBABILITY,
      // When we get a remote update from a teammate, the weight of the update applied to our state
      // estimation must be above this amount to be considered valud.
      INVALID_REMOTE_OBSERVATION_PROBABILITY,
      // Similar to the value above, but goalie is much more strict when accepting a teammate update,
      // because we dont want a teammates observations drifting our goalie away.
      INVALID_REMOTE_OBSERVATION_PROBABILITY_GOALIE,
      INVALID_TEAMMATE_ROBOT_OBSERVATION_PROBABILITY,
      MIN_MODE_WEIGHT, // The minimum weight of a mode before it is purged as too unlikely.
      
      // We can generate a mode that is symmetric to the current mode. Its weight is set to the parent
      // mode's weight multiplied by this amount.
      SYMMETRIC_MODE_WEIGHT,
      TEAMMATE_FLIP_WEIGHT, // If teammates thing our mode is wrong/flipped, scale it down by this much.
      
      // The ball must be at least this far from centre to create symmetric modes.
      MIN_SYMMETRIC_BALL_DISTANCE_FROM_CENTRE,
      
      UNRELIABLE_DISTANCE_CUTOFF_FACTOR,
      UNRELIABLE_DISTANCE_VARIANCE_SCALE,
      UNRELIABLE_HEADING_VARIANCE_SCALE,
      
      // The maximum distance to the ball, goalpost, and centre circle which we would use in an
      // observation update. Any further observations will only use heading.
      BALL_MAX_DISTANCE_OBSERVATION,
      GOALPOST_MAX_DISTANCE_OBSERVATION,
      CENTRE_CIRCLE_MAX_DISTANCE_OBSERVATION,
      TEAMMATE_ROBOT_MAX_DISTANCE_OBSERVATION,
      
      // The heading variance of a landmark observation. We assume that the heading variance is the
      // same regardless of the type of landmark observed.
      HEADING_VARIANCE,
      
      // We calculate the distance variance of a landmark observation to be:
      // exp(DISTANCE_VARIANCE_B * distance + DISTANCE_VARIANCE_C); 
      DISTANCE_VARIANCE_B,
      DISTANCE_VARIANCE_C,
      
      TEAMMATE_ROBOT_HEADING_VARIANCE,
      TEAMMATE_ROBOT_DISTANCE_VARIANCE_SCALE,

      // The variance of the x,y position estimate of the ICP update is scaled by this amount.
      ICP_DISTANCE_VARIANCE_SCALE,
      // The variance of the heading estimate of the ICP update.
      ICP_HEADING_VARIANCE,
      
      NUM_CONSTANTS
   };


   /**
    * Singleton instance method. Returns an instance of the provider.
    */
   static LocalisationConstantsProvider& instance(void);

   /**
    * Gets the double value of the given localisation constant.
    */
   double get(LocalisationConstant key) const;
   
   /**
    * This should only be used by the offloc utility to optimise the parameters. There is not rason
    * that any robot code should call this method.
    */
   void set(LocalisationConstant key, double value);
   
   void setReadyMode(bool enabled);

private:
   std::vector<double> keyValues;

   LocalisationConstantsProvider();
};
