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

#include "RobotObservation.hpp"

#include <Eigen/Eigen>
#include "types/AbsCoord.hpp"
#include "utils/angles.hpp"
#include "perception/vision/VisionDefs.hpp"

using namespace Eigen;

const int RobotObservation::ESTIMATED_FRAMES_PER_SECOND = 20;


const int RobotObservation::MIN_WEIGHT = 1;
const int RobotObservation::WEIGHT_RANGE = 20;

const double RobotObservation::SECONDS_ALIVE_ON_FOV = 1;
const double RobotObservation::SECONDS_ALIVE_OFF_FOV = 10;

const int RobotObservation::MAX_LIFE_SCORE =
        RobotObservation::SECONDS_ALIVE_OFF_FOV * ESTIMATED_FRAMES_PER_SECOND;

static const double SECONDS_ALIVE_RATIO =
        RobotObservation::SECONDS_ALIVE_OFF_FOV / RobotObservation::SECONDS_ALIVE_ON_FOV;

const int RobotObservation::OFF_SCREEN_SCORE_REDUCER = 1;
const int RobotObservation::ON_SCREEN_SCORE_REDUCER = SECONDS_ALIVE_RATIO * OFF_SCREEN_SCORE_REDUCER;
const int RobotObservation::DISTANCE_SCORE_REDUCER = 4; //Max life score reduced per tick for being far away.


RobotObservation::RobotObservation(const UNSWRobotInfo &visualRobot) :
    lifeScore(MAX_LIFE_SCORE) {

    rr = visualRobot.rr;
    type = visualRobot.type;
}


void RobotObservation::tick(Odometry odometryDiff, float headYaw, double distanceObservationToGroupPercentageOfMax) {
    Point cartesianCoords = rr.toCartesian();
    cartesianCoords[0] -= odometryDiff.forward;
    cartesianCoords[1] -= odometryDiff.left;

    rr = AbsCoord(cartesianCoords[0], cartesianCoords[1], 0).convertToRobotRelative();
    rr.heading() -= odometryDiff.turn;

    lifeScore -= (distanceObservationToGroupPercentageOfMax * DISTANCE_SCORE_REDUCER);
    bool onScreen = inView(headYaw);
    if (onScreen) {
        lifeScore -= ON_SCREEN_SCORE_REDUCER;
    } else {
        lifeScore -= OFF_SCREEN_SCORE_REDUCER;
    }
}

/**`
 * Calculates the weight for this given observation. It weights a recently seen
 * observations significantly more then old so it is using quadratic functions.
 */
int RobotObservation::getWeight() const {
    double lifeScoreSquared = lifeScore * lifeScore;
    double maxScoreSquared = MAX_LIFE_SCORE * MAX_LIFE_SCORE;

    return (lifeScoreSquared / maxScoreSquared) * WEIGHT_RANGE + MIN_WEIGHT;
}

bool RobotObservation::inView(float headYaw) const {
    float visibleLo = headYaw - 0.5f * IMAGE_HFOV;
    float visibleHi = headYaw + 0.5f * IMAGE_HFOV;

    return (visibleLo <= rr.heading() && rr.heading() <= visibleHi);
}

int RobotObservation::getMinWeight() {
    return MIN_WEIGHT;
}

int RobotObservation::getMaxWeight() {
    return MIN_WEIGHT + WEIGHT_RANGE;
}

unsigned int RobotObservation::numberOnScreenFramesToBeStale() {
    return ESTIMATED_FRAMES_PER_SECOND * SECONDS_ALIVE_ON_FOV;
}

unsigned int RobotObservation::numberOffScreenFramesToBeStale() {
    return ESTIMATED_FRAMES_PER_SECOND * SECONDS_ALIVE_OFF_FOV;
}

bool RobotObservation::isStale() const {
    return lifeScore <= 0;
}

Point RobotObservation::getCartesianCoordinates() const {
    return getRRCoordinates().toCartesian();
}

RRCoord RobotObservation::getRRCoordinates() const {
    return rr;
}
