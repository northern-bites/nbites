// This file is part of Man, a robotic perception, locomotion, and
// team strategy application created by the Northern Bites RoboCup
// team of Bowdoin College in Brunswick, Maine, for the Aldebaran
// Nao robot.
//
// Man is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Man is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser Public License for more details.
//
// You should have received a copy of the GNU General Public License
// and the GNU Lesser Public License along with Man.  If not, see
// <http://www.gnu.org/licenses/>.


/*
 * Robots.cpp is where we do our robot recognition work in vision.
 */

#include <iostream>
#include "Robots.h"
#include <vector>

using namespace std;

namespace man {
namespace vision {

#ifdef OFFLINE
static const bool ROBOTSDEBUG = false;
#else
static const bool ROBOTSDEBUG = false;
#endif

Robots::Robots(Vision* vis, Threshold* thr, Field* fie, Context* con,
               unsigned char col)
	: vision(vis), thresh(thr), field(fie), context(con), color(col)
{
    const int MAX_ROBOT_RUNS = 400;
    blobs = new Blobs(MAX_ROBOT_RUNS);
    allocateColorRuns();
}

/* Initialize the data structure.
 * @param s     the slope corresponding to the robot's head tilt
 */
void Robots::init()
{
	for (int i = 0; i <IMAGE_WIDTH/widthScale; i++){
		for (int j = 0; j < IMAGE_HEIGHT/heightScale; j++){
			imageBoxes[i][j] = 0;
		}
	}
	blobs->init();
	numberOfRuns = 0;

#ifdef OFFLINE
	debugRobots = false;
#endif
}

/* Allocate the required amount of memory dependent on the primary color
 */
void Robots::allocateColorRuns()
{
	const int RUN_VALUES = 3;         // for x, y and h
	const int RUNS_PER_SCANLINE = 15;
	const int RUNS_PER_LINE = 5;

	int run_num = RUN_VALUES;
	// depending on the color we have more or fewer runs available
	run_num = IMAGE_WIDTH * RUNS_PER_SCANLINE;
	runsize = IMAGE_WIDTH * RUNS_PER_LINE;
	runs = (run*)malloc(sizeof(run) * run_num);
}


/* Robot recognition methods
 */

void Robots::preprocess() {
    // do some robot scanning stuff - we're going to analyze our runs and see if
	//they could be turned into viable robot runs
    int bigh = IMAGE_HEIGHT, firstn = -1, lastn = -1, bot = -1;
    // first do the Navy robots
    for (int i = 0; i < IMAGE_WIDTH - 1; i+= 1) {
		int colorRun = thresh->getRobotTop(i, color);
		// if we saw a robot run in this scanline then process it
        if (colorRun != -1) {
			// our goal is to find a swath of runs and essentially grab them all at once
			// in a sense we're blobbing here
            firstn = i;
            lastn = 0;
            bigh = colorRun;
            bot = thresh->getRobotBottom(i, color);
			// as long as we're connected to more runs, keep scooping them up
            while ((thresh->getRobotTop(i, color) != -1 ||
					thresh->getRobotTop(i+1, color) != -1)
				   && i < IMAGE_WIDTH - 3) {
                if (thresh->getRobotTop(i, color) < bigh &&
					thresh->getRobotTop(i, color) != -1) {
                    bigh = thresh->getRobotTop(i, color);
                }
                if (thresh->getRobotBottom(i, color) > bot) {
                    bot = thresh->getRobotBottom(i, color);
                }
                i+=1;
                lastn+=1;
            }
			// now feed them all into our run structure
            for (int k = firstn; k < firstn + lastn; k+= 1) {
                newRun(k, bigh, bot - bigh);
                // cout << "Runs " << k << " " << bigh << " " << (bot - bigh)
                //      << endl;
            }
            //cout << "Last " << lastn << " " << bigh << " " << bot << endl;
            //drawRect(firstn, bigh, lastn, bot - bigh, RED);
        }
    }
}


/* Try and recognize robots.  Basically we're doing blobbing here, but with lots
   of extra twists.  Mainly we're being extremely loose with the conditions
   under which we consider blobs to be "connected."  We're trying to take
   advantage of the properties of the robots - namely that they stand vertically
   normally.
   @param cross     The Field cross data structure - contains white blob info
 */


void Robots::robot(Cross* cross)
{
	if (numberOfRuns < 1) return;
    const int lastRunXInit = -30;
    const int resConst = 10;
    const int robotBlobMin = 10;

    int lastrunx = lastRunXInit, lastruny = 0, lastrunh = 0;

    // loop through all of the runs of this color
    for (int i = 0; i < numberOfRuns; i++) {
        if (runs[i].x < lastrunx + resConst) {
            for (int k = lastrunx; k < runs[i].x; k+= 1) {
                blobs->blobIt(k, lastruny, lastrunh, true);
            }
        }
		// now we can add the run normally
        blobs->blobIt(runs[i].x, runs[i].y, runs[i].h, true);
		// set the current as the last
        lastrunx = runs[i].x; lastruny = runs[i].y; lastrunh = runs[i].h;
    }
    // check each of the candidate blobs to see if it might reasonably be
    // called a piece of a robot
	int viable = 0;
    for (int i = 0; i < blobs->number(); i++) {
		// see if we can tighten the blob up
		int x = blobs->get(i).getLeft();
		int y = blobs->get(i).getTop();
		int w = blobs->get(i).width();
		int h = blobs->get(i).height();
		for (int j = y; j < y + h && j < IMAGE_HEIGHT; j++) {
			int count = 0;
			for (int k = x; k < x + w && k < IMAGE_WIDTH; k++) {
				if (Utility::colorsEqual(thresh->getThresholded(j, k), color)) {
					count++;
					if (count > w / 3) {
						blobs->setTop(i, j);
						j = 1000;
						k = 1000;
					}
				}
			}
		}
		for (int j = y + h; j > y  && j > 0; j--) {
			int count = 0;
			for (int k = x; k < x + w && k < IMAGE_WIDTH; k++) {
				if (Utility::colorsEqual(thresh->getThresholded(j, k), color)) {
					count++;
					if (count > w / 3) {
						blobs->setBottom(i, j);
						j = -1;
						k = 1000;
					}
				}
			}
		}
        if (!sanityChecks(blobs->get(i), cross)) {
            if (blobs->get(i).getRight() > 0) {
                if (debugRobots) {
                    vision->drawRect(blobs->get(i).getLeft(),
                                     blobs->get(i).getTop(),
                                     blobs->get(i).width(),
                                     blobs->get(i).height(),
                                     WHITE);
                }
            }
            blobs->init(i);
        } else {
			// one last sanity check - make sure the "robot" isn't off the field
			estimate pose_est = vision->pose->pixEstimate(blobs->get(i).getLeft(),
												  blobs->get(i).getBottom(),
												  270);
			float farDistance = vision->fieldEdge->getDistanceLeft();
			if (blobs->get(i).getLeft() > 2 * IMAGE_WIDTH / 3) {
				farDistance = vision->fieldEdge->getDistanceCenter();
			} else if (blobs->get(i).getLeft() > IMAGE_WIDTH / 3) {
				farDistance = vision->fieldEdge->getDistanceRight();
			}
			if (debugRobots) {
				cout << "Distance estimate to perspective robot is " <<
					pose_est.dist << " " << farDistance << endl;
			}
			if ((pose_est.dist > 200 && pose_est.dist > farDistance && farDistance != 0)
				|| (pose_est.dist == 0 && !farDistance == 0) || (pose_est.dist > 500)) {
				if (debugRobots) {
					cout << "Robot too far away " << endl;
                    vision->drawRect(blobs->get(i).getLeft(),
                                     blobs->get(i).getTop(),
                                     blobs->get(i).width(),
                                     blobs->get(i).height(),
                                     BLUE);
				}
				blobs->init(i);
			} else {

				if (debugRobots) {
					vision->drawRect(blobs->get(i).getLeft(), blobs->get(i).getTop(),
									 blobs->get(i).width(), blobs->get(i).height(),
									 MAROON);
				}
				viable++;
			}
        }
    }
    // if we have some viable robots then let everyone know
	if (viable > 0) {
		int robotnum;
		for (int i = 1; i < viable + 1 && i < 4; i++) {
			robotnum = blobs->getBiggest();
			updateRobots(i, robotnum);
		}
	}
}


/* Robot sanity checks.  Takes a candidate blob and puts it through a
   bunch of tests to make sure it is ok.
   @param  candidate        the blob to check
   @return                  whether we judge it to be reasonable
 */

bool Robots::sanityChecks(Blob candidate, Cross* cross) {
    const int blobHeightMin = 8;
	const int blobWidthMin = 6;
    int height = candidate.height();
	int width = candidate.width();
	int bottom = candidate.getBottom();
    if (candidate.getRight() > 0) {
        // the bottom of the uniform shouldn't be above field horizon
        //if (bottom < field->horizonAt(candidate.getLeft())) {
		//  return false;
        //}
        // blobs must be big enough
        if (height < blobHeightMin || width < blobWidthMin) {
			if (debugRobots) {
				cout << "RObot is too small " << height << " " << width << endl;
			}
            return false;
        }
		if (width > 2 * height) {
			if (debugRobots) {
				cout << "Too wide" << endl;
			}
			return false;
		}
		// blob must be reasonably of the right color
		int x = candidate.getLeft();
		int y = candidate.getTop();
		int count = 0;
		for ( ; x < candidate.getRight() && x < IMAGE_WIDTH; x++) {
			for (y = candidate.getTop(); y < candidate.getBottom() && y < IMAGE_HEIGHT;
				 y++) {
				if (Utility::colorsEqual(thresh->getThresholded(y, x), color)) {
					count++;
				}
			}
		}
		if (count < width * height / 3) {
			if (debugRobots) {
				cout << "Not enough of the color " << endl;
			}
			return false;
		}
        // there ought to be some white below the uniform
        if (bottom < IMAGE_HEIGHT - 10 &&
			!cross->checkForRobotBlobs(candidate)) {
			if (debugRobots) {
				cout << "Bad robot from cross check" << endl;
			}
            return false;
        }
        // the last check was pretty general, let's improve
        if (candidate.getBottom() < IMAGE_HEIGHT - candidate.height() * 2
            && !whiteBelow(candidate)) {
			if (debugRobots) {
				cout << "Got rid for lack of white below" << endl;
			}
            return false;
        }
        /*if (candidate.getTop() > candidate.height() * 2
            && !whiteAbove(candidate)) {
			if (debugRobots) {
				cout << "Got rid for lack of white above" << endl;
			}
            return false;
			}*/
        // for some blobs we check even harder for white
        if (height < 2 * blobHeightMin && noWhite(candidate)) {
			if (debugRobots) {
				cout << "Got rid of small one for white" << endl;
			}
            return false;
        }

		if (color == NAVY_BIT && vision->pose->getHorizonY(0) < 0 &&
			notGreen(candidate)) {
			if (debugRobots) {
				cout << "Scared of a possible navy robot" << endl;
			}
			return false;
		}
        return true;
    }
    return false;
}



/* When we are looking down, the shadowed carpet often has lots of Navy.
   Make sure we aren't just looking at carpet
*/
bool Robots::notGreen(Blob candidate) {
	int bottom = candidate.getBottom();
	int top = candidate.getTop();
	int left = candidate.getLeft();
	int right = candidate.getRight();
	int area = candidate.width() * candidate.height() / 5;
	int greens = 0;
	for (int i = left; i < right; i++) {
		for (int j = top; j < bottom; j++) {
			if (Utility::isGreen(thresh->getThresholded(j, i))) {
				greens++;
				if (greens > area) {
					return true;
				}
			}
		}
	}
	return false;
}

/* Make sure there is some white alligned with the candidate blob
 */
bool Robots::checkWhiteAllignment(Blob candidate) {
	for (int i = 0; i < whiteBlobs->number(); i++) {
		if (whiteBlobs->get(i).isAligned(candidate)) {
			return true;
		}
	}
	return false;
}

/* Since the white blob check catches a lot of extra stuff like lines,
   we need to check a bit more carefully.
*/
bool Robots::whiteBelow(Blob candidate) {
	int bottom = candidate.getBottom();
	int height = candidate.height();
	int scanline = bottom;
	// first cut - if we're occluded where the robot is then this is a great sign
	// that we have a robot - may need more work for cases where robot is far off
	for (int x = candidate.getLeft(); x < candidate.getRight(); x++) {
		if (field->occludingHorizonAt(x) > bottom) {
			return true;
		}
	}
	if (bottom > -1) {
		if (debugRobots) {
			cout << "Bad white below " << endl;
		}
		return false;
	}
	return false;
}

/* Since the white blob check catches a lot of extra stuff like lines,
   we need to check a bit more carefully.
*/
bool Robots::whiteAbove(Blob candidate) {
	int top = candidate.getTop();
	int height = candidate.height();
	int scanline = top - height;
	for (int y = scanline; y > 0 && y > scanline - height; y -= 3) {
		int white = 0;
		int green = 0;
		for (int x = candidate.getLeft(); x < candidate.getRight(); x++) {
			if (Utility::isWhite(thresh->getThresholded(y, x))) {
				white++;
			} else if (Utility::isGreen(thresh->getThresholded(y, x))) {
				green++;
			}
		}
		if (green > candidate.width() / 2 && white == 0) {
			return false;
		}
		if (white > candidate.width() / 4) {
			return true;
		}
	}
	return false;
}

/* Calculate the horizontal distance between two objects
 * (the end of one to the start of the other).
 * @param x1	left x of one object
 * @param x2	right x of the object
 * @param x3	left x of the other
 * @param x4	right x of the other
 * @return		the distance between the objects in the x dimension
 */
int Robots::distance(int x1, int x2, int x3, int x4) {
	if (x2 < x3) {
		return x3 - x2;
	}
	if (x1 > x4) {
		return x1 - x4;
	}
    return 0;
}

/* We have a swatch of color.  Let's make sure that there is some white
   around somewhere as befits our robots.
*/
bool Robots::noWhite(Blob b) {
    const int MINWHITE = 5;

    int left = b.getLeft(), right = b.getRight();
    int top = b.getTop(), bottom = b.getBottom();
    int width = b.width();
    int tops, bottoms;
    for (int i = 1; i < b.height(); i++) {
        tops = 0; bottoms = 0;
        for (int x = left; x <= right; x++) {
            if (top - i >= 0 && Utility::isWhite(thresh->getThresholded(top - i,x)))
                tops++;
            if (bottom + i < IMAGE_HEIGHT &&
                Utility::isWhite(thresh->getThresholded(bottom+i,x)))
                bottoms++;
            if (tops > width / 2 || tops == width) return false;
            if (bottoms > width / 2 || tops == width) return false;
        }
    }
    return true;
}


/*
  We have detected something big enough to be called a robot.  Set the appropriate
  field object.
  @param which    whether it is the biggest or the second biggest object
  @param index    the index of the blob in question
*/

void Robots::updateRobots(int which, int index)
{
	if (debugRobots) {
		cout << "Updating robot " << which;
		if (color == RED_BIT) {
			cout << " Red " << endl;
		} else {
			cout << " Navy " << endl;
		}
	}
	//printBlob(blobs[index]);
	if (color == RED_BIT) {
		if (which == 1) {
			vision->red1->updateRobot(blobs->get(index));
		} else if (which == 2) {
			vision->red2->updateRobot(blobs->get(index));
		} else {
			vision->red3->updateRobot(blobs->get(index));
		}
	} else if (color == NAVY_BIT) {
		if (which == 1) {
			vision->navy1->updateRobot(blobs->get(index));
		} else if (which == 2) {
			vision->navy2->updateRobot(blobs->get(index));
		} else {
			vision->navy3->updateRobot(blobs->get(index));
		}
	}
	blobs->init(index);
}

/* Adds a new run to the basic data structure.

   runs structure contains:
   -x of start column
   -y of start column
   -height of run

   @param x     x value of run
   @param y     y value of top of run
   @param h     height of run
*/
void Robots::newRun(int x, int y, int h)
{
    const int RUN_VALUES = 3;    // x, y, and h of course
	const int SKIPS = 8;

    if (numberOfRuns < runsize) {
        int last = numberOfRuns - 1;
        // skip over noise --- jumps over two pixel noise currently.
        //HW--added CONSTANT for noise jumps.
        if (last > 0 && runs[last].x == x &&
            (runs[last].y - (y + h) <= SKIPS)) {
            runs[last].h += runs[last].y - y; // merge run lengths
            runs[last].y = y; // reset the new y val
            h = runs[last].h;
            numberOfRuns--; // don't count this merge as a new run
        } else {
            runs[numberOfRuns].x = x;
            runs[numberOfRuns].y = y;
            runs[numberOfRuns].h = h;
        }
        numberOfRuns++;
    }
}


void Robots::setImageBox(int i, int j, int value){
	imageBoxes[i][j] = value;
}

/* Print debugging information for a blob.
 * @param b    the blob
 */
void Robots::printBlob(Blob b) {
#if defined OFFLINE
	cout << "Outputting blob" << endl;
	cout << b.getLeftTopX() << " " << b.getLeftTopY() << " " << b.getRightTopX() << " "
		 << b.getRightTopY() << endl;
	cout << b.getLeftBottomX() << " " << b.getLeftBottomY() << " " << b.getRightBottomX()
		 << " " << b.getRightBottomY() << endl;
#endif
}

}
}
