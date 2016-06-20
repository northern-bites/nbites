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

#include "RobotSideDetector.hpp"

/**
 * Looks in the robot's jersey by starting at the bottom and stopping at the first
 * occurrence of significant amount of a jersey colour.
 */
std::vector<PossibleRobot> RobotSideDetector::complete(VisionFrame &frame,
		const Fovea &saliency, std::vector<PossibleRobot> possibleRobots) const {

	return findJerseys(frame, saliency, possibleRobots);
}


bool validPixel(int row, int col, int rowStart, int rowEnd, int colStart, int colEnd) {
	return ((rowStart <= row && row <= rowEnd)
			&& (colStart <= col && col < colEnd));
}

bool significantColourAroundPixel(const Fovea &saliency, int rowStart, int rowEnd,
		int colStart, int colEnd, int col, int row, Colour pixelColour) {
	unsigned int totalMatched = 0;
	unsigned int totalChecked = 0;


	for (int scanCol = col - 1; scanCol <= col + 1; ++scanCol) {
		for (int scanRow = row - 1; scanRow <= row + 1; ++scanRow) {
			if (scanRow == row && scanCol == col) {
				continue;
			}
			if (validPixel(scanRow, scanCol, rowStart, rowEnd, colStart, colEnd)) {
				if (saliency.colour(scanCol, scanRow) == pixelColour) {
					++totalMatched;
				}
				++totalChecked;
			} else {
			}
		}
	}


    const static int SIGNIFICANT_LIMIT = 2; //Atleast half are the desired colour
	return totalMatched * SIGNIFICANT_LIMIT >= totalChecked;
}


/**
 * Looks at the pixels in the image and counts how many jersey colour pixels are adjacent
 * to other colour pixels of the same colour. E.g. if there is a lot of blue
 * coloured pixels clustered together it has a higher score then blue pixels
 * scattered around the box.
 */
std::vector<PossibleRobot> RobotSideDetector::findJerseys(VisionFrame &frame, const Fovea &saliency, std::vector<PossibleRobot> possibleRobots) const {

	std::vector<PossibleRobot> robots;

	//Used if I want to only scan a certain row section of the possible robot.
	const static double START_SCAN_ROW_PERCENTAGE = 0.0F;
	const static double END_SCAN_ROW_PERCENTAGE = 1.0F;

	for (unsigned int i = 0; i < possibleRobots.size(); ++i) {
		PossibleRobot robot = possibleRobots[i];

		int robotHeight = robot.region.height();
		int start_row = robot.region.a.y() +
						START_SCAN_ROW_PERCENTAGE * robotHeight;
		int end_row = robot.region.a.y() +
						END_SCAN_ROW_PERCENTAGE * robotHeight;
		int start_col = robot.region.a.x();
		int end_col = robot.region.b.x();


		unsigned int adjacentBlue = 0;
		unsigned int adjacentRed = 0;

		for (int row = start_row; row < end_row; ++row) {
			for (int col = start_col; col < end_col; ++col) {
				const Colour &pixelColour = saliency.colour(col, row);
				if (pixelColour == cROBOT_RED) {
					if (significantColourAroundPixel(saliency, start_row, end_row,
							start_col, end_col, col, row, cROBOT_RED)) {

						++adjacentRed;
					}
				} else if (pixelColour == cROBOT_BLUE) {
					if (significantColourAroundPixel(saliency, start_row, end_row,
							start_col, end_col, col, row, cROBOT_BLUE)) {

						++adjacentBlue;
					}
				}
			}
		}

		const static unsigned int JERSEY_MATCHING_LIMIT = 3;
		//Must have significantly more of one colour than the other to match a
		//jersey.
		if (adjacentRed > JERSEY_MATCHING_LIMIT * adjacentBlue) {
			robot.type = PossibleRobot::RED_TEAM;
		} else if (adjacentBlue > JERSEY_MATCHING_LIMIT * adjacentRed) {
			robot.type = PossibleRobot::BLUE_TEAM;
		} else {
			robot.type = PossibleRobot::UNKNOWN;
		}

		robots.push_back(robot);
	}


	return robots;
}
