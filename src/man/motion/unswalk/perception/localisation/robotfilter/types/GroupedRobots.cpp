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

#include "GroupedRobots.hpp"
#include "types/RRCoord.hpp"
#include "types/AbsCoord.hpp"

#include "utils/angles.hpp"
#include <string>
#include <sstream>

#define CMs(x) (x * 10)

const unsigned int ROBOT_WIDTH = 450;

const unsigned int GroupedRobots::IMPORTANT_MIN_OBSEVATIONS = 3;

const unsigned int GroupedRobots::MIN_MERGE_SCALE = 1;
const unsigned int GroupedRobots::MAX_MERGE_SCALE = 2;
const unsigned int GroupedRobots::ELLIPSE_VERTICAL = CMs(80);
const unsigned int GroupedRobots::ELLIPSE_HORIZONTAL = CMs(40);
const unsigned int GroupedRobots::ELLIPSE_VER_HOR_RATIO = ELLIPSE_VERTICAL / ELLIPSE_HORIZONTAL;

const static unsigned int LEFT = 0;
const static unsigned int RIGHT = 1;
const static unsigned int NUM_SIDES = 2;

static double pointDistance(Point a, Point b) {
    int xDiff = a[0] - b[0];
    int yDiff = a[1] - b[1];

    return sqrt((double)(xDiff * xDiff + yDiff * yDiff));
}

GroupedRobots::GroupedRobots() {

}

GroupedRobots::GroupedRobots(const UNSWRobotInfo &visualRobot){
    mergeRobot(visualRobot);
}

RRCoord GroupedRobots::getRRCoordinates() const {
    return _rr;
}

Point GroupedRobots::getCartesianCoordinates() const {
    return _rrc;
}

AbsCoord GroupedRobots::getAbsCoord() const {
    return _pos;
}


bool GroupedRobots::isImportantObstacle() const {
    if (observations.size() >= IMPORTANT_MIN_OBSEVATIONS) {
        return true;
    }

    return false;
}

Point GroupedRobots::getRobotRelativeCartesianToGroup(RRCoord robotRR) const {
    RRCoord groupRR = getRRCoordinates();

    robotRR.heading() -= groupRR.heading();
    groupRR.heading() = 0;

    Point robotAngledCoordinates = robotRR.toCartesian();
    Point groupAngledCoordinates = groupRR.toCartesian();

    //Relative to group.
    Point robotRelativeCoordinates = robotAngledCoordinates - groupAngledCoordinates;

    return robotRelativeCoordinates;
}

Point GroupedRobots::getScaledRobotRelativeCartesianToGroup(RRCoord robotRR, double scaleFactor) const {
    Point robotRelativeCoordinates = getRobotRelativeCartesianToGroup(robotRR);
    const double verticalMaxDistance = (double)ELLIPSE_VERTICAL * scaleFactor;

    robotRelativeCoordinates[1] *= ELLIPSE_VER_HOR_RATIO;
    return robotRelativeCoordinates;
}

double GroupedRobots::distanceToRobot(const UNSWRobotInfo &robot) const {
    Point rrc = getRobotRelativeCartesianToGroup(robot.rr);

    return pointDistance(Point(0, 0), rrc);
}


bool GroupedRobots::canMergeRobot(const UNSWRobotInfo &robot) const {
    double scaleFactor = getScaleFactor(getLargestWeight());
    Point robotRelativeCoordinates = getScaledRobotRelativeCartesianToGroup(robot.rr, scaleFactor);

    const double distance = pointDistance(Point(0, 0), robotRelativeCoordinates);

    if (distance < (double)ELLIPSE_VERTICAL * scaleFactor) {
        return true;
    } else {
        return false;
    }
}


double GroupedRobots::getScaleFactor(int weight) const {
    const static int MIN_MERGE_SCALE = 1;

    //Make these weights from 0 -> n not c -> n + c, where c = minWeight
    weight = weight - RobotObservation::getMinWeight();
    int maxWeight = RobotObservation::getMaxWeight() - RobotObservation::getMinWeight();

    int inverseWeight = maxWeight - weight;
    double weightRatio = (double)inverseWeight / (double)maxWeight;

    const unsigned int SCALE_RANGE = MAX_MERGE_SCALE - MIN_MERGE_SCALE;

    const double scaleFactor =
            MIN_MERGE_SCALE + (double)SCALE_RANGE * weightRatio;

    return scaleFactor;
}


UNSWRobotInfo::Type GroupedRobots::getType() const {
    unsigned int totalRed = 0;
    unsigned int totalBlue = 0;

    std::vector<RobotObservation>::const_iterator it;
    for (it = observations.begin(); it != observations.end(); ++it) {
        const RobotObservation &observation = (*it);
        if (observation.type == UNSWRobotInfo::rBlue) {
            ++totalBlue;
        } else if (observation.type == UNSWRobotInfo::rRed) {
            ++totalRed;
        }
    }

    if (totalBlue == 0 && totalRed == 0) {
        return UNSWRobotInfo::rUnknown;
    } else if (totalBlue * 2 < totalRed) {
        return UNSWRobotInfo::rRed;
    } else if (totalRed * 2 < totalBlue) {
        return UNSWRobotInfo::rBlue;
    } else {
        return UNSWRobotInfo::rUnknown;
    }
}

bool GroupedRobots::isOnField() const {
    AbsCoord absCoord = getAbsCoord();

    if (absCoord.x()* 2 > FIELD_LENGTH || absCoord.x() * 2 < -FIELD_LENGTH) {
        return false;
    }

    if (absCoord.y() * 2 > FIELD_WIDTH || absCoord.y() * 2 < -FIELD_WIDTH) {
        return false;
    }

    return true;
}

void GroupedRobots::mergeRobot(const UNSWRobotInfo &robot) {
    observations.push_back(RobotObservation(robot));
    calculateCoordinates();
}

bool GroupedRobots::isEmpty() const {
    return observations.empty();
}

void GroupedRobots::tick(Odometry odometryDiff, float headYaw, const AbsCoord &robotPos) {

    calculateCoordinates(); // Hack for some reason crashing, testing whether this fixes it.
    this->robotPos = robotPos;
    std::vector<RobotObservation>::iterator it = observations.begin();
    while (it != observations.end()) {
        RobotObservation &observation = (*it);

        //Calculates the distance to the center of the group and calculates
        //the percentage of this with the MAX_MERGE ellipse for the group.
        double distance = pointDistance(Point(0, 0),
                getScaledRobotRelativeCartesianToGroup(observation.rr, MAX_MERGE_SCALE));
        double percentage = distance / (double)ELLIPSE_VERTICAL;

        observation.tick(odometryDiff, headYaw, percentage);

        if (observation.isStale()) {
            it = observations.erase(it);
        } else {
            ++it;
        }
    }
    calculateCoordinates();
}


int GroupedRobots::getLargestWeight() const {
    int largestWeight = -1;
    for (unsigned int i = 0; i < observations.size(); ++i) {
        const RobotObservation &observation = observations[i];

        int weight = observation.getWeight();
        if (weight > largestWeight) {
            largestWeight = weight;
        }
    }
    return largestWeight;
}

std::vector<double> GroupedRobots::calculateTangentHeadings() const {
    std::vector<double> obstructionOutsideHeadings(NUM_SIDES);

    double headingOffset;
    if (_rr.distance() < ROBOT_WIDTH) {
        headingOffset = UNSWDEG2RAD(90);
    } else {
        headingOffset = ABS(atan(ROBOT_WIDTH / _rr.distance()));
    }

    obstructionOutsideHeadings[LEFT] = _rr.heading() + headingOffset;
    obstructionOutsideHeadings[RIGHT] = _rr.heading() - headingOffset;

    return obstructionOutsideHeadings;
}

std::vector<RRCoord> GroupedRobots::calculateEvadeVectors() const {
    const RRCoord DEFAULT_EVADE = _rr;
    std::vector<RRCoord> evadeVectors(NUM_SIDES, DEFAULT_EVADE);

    //Only calculate evade vectors if it is in our current view and pretty close
    const unsigned int minimumEvadeDistance = ROBOT_WIDTH;
    const double r = minimumEvadeDistance;
    const double d = _rr.distance();

    if (d < r) {
        //want to quickly avoid as we are super close
        evadeVectors[LEFT] = RRCoord(minimumEvadeDistance, UNSWDEG2RAD(90));
        evadeVectors[RIGHT] = RRCoord(minimumEvadeDistance, -UNSWDEG2RAD(90));
    } else {
        //Helpers to reduce calculations
        double r2 = r * r;
        double r4 = r2 * r2;
        double d2 = d * d;

        double x = d - r2 / d;
        double y = sqrt(r2 - r4 / d2);

        //make sure that distance should be at least
        double distance = ABS(sqrt(d2 - r2));
        if (distance < ROBOT_WIDTH) {
            distance = ROBOT_WIDTH;
        }
        double evadeAngle = atan2(x, y);
        double leftHeading = NORMALISE(_rr.heading() + (UNSWDEG2RAD(90) - evadeAngle));
        double rightHeading = NORMALISE(_rr.heading() -(UNSWDEG2RAD(90) - evadeAngle));

        evadeVectors[LEFT] = RRCoord(distance, leftHeading);
        evadeVectors[RIGHT] = RRCoord(distance, rightHeading);
    }

    return evadeVectors;
}


RobotObstacle GroupedRobots::generateRobotObstacle() const {

    RobotObstacle obstacle;
    obstacle.rr = getRRCoordinates();
    obstacle.type = getType();
    Point cartesian = getCartesianCoordinates();
    obstacle.rrc = AbsCoord(cartesian[0], cartesian[1], 0);
    obstacle.pos = getAbsCoord();

    std::vector<double> tangentHeadings = calculateTangentHeadings();
    obstacle.tangentHeadingLeft = tangentHeadings[LEFT];
    obstacle.tangentHeadingRight = tangentHeadings[RIGHT];

    std::vector<RRCoord> evadeVectors = calculateEvadeVectors();
    obstacle.evadeVectorLeft = evadeVectors[LEFT];
    obstacle.evadeVectorRight = evadeVectors[RIGHT];

    return obstacle;
}

void GroupedRobots::calculateCoordinates() {
    calculateCartesian();
    calculateRRCoord();
    calculateAbsCoord();
}

void GroupedRobots::calculateRRCoord() {
    AbsCoord abs(_rrc[0], _rrc[1], 0);
    _rr = abs.convertToRobotRelative();
}

void GroupedRobots::calculateCartesian() {
    if (isEmpty()) {
        _rrc = Point(0, 0);
    }

    long groupX = 0;
    long groupY = 0;
    long totalWeight = 0;

    std::vector<RobotObservation>::const_iterator it;
    for (it = observations.begin(); it != observations.end(); ++it) {
        const RobotObservation &observation = (*it);
        int weight = observation.getWeight();

        Point coordinates = observation.getCartesianCoordinates();
        groupX += (weight * coordinates[0]);
        groupY += (weight * coordinates[1]);

        totalWeight += weight;
    }

    if (totalWeight != 0) {
        groupX /= totalWeight;
        groupY /= totalWeight;
    }

    _rrc = Point((int)groupX, (int)groupY);
}

void GroupedRobots::calculateAbsCoord() {
    RRCoord robotCoords = getRRCoordinates();
    robotCoords.heading() += robotPos.theta();

    Point robotCartesian = robotCoords.toCartesian();
    int x = robotPos.x() + robotCartesian[0];
    int y = robotPos.y() + robotCartesian[1];

    _pos = AbsCoord(x, y, 0);
}

