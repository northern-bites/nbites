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

#include <vector>

#include "types/UNSWRobotInfo.hpp"
#include "types/RobotObstacle.hpp"
#include "RobotObservation.hpp"
#include "types/Odometry.hpp"
#include "types/AbsCoord.hpp"
#include "types/RRCoord.hpp"


/**
 * Represents a group of observations, merging them into a single coordinate on
 * the field.
 */
class GroupedRobots {
public:
    GroupedRobots();
    GroupedRobots(const UNSWRobotInfo &visualRobot);

    /**
     * Tests whether the given visual robot is able to be merged into the group.
     */
    bool canMergeRobot(const UNSWRobotInfo &robot) const;

    /**
     * Merge the visual robot into this group.
     */
    void mergeRobot(const UNSWRobotInfo &robot);

    /**
     * Indicate that there we have done another set of observations and to
     * do any changes to the group.
     */
    void tick(Odometry odometryDiff, float headYaw, const AbsCoord &robotPos);

    /**
     * Tests whether there are still any available robots inside the group.
     */
    bool isEmpty() const;

    /**
     * Tests whether this group should be an obstacle.
     */
    bool isImportantObstacle() const;

    /**
     * Get the relative cartesian coordinates of the group.
     */
    Point getCartesianCoordinates() const;

    /**
     * Get the robot relative vector for the group.
     */
    RRCoord getRRCoordinates() const;

    /**
     * Get the absolute coord of the group
     */
    AbsCoord getAbsCoord() const;

    /**
     * Calculates the distance to a robot from the group's coordinates
     */
    double distanceToRobot(const UNSWRobotInfo &robot) const;

    /**
     * Given the robot's position is this group on the field.
     */
    bool isOnField() const;

    /**
     * Generate an obstacle from this group.
     */
    RobotObstacle generateRobotObstacle() const;

    const static unsigned int IMPORTANT_MIN_OBSEVATIONS;

    const static unsigned int MIN_MERGE_SCALE;
    const static unsigned int MAX_MERGE_SCALE;
    const static unsigned int ELLIPSE_VERTICAL;
    const static unsigned int ELLIPSE_HORIZONTAL;
    const static unsigned int ELLIPSE_VER_HOR_RATIO;


private:
    std::vector<RobotObservation> observations;

    /**
     * Given a robot's weight we calculate a scale factor between MIN_MERGE_SCALE
     * and MAX_MERGE_SCALE
     */
    double getScaleFactor(int weight) const;

    /**
     * Gets robot relative cartesian coordinates from the group to the robot
     * coordinates.
     */
    Point getRobotRelativeCartesianToGroup(RRCoord robot) const;

    Point getScaledRobotRelativeCartesianToGroup(RRCoord robotRR, double scaleFactor) const;

    /**
     * Given the observations it determined what type it is most likely.
     */
    UNSWRobotInfo::Type getType() const;

    /**
     * Of the list of observations it finds the one with the largest weight.
     */
    int getLargestWeight() const;

    /**
     * Calculates and stores the coordinates for later use.
     */
    void calculateCoordinates();

    /**
     * Calculate the cartesian coordinates of the robot group
     */
    void calculateCartesian();

    /**
     * Calculate the RRCoord of the robot group
     */
    void calculateRRCoord();

    /**
     * Calculate the absolute position of the robot on the field.
     */
    void calculateAbsCoord();

    /**
     * Calculates the vectors to evade the robots on the left or right.
     */
    std::vector<RRCoord> calculateEvadeVectors() const;

    /**
     * Calculate the heading that point to the left and right side of the robot.
     */
    std::vector<double> calculateTangentHeadings() const;


    /**
     *
     */
    RRCoord _rr;
    Point _rrc;
    AbsCoord _pos;

    AbsCoord robotPos;
};
