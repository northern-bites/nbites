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

#include "types/Odometry.hpp"
#include "SimpleGaussian.hpp"
#include "SharedLocalisationUpdateBundle.hpp"

/**
 * The SharedDistribution is the distribution that encapsulates the updates that are sent to
 * our teammates so that they can incorporate them into their state estimation. This
 * distribution consists of a single mode, and as such only accepts UniModalVisionUpdates. The 
 * way that we get a uni-modal vision update from a normal vision update is to take the
 * UniModalVisionUpdate from the top weighted mode from the main distribution. 
 * Another caveat of the SharedDistribution is that we reset it every time it is broadcast
 * to our teammates. This is done to prevent 'double counting' an observation. IF we did not
 * reset, then when our teammates receive the update and apply it, it would in effect be applying
 * the same observation multiple times.
 */
class SharedDistribution {
public:
   SharedDistribution();
   virtual ~SharedDistribution();
   
   void processUpdate(const Odometry &odometry, const double dTimeSeconds, const bool canSeeBall);

   void visionUpdate(const StoredICPUpdate &icpUpdate, const UniModalVisionUpdate &vu);

   /**
    * Returns the data bundle to broadcast to our teammates.
    */
   SharedLocalisationUpdateBundle getBroadcastData(void) const;
   
   /**
    * Reset the distribution. This basically sets the covariance of the state estimation to be 
    * very high and takes the mean of the given best distribution from the main distribution.
    * This should be called every time we broadcast to our teammates the SharedDistribution.
    */
   void reset(const SimpleGaussian *topDistribution);
   void setReadyMode(bool enabled);
   
private:
   unsigned ballSeenCount;
   unsigned framesCount;
   
   bool haveVisionUpdates;
   SimpleGaussian *sharedGaussian;

   double sharedDx;
   double sharedDy;
   double sharedDh;
   
   double sharedCovarianceDx;
   double sharedCovarianceDy;
   double sharedCovarianceDh;
};
