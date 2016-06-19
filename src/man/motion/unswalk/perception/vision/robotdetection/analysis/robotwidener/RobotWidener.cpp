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

#include "RobotWidener.hpp"

#include "perception/vision/VisionDefs.hpp"

/**
 *
 */
std::vector<PossibleRobot> RobotWidener::complete(VisionFrame &frame,
		const Fovea &saliency, std::vector<PossibleRobot> possibleRobots) const {
	std::vector<PossibleRobot> robots = expandJerseys(frame, saliency, possibleRobots);


	return robots;
}


/**
 * Look in the region around about where the jersey would be (25% - 75% height)
 * and if there is more then half of a jersey colour on the left or right of the
 * region expand the box outwards.
 */
std::vector<PossibleRobot> RobotWidener::expandJerseys(VisionFrame &frame,
		const Fovea &saliency, std::vector<PossibleRobot> possibleRobots) const {
	std::vector<PossibleRobot> robots;

	for (unsigned int i = 0; i < possibleRobots.size(); ++i) {
		PossibleRobot robot = possibleRobots[i];

		int leftCol = robot.region.a.x();
		int rightCol = robot.region.b.x();
		int startRow = robot.region.a.y() + robot.region.height() / 4;
		int endRow = robot.region.b.y() - robot.region.height() / 4;
		int regionHeight = endRow - startRow;

		int previousCol = 0;
		if (i > 0) {
			previousCol = possibleRobots[i - 1].region.b.x();
		}
		//do left
		int col = leftCol;
		int type = 0; //0:none 1: blue 2: red;
		while (col > previousCol) {
			int numberRed = 0;
			int numberBlue = 0;
			for (int row = startRow; row < endRow; ++row) {
				Colour c = saliency.colour(col, row);
				if (c == cROBOT_BLUE) {
					++numberBlue;
				} else if (c == cROBOT_RED) {
					++numberRed;
				}
			}

			if (numberBlue * 2 > regionHeight && (type == 0 || type == 1)) {
				type = 1;
				--col;
			} else if (numberRed * 2 > regionHeight && (type == 0 || type == 2)) {
				type = 2;
				--col;
			} else {
				break;
			}
		}

		if (col < leftCol) {
			robot.region.a.x() = col;
		}

		int nextCol = saliency.bb.width();
		if (i + 1 < possibleRobots.size()) {
			nextCol = possibleRobots[i + 1].region.a.x();
		}

		col = rightCol;
		while (col < saliency.bb.width()) {
			int numberRed = 0;
			int numberBlue = 0;
			for (int row = startRow; row < endRow; ++row) {
				Colour c = saliency.colour(col, row);
				if (c == cROBOT_BLUE) {
					++numberBlue;
				} else if (c == cROBOT_RED) {
					++numberRed;
				}
			}

			if (numberBlue * 2 > regionHeight && (type == 0 || type == 1)) {
				type = 1;
				++col;
			} else if (numberRed * 2 > regionHeight && (type == 0 || type == 2)) {
				type = 2;
				++col;
			} else {
				break;
			}
		}

		if (col > rightCol) {
			robot.region.b.x() = col;
		}

		robots.push_back(robot);
	}


	return robots;
}
