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

#include "RobotMerger.hpp"

/**
 * Finds the distance between two boxes, assumes that first is before (on the
 * left) of second.
 */
static int boxDistance(const BBox &first, const BBox &second) {
	return second.a.x() - first.b.x();
}

static BBox mergeBoxes(const BBox &box1, const BBox &box2) {

	const int mostLeft = MIN(box1.a.x(), box2.a.x());
	const int mostRight = MAX(box1.b.x(),box2.b.x());
	const int highest = MIN(box1.a.y(), box2.a.y());
	const int lowest = MAX(box1.b.y(),box2.b.y());

	return BBox(Point(mostLeft, highest), Point(mostRight, lowest));
}

/**
 * Tests whether two boxes are close together. This is defined by the distance
 * between the two boxes is smaller then either of the boxes width.
 */
static bool boxesCloseTogether(const BBox &box1, const BBox &box2) {
	int distance = boxDistance(box1, box2);
	bool closeTogether = (
			distance < box1.width() ||
			distance < box2.width()
	);
	return closeTogether;
}

static bool boxesTouching(const BBox &leftBox, const BBox &rightBox) {
    return (boxDistance(leftBox, rightBox) == 0);
}

RobotMerger::RobotMerger(bool useMidHeightCheck) : useMidHeightCheck(useMidHeightCheck) {}

/**
 * @prereq must have same types or one unknown
 */
PossibleRobot RobotMerger::mergePossibleRobots(const PossibleRobot &a, const PossibleRobot &b) {
	BBox box = mergeBoxes(a.region, b.region);
	RRCoord feet;

	if (a.region.b.y() > b.region.b.y()) {
		feet = a.feet;
	} else {
		feet = b.feet;
	}

	PossibleRobot r(box, feet);
	if (a.type == PossibleRobot::UNKNOWN) {
		r.type = b.type;
	} else {
		r.type = a.type;
	}
	return r;
}

/**
 * Determines whether two robots are valid to be merged.
 */
bool RobotMerger::canMergePossibleRobots(VisionFrame &frame,
		const Fovea &saliency, const PossibleRobot &robotLeft, const PossibleRobot &robotRight) const {
	bool similarTypes = (
			robotLeft.type == robotRight.type ||
			robotLeft.type == PossibleRobot::UNKNOWN ||
			robotRight.type == PossibleRobot::UNKNOWN
	);

	bool closeTogether = boxesCloseTogether(robotLeft.region, robotRight.region);

	if (!useMidHeightCheck) {
	    return similarTypes && closeTogether;
	}

	//Only allows the merging of boxes if smaller box goes at least to the
	//second half on the larger box. This is to allow merging of arms, which
	//usually go pretty far down the body, but will not merge a robot in the
	//background.
	int mergedMiddleHeight = robotLeft.region.a.y() + robotLeft.region.height() / 2;
	int currentMiddleHeight = robotRight.region.a.y() + robotRight.region.height() / 2;
	bool mergedBelowMidpoint = (robotLeft.region.b.y() > currentMiddleHeight &&
			robotLeft.region.b.y() <= robotRight.region.b.y());

	bool currentBelowMergedMidpoint = (robotRight.region.b.y() > mergedMiddleHeight &&
			robotRight.region.b.y() <= robotLeft.region.b.y());

	return (similarTypes && closeTogether && (mergedBelowMidpoint || currentBelowMergedMidpoint));
}


std::vector<PossibleRobot> RobotMerger::mergeTouchingPossibleRobots(VisionFrame &frame,
        const Fovea &saliency, const std::vector<PossibleRobot> &possibleRobots) const {

    if (possibleRobots.size() == 0) {
        return possibleRobots;
    }

    std::vector<PossibleRobot> mergedRobots;
    PossibleRobot merged = possibleRobots[0];
    for (unsigned int i = 1; i < possibleRobots.size(); ++i) {
        if (boxesTouching(merged.region, possibleRobots[i].region)) {
            merged = mergePossibleRobots(merged, possibleRobots[i]);
        } else {
            mergedRobots.push_back(merged);
            merged = possibleRobots[i];
        }
    }

    mergedRobots.push_back(merged);

    return mergedRobots;

}


/**
 * Merges boxes left but does not merge it left if the next box would be a better merge.
 */

std::vector<PossibleRobot> RobotMerger::complete(VisionFrame &frame,
		const Fovea &saliency, std::vector<PossibleRobot> possibleRobots) const {

	if (possibleRobots.size() == 0) {
		return possibleRobots;
	}

	std::vector<PossibleRobot> mergedRobots;
	PossibleRobot merged;

	for (unsigned int i = 0; i < possibleRobots.size(); ++i) {
		const bool notFirst = i > 0;
		const bool notLast = (i + 1 < possibleRobots.size());
		PossibleRobot &robot = possibleRobots[i];

		bool canMergeLeft = false;
		if (notFirst) {
			canMergeLeft = canMergePossibleRobots(frame, saliency, merged, robot);
		}

		bool canMergeRight = false;
		if (notLast) {
			canMergeRight = canMergePossibleRobots(frame, saliency, robot, possibleRobots[i + 1]);
		}

		if (canMergeLeft && canMergeRight) {
			int distanceLeft = boxDistance(merged.region, robot.region);
			int distanceRight = boxDistance(robot.region, possibleRobots[i + 1].region);
			//If right is closer than left
			if (distanceRight < distanceLeft) {
				mergedRobots.push_back(merged);
				merged = robot;
			//Left is closer
			} else {
				merged = mergePossibleRobots(merged, robot);
			}
		} else if (canMergeLeft) {
			merged = mergePossibleRobots(merged, robot);
		} else if (canMergeRight) {
			if (notFirst) {
				mergedRobots.push_back(merged);
			}
			merged = mergePossibleRobots(robot, possibleRobots[i + 1]);
			i = i + 1;
		} else {
			if (notFirst) {
				mergedRobots.push_back(merged);
			}
			merged = robot;
		}
	}

	mergedRobots.push_back(merged);

	return mergedRobots;
}
