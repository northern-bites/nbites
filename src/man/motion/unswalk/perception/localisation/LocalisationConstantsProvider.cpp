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

#include "LocalisationConstantsProvider.hpp"
#include "utils/basic_maths.hpp"
#include "utils/angles.hpp"
#include <cmath>
#include <iostream>

LocalisationConstantsProvider::LocalisationConstantsProvider() :
      keyValues(static_cast<int>(NUM_CONSTANTS)) {

   set(MEAN_DIST_SIMILARITY_THRESHOLD, 200.0);
   set(MEAN_DIST_SIMILARITY_THRESHOLD_NEAR_GOALS, 100.0);
   
   set(MEAN_HEADING_SIMILARITY_THRESHOLD, UNSWDEG2RAD(20.0));
   set(MEAN_HEADING_SIMILARITY_THRESHOLD_NEAR_GOALS, UNSWDEG2RAD(10.0));
   
   set(COVARIANCE_DIST_SIMILARITY_THRESHOLD, 400000.0);
   set(BALL_FRICTION, 0.9);
   
   set(ROBOT_POS_MOTION_UPDATE_COVARIANCE_A, 2.75);
   set(ROBOT_POS_MOTION_UPDATE_COVARIANCE_C, 85437.3);
   set(ROBOT_HEADING_MOTION_UPDATE_COVARIANCE_A, 3.11);
   set(ROBOT_HEADING_MOTION_UPDATE_COVARIANCE_C, 0.8);
   
   set(BALL_POS_MOTION_UPDATE_COVARIANCE_C, 344290.0);
   set(BALL_UNSEEN_POS_MOTION_UPDATE_COVARIANCE_C, 4000000.0); // 2m per second
   set(BALL_POS_MOTION_UPDATE_LINE_UP_COVARIANCE_C, 150000.0); // ~40cm per second
   set(BALL_VEL_MOTION_UPDATE_COVARIANCE_C, 361396.0);
   
   set(REMOTE_OBSERVATION_UNCERTAINTY_FACTOR, 4.0);
   set(SHARED_GAUSSIAN_RESET_UNCERTAINTY_FACTOR, 1000.0);
   set(TEAMMATE_ODOMETRY_UNCERTAINTY_SCALE, 4.0);
   
   set(INVALID_OBSERVATION_PROBABILITY, 0.3);
   set(INVALID_ICP_PROBABILITY, 0.8);
   set(INVALID_REMOTE_OBSERVATION_PROBABILITY, 0.5);
   set(INVALID_REMOTE_OBSERVATION_PROBABILITY_GOALIE, 0.8);
   set(INVALID_TEAMMATE_ROBOT_OBSERVATION_PROBABILITY, 0.8);
   
   set(MIN_MODE_WEIGHT, 0.001);
   
   set(SYMMETRIC_MODE_WEIGHT, 0.002);
   set(TEAMMATE_FLIP_WEIGHT, 0.01);
   
   set(UNRELIABLE_DISTANCE_CUTOFF_FACTOR, 0.66);
   set(UNRELIABLE_DISTANCE_VARIANCE_SCALE, 4.0);
   set(UNRELIABLE_HEADING_VARIANCE_SCALE, 2.0);
   
   set(BALL_MAX_DISTANCE_OBSERVATION, 4000.0);
   set(GOALPOST_MAX_DISTANCE_OBSERVATION, 4000.0);
   set(CENTRE_CIRCLE_MAX_DISTANCE_OBSERVATION, 4000.0);
   set(TEAMMATE_ROBOT_MAX_DISTANCE_OBSERVATION, 3000.0);

   set(HEADING_VARIANCE, 0.3);
   set(DISTANCE_VARIANCE_B, 0.00144844);
   set(DISTANCE_VARIANCE_C, 9.04618);
   
   set(TEAMMATE_ROBOT_HEADING_VARIANCE, 0.8);
   set(TEAMMATE_ROBOT_DISTANCE_VARIANCE_SCALE, 9.0);
   
   set(ICP_DISTANCE_VARIANCE_SCALE, 1.0);
   set(ICP_HEADING_VARIANCE, 0.4);
}

LocalisationConstantsProvider& LocalisationConstantsProvider::instance(void) {
   static LocalisationConstantsProvider provider;
   return provider;
}

double LocalisationConstantsProvider::get(LocalisationConstant key) const {
   return keyValues[static_cast<int>(key)];
}

void LocalisationConstantsProvider::set(LocalisationConstant key, double value) {
   keyValues[static_cast<int>(key)] = value;
}

void LocalisationConstantsProvider::setReadyMode(bool enabled) {
   // Because I cbf writing this properly anymore, 4 days to go... TODO: fix this ugly hack.
   if (enabled) {
      set(UNRELIABLE_DISTANCE_CUTOFF_FACTOR, 0.5);
      set(GOALPOST_MAX_DISTANCE_OBSERVATION, 5000.0);
   } else {
      set(UNRELIABLE_DISTANCE_CUTOFF_FACTOR, 0.66);
      set(GOALPOST_MAX_DISTANCE_OBSERVATION, 4000.0);
   }
}
