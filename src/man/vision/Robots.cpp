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
#include "debug.h"
#include <vector>

using namespace std;

#ifdef OFFLINE
static const bool ROBOTSDEBUG = false;
#else
static const bool ROBOTSDEBUG = false;
#endif

Robots::Robots(Vision* vis, Threshold* thr, Field* fie, Context* con, int col)
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
	blobs->init();
	numberOfRuns = 0;
}

/* Set the primary color.  Depending on the color, we have different space needs
 * @param c        the color
 */
void Robots::setColor(int c)
{
    const int RUN_VALUES = 3;           // x, y, and h
    const int RUNS_PER_LINE = 5;
    const int RUNS_PER_SCANLINE = 15;

    runsize = 1;
    int run_num = RUN_VALUES;
    color = c;
	run_num = IMAGE_WIDTH * RUNS_PER_SCANLINE;
	runsize = IMAGE_WIDTH * RUNS_PER_LINE;
    runs = (run*)malloc(sizeof(run) * run_num);
}


/* Allocate the required amount of memory dependent on the primary color
*/
void Robots::allocateColorRuns()
{
	const int RUN_VALUES = 3;         // x, y and h
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


/* Try and recognize robots.  Basically we're doing blobbing here, but with lots of extra
   twists.  Mainly we're being extremely loose with the conditions under which we consider
   blobs to be "connected."  We're trying to take advantage of the properties of the robots -
   namely that they stand vertically normally.
 */


void Robots::robot(Cross* cross)
{
	if (numberOfRuns < 1) return;
    const int lastRunXInit = -30;
    const int resConst = 10;
    const int blobHeightMin = 5;
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
    for (int i = 0; i < blobs->number(); i++) {
		if (blobs->get(i).getBottom() < field->horizonAt(blobs->get(i).getLeft())) {
			blobs->init(i);
        } else if (!cross->checkForRobotBlobs(blobs->get(i))) {
            //} else if (noWhite(blobs->get(i))) {
			thresh->drawRect(blobs->get(i).getLeft(), blobs->get(i).getTop(),
							 blobs->get(i).width(), blobs->get(i).height(), WHITE);
			blobs->init(i);
		} else if (blobs->get(i).height() > blobs->get(i).width()) {
			thresh->drawRect(blobs->get(i).getLeft(), blobs->get(i).getTop(),
							 blobs->get(i).width(), blobs->get(i).height(), YELLOW);
            blobs->init(i);
        }
		if (blobs->get(i).height() > blobHeightMin) {
            thresh->drawRect(blobs->get(i).getLeft(), blobs->get(i).getTop(),
							 blobs->get(i).width(), blobs->get(i).height(), MAROON);
		} else if (blobs->get(i).height() > 0) {
            thresh->drawRect(blobs->get(i).getLeft(), blobs->get(i).getTop(),
							 blobs->get(i).width(), blobs->get(i).height(), WHITE);
        }
    }
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
			if (top - i >= 0 && isWhite(thresh->getThresholded(top - i,x)))
				tops++;
			if (bottom + i < IMAGE_HEIGHT && isWhite(thresh->getThresholded(bottom+i,x)))
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
    //cout << "Updating robot " << which << " " << color << endl;
    //printBlob(blobs[index]);
    if (color == RED) {
        if (which == 1) {
            vision->red1->updateRobot(blobs->get(index));
        } else {
            vision->red2->updateRobot(blobs->get(index));
        }
    } else {
        if (which == 1) {
            vision->navy1->updateRobot(blobs->get(index));
        } else {
            vision->navy2->updateRobot(blobs->get(index));
        }
    }
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

/* Calculate the horizontal distance between two objects
 * (the end of one to the start of the other).
 * @param x1    left x of one object
 * @param x2    right x of the object
 * @param x3    left x of the other
 * @param x4    right x of the other
 * @return      the distance between the objects in the x dimension
 */
int Robots::distance(int x1, int x2, int x3, int x4) {
    if (x2 < x3)
        return x3 - x2;
	if (x1 > x3 && x1 < x4)
		return 0;
    if (x1 > x4)
        return x1 - x4;
    return 0;
}

/* Print debugging information for a blob.
 * @param b    the blob
 */
void Robots::printBlob(Blob b) {
#if defined OFFLINE
/*    cout << "Outputting blob" << endl;
    cout << b.getLeftTopX() << " " << b.getLeftTopY() << " " << b.getRightTopX() << " "
         << b.getRightTopY() << endl;
    cout << b.getLeftBottomX() << " " << b.getLeftBottomY() << " " << b.getRightBottomX()
	<< " " << b.getRightBottomY() << endl;*/
#endif
}

