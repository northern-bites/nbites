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

Robots::Robots(Vision* vis, Threshold* thr, int col)
    : vision(vis), thresh(thr), color(col)
{
	const int MAX_ROBOT_RUNS = 400;
	blobs = new Blobs(MAX_ROBOT_RUNS);
    allocateColorRuns();
#ifdef OFFLINE
    ROBOTSDEBUG = false;
#else
	ROBOTSDEBUG = false;
#endif
}


/* Initialize the data structure.
 * @param s     the slope corresponding to the robot's head tilt
 */
void Robots::init()
{
	blobs->init();
	numberOfRuns = 0;
#ifdef OFFLINE
	ROBOTSDEBUG = false;
#endif
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

/* Try and recognize robots.  Basically we're doing blobbing here, but with lots of extra
   twists.  Mainly we're being extremely loose with the conditions under which we consider
   blobs to be "connected."  We're trying to take advantage of the properties of the robots -
   namely that they stand vertically normally.
 */
 
 
void Robots::robot(int bigGreen)
{
	if (numberOfRuns < 1) return;
    const int lastRunXInit = -30;
    const int resConst = 20;
    const int blobWidthMin = 5;
    const int robotBlobMin = 10;

    int lastrunx = lastRunXInit, lastruny = 0, lastrunh = 0;

    // loop through all of the runs of this color
    // NOTE: "20" is a guess at something that should be a constant
    // and with the change in resolution, this ought to be changed
    for (int i = 0; i < numberOfRuns; i++) {
        //drawPoint(runs[i].x, runs[i].y, BLACK);
        if (runs[i].x < lastrunx + resConst) {
            for (int k = lastrunx; k < runs[i].x; k+= 1) {
                //cout << "merging " << k << " " << runs[i].x << endl;
                blobs->blobIt(k, lastruny, lastrunh);
            }
        }
		// now we can add the run normally
        blobs->blobIt(runs[i].x, runs[i].y, runs[i].h);
		// set the current as the last
        lastrunx = runs[i].x; lastruny = runs[i].y; lastrunh = runs[i].h;
    }
    // make first pass at attempting to identify robots
    getRobots(bigGreen);
    // check each of the candidate blobs to see if it might reasonably be
    // called a piece of a robot
    for (int i = 0; i < blobs->number(); i++) {
		// NOTE: "5" is another constant that needs to be checked and changed
		if (blobs->get(i).width() > blobWidthMin) {
			// see if we can expand it to other parts
			expandRobotBlob(i);
		}
    }
    // now that we've done some expansion, see if we can merge any of the big blobs
    mergeBigBlobs();
    // try expanding again after the merging
    for (int i = 0; i < blobs->number(); i++) {
        if (blobs->get(i).width() > blobWidthMin) {
            expandRobotBlob(i);
        }
    }
    int biggest = -1, index1 = -1, second = -1, index2 = -1;
    // collect up the two biggest blobs - those are the two we'll put into field objects
    for (int i = 0; i < blobs->number(); i++) {
		// for now we'll use closest y - eventually we should use pixestimated distance
        // TODO: for now we'll use closest y - eventually we should use
        // pixestimated distance
        int area = blobs->get(i).getArea();
        if (viableRobot(blobs->get(i)) && blobs->get(i).getArea() >= biggest) {
            second = biggest;
            index2 = index1;
            index1 = i;
            biggest = blobs->get(i).getArea();
        } else if (viableRobot(blobs->get(i)) && blobs->get(i).getArea() > robotBlobMin) {
            second = area;
            index2 = i;
        }
    }
    // if we found some viable blobs, then add them as field objects
    if (index1 != -1) {
        updateRobots(1, index1);
        if (index2 != -1)
            updateRobots(2, index2);
    }
}

/*  We have a "blob" that might be part of a robot.  We built our blobs a little strangely and they
    may not be complete.  Let's see if we can expand them a bit.  Essentially we look around the
    sides of the blobs and see if we can expand the area of the blob.
 */

void Robots::expandRobotBlob(int which)
{
	const int blobDiv = 4;
    const int goodsMin = 5;

    //int spany = blobHeight(topBlob);
    //int spanx = blobWidth(topBlob);
    // expand right side
    int x, y;
    int bestr = blobs->get(which).getRight();
    bool good = true;
    // start on the right side and keep going until we're sure we're done
    for (x = bestr; good && x < IMAGE_WIDTH - 1; x++) {
        good = false;
		// if we see anything of the right color that's good enough to expand our blob
        for (y = blobs->get(which).getRightTopY(); y < blobs->get(which).getRightBottomY() && !good;
             y += 1) {
            if (thresh->thresholded[y][x] == color)
                good = true;
        }
    }
    // now reset the right side information to reflect the new state of things
    blobs->get(which).setRightTopX(x - 1);
    blobs->get(which).setRightBottomX(x - 1);
    // repeat the process on the left side
    good = true;
    for (x = blobs->get(which).getLeftTopX(); good && x >  -1; x--) {
        good = false;
        for (y = blobs->get(which).getRightTopY(); y < blobs->get(which).getRightBottomY() && !good;
             y += 2) {
            if (thresh->thresholded[y][x] == color)
                good = true;
        }
    }
    blobs->get(which).setLeftTopX(x + 1);
    blobs->get(which).setLeftBottomX(x + 1);


    // now try the bottom.  We're going to do this differently.
    // robots are mainly white, so if we run into a big swatch of white we'll
    // assume that its the same robot.
    int whites = IMAGE_WIDTH, pix, width = blobs->get(which).width() / blobDiv;
    int goods = 0, lastSaw = 0;
    // loop down from the bottom until we can't expand anymore
    for (y = blobs->get(which).getLeftBottomY(); whites >= width && y < IMAGE_HEIGHT - 1;y++){
        whites = 0;
        goods = 0;
		// check this row of pixels for white or same color (good), or for opposite color (bad)
        for (x = blobs->get(which).getLeftBottomX(); x < blobs->get(which).getRightTopX() && whites < width;
             x++) {
            if(blobs->get(which).getLeftBottomX() < 0) {
                cout << "Top blob left bottom is negative" << endl;
            }
            pix = thresh->thresholded[y][x];
            if (pix == color) {
                whites++;
				// theoretically a color match of enough pixels would be plenty of evidence that we're
				// able to expand here.  Currently we don't seem to increment goods.  Something to
				// experiment with.
                if (goods > goodsMin) {
                    whites = width;
                }
            } else if (pix == WHITE) {
                whites++;
            } else if ((color ==  NAVY && pix == RED) ||
                       (color == RED && pix == NAVY)) {
				// Uh oh, we may be seeing another robot of the opposite color.  Could be trouble.
                whites -= goodsMin;
            }
        }
        if (goods < goodsMin) {
            lastSaw++;
        } else {
            lastSaw = 0;
        }
    }
    int gain = y - 1 - blobs->get(which).getLeftBottomY();
    blobs->get(which).setLeftBottomY(y - 1);
    blobs->get(which).setRightBottomY(y - 1);
    // if we expanded enough, it is probably worth looking at the sides again.
    if (gain > goodsMin) {
        good = true;
        for (x = blobs->get(which).getRightTopX(); good && x < IMAGE_WIDTH - 1; x++) {
            good = false;
            for (y = blobs->get(which).getRightBottomY() - gain; y < blobs->get(which).getRightBottomY()
                     && !good; y++) {
                if (thresh->thresholded[y][x] == color)
                    good = true;
            }
        }
        blobs->get(which).setRightTopX(x - 1);
        blobs->get(which).setRightBottomX(x - 1);
        good = true;
        for (x = blobs->get(which).getLeftTopX(); good && x >  -1; x--) {
            good = false;
            for (y = blobs->get(which).getRightBottomY() - gain; y < blobs->get(which).getRightBottomY()
                     && !good; y++) {
                if (thresh->thresholded[y][x] == color)
                    good = true;
            }
        }
        blobs->get(which).setLeftTopX(x + 1);
        blobs->get(which).setLeftBottomX(x + 1);
    }
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

/* Like regular merging of blobs except that with robots we used a relaxed criteria.
 */
void Robots::mergeBigBlobs()
{
    for (int i = 0; i < blobs->number() - 1; i++) {
        for (int j = i+1; j < blobs->number(); j++) {
            if (closeEnough(blobs->get(i), blobs->get(j)) &&
                bigEnough(blobs->get(i), blobs->get(j))) {
                //cout << "Merging blobs " << endl;
                //printBlob(blobs[i]);
                //printBlob(blobs[j]);
				blobs->get(j).merge(blobs->get(i));
                //cout << "Merged are now" << endl;
                //printBlob(blobs[i]);
                //printBlob(blobs[j]);
            }
        }
    }
}

/*  Are two robot blobs close enough to merge?
    Needless to say this needs lots of experimentation.  "40" was based on some, but
    at high resolution.  Obviously it should be a constant.
 */

bool Robots::closeEnough(Blob a, Blob b)
{
    // EXAMINED: change constant to lower res stuff
    const int closeDistMax = 40;

    int xd = distance(a.getLeftTopX(),a.getRightTopX(),
                      b.getLeftTopX(),b.getRightTopX());
    int yd = distance(a.getLeftTopY(),a.getLeftBottomY(),
                      b.getLeftTopY(),b.getRightBottomY());
    if (xd < closeDistMax) {
        if (yd < closeDistMax)
            return true;
    }
    // if (xd < max(blobWidth(a), blobWidth(b)) &&
    //     yd < max(blobHeight(a), /blobHeight(b))) return true;
    return false;
}

/*  Are the two blobs big enough to merge.  Again the constants are merely guesses
    at this stage.  And guesses at high rez to boot.
 */

bool Robots::bigEnough(Blob a, Blob b)
{
    // EXAMINED: change constant to lower res stuff // at half right now
    const int minBlobArea = 100;
    const int horizonOffset = 50;

    if (a.getArea() > minBlobArea && b.getArea() > minBlobArea)
        return true;
    return false;
}


/*  Is this blob potentially a robot?  Return true if so.  Basically we look at
    the blob and see how many pixels seem to be "robot" pixels.  If there are
    enough, then we call it good enough.  This is probably a dumb way to do this
    because it is slow as heck.
    See the comments for the other things - the constants should become real constants!
    @param a   the index of the blob we're checking
    @return    whether it meets our criteria
 */

bool Robots::viableRobot(Blob a)
{
    const int blobPix = 10;
    const float blobAreaMin = 0.10f;

    // get rid of obviously false ones
    // TODO: change constant to lower res stuff
    if (!(a.width() > blobPix)) {
        return false;
    }
    int whites = 0;
    int col = 0;
    for (int i = 0; i < a.width(); i+=2) {
        for (int j = 0; j < a.height(); j+=2) {
            int newpix = thresh->thresholded[j+a.getLeftTopY()][i+a.getLeftTopX()];
            if (newpix == WHITE) {
                whites++;
            } else if (newpix == color) {
                col++;
            }
        }
    }
    if ((float)(whites + col) / (float)a.getArea() > blobAreaMin)
        return true;
    return false;
}


/* Find robot blobs.
   @param maxY     max value - will be used to pick heads out
*/

void Robots::getRobots(int maxY)
{
    const int spreadInit = 3;

    int size = 0;
    int topSpot = 0;
    // TODO: experiment with this constant; probably 3; maybe 2
    int spreadSeparation = spreadInit;
    //check each blob in the array
    for (int i = 0; i < blobs->number(); i++) {
        int spread = blobs->get(i).width();
        if (blobs->get(i).getLeftTopX() < 0) {
        } else if (blobs->get(i).getLeftTopY() > maxY || spread < spreadSeparation) {
            //drawBlob(blobs->get(i), BLACK);
        } else {
            // could be a robot or robots
            //splitBlob(i);
        }
        if (blobs->get(i).getArea() > size) {
            size = blobs->get(i).getArea();
            //topBlob = blobs->get(i);
            topSpot = i; //store the one with the largest size.
        }
    }
    size = 0;
    /*for (int i = 0; i < numBlobs; i++) {
        int spread = blobWidth(blobs->get(i));
        if (blobs->get(i).getLeftTopX() != BADVALUE && blobs->get(i).getLeftTopY() < maxY
            && spread > spreadSeparation) {
            //drawBlob(blobs->get(i), ORANGE);
        }
        if (blobs->get(i).area > size && i != topSpot) {
            size = blobs->get(i).area;
            secondBlob = blobs->get(i);
        }
        //drawBlob(blobs->get(i), BLACK);
		}*/
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
	const int SKIPS = 4;

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
    if (x1 > x4)
        return x1 - x4;
    return 0;
}

