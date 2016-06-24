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

#include "RobotSanityChecker.hpp"

/**
 * Removes any obstructions with proportionally invalid sizes. E.g. very wide
 * or very tall are assumed to be incorrect and removed.
 */
std::vector<PossibleRobot> RobotSanityChecker::complete(VisionFrame &frame,
		const Fovea &saliency, std::vector<PossibleRobot> possibleRobots) const {
	std::vector<PossibleRobot> robots;

	const static double UNKNOWN_MIN_GRADIENT = 1;
	const static double UNKNOWN_MAX_GRADIENT = 4.5;

	const static double KNOWN_MIN_GRADIENT = 0.8;
	const static double KNOWN_MAX_GRADIENT = 7;

	const static int MAX_ROBOT_DISTANCE = 4000;

	for (unsigned int i = 0; i < possibleRobots.size(); ++i) {
		const PossibleRobot &robot = possibleRobots[i];

		const double gradient = (double)robot.region.height() / (double)robot.region.width();

		bool validRobot = true;

		if (!robotBelowPostsCheck(frame, saliency, robot)) {
		    validRobot = false;
		}


		if (robot.type == PossibleRobot::UNKNOWN) {
			if (gradient < UNKNOWN_MIN_GRADIENT || gradient > UNKNOWN_MAX_GRADIENT) {
				validRobot = false;
			}
		} else {
			if (gradient < KNOWN_MIN_GRADIENT || gradient > KNOWN_MAX_GRADIENT) {
				validRobot = false;
			}
		}

		//Only considers fairly close robots
		if (robot.feet.distance() > MAX_ROBOT_DISTANCE) {
			validRobot = false;
		}


		if (validRobot) {
			robots.push_back(robot);
		}
	}

	return robots;
}


//If the robot is above the bottom of all seen posts it is likely off
//the field or a false positive.
bool RobotSanityChecker::robotBelowPostsCheck(VisionFrame &frame,
        const Fovea &saliency, const PossibleRobot &robot) const {

    bool belowPosts = true;
    int robotYImage = saliency.mapFoveaToImage(robot.region.b).y();
    if (frame.posts.size() == 1) {
        if (robotYImage < frame.posts[0].imageCoords.b.y()) {
            belowPosts = false;
        }
    }

    //Calculates the linear equation that joins the two posts and checks
    //if the bottom of the robot is above this line.
    else if (frame.posts.size() == 2) {
        int robotXMidSaliency = robot.region.a.x() +
                (robot.region.b.x() - robot.region.a.x()) / 2;

        int robotXImage = saliency.mapFoveaToImage(Point(robotXMidSaliency, 0)).x();

        int postHeightDiff = abs(frame.posts[0].imageCoords.b.y() -
                                 frame.posts[1].imageCoords.b.y());
        int postWidthDiff = abs(frame.posts[0].imageCoords.b.x() -
                                frame.posts[1].imageCoords.b.x());

        double m = (double)postHeightDiff / (double) postWidthDiff;

        int b = frame.posts[0].imageCoords.b.y() - m * frame.posts[0].imageCoords.b.x();

        int postLineY = m * robotXImage + b;
        if (postLineY > robotYImage) {
            belowPosts = false;
        }

    } else {
        //I think the assumption in Goal Detection is that size <= 2.
    }

    return belowPosts;
}

