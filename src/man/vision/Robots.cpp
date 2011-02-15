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
 * While it is true that this code sort of worked in 2008, we were
 * foolishly using high-res images at that time.  Since then the code
 * has languished and is frankly a mess.  However, the basic ideas
 * are pretty good and it could probably be salvaged when someone
 * has the time to do so.  We do not currently have this code turned
 * on.
 *
 * The major idea of this code is to do a highly modified version of
 * run-length-encoding where the runs are hybrid color runs that run
 * from the first seen bit of color in a scanline to the last.  Since
 * the colored pieces of the robot are not connected we often have
 * to glue disparate blobs together.  This is not easy to do and it
 * is where most of the work is needed.
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


void Robots::robot(int bigGreen)
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
                blobs->blobIt(k, lastruny, lastrunh);
            }
        }
		// now we can add the run normally
        blobs->blobIt(runs[i].x, runs[i].y, runs[i].h);
		// set the current as the last
        lastrunx = runs[i].x; lastruny = runs[i].y; lastrunh = runs[i].h;
    }
    // check each of the candidate blobs to see if it might reasonably be
    // called a piece of a robot
    for (int i = 0; i < blobs->number(); i++) {
		// TODO:  Also need a check that there is some actual blob of
		// color - we get carried away building this and can do so with
		// very little in the way of actual robot color, which is dangerous
		if (blobs->get(i).getBottom() < field->horizonAt(blobs->get(i).getLeft())) {
			blobs->init(i);
		} else if (noWhite(blobs->get(i))) {
			thresh->drawRect(blobs->get(i).getLeft(), blobs->get(i).getTop(),
							 blobs->get(i).width(), blobs->get(i).height(), WHITE);
			blobs->init(i);
		}
		if (blobs->get(i).height() > blobHeightMin) {
			expandRobotBlob(i);
		}
    }
    // now that we've done some expansion, see if we can merge any of the big blobs
    mergeBigBlobs();
    // try expanding again after the merging
    for (int i = 0; i < blobs->number(); i++) {
		if (ROBOTSDEBUG) {
			if (blobs->get(i).width() > 1) {
				thresh->drawRect(blobs->get(i).getLeft(), blobs->get(i).getTop(),
								 blobs->get(i).width(), blobs->get(i).height(), BLACK);
			}
		}
	}
	// TODO:  Need to see if any of our remaining blobs are subsumed by other blobs
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

/* We have a swatch of color.  Let's make sure that there is some white
   around somewhere as befits our robots.
 */

bool Robots::noWhite(Blob b) {
	const int MINWHITE = 5;

	int left = b.getLeft(), right = b.getRight();
	int top = b.getTop(), bottom = b.getBottom();
	int width = b.width();
	int tops, bottoms;
	for (int i = 1; i < 10; i++) {
		tops = 0; bottoms = 0;
		for (int x = left; x <= right; x++) {
			if (thresh->thresholded[top - i][x] == WHITE)
				tops++;
			if (thresh->thresholded[bottom+i][x] == WHITE)
				bottoms++;
			if (tops > width / 2 || tops == width) return false;
			if (bottoms > width / 2 || tops == width) return false;
		}
	}
	// perhaps there is white inside (hands do this sometime)
	for (int i = top; i <= bottom; i++) {
		tops = 0; bottoms = 0;
		for (int x = left; x <= right; x++) {
			if (thresh->thresholded[i][x] == WHITE)
				tops++;
			if (tops > width / 4) return false;
		}
	}
	return true;
}

/*  We have a "blob" that might be part of a robot.
	We built our blobs a little strangely and they
    may not be complete.  Let's see if we can expand them a bit.
	Essentially we look around the
    sides of the blobs and see if we can expand the area of the blob.
 */

void Robots::expandRobotBlob(int which)
{
	const int BIGGAIN = 10;
	int gain;
	expandHorizontally(which, 1);
	expandHorizontally(which, -1);
	gain = expandVertically(which, 1);
	gain += expandVertically(which, -1);
	//cout << "Blob " << which << " gained " << gain << endl;
	if (gain > BIGGAIN) {
		expandHorizontally(which, 1);
		expandHorizontally(which, -1);
    }
}

/* Try and expand one of the sides of the blob.
   Keep scanning vertically from the side of the robot
   looking for evidence that you are still getting part
   of the robot.  Stop when you don't have that evidence.
   Reset the size of the robot accordingly.
 */

void Robots::expandHorizontally(int which, int dir) {
	int boundary;
	if (dir < 0)
		boundary = blobs->get(which).getLeft();
	else
		boundary = blobs->get(which).getRight();
	int top = blobs->get(which).getTop();
	int bottom = blobs->get(which).getBottom();
	int height = blobs->get(which).height();
    int x, y;
    bool good = true;   // lets us know if we are still expanding
	int goodPix, gotCol, white, opposites, green;
	// loop until we hit the image edge or can't expand anymore
    for (x = boundary; good && x >  -1 && x < IMAGE_WIDTH; x+= dir) {
        good = false;
		gotCol = 0;
		goodPix = 0;
		white = 0;
		opposites = 0;
		green = 0;
        for (y = top; y < bottom && !good; y += 1) {
			int pix = thresh->thresholded[y][x];
            if (pix == color) {
                gotCol++;
			}
			else if (pix == WHITE) {
				white++;
			} else if (pix == GREY) {
				// useful, but not as good as white
				goodPix++;
			} else if ((color ==  NAVY && pix == RED) ||
					   (color == RED && pix == NAVY)) {
				opposites++;
			} else if (pix == GREEN) {
				green++;
			}
			good = goodScan(gotCol, white, opposites, goodPix, green, height);
		}
	}
	if (dir < 0)
		blobs->setLeft(which, x+1);
	else
		blobs->setRight(which, x-1);
}

/* Try and expand one of the sides of the blob.
   Keep scanning horizontally from above or below the robot
   looking for evidence that you are still getting part
   of the robot.  Stop when you don't have that evidence.
   Reset the size of the robot accordingly.  Keep track
   of how much the robot grew so that we might make
   a decision about whether or not to do the horizontal
   over again.
 */

int Robots::expandVertically(int which, int dir) {
	int boundary;
	if (dir < 0)
		boundary = blobs->get(which).getTop();
	else
		boundary = blobs->get(which).getBottom();
	int left = blobs->get(which).getLeft();
	int right = blobs->get(which).getRight();
	int width = blobs->get(which).width();
	int topBound = field->horizonAt(left); // cap at horizon
    int whites = 0, pix, greys = 0, col = 0, opposites = 0, green = 0, gain = 0;
	bool good = true;
	int x, y;
    // loop until we can't expand anymore
    for (y = boundary; good && y > topBound && y < IMAGE_HEIGHT; y+=dir){
        whites = 0;
		col = 0;
		greys = 0;
		opposites = 0;
		good = false;
		gain++;
		green = 0;
		// check this row of pixels for white or same color (good),
		// grey (pretty good), or for opposite color (bad)
        for (x = left; x < right && !good; x++) {
            pix = thresh->thresholded[y][x];
            if (pix == color) {
                col++;
            } else if (pix == WHITE) {
                whites++;
            } else if ((color ==  NAVY && pix == RED) ||
                       (color == RED && pix == NAVY)) {
				// Uh oh, we may be seeing another robot of the opposite color.
				opposites++;
            } else if (pix == GREY) {
				greys++;
			} else if (pix == GREEN) {
				green++;
			}
			good = goodScan(col, whites, opposites, greys, green, width);
        }
    }
	//cout << "In vert " << col << " " << whites << " " << opposites <<
	//" " << greys << " " << green << " " << width << endl;
	gain--;
	if (gain <= 0) return 0;
	if (dir < 0) {
		blobs->setTop(which, y+1);
	} else {
		blobs->setBottom(which, y-1);
	}
	return gain;
}

/* Determines whether we have enough evidence to call a scan for
   expanding the robot "good" or not.
 */
bool Robots::goodScan(int col, int white, int opp, int greys, int green, int total) {
	const int EXPAND = 10; // set at a level that is bigger than most lines
	const int COLORSWATCH = 5;

	if (green > total / 4)
		return false;
	if (greys + white > total / 2 && white > EXPAND)
		return true;
	if (col > COLORSWATCH && white * 2 > EXPAND)
		return true;
	return false;
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
	//cout << "In merge" << endl;
    for (int i = 0; i < blobs->number() - 1; i++) {
		if (blobs->get(i).getLeft() > -1 && blobs->get(i).height() > 1) {
			//cout << "Blob #" << (i+1) << endl;
			printBlob(blobs->get(i));
			for (int j = i+1; j < blobs->number(); j++) {
				if (closeEnough(blobs->get(i), blobs->get(j)) &&
					noGreen(blobs->get(i), blobs->get(j)) &&
					bigEnough(blobs->get(i), blobs->get(j))) {
					//cout << "Merging blobs " << endl;
					blobs->mergeBlobs(i, j);
					expandRobotBlob(i);
				} else {
					if (!closeEnough(blobs->get(i), blobs->get(j))) {
						//cout << "blobs were too far apart " << endl;
					} else if (!noGreen(blobs->get(i), blobs->get(j))) {
						//cout << "Blobs had green between" << endl;
					} else {
						//cout << "one blob was too small" << endl;
					}
				}
			}
		}
    }
}

/* Before we merge make sure there isn't a lot of green
 * between the pieces
 * TODO: write this
 */

bool Robots::noGreen(Blob a, Blob b) {
	// first determine where the blobs are relative to each other
	// the way we create robot blobs means that blob a will always
	// be to the left of blob b (or they will overlap)
	int left, right, top, bottom;
	if (a.getRight() > b.getLeft()) {
		// they overlap in the horizontal dimension - find the intersection
		left = b.getLeft();
		right = a.getRight();
		if (a.getBottom() < b.getTop()) {
			// a is over b
			top = a.getBottom();
			bottom = b.getTop();
		} else if (b.getBottom() < a.getTop()) {
			// b is over a
			top = b.getBottom();
			bottom = a.getTop();
		} else
			// they overlap in both dimensions
			return true;
		return checkVertical(left, right, top, bottom);
	} else {
		left = a.getRight();
		right = b.getLeft();
		if (a.getBottom() < b.getTop()) {
			// a is also fully over b - check both
			return checkVertical(left, right, a.getBottom(), b.getTop());
		} else if (b.getBottom() < a.getTop()) {
			// b is fully over a
			return checkVertical(left, right, b.getBottom(), a.getTop());
		} else {
			// they overlap in the vertical dimension
			if (a.getTop() > b.getTop()) {
				top = a.getTop();
				if (a.getBottom() > b.getBottom()) {
					bottom = b.getBottom();
				} else {
					bottom = a.getBottom();
				}
			} else {
				top = b.getTop();
				if (a.getBottom() > b.getBottom()) {
					bottom = b.getBottom();
				} else {
					bottom = a.getBottom();
				}
			}
			return checkHorizontal(left, right, top, bottom);
		}
	}
	return true;
}

/* Check for a swath of green between two robots.  In this case
   the robots overlap in the horizontal direction so we check if
   they are separated in the vertical direction
 */

bool Robots::checkVertical(int left, int right, int top, int bottom) {
	// scan vertically
	int green = 0, width = right - left;
	for (int y = top; y < bottom; y++) {
		green = 0;
		for (int x = left; x < right; x++) {
			if (thresh->thresholded[y][x] == GREEN)
				green++;
		}
		if (green > width / 2)
			return false;
	}
	return true;
}

/* Check for a swath of green between two robots.  In this case
   the robots overlap in the vertical direction so we check if
   they are separated in the horizontal direction
 */

bool Robots::checkHorizontal(int left, int right, int top, int bottom) {
	// scan horizontally
	int green = 0, height = bottom - top;
	for (int x = left; x < right; x++) {
		green = 0;
		for (int y = top; y < bottom; y++) {
			if (thresh->thresholded[y][x] == GREEN)
				green++;
		}
		if (green > height / 2)
			return false;
	}
	return true;
}

/*  Are two robot blobs close enough to merge?
    Needless to say this needs lots of experimentation.  "40" was based on some,
	but at high resolution.  Obviously it should be a constant.
 */

bool Robots::closeEnough(Blob a, Blob b)
{
    // EXAMINED: change constant to lower res stuff
    const int closeDistMax = 40;

    int xd = distance(a.getLeft(),a.getRight(),
                      b.getLeft(),b.getRight());
    int yd = distance(a.getTop(),a.getBottom(),
                      b.getTop(),b.getBottom());
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
    const int minBlobArea = 10;
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

