/*
Copyright 2014 The University of New South Wales (UNSW).

This file is part of the 2014 team rUNSWift RoboCup entry. You may
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

#include "types/UNSWRobotInfo.hpp"
#include "types/Odometry.hpp"
#include "types/RRCoord.hpp"

class RobotObservation {
public:
    /**
     * Creates a robot observation from a visual robot. If it is static, it
     * will never go stale and therefore be removed.
     */
    RobotObservation(const UNSWRobotInfo &visualRobot);

    /**
     * Apply odometry changes to the robot and update the staleness of the
     * observation.
     */
    void tick(Odometry odometryDiff, float headYaw, double distanceObservationToGroupPercentageOfMax);

    /**
     * Determines whether this observation should now be considered stale.
     */
    bool isStale() const;

    /**
     * Get the relative cartesian coordinates of the robot.
     */
    Point getCartesianCoordinates() const;

    /**
     * Get the relative vector to the robot.
     */
    RRCoord getRRCoordinates() const;

    /**
     * Get the weight of the robot, e.g. how important we should consider this
     * observation.
     */
    int getWeight() const;

    /**
     * Given a head yaw it determines whether this observation would be in the
     * field of view.
     */
    bool inView(float headYaw) const;

    /**
     * Get the minimum weight a robot can have.
     */
    static int getMinWeight();

    /**
     * Get the maximum weight a robot can have.
     */
    static int getMaxWeight();

    /**
     * Returns the number of frames that an observation would be alive if
     * it was onscreen the whole time.
     */
    static unsigned int numberOnScreenFramesToBeStale();

    /**
     * Returns the number of frames that an observation would be alive if
     * it was offscreen the whole time.
     */
    static unsigned int numberOffScreenFramesToBeStale();

    static const int MIN_WEIGHT;
    static const int WEIGHT_RANGE;

    static const int ESTIMATED_FRAMES_PER_SECOND;
    static const int MAX_LIFE_SCORE;

    static const double SECONDS_ALIVE_ON_FOV;
    static const double SECONDS_ALIVE_OFF_FOV;

    static const int OFF_SCREEN_SCORE_REDUCER;
    static const int ON_SCREEN_SCORE_REDUCER;
    static const int DISTANCE_SCORE_REDUCER;

private:

    RRCoord rr;
    UNSWRobotInfo::Type type;
    int lifeScore;
};
