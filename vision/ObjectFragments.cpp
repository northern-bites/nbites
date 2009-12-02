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
 * Object Fragment class - a ChownDawg production
 * ObjectFragments is no longer the major vision class. Instead it is
 * now simply the place where we process goals.  We no longer do any
 * run-length-encoding here.  We still use the color "run" collected
 * in Threshold.cpp, but now we simply use them to look for likely
 * posts.  Once we have identified a likely location then we  do
 * active scanning to determine the outline of the post (if it is
 * indeed a post).  After that we do lots of sanity checking to
 * make sure that the post is reasonable.  In addition we try and
 * figure out whether we are looking at a right or left post.  There
 * are a number of ways to do this (is another post around?  do we
 * see the crossbar?  the goalbox? etc.).
*/

#include <iostream>
#include "ObjectFragments.h"
#include "debug.h"
#include <vector>

using namespace std;


//here are defined the lower bounds on the sizes of goals, posts, and balls
//IMPORTANT: they are only guesses right now.

#define MIN_GOAL_HEIGHT	25
#define MIN_GOAL_WIDTH  5

// ID information on goal post constant
static const int NOPOST = 0;   // don't know which
static const int RIGHT = 1;
static const int LEFT = 2;
static const int BACKSTOP = 4;

// Relative size of posts
static const int LARGE = 2;
static const int MEDIUM = 1;
static const int SMALL = 0;

// Universal bad value used to id whether or not we successfully did something
static const int BADVALUE = -100;

// actually just skips 2 pixel noise in runs
static const int NOISE_SKIP = 3;
// minimum distance between goal and post - changed from 50 to 40 JS
static const int MIN_SPLIT = 40;
// Amount of post that has to match (so backstop isn't counted in posts)
static const float PERCENTMATCH = 0.65f;
// threshold for expanding sides back out
static const float HALFISH = 0.49f;
// highest ratio of width over height for posts
static const float GOODRAT = 0.75f;
// indicator that post may be salvagable
static const float SQUATRAT = 1.2f;
// EXAMINED: lowered
// goal posts of the same color have to be this far apart
static const int MIN_POST_SEPARATION = 7;
// how big a post is to be declared a big post
// EXAMINED: change this
static const int BIGPOST = 25;
static const float NORMALPOST = 0.45f;
static const float QUESTIONABLEPOST = 0.85f;
static const int DIST_POINT_FUDGE = 5;

#ifdef OFFLINE
static const bool PRINTOBJS = false;
static const bool POSTDEBUG = false;
static const bool POSTLOGIC = false;
static const bool TOPFIND = false;
static const bool CORNERDEBUG = false;
static const bool SANITY = false;
static const bool CORRECT = false;
#else
static const bool PRINTOBJS = false;
static const bool POSTDEBUG = false;
static const bool POSTLOGIC = false;
static const bool TOPFIND = false;
static const bool CORNERDEBUG = false;
static const bool SANITY = false;
static const bool CORRECT = false;
#endif

//previous constants inserted from .h class


ObjectFragments::ObjectFragments(Vision* vis, Threshold* thr, Field* fie, int _color)
    : vision(vis), thresh(thr), field(fie), color(_color), runsize(1)
{
    init(0.0);
    allocateColorRuns();
}


/* Initialize the data structure.
 * @param s     the slope corresponding to the robot's head tilt
 */
void ObjectFragments::init(float s)
{
    slope = s;
    numberOfRuns = 0;
}


/*  Our "main" methods.  Entry points into just about everything else.
 */


/* This is the entry  point from object recognition in Threshold.cc  For now it
 * is only called on yellow, blue and orange.
 * In each case we search for objects that have the corresponding colors.
 * @param  c            color we are processing
 * @return              always 0
 */

void ObjectFragments::createObject() {
    // these are in the relative order that they should be called
    switch (color) {
    case BLUE:
		goalScan(vision->bglp, vision->bgrp, vision->bgCrossbar, BLUE, BLUEGREEN);
        break;
    case YELLOW:
		goalScan(vision->yglp, vision->ygrp, vision->ygCrossbar, YELLOW,
				 ORANGEYELLOW);
        break;
    }
}


/*  Methods that have to do with the processing of "runs"
 */

/* Set the primary color.  Depending on the color, we have different space needs
 * @param c        the color
 */
void ObjectFragments::setColor(int c)
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
void ObjectFragments::allocateColorRuns()
{
	const int RUN_VALUES = 3;         // x, y and h
	const int RUNS_PER_SCANLINE = 15;
    const int RUNS_PER_LINE = 5;

    int run_num = RUN_VALUES;
	run_num = IMAGE_WIDTH * RUNS_PER_SCANLINE;
	runsize = IMAGE_WIDTH * RUNS_PER_LINE;
    runs = (run*)malloc(sizeof(run) * run_num);
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
void ObjectFragments::newRun(int x, int y, int h)
{
    const int RUN_VALUES = 3;    // x, y, and h of course

    if (numberOfRuns < runsize) {
        int last = numberOfRuns - 1;
        // skip over noise --- jumps over two pixel noise currently.
        //HW--added CONSTANT for noise jumps.
        if (last > 0 && runs[last].x == x &&
            (runs[last].y - (y + h) <= NOISE_SKIP)) {
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


/* Find the index of the biggest run left.  The "run" is our basic currency in
 * vision.  Our idea in looking for the goal is to find the biggest run we can
 * and assume that it is part of a goal beacon.  We look from there for a big
 * square of the right color.  This method is very simple, it just scans all of
 * the runs between "left" and "right" and picks out the bigest one.  Once done
 * it returns the corresponding index.
 *
 * @param left    the left boundary of legal runs to consider
 * @param right   the right boundary of legal runs to consider
 * @param hor     a horizon boundary that we do not currently use
 * @return index  the index of the largest run that meets the criteria
 */
int ObjectFragments::getBigRun(int left, int right) {
    int maxRun = -100;
    int nextH = 0;
    int nextX = 0;
    int nextY = 0;
    int index = BADVALUE;
    // find the biggest Run
    for (int i = 0; i < numberOfRuns; i++) {
        nextH = runs[i].h;
        nextX = runs[i].x;
        nextY = runs[i].y;
        if (nextH > maxRun && (nextX < left || nextX > right)) {
            maxRun = nextH;
            index = i;
        }
    }
    return index;
}


/* The next group of methods has to do with scanning along axis parallel
 * dimensions in order to create objects without blobbing.
 */


/*  Find the visual horizon at the indicated x value.
 * that uses the edge of the field for a better estimate.
 * @param x     column where we'd like to know the horizon
 * @return      field horizon at that point
 */
int ObjectFragments::horizonAt(int x) {
    return field->horizonAt(x);
}

/* Project a line given a start coord and a new y value - note that this is
 * dangerous depending on how you do the projection.
 *
 * @param startx   the x point to start at
 * @param starty   the y point to start at
 * @param newy     the y point to end at
 * @return         the corresponding x point
 */
int ObjectFragments::xProject(int startx, int starty, int newy)
{
    //slope is a float representing the slope of the horizon.
    return startx - ROUND2(slope * (float)(newy - starty));
}

/* Project a line given a start coord and a new y value - note that this is
 * dangerous depending on how you do the projection.
 *
 * @param point    the point to start at
 * @param newy     the y point to end at
 * @return         the corresponding x point
 */
int ObjectFragments::xProject(point <int> point, int newy) {
    //slope is a float representing the slope of the horizon.
    return point.x - ROUND2(slope * (float)(newy - point.y));
}

/* Project a line given a start coord and a new x value
 * @param startx   the x point to start at
 * @param starty   the y point to start at
 * @param newx     the x point to end at
 * @return         the corresponding y point
 */
int ObjectFragments::yProject(int startx, int starty, int newx)
{
    return starty + ROUND2(slope * (float)(newx - startx));
}

/* Project a line given a start coord and a new x value
 * @param point    the point to start at
 * @param newx     the x point to end at
 * @return         the corresponding y point
 */
int ObjectFragments::yProject(point <int> point, int newx)
{
    return point.y + ROUND2(slope * (float)(newx - point.x));
}

/* Scan from the point along the line until you have hit "stopper" points that
 * aren't color "c" return the last good point found and how many good and bad
 * points seen.  Though this is a void function it actually "returns"
 * information in the scan variable. scan.x and scan.y represent the finish
 * points of the line (last point of appropriate color) and bad and good
 * represent how many bad and good pixels (pixels that are of the right color
 * or not) along the way.
 * @param x        the x point to start at
 * @param y        the y point to start at
 * @param dir      the direction of the scan (positive or negative)
 * @param stopper  how many incorrectly colored pixels we can live with
 * @param c        color we are most interested in
 * @param c2       soft color that could also work
 */
void ObjectFragments::vertScan(int x, int y, int dir, int stopper, int c,
                               int c2, stop &scan)
{
    scan.good = 0;
    scan.bad = 0;
    scan.x = x;
    scan.y = y;
    int bad = 0;
    int good = 0;
    int startX = x;
    int startY = y;
    int run = 1;
    int width = IMAGE_WIDTH;
    int height = IMAGE_HEIGHT;
    // go until we hit enough bad pixels or are at a screen edge
    for ( ; x > -1 && y > -1 && x < width && y < height && bad < stopper; ) {
        //cout << "Vert scan " << x << " " << y << endl;
        // if it is the color we're looking for - good
        if (thresh->thresholded[y][x] == c || thresh->thresholded[y][x] == c2) {
            good++;
            run++;
            if (run > 1) {
                scan.x = x;
                scan.y = y;
            }
        } else {
            bad++;
            run = 0;
        }
        y = y + dir;
        x = xProject(startX, startY, y);
    }
    scan.bad = bad;
    scan.good = good;
    //cout << " out vert " << endl;
}

/* Scan from the point along the line until you have hit "stopper" points that
 * aren't color "c"
 * return the last good point found and how many good and bad points seen.
 * Though this is a void function it actually "returns" information in the scan
 * variable. scan.x and scan.y represent the finish points of the line (last
 * point of appropriate color) and bad and good represent how many bad and good
 * pixels (pixels that are of the right color or not) along the way.
 *
 * @param x          the x point to start at
 * @param y          the y point to start at
 * @param dir        the direction of the scan (positive or negative)
 * @param stopper    how many incorrectly colored pixels we can live with
 * @param c          color we are most interested in
 * @param c2         soft color that could also work
 * @param leftBound  furthest left we can go
 * @param rightBound further right we can go
 */
void ObjectFragments::horizontalScan(int x, int y, int dir, int stopper, int c,
                                     int c2, int leftBound, int rightBound,
									 stop & scan) {
    scan.good = 0;
    scan.bad = 0;
    scan.x = x;
    scan.y = y;
    int bad = 0;
    int good = 0;
    int run = 0;
    int startX = x;
    int startY = y;
    int height = IMAGE_HEIGHT;
    // go until we hit enough bad pixels or are at a screen edge
    for ( ; x > leftBound && y > -1 && x < rightBound && x < IMAGE_WIDTH
              && y < height && bad < stopper; ) {
        if (thresh->thresholded[y][x] == c || thresh->thresholded[y][x] == c2) {
            // if it is either of the colors we're looking for - good
            good++;
            run++;
            if (run > 1) {
                scan.x = x;
                scan.y = y;
            }
        } else {
            bad++;
            run = 0;
        }
        x = x + dir;
        y = yProject(startX, startY, x);
    }
    scan.bad = bad;
    scan.good = good;
    //cout << "return with " << temp.x << endl;
}

/*
 * TODO: Check this edge value
 * TODO: This isn't actually used.  But it would be nice to break free
 * of the standard color approach.
 * Given two points determine if they constitute an "edge".  For now our
 * definition of an edge is a difference in Y values of 30 (sort of a standard
 * approach in our league).  This is a place for potential improvements in the
 * future.
 *
 * @param x        the x value of the first point
 * @param y        the y value of the first point
 * @param x2       the x value of the second point
 * @param y2       the y value of the second point
 * @return         was an edge detected?
 */

bool ObjectFragments::checkEdge(int x, int y, int x2, int y2)
{
    const int DIFFMIN = 30;  // the difference that constitutes an edge

    int ydiff = abs(thresh->getY(x, y) - thresh->getY(x2, y2));
    if (ydiff > DIFFMIN) {
        return true;
    }
    return false;
}

/*
 * Given two points that should define an edge of a structure, try and improve
 * them to find the true edge. The idea is pretty simply.  The passed in points
 * form a potential edge to our goal.  However, they are just a starting point.
 * WHat we want to do is to move the edge until it hits the real edge of the
 * object.  So we essentially loop, constantly moving the edge in the passed
 * in direction until we get to the new edge.
 * TODO:  Use edge detection?
 * TODO:  Do lower res scanning?
 *
 * @param top         the topmost estimated point
 * @param bottom      the bottommost estimated point
 * @param c           the primary color of the structure
 * @param c2          the secondary color of the structure
 * @param left        whether the open space is to the left or the right
 */

void ObjectFragments::findVerticalEdge(point <int>& top,
                                                 point <int>& bottom,
                                                 int c, int c2, bool left)
{
    const int POST_DIVISOR = 3;
	const int MIN_WIDTH = 5;
	const int INITIAL_MIN = 5;
	const int IMAGE_DIFFERENCE = 3;

    int increment = 1;
    int spanY = bottom.y - top.y + 1;
    int count = 0;
    int good = spanY;
    int goodRun = 0;
    int j = 0;
    int dir = 1;
    int theSpot = 0;
    //int xShift = 0;
    int run = 0;
    int initRun = 0;
    if (left)
        dir = -1;
    int minCount = (spanY / POST_DIVISOR) / increment;
    int minRun = min(spanY, max(MIN_WIDTH, spanY / 2));
    int minGood = max(1, (spanY / 4) / increment);
    int badLines = 0;
    int i = 0;
    int fake = 0;
    bool atTop = false;
    //drawPoint(top.x, top.y, RED);
    //drawPoint(bottom.x, bottom.y, RED);
    /* loop until we now longer have viable expansion
          too many bad lines in a row
          top is off the edge of the screen
       basically we scan the next line out and count how many good and bad
	   points we get along that scanline.  If there are enough good ones,
	   we expand and keep moving.  If not, then we may stop
    */
    for (j = 1; badLines < 2 && top.x + dir * j >= 0
             && top.x + dir * j < IMAGE_WIDTH; j+=increment) {
        //count = 0;
        good = 0;
        fake = 0;
        run = 0;
        goodRun = 0;
        initRun = 0;
        int actualY = yProject(top.x, top.y, top.x + dir * j);
        if (actualY < 1) atTop = true;
		// here's where we do the scanning.  Stop early if we have enough information
        for (i = actualY; count < minCount  &&
                 i <= actualY + spanY && (run < minRun || goodRun > spanY / 2)
                 && (top.y > 1 || initRun < minRun);
             i+= increment) {
            theSpot = xProject(top.x + dir * j, actualY, i);
            if (theSpot < 0 || theSpot > IMAGE_WIDTH - 1 || theSpot - dir < 0 ||
                theSpot - dir > IMAGE_WIDTH - 1 || i < 0 ||
                i > IMAGE_HEIGHT - 1) {
                fake++;
            } else {
                int curcol = thresh->thresholded[i][theSpot];
                if (curcol == c || curcol == c2) {
                    good++;
                    goodRun++;
                    initRun = -1;
                    run = 0;
                } else {
                    goodRun = 0;
                    if (initRun > -1) {
                        initRun++;
                        if (atTop && initRun > INITIAL_MIN) break;
                    }
                    if (curcol != ORANGE && curcol != WHITE)
                        run++;
                }
            }
        }
        if (good + fake < minGood || good < 1) {
            badLines++;
        }
    }

    // ok so we did the basic scanning.  The problem is that sometimes we ended
	// early because we hit a screen edge and weren't really done in the sense that
	// we had found the edge.  Try and
	// check for those situations and see if we can continue to expand the edge.
	// TODO:  Surely this could be improved
    int temp = top.x;
    top.x = top.x + dir * (j - badLines) - dir;
    top.y = yProject(temp, top.y, top.x);
    bottom.y = top.y + spanY;
    bottom.x = xProject(top.x, top.y, top.y + spanY);
    if (top.x < 2 || top.x > IMAGE_WIDTH - IMAGE_DIFFERENCE) {
        for (j = 1; count < minCount && bottom.x + dir * j >= 0
                 && bottom.x + dir * j < IMAGE_WIDTH
                 && good > minGood && run < minRun; j+=increment) {
            good = 0;
            run = 0;
            int actualY = yProject(bottom.x, bottom.y, bottom.x + dir * j);
            for (i = actualY; count < minCount && i >= actualY - spanY &&
                     run < minRun; i-= increment) {
                theSpot = xProject(bottom.x + dir * j, actualY, i);

                if (theSpot < 0 || theSpot > IMAGE_WIDTH - 1
                    || theSpot - dir < 0 || theSpot - dir > IMAGE_WIDTH - 1
                    || i < 0 || i > IMAGE_HEIGHT - 1)
                    good++;

                else {
                    if (checkEdge(theSpot, i, theSpot - dir, i)) {
                        //count++;
                    }
                    int curcol = thresh->thresholded[i][theSpot];
                    if (curcol == c || curcol == c2) {
                        good++;
                        run = 0;
                    } else {
                        run++;
                    }
                }
            }
        }
        temp = bottom.x;
        bottom.x = bottom.x + dir * (j - increment) - dir;
        bottom.y = yProject(temp, bottom.y, bottom.x);
        top.y = bottom.y - spanY;
        top.x = xProject(bottom.x, bottom.y, bottom.y - spanY);
    }
}

/*
 * Given two points that should define an edge of a structure, try and improve
 * them to find the true edge.  The idea is pretty simply.  The passed in points
 * form a potential edge to our goal.  However, they are just a starting point.
 * WHat we want to do is to move the edge until it hits the real edge of the
 * object.  So we essentially loop, constantly moving the edge in the passed
 * in direction until we get to the new edge.
 * TODO:  Use edge detection?
 * TODO:  Do lower res scanning?
 * @param left        the leftmost estimated point
 * @param right       the rightmost estimated point
 * @param c           the primary color of the structure
 * @param c2          the secondary color of the structure
 * @param up          whether the open space is up or down
 */

void ObjectFragments::findHorizontalEdge(point <int>& left,
                                                   point <int>& right, int c,
                                                   int c2, bool up)
{
    const int INITIAL_GREEN = 3;
    const int MIN_WIDTH = 5;
	const int RUN_DIVISOR = 5;
	const int COUNT_DIVISOR = 3;
	const int GOOD_DIVISOR = 3;
	const float SPAN_MULTIPLIER = 0.5;

    int spanX = right.x - left.x + 1;
    int spanY = right.y - left.y + 1;
    int count = 0;
    int good = spanX;
    int j = 0;
    int dir = 1;
    int theSpot = 0;
    int run = 0;
    int badLines = 0;
    int maxgreen = INITIAL_GREEN;
    // EXAMINED: change to lower res
    maxgreen = 100;
    if (up)
        dir = -1;
    int minRun = min(spanX, max(MIN_WIDTH, spanX / RUN_DIVISOR));
    int minCount = min(spanX, max(2, spanX / COUNT_DIVISOR));
    int minGood = max(1, spanX / GOOD_DIVISOR);
    int greens = 0;
    int fakegood = 0;

	// Main loop - keep moving the edge in the right direction
    // loop until we can't expand anymore
    for (j = 1; count < minCount && left.y + dir * j >= 0
             && left.y + dir * j < IMAGE_HEIGHT && badLines < 2
             && greens < max(minRun, maxgreen); j++) {
        good = 0;
        run = 0;
        greens = 0;
        fakegood = 0;
        int actualX = xProject(left.x, left.y, left.y + dir * j);
		// count up the good and bad pixels in this scanline
        for (int i = actualX; count < minCount && i <= actualX + spanX
                 && greens < maxgreen; i++) {
            theSpot = yProject(actualX, left.y + dir * j, i);
            if (theSpot < 0 || theSpot > IMAGE_HEIGHT - 1 || theSpot - dir < 0
                || theSpot - dir > IMAGE_HEIGHT - 1 || i < 0
                || i > IMAGE_WIDTH - 1) {
                // assume the best?
                fakegood++;
            } else {
                if (checkEdge(i, theSpot, i, theSpot - dir)) {
                    //count++;
                }
                int curcol = thresh->thresholded[theSpot][i];
                if (curcol == c || curcol == c2) {
                    good++;
                    run = 0;
                } else {
                    if (curcol != ORANGE && curcol != WHITE)
                        run++;
                    if (curcol == GREEN) {
                        greens++;
                    }
                }
            }
        }
        good += fakegood / 2;
        if (good < minGood)
            badLines++;
    }
    // if we had to stop because we hit the left edge, then see if we can go
    // farther by using the bottom.
    int temp = left.y;
    left.y =  left.y + dir * (j - badLines) - dir;
    left.x = xProject(left.x, temp, left.y);
    right.x = left.x + spanX;
    right.y = left.y + spanY;

    if( right.x > IMAGE_WIDTH) {
        right.x = IMAGE_WIDTH;
    }

    if( left.x < 0) {
        left.x = 0;
    }

    if (!up && horizonAt(left.x) > left.y) {
        // for the heck of it let's scan down
        int found = left.y;
        for (int d = left.y; d < horizonAt(left.x); d+=1) {
            good = 0;
            for (int a = left.x; a < right.x; a++) {
                if (thresh->thresholded[d][a] == c) {
                    good++;
                }
            }
            if (good > spanX * SPAN_MULTIPLIER) {
                found = d;
            }
        }
        if (found != left.y) {
            left.y = found;
            left.x = xProject(left.x, temp, left.y);
            right.x = left.x + spanX;
            right.y = left.y + spanY;
            //drawPoint(right.x, right.y, ORANGE);
            findHorizontalEdge(left, right, c, c2, up);
        }
    }
}


/*  In theory our pose information tells us the slant of an object.  In practice
	it doesn't always get it for a vareity of reasons.  This is an attempt to
	correct for the errors in the process.  At this point
    it is basically a rough draft of a good methodology, but it does seem to
	work pretty well.

	Here's what we do:  We scan out from the corners of our blob.  For each corner
	we check how much extra stuff of the same color as the goal is around.
	If opposite corners both have extra stuff then it is taken as evidence
	that the post is slanted differently than our pose estimate.  We then use
	this evidence to generate a new slope estimate that we feed back to our
	goal scanning routine.  It then uses that slope instead of the pose estimate
	to find the post.
    @param post    the blob we're examining
    @param c       primary color
    @param c2      secondary color
 */

float ObjectFragments::correct(Blob b, int color, int c2) {

	const int MIN_SIZE = 10;
	const int HEIGHT_DIVISOR = 5;
	const int ERROR_TOLERANCE = 6;
	const int LEAN_THRESH = 5;
	const int MINIMUM_SKEW = 5;

    // try and find the cross bar - start at the upper left corner
    int biggest = 0, biggest2 = 0;
	int skewr = 0, skewl = 0;
    int x = b.getLeftTopX();
    int y = b.getLeftTopY();
    int h = b.height();
    int w = b.width();
    //int need = min(w / 2, 20);
    int need = max(w, MIN_SIZE);
	float newSlope = 0.0f;
	if (CORRECT) {
		//drawBlob(b, ORANGE);
	}
	stop scan;
	// scan the left side to see how far out we can go seeing post
    for (int i = 0; i < h / HEIGHT_DIVISOR && biggest < need; i+=1) {
        int tx = xProject(x, y, y + i);
        horizontalScan(tx, y + i, -1, ERROR_TOLERANCE, color, color,
					   max(0, x - 2 * w), IMAGE_WIDTH - 1, scan);
        if (scan.good > biggest) {
            biggest = scan.good;
        }
    }
	// now the right side
    x = b.getRightTopX();
    y = b.getRightTopY();
    for (int i = 0; i < h / HEIGHT_DIVISOR && biggest2 < need; i+=1) {
        int tx = xProject(x, y, y + i);
        horizontalScan(tx, y + i, 1, ERROR_TOLERANCE, color, color, x - 1,
					   IMAGE_WIDTH - 1, scan);
        if (scan.good > biggest2) {
            biggest2 = scan.good;
        }
    }
	// What we're going to do is scan opposite corners at the bottom and subtract
	// those results.
	x = b.getRightBottomX();
	y = b.getRightBottomY();
	int bestr = 0, bestl = 0;
    for (int i = 0; i < h / HEIGHT_DIVISOR && skewr < biggest; i+=1) {
        int tx = xProject(x, y, y - i);
        horizontalScan(tx, y - i, 1, ERROR_TOLERANCE, color, color,
					   x - 1, IMAGE_WIDTH - 1, scan);
        if (scan.good > skewr) {
            skewr = scan.good;
			bestr = i;
        }
    }
	x = b.getLeftBottomX();
	y = b.getLeftBottomY();
    for (int i = 0; i < h / HEIGHT_DIVISOR && skewl < biggest2; i+=1) {
        int tx = xProject(x, y, y - i);
        horizontalScan(tx, y - i, -1, ERROR_TOLERANCE, color, color,
					   max(0, x - 2 * w), IMAGE_WIDTH - 1, scan);
        if (scan.good > skewl) {
			bestl = i;
            skewl = scan.good;
        }
    }

	int stops = 10;
	// Now based on our scanning, check if the post is leaning left
	if (biggest > LEAN_THRESH && skewr > MINIMUM_SKEW) {
		if (CORRECT)
			cout << "Left lean detected " << biggest <<
				" " << skewr << " " << bestr << endl;
		int topmove = min(skewr, biggest);
		//drawLine(b.getLeftTop.x - skewr, b.getLeftTopY(), b.getLeftBottom.x + skewr,
		//    b.getLeftBottomY(), ORANGE);
		b.setLeftTopX(max(0, b.getLeftTopX() - topmove));
		b.setRightBottomX(b.getRightBottomX() + skewr);
		// make sure we don't over-correct
		x = b.getLeftBottomX() + skewr;
		y = b.getLeftBottomY();
		int backr = 0;
		int stopper = max(stops, bestr);
		for (int i = 0; i < stopper; i+=1) {
			int tx = xProject(x, y, y - i);
			horizontalScan(tx, y - i, -1, ERROR_TOLERANCE, color, color,
						   max(0, x - 2 * w), IMAGE_WIDTH - 1, scan);
			if (scan.good > backr) {
				backr = scan.good;
			}
		}
		b.setLeftBottomX(b.getLeftBottomX() + skewr - backr);
		b.setRightTopX(min(IMAGE_WIDTH - 1, b.getRightTopX() - topmove + backr));
		newSlope = -(float)(b.getLeftBottomX() - b.getLeftTopX()) /
			(float)(b.getLeftBottomY() - b.getLeftTopY());
		return newSlope;
	}
	// based on our scanning check if it is leaning right
	if (biggest2 > LEAN_THRESH && skewl > MINIMUM_SKEW) {
		if (CORRECT)
			cout << "Right lean detected " << biggest2 << " "
				 << skewl << " " << bestl << endl;
		int topmove = min(skewl, biggest2);
		b.setRightTopX(b.getRightTopX() + topmove);
		b.setLeftBottomX(max(0, b.getLeftBottomX() - skewl));
		x = max(0, b.getRightBottomX() - skewl);
		y = b.getRightBottomY();
		int backl = 0;
		int stopper = max(stops, bestl);
		for (int i = 0; i < stopper; i+=1) {
			int tx = xProject(x, y, y - i);
			horizontalScan(tx, y - i, 1, ERROR_TOLERANCE, color, color, 0,
						   IMAGE_WIDTH - 1, scan);
			if (scan.good > backl) {
				backl = scan.good;
			}
		}
		b.setRightBottomX(min(IMAGE_WIDTH - 1, b.getRightBottomX() - skewl + backl));
		b.setLeftTopX(b.getLeftTopX() + topmove - backl);
		newSlope = -(float)(b.getRightBottomX() - b.getRightTopX()) /
			(float)(b.getRightBottomY() - b.getRightTopY());
	}
	return newSlope;
	}

/*  Routine to find a general rectangular goal.
 * We start with a point.  We scan up from the point and down from the point
 * looking for a strip of the right color.  That serves as our starting point.
 * Then we try expanding the sides outward.  The we try expanding the top and
 * bottom in a similar fashion.  Once we have our goal we call the correction
 * routine to see if we could get a better rectangle by using a different slope.
 *
 * @param x         x value of our starter point
 * @param y         y value of our starter point
 * @param c         the primary color
 * @param c2        the secondary color
 * @param obj       blob to store the data in
 */
void ObjectFragments::squareGoal(int x, int y, int c, int c2, Blob & obj)
{
    const int ERROR_TOLERANCE = 3;

	int count = 0;
	bool looping = false;
    // set bad values so we can check for failure
    obj.setLeftTopX(BADVALUE); obj.setLeftTopY(BADVALUE);
	stop scan;
	do {
		// first we try going up from our start
		vertScan(x, y, -1,  ERROR_TOLERANCE, c, c2, scan);
		int h = scan.good;
		// at this point we have a very rough idea of how tall the square is
		int top = scan.y;
		int topx = scan.x;
		// now go down
		vertScan(x, y, 1,  ERROR_TOLERANCE, c, c2, scan);
		h += scan.good;
		// if we have a really short strip, then punt
		if (h < 2) return;
		int bottom = scan.y;
		int bottomx = scan.x;
		// we have the starting points of our goal
		point <int> leftTop = point<int>(topx, top);
		point <int> rightTop = point<int>(topx, top);
		point <int> leftBottom = point<int>(bottomx, bottom);
		point <int> rightBottom = point<int>(bottomx, bottom);
		// first expand the sides
		findVerticalEdge(leftTop, leftBottom, c, c2, true);
		findVerticalEdge(rightTop, rightBottom, c, c2, false);
		// now expand the top and bottom
		findHorizontalEdge(leftTop, rightTop, c, c2, true);
		findHorizontalEdge(leftBottom, rightBottom, c, c2, false);
		// store the information into our blob
		obj.setLeftTop(leftTop);
		obj.setRightTop(rightTop);
		obj.setLeftBottom(leftBottom);
		obj.setRightBottom(rightBottom);
		float newSlope = correct(obj, c, c2);
		// if we detected that the post was leaning then redo with a new slope
		if (newSlope != 0.0) {
			if (CORRECT)
				cout << "Old slope was " << slope << " " << newSlope << endl;
			slope = newSlope;
			looping = true;
		}
		count++;
	} while (count < 2 && looping);
}

/* A collection of miscelaneous methods used in processing goals.
 */


/*  As we saw with beacons, we tend to work with blobs for convenience.  So at
 *  some point need to transfer their contents over to the field object that
 * we have identified.
 * In this case we have a goal.  Before we commit we make sure it has enough of
 * the right color.  We also collect up certainty information and pass that
 * along.
 * @param one             the field object we'd like to update
 * @param two             the blob that contains the information we need
 * @param certainty       how certain are we of its ID?
 * @param distCertainty   how certain are we of how big we think it is?
 */
bool ObjectFragments::updateObject(VisualFieldObject* one, Blob two,
                                   certainty _certainty,
                                   distanceCertainty _distCertainty) {
    // before we do this let's make sure that the object is really our color
	const float BLUEPOST = 0.6f;
	float perc = NORMALPOST;
	if (_certainty != _SURE && two.height() < 40 && color == BLUE) {
		//cout << "uppint the anty on blue" << endl;
		perc = BLUEPOST;
	}
    if (rightBlobColor(two, perc)) {
        one->updateObject(&two, _certainty, _distCertainty);
        return true;
    } else {
		//cout << "Screening object for low percentage of real color " << endl;
        return false;
    }
}

/* Here we are trying to figure out how confident we are about our values with
 * regard to how they might be used for distance calculations.  Basically if
 * an object is too near one of the screen edges, or if we have some evidence
 * that it is occluded then we note uncertainty in that dimension.
 *
 * @param pole     the object we are checking
 * @return         a constant indicating where the uncertainties (if any) lie
 */
distanceCertainty ObjectFragments::checkDist(Blob pole)
{
    const int ERROR_TOLERANCE = 6;
	int left = pole.getLeft();
	int right = pole.getRight();
	int bottom = pole.getBottom();
	int top = pole.getTop();

    distanceCertainty dc = BOTH_SURE;
    int nextX, nextY;
    if (left < DIST_POINT_FUDGE || right > IMAGE_WIDTH - DIST_POINT_FUDGE) {
        if (top < DIST_POINT_FUDGE || bottom > IMAGE_HEIGHT - DIST_POINT_FUDGE){
            return BOTH_UNSURE;
        } else {
            dc =  WIDTH_UNSURE;
        }
    } else if (top < DIST_POINT_FUDGE || bottom > IMAGE_HEIGHT -
               DIST_POINT_FUDGE) {
        return HEIGHT_UNSURE;
    }
    // we need to do one more check - make sure that the bottom of the object is
    // not obscured
    nextX = pole.getLeftBottomX();
    nextY = pole.getLeftBottomY();
	stop scan;
    do {
        vertScan(nextX, nextY, 1,  ERROR_TOLERANCE, GREEN, GREEN, scan);
        nextX = nextX + 2;
        nextY = yProject(pole.getLeftBottom(), nextX);
    } while (nextX <= pole.getRightBottomX() && scan.good < 2);
    if (scan.good > 1)
        return dc;
    else if (dc == WIDTH_UNSURE)
        return BOTH_UNSURE;
    return HEIGHT_UNSURE;
}

/*  Sometimes we process posts differently depending on how big they are.
 * This just characterizes a post's size such that we can make that
 * determination.
 *
 * TODO: These sizes are almost meaningless for the NAOs.  They have been
 * updated some, but do not reflect the true size of the Nao goals.
 * @param b   the post in question
 * @return    a constant indicating size - SMALL, MEDIUM, or LARGE
 */
// EXAMINED: change these constants
int ObjectFragments::characterizeSize(Blob b) {
    int w = b.getRightTopX() - b.getLeftTopX() + 1;
    int h = b.getLeftBottomY() - b.getLeftTopY() + 1;
    const int largePostHeight = 30;
    const int smallPostHeight = 15;
    const int smallPostWidth = 10;
    const int midPostHeight = 30;
    const int midPostWidth = 15;
    if (h > largePostHeight) return LARGE;
    if (h < smallPostHeight || w < smallPostWidth) return SMALL;
    if (h < midPostHeight || w < midPostWidth) return MEDIUM;
    return LARGE;
}

/* Sets a standard of proof for a post.  In this case that the blob comprising
 * the post is at least 60% correct color.
 * Note:  this is actually not the greatest idea when the angle of the head is
 * significantly off horizontal.
 * TODO:  This should use slopes to scan the actual blob.
 *
 * @param b   the post in question
 * @return    a constant indicating size - SMALL, MEDIUM, or LARGE
 */

bool ObjectFragments::qualityPost(Blob b, int c)
{
    const float PERCENT_NEEDED = 0.6f;  // percent of pixels that must be right

    int good = 0;
    //bool soFar;
    for (int i = b.getLeftTopX(); i < b.getRightTopX(); i++)
        for (int j = b.getLeftTopY(); j < b.getLeftBottomY(); j++)
            if (thresh->thresholded[j][i] == c)
                good++;
    if (good < b.getArea() * PERCENT_NEEDED) return false;
    return true;
}

/* Provides a kind of sanity check on the size of the post.  Essentially we are
 * looking for cases where we don't have a post, but are looking at a backstop.
 * Also just let's us know how good the size estimate is.
 * This needs lots of beefing up.
 * TODO: This needs to be updated for the Naos.  It was written for the Aibos
 * and has not been updated.
 *
 * @param b   the post in question
 * @return    a constant indicating size - SMALL, MEDIUM, or LARGE
 */

bool ObjectFragments::checkSize(Blob b, int c)
{
    const int ERROR_TOLERANCE = 6;     // how many bad pixels to scan over
	const int FAR_ENOUGH = 10;         // how far to scan to be sure
	const int PROBLEM_THRESHOLD = 5;   // haw many pixels presents a problem

    int midY = b.getLeftTopY() + (b.getLeftBottomY() - b.getLeftTopY()) / 2;
	stop scan;
    horizontalScan(b.getLeftTopX(), midY, -1,  ERROR_TOLERANCE, c, c, 0,
				   b.getLeftTopX() + 1, scan);
    //drawPoint(scan.x, scan.y, RED);
    int leftMid = scan.good;
    horizontalScan(b.getRightTopX(), midY, 1, ERROR_TOLERANCE, c, c,
				   b.getRightTopX() - 1, b.getRightTopX() + FAR_ENOUGH, scan);
    //drawPoint(scan.x, scan.y, RED);
    if (leftMid > PROBLEM_THRESHOLD && scan.good > PROBLEM_THRESHOLD)
		return false;
    return true;
}

/* Try and find the biggest post left on the screen.  We start by looking for
 * our longest "run" of the current color.
 * We then call squareGoal to expand that into a post.  Later
 * we will check if it actually meets the criteria for a good post.
 * @param c       current color
 * @param c2      secondary color
 * @param left    leftmost limit to look
 * @param right   rightmost limit to look
 * @param         indication of whether we found a decent candidate
 */

int ObjectFragments::grabPost(int c, int c2, int left,
							  int right, Blob & obj) {
    int maxRun = 0, maxY = 0, maxX = 0, index = 0;
    // find the biggest Run
    index = getBigRun(left, right);
    if (index == BADVALUE) return NOPOST;
    maxRun = runs[index].h;  maxY = runs[index].y;  maxX = runs[index].x;

    // Try and figure out the true axis-parallel post dimensions - we're going
    // to try and start right in the middle
    int startX = maxX;
    int startY = maxY + maxRun / 2;
    // starts a scan in the middle of the tallest run.
    squareGoal(startX, startY, c, c2, obj);
    // make sure we're looking at something big enough to be a post
    if (!postBigEnough(obj)) {
        return NOPOST;
    }
	// check how big it is versus how big we think it should be
	if (badDistance(obj)) {
		return NOPOST;
	}
    return LEFT; // Just return something other than NOPOST
}

/*
 * Post recognition for NAOs
 */

/*  Look for a crossbar once we think we've found a goalpost.  The idea is
 * pretty simple, if we  have done a good job with the square post,
 * then the crossbar should either shoot out of the left or right top corner.
 * So try scanning from those positions.  The tricky part, or course involves
 * being able to definitively say that what we see is in fact a bar.  I'd say
 * that right now this probably needs more work.
 *
 *  @param b   the square post
 *  @return   either RIGHT or LEFT if a crossbar found, or NOPOST if not
 */
int ObjectFragments::classifyByCrossbar(Blob b)
{
    const int MINIMUM_WIDTH = 10;
	const int HEIGHT_DIVISOR = 5;
	const int ERROR_TOLERANCE = 6;
	const int DEBUG_DRAW_SIZE = 20;
	const int DISTANTPOST = 10;

    // try and find the cross bar - start at the upper left corner
    int biggest = 0, biggest2 = 0;
	int skewr = 0, skewl = 0;
    int x = b.getLeftTopX();
    int y = b.getLeftTopY();
    int h = b.height();
    int w = b.width();
    //int need = min(w / 2, 20);
    int need = max(w, MINIMUM_WIDTH);
	stop scan;

	// don't try and look for the crossbar when the post is at the top
	// unless it is small
	if (y < 1 && w > DISTANTPOST) {
		return NOPOST;
	}

	// scan the left side to see how far out we can go seeing post
    for (int i = 0; i < h / HEIGHT_DIVISOR && biggest < need; i+=1) {
        int tx = xProject(x, y, y + i);
        horizontalScan(tx, y + i, -1, ERROR_TOLERANCE, color, color,
					   max(0, x - 2 * w), IMAGE_WIDTH - 1, scan);
        if (scan.good > biggest) {
            biggest = scan.good;
        }
    }
	// now the right side
    x = b.getRightTopX();
    y = b.getRightTopY();

    for (int i = 0; i < h / HEIGHT_DIVISOR && biggest2 < need; i+=1) {
        int tx = xProject(x, y, y + i);
        horizontalScan(tx, y + i, 1, ERROR_TOLERANCE, color, color,
					   x - 1, IMAGE_WIDTH - 1, scan);
        if (scan.good > biggest2) {
            biggest2 = scan.good;
        }
    }

    if (POSTLOGIC) {
        cout << "Cross check " << biggest << " " << biggest2 << endl;
		if (biggest > need)
			drawRect(b.getLeftTopX() - biggest, b.getLeftTopY(), biggest,
					 DEBUG_DRAW_SIZE, ORANGE);
		if (biggest2 > need)
			drawRect(x, y, biggest2, DEBUG_DRAW_SIZE, ORANGE);
    }

	if (biggest > need && biggest > 2 * biggest2) return RIGHT;
	if (biggest2 > need && biggest2 > 2 * biggest) return LEFT;

    return NOPOST;
}


/* Try to use field line information to decide which post we're looking at.
   This is actually considerably easier with the large goal boxes as the only
   times you'll see corners reasonably near a post is when they are on the
   same side as the post.
 */

int ObjectFragments::classifyByLineIntersection(Blob post) {

	const int MAXIMUM_Y_DIFF = 30;

    // TODO: check if this should be the same standard minHeight for a post
    if (post.getRightBottomY() - post.getRightTopY() < MAXIMUM_Y_DIFF) return NOPOST;
    const list <VisualCorner>* corners = vision->fieldLines->getCorners();
    int spanx = post.width();
    int spany = post.height();
    for (list <VisualCorner>::const_iterator k = corners->begin();
         k != corners->end(); k++) {
        if (k->getShape() == T) {
            if (POSTLOGIC) {
                cout << "Got a T" << endl;
            }

            //int mid = midPoint(post.getLeftBottomX(), post.getRightBottomX());
            int x = k->getX();
            int y = k->getY();
            bool closeEnough = false;
            if (y < post.getLeftBottomY() + spany) closeEnough = true;
			// if the T is higher in the visual field than the bottom of the post
			// then we have an easy job - we can decide based on which side its on
			if (closeEnough) {
				if (x <= post.getLeftBottomX())
					return LEFT;
				return RIGHT;
			}
        } else {
            int x = k->getX();
            int y = k->getY();
            bool closeEnough = false;
			// we have a somwhat more stringent standard of closeness for corners for now
            if (y < post.getLeftBottomY()) closeEnough = true;
			// if the corner is higher in the visual field than the bottom of the post
			// then we have an easy job - we can decide based on which side its on
			// This is based on the layout of the field and the limitations of the
			// camera (and the fact that the goal box is absurdly large)
			if (closeEnough) {
				if (x <= post.getLeftBottomX())
					return LEFT;
				return RIGHT;
			}
        }
    }
    return NOPOST;
}

/*  We have a post and wonder which one it is.  This method looks for the nearby
	intersection of the goal line and the goal box.  If it can be found it is the
	best way to ID the goal.
	TODO:  In the side goal case sometimes you can see a T behind the near
	post.  In such a case the post may be misidentified.  That should be fixed.
	TODO:  This isn't actually used right now.  With better corner recognition
	it should be.
    @param post    the post we have id'd
    @return        the id of the post (or lack of id)
 */

int ObjectFragments::classifyByCheckingCorners(Blob post)
{
    const int MAXIMUM_Y_DIFFERENCE = 30;  // max offset between corner and post
	const int X_TOLERANCE = 5;            // how close do X values need to be

    if (post.getRightBottomY() - post.getRightTopY() < MAXIMUM_Y_DIFFERENCE)
		return NOPOST;
    const list <VisualCorner>* corners = vision->fieldLines->getCorners();
    int spanx = post.getRightBottomX() - post.getLeftBottomX();
    for (list <VisualCorner>::const_iterator k = corners->begin();
         k != corners->end(); k++) {
        // we've already checked the Ts so ignore them
        if (k->getShape() != T) {
            if (k->getX() > post.getLeftBottomX() - spanx &&
                k->getX() < post.getRightBottomX() + spanx &&
                post.getLeftBottomY() < k->getY()) {
                if (k->getX() > post.getLeftBottomX() - X_TOLERANCE &&
                    k->getX() < post.getRightBottomX() + X_TOLERANCE) {
                    // we checked these when we checked for Ts so ignore them
                } else {
                    // if the corner is near enough we can use it to id the post
                    if (k->getY() < post.getRightBottomY() + spanx) {
                        if (k->getX() > post.getRightBottomX()) return RIGHT;
                        return LEFT;
                    }
                }
            }
        }
    }
    return NOPOST;
}


/* Another post classification method.  In this one we look left and right of
 * the post trying to find a really long run of the same color.  If we find one
 * this is good evidence since it
 *
 * may correspond to the matching post of this color.
 * @param left       leftmost value (x dim) of the post
 * @param right      rightmost value
 * @param height     the height of the post
 * @return           potential classification
 */

int ObjectFragments::classifyByOtherRuns(int left, int right, int height)
{
    const int HORIZON_TOLERANCE = 10;    // our other post should be near horizon
	const int MIN_OTHER_THRESHOLD = 20;  // how big does it have to be?

    int largel = 0;
    int larger = 0;
	int mind = MIN_POST_SEPARATION + (right - left) / 2;
    for (int i = 0; i < numberOfRuns; i++) {
        int nextX = runs[i].x;
        int nextY = runs[i].y;
        int nextH = runs[i].h;
        // meanwhile collect some information on which post we're looking at
        if (nextH > 0) {
            if (nextX < left - mind && nextH > MIN_GOAL_HEIGHT &&
                nextY < horizonAt(nextX) &&
                nextY + nextH > horizonAt(nextX) - HORIZON_TOLERANCE) {
                if (nextH > largel)
                    largel = nextH;
            } else if (nextX > right + mind && nextH > MIN_GOAL_HEIGHT &&
                       nextY < horizonAt(nextX) &&
                       nextY + nextH > horizonAt(nextX) - HORIZON_TOLERANCE) {
                if (nextH > larger) {
                    larger = nextH;
                }
            }
		}
    }
    if ((larger > height / 2 || larger > MIN_OTHER_THRESHOLD) && larger >
		largel) {
        if (POSTLOGIC)
            cout << "Larger" << endl;
        return LEFT;
    } else if (largel > MIN_OTHER_THRESHOLD || largel > height / 2) {
        if (POSTLOGIC) cout << "Largel" << endl;
        return RIGHT;
    }
	if (POSTLOGIC)
		cout << "Large R " << larger << " " << largel << " " << endl;
    return NOPOST;
}


/* Main routine for classifying posts.  We have a variety of methods to
 * classify posts in our tool box.  The idea is to start with the best ones and
 * keep trying until one produces an answer.
 *
 * @param c              color of the post
 * @param c2             secondary color
 * @param beaconFound      did we find a beacon in this image?
 * @param left           field object to send out if we find a left post
 * @param right          ditto for right post
 * @param mid            ditto for backstop
 * @return               classification
 */

int ObjectFragments::classifyFirstPost(int c,int c2,
                                       VisualFieldObject* left,
                                       VisualFieldObject* right,
                                       VisualCrossbar* mid, Blob pole)
{
    const int MIN_BLOB_SIZE = 10;

    int trueLeft = pole.getLeft();  // leftmost value
    int trueRight = pole.getRight(); // rightmost value
	int trueTop = pole.getTop();
    int trueBottom = pole.getBottom();
    int horizonLeft = horizonAt(trueLeft);
    int fakeBottom = max(trueBottom, horizonLeft);

    // start the process of figuring out which post we've got - fortunately
    // with the Naos it is easier

	// Our first test is whether we see a big blob of the same color
	// somewhere else
    int post = classifyByOtherRuns(trueLeft, trueRight,
								   fakeBottom - trueTop);
    if (post != NOPOST) {
        if (POSTLOGIC)
            cout << "Found from checkOther" << endl;
        return post;
    }

    post = classifyByCrossbar(pole);        // look for the crossbar
    if (post != NOPOST) {
        if (POSTLOGIC) {
            cout << "Found crossbar " << post << endl;
        }
        return post;
    }

    post = classifyByLineIntersection(pole);
    if (post != NOPOST) {
        if (POSTLOGIC)
            cout << "Found from Intersection" << endl;
        return post;
    }

    return post;
}

/* Look for goal posts.  This is the biggest method we've got and probably the
 * hardest and most complicated.
 * However, the basic idea is pretty simple.  We start by looking for the
 * biggest run of color we can find.
 * We then build a rectangular blob out of that color.  From there we need to
 * determine if the rectangular blob
 * is a post, and if so, then which post it is.  If that all goes well we look
 * to see if there is a second post,
 * and potentially a backstop.
 * @param left        the left goal post
 * @param right       the right post
 * @param mid         the backstop
 * @param c           the color we're processing
 * @param c2          the soft color closest to it (e.g. bluegreen for blue)
 */
// Look for posts and goals given the runs we've collected
void ObjectFragments::goalScan(VisualFieldObject* left,
                               VisualFieldObject* right,
                               VisualCrossbar* mid, int c, int c2)
{
    const int IMAGE_EDGE = 3;
	const int NEAR_DISTANCE = 10;
	const int POST_NEAR_DIST = 5;
	const int MAX_Y_VALUE = 10;

    // if we don't have any runs there is nothing to do
    if (numberOfRuns <= 1) return;

    int nextX = 0;
    int nextH = 0;
    distanceCertainty dc = BOTH_UNSURE;
	Blob pole;
    int isItAPost = grabPost(c, c2, IMAGE_WIDTH, -1, pole);

    // make sure we're looking at something big enough to be a post
    if (isItAPost == NOPOST) {
        if (POSTDEBUG) {
            cout << "Not a post" << endl;
        }
        return;
    }

    // ok now we're going to try and figure out which post it is and where
	// the other one might be

    int trueLeft = pole.getLeft();       // leftmost value
    int trueRight = pole.getRight();     // rightmost value
    int trueTop = pole.getTop();         // topmost
    int trueBottom = pole.getBottom();   // bottommost
    int lx = pole.getLeftTopX();         // save these values in case we need
    int ly = pole.getLeftTopY();         // to look for the crossbar
    int rx = pole.getRightTopX();
    // before proclaiming this a post, let's make sure its boundaries
	// are in reasonable places
    int horizonLeft = horizonAt(trueLeft);
    int fakeBottom = max(trueBottom, horizonLeft);
    int spanX = rx - lx + 1;
    int spanY = pole.getLeftBottomY() - ly;
    int pspanY = fakeBottom - trueTop;

    // do some sanity checking - this one makes sure the blob is ok
    if (!locationOk(pole)) {
        if (POSTLOGIC)
            cout << "Bad location on post" << endl;
        return;
    }
	// make sure we have some size to our post
    if (spanY + 1 == 0) return;

	drawBlob(pole, BLACK);
	// make sure that the ratio of height to width is reasonable
    float rat = (float)(spanX) / (float)(spanY);
    if (!postRatiosOk(rat) && spanY < IMAGE_HEIGHT / 2 && spanX < 30) {
        return;
    }

    dc = checkDist(pole);
    // first characterize the size of the possible pole
    int howbig = characterizeSize(pole);
	// now see if we can figure out whether it is a right or left post
    int post = classifyFirstPost(c, c2, left, right,
								 mid, pole);
	// based on those results update the proper data structure
    if (post == LEFT) {
        updateObject(right, pole, _SURE, dc);
    } else if (post == RIGHT) {
        updateObject(left, pole, _SURE, dc);
    } else if (post == BACKSTOP) {
		// should no longer happen with the Naos
    } else if (pole.getRightTopX() - pole.getLeftTopX()
			   > IMAGE_WIDTH - IMAGE_EDGE) {
		// in this case we'll continue on
    } else {
		// if we have a big post and no idea what it is, then stop
		// we'll mark it as uncertain for the localization system
        if (howbig == LARGE)
            updateObject(right, pole, NOT_SURE, dc);
        if (POSTLOGIC)
            cout << "Post not classified" << endl;
        return;
    }

	// at this point we have a post and it is normally classified
	// if we feel pretty good about this, then prepare for the next post

	// first get rid of all the color that corresponds to this post
    bool questions = howbig == SMALL && !rightBlobColor(pole, QUESTIONABLEPOST);
    for (int i = 0; i < numberOfRuns; i++) {
        nextX = runs[i].x;
        if (nextX >= trueLeft && nextX <= trueRight) {
            nextH = 0;
            runs[i].h = 0;
        }
    }
    // now get rid of all the ones on the wrong side of the post
    for (int i = 0; i < numberOfRuns; i++) {
        nextX = runs[i].x;
        if ((nextX < trueLeft && post == LEFT) ||
            (nextX > trueRight && post == RIGHT)) {
            runs[i].h = 0;
        }
        if ( (nextX > trueLeft - NEAR_DISTANCE && post == RIGHT) ||
			 (nextX < trueRight + NEAR_DISTANCE && post == LEFT) ) {
            runs[i].h = 0;
        }
    }

    // find the other post if possible - the process is basically identical to
    // the first post
    point <int> leftP = pole.getLeftTop();
    point <int> rightP = pole.getRightTop();
	point <int> leftB = pole.getLeftBottom();
	point <int> rightB = pole.getRightBottom();
	// ready to grab the potential post
    isItAPost = grabPost(c, c2, trueLeft - POST_NEAR_DIST,
						 trueRight + POST_NEAR_DIST, pole);
    if (isItAPost == NOPOST) {
		// we didn't get one
        // before returning make sure we don't need to screen the previous post
        if (questions) {
            if (post == LEFT) {
                if (right->getIDCertainty() != _SURE)
                    right->init();
            } else {
                if (left->getIDCertainty() != _SURE)
                    left->init();
            }
        }
        return;
    } else {
		// we managed to grab a second post - lets do more sanity checks
        int trueLeft2 = pole.getLeft();
        int trueRight2 = pole.getRight();
        int trueTop2 = pole.getTop();
        int trueBottom2 = pole.getBottom();
        int spanX2 = pole.width();
        int spanY2 = pole.height();
        dc = checkDist(pole);
        rat = (float)spanX2 / (float)spanY2;
        bool ratOk = postRatiosOk(rat) || (!greenCheck(pole) && rat < SQUATRAT);
        bool goodSecondPost = checkSize(pole, c);
        if (SANITY) {
            if (ratOk && goodSecondPost) {
                cout << "Ratio and second are ok" << endl;
            } else {
                cout << "Problem with ratio or second" << endl;
            }
        }
        if (POSTDEBUG) {
            if (ratOk && goodSecondPost) {
                cout << "First two ok on 2d" << endl;
                if (secondPostFarEnough(leftP, rightP,
                                        pole.getLeftTop(), pole.getRightTop(), post)) {
                    cout << "separation is fine" << endl;
                } else {
                    cout << "Not far enough apart" << endl;
                }
            } else {
                cout << "First two not ok on 2d" << endl;
            }
        }
        // if things look ok, then we have ourselves a second post
        int fudge = 0;
		// allow a fudge factor when we're near the image edge
        if (trueLeft < 1 || trueRight > IMAGE_WIDTH - 2) {
            fudge = spanX / 2;
        }
		// sanity checks: post must be reasonably placed with regard
		// to the field, and must not be too close to the other post
		// also its size relative to the other post must be ok
        if (locationOk(pole) && ratOk && goodSecondPost &&
            secondPostFarEnough(leftP, rightP, pole.getLeftTop(),
								pole.getRightTop(), post) &&
			secondPostFarEnough(leftB, rightB, pole.getLeftBottom(),
								pole.getRightBottom(), post) &&
			relativeSizesOk(spanX, pspanY, spanX2, spanY2, trueTop, trueTop2, fudge)) {
            if (post == LEFT) {
                updateObject(left, pole, _SURE, dc);
                // make sure the certainty was set on the other post
                right->setIDCertainty(_SURE);
            } else {
                updateObject(right, pole, _SURE, dc);
                left->setIDCertainty(_SURE);
            }
        } else {
			// we failed at least one sanity check
            if (SANITY) {
                drawBlob(pole, ORANGE);
            }
			// before punting let's check if the post came from a side-goal
			// situation where the relative locations and distances could
			// be quite awkward
            if (locationOk(pole) && ratOk && goodSecondPost &&
				pole.getLeftTopX() > trueRight) {
                // maybe it really is a side-goal situation
                if (abs(trueTop - trueTop2) < MAX_Y_VALUE &&
					qualityPost(pole, c)) {
                    if (post == LEFT) {
                        updateObject(left, pole, _SURE, dc);
                        // make sure the certainty was set on the other post
                        right->setIDCertainty(_SURE);
                    } else {
                        updateObject(right, pole, _SURE, dc);
                        left->setIDCertainty(_SURE);
                    }
                }
            }
        }
    }
}

/* We misidentified the first post.  Now that we've figured that out we need to
 *  switch it to the correct post.  Just transfer the information and reinit the
 *  previously IDd post.
 * @param p1    the correct post
 * @param p2    the wrong one
 */
// TODO: Use a copy constructor...
void ObjectFragments::postSwap(VisualFieldObject * p1, VisualFieldObject * p2){
    p1->setLeftTopX(p2->getLeftTopX());
    p1->setLeftTopY(p2->getLeftTopY());
    p1->setLeftBottomX(p2->getLeftBottomX());
    p1->setLeftBottomY(p2->getLeftBottomY());
    p1->setRightTopX(p2->getRightTopX());
    p1->setRightTopY(p2->getRightTopY());
    p1->setRightBottomX(p2->getRightBottomX());
    p1->setRightBottomY(p2->getRightBottomY());
    p1->setX(p2->getLeftTopX());
    p1->setY(p2->getLeftTopY());
    p1->setWidth(p2->getWidth());
    p1->setHeight(p2->getHeight());
    p1->setCenterX(p2->getCenterX());
    p1->setCenterY(p2->getCenterY());
    p1->setIDCertainty(_SURE);
    p1->setDistanceCertainty(p2->getDistanceCertainty());
    p1->setDistance(1);
    p2->init();
}

/* Sanity check routines for beacons and posts
 */

/*  When we're looking for posts it is helpful if they are surrounded by green.
 * The best place to look is underneath.  So let's do that.
 * @param b    the potential post
 * @return     did we find some green?
 */
bool ObjectFragments::greenCheck(Blob b)
{
    const int ERROR_TOLERANCE = 5;
	const int EXTRA_LINES = 10;
	const int MAX_BAD_PIXELS = 4;

	// if the bottom of the post is at the bottom of the screen default to true
    if (b.getRightBottomY() >= IMAGE_HEIGHT - 1 || b.getLeftBottomY() >=
		IMAGE_HEIGHT-1)
		return true;
	// for huge posts default to true
    if (b.width() > IMAGE_WIDTH / 2) return true;
    int w = b.width();
    int y = 0;
    int x = b.getLeftBottomX();
	stop scan;
	// do the actual scanning under the blob
    for (int i = 0; i < w; i+= 2) {
        y = yProject(x, b.getLeftBottomY(), x + i);
        vertScan(x + i, y, 1, ERROR_TOLERANCE, GREEN, GREEN, scan);
        if (scan.good > 1)
            return true;
    }
    // try one more in case its a white line
    int bad = 0;
    for (int i = 0; i < EXTRA_LINES && bad < MAX_BAD_PIXELS; i++) {
        x = max(0, xProject(x, b.getLeftBottomY(), b.getLeftBottomY() + i));
        int pix = thresh->thresholded[min(IMAGE_HEIGHT - 1,
										  b.getLeftBottomY() + i)][x];
        if (pix == GREEN) return true;
        if (pix != WHITE) bad++;
    }
    return false;
}

/* Checks out how much of the blob is of the right color.
 * If it is enough returns true, if not false.
 * @param tempobj     the blob we're checking (usually a post)
 * @param minpercent  how good it needs to be
 * @return            was it good enough?
 */
bool ObjectFragments::rightBlobColor(Blob tempobj, float minpercent) {
    int x = tempobj.getLeftTopX();
    int y = tempobj.getLeftTopY();
    int spanX = tempobj.width();
    int spanY = tempobj.height();
    if (spanX < 1 || spanY < 1) {
		if (POSTDEBUG) {
			cout << "Invalid size in color check" << endl;
		}
		return false;
	}
    int ny, nx, starty, startx;
    int good = 0, total = 0;
    for (int i = 0; i < spanY; i++) {
        starty = y + i;
        startx = xProject(x, y, starty);
        for (int j = 0; j < spanX; j++) {
            nx = startx + j;
            ny = yProject(startx, starty, nx);
            if (ny > -1 && nx > -1 && ny < IMAGE_HEIGHT && nx < IMAGE_WIDTH) {
                total++;
                if (thresh->thresholded[ny][nx] == color) {
                    good++;
                }
            }
        }
    }
    float percent = (float)good / (float) (total);
	//cout << "Color check " << percent << " " << minpercent << endl;
    if (percent > minpercent) {
        return true;
    }
	if (POSTDEBUG) {
		cout << "Percentages " << percent << " " << minpercent << endl;
	}
    return false;
}

/* Checks if a potential post meets our size requirements.
 * @param b     the post
 * @return      true if its big enough, false otherwise
 */
bool ObjectFragments::postBigEnough(Blob b) {
    if (b.getLeftTopX() == BADVALUE || (b.getRightTopX() - b.getLeftTopX() + 1 <
									MIN_GOAL_WIDTH) ||
        b.getLeftBottomY() - b.getLeftTopY() + 1 < MIN_GOAL_HEIGHT) {
        return false;
    }
    return true;
}

/** Compare the pixEstimated distance with the distance we get from
 *  widths and/or height if possible.  If they are off by too much
 *  then punt this object
 */

bool ObjectFragments::badDistance(Blob b) {
	int x = b.getLeftBottomX();
	int y = b.getLeftBottomY();
	int bottom = b.getBottom();
	estimate e = vision->pose->pixEstimate(x, y, 0.0);
	distanceCertainty dc = checkDist(b);
	float disth = thresh->getGoalPostDistFromHeight(b.height());
	float distw = thresh->getGoalPostDistFromWidth(b.width());
	float diste = e.dist;
	// this is essentially the code from Threshold.h
	float choose = thresh->chooseGoalDistance(dc, disth, distw, diste,
											  bottom);
	if (diste > 0.0f && choose > 2 * diste || choose * 2 < diste) {
		cout << "Trowing out post.  Distance estimate is " << e.dist << endl;
		cout << "Dist from height width " << disth << " " << distw << endl;
	}
	return false;
}

/* Combines several sanity checks into one.  Checks that the bottom of the
 * object is ok and the top too.
 * Also, just makes sure that the object is in fact an object.
 * @param b        the potential post
 * @return         true if it is reasonably located, false otherwise
 */

bool ObjectFragments::locationOk(Blob b)
{
    const int MIN_HORIZON = -50;
	const int TALL_POST = 55;
	const int MIN_WIDTH = 5;
	const int ALLOWABLE_HORIZON_DIFF = 25;

    if (!blobOk(b)) {
        if (POSTLOGIC) {
            cout << "Blob not okay on location check" << endl;
        }
        return false;
    }
    int trueLeft = b.getLeft();       // leftmost value
    int trueRight = b.getRight();
    int trueTop = b.getTop();
    int trueBottom = b.getBottom();
    int horizonLeft = horizonAt(b.getLeftBottomX());
    int horizonRight = horizonAt(b.getRightBottomX());
    int spanX = b.width();
    int spanY = b.height();
    int mh = min(horizonLeft, horizonRight);
    if (!horizonBottomOk(spanX, spanY, mh, trueLeft, trueRight, trueBottom,
                         trueTop)) {
        if (!greenCheck(b) || mh - trueBottom > spanY || spanX < MIN_WIDTH ||
            mh - trueBottom > ALLOWABLE_HORIZON_DIFF) {
            if (spanY > TALL_POST) {
	      //return true;
            } else {
	      if (SANITY) {
                cout << "Screening blob for bottom reasons" << endl;
                printBlob(b);
	      }
	      return false;
	    }
        } else {
        }
    }
	return true;
}

/* Objects need to be at or below the horizon.  We get the basic shape of the
 * object and either the horizon
 * or the projected horizon.
 * @param spanX      how wide the post is
 * @param spanY      how tall it is
 * @param minHeight  the normal minimum value for where the bottom could be
 * @param left       the leftmost x value of the object
 * @param right      the rightmost x value of the object
 * @param bottom     the biggest y value of the object
 * @param top        the smallest y value of the object
 * @return           true if it seems reasonable, false otherwise
 */

bool ObjectFragments::horizonBottomOk(int spanX, int spanY, int minHeight,
									  int left, int right, int bottom, int top)
{
    const int TALL_POST = 100;
	const int BOTTOM_FUDGE_FACTOR = 5;
	const int ALLOWANCE_DUE_TO_WIDTH = 20;
	const int IMAGE_EDGE = 10;
	const int MAX_Y_VALUE = 5;
	const int MIN_WIDTH = 15;
	const int MIN_GOOD = 5;

    // add a width fudge factor in case the object is occluded
	//bigger objects will also be taller
    //int fudge = 20;
    if (spanY > TALL_POST) return true;
    if (color == BLUE) {
      if (bottom + BOTTOM_FUDGE_FACTOR < minHeight) {
	if (SANITY)
	  cout << "Removed risky blue post" << endl;
	return false;
      }
    }
    if (bottom + BOTTOM_FUDGE_FACTOR + min(spanX, ALLOWANCE_DUE_TO_WIDTH) <
		minHeight) {
        if (SANITY) {
            cout << "Bad height" << endl;
        }
        return false;
    }
    // when we're at the edges of the image make the rules a bit more stringent
    if (bottom + BOTTOM_FUDGE_FACTOR < minHeight &&
		(left < IMAGE_EDGE || right > IMAGE_WIDTH - IMAGE_EDGE || top < MAX_Y_VALUE)
		&& (spanY < MIN_WIDTH)) {
        if (SANITY)
            cout << "Bad Edge Information" << endl;
        return false;
    }
    return true;
}


/*  Posts shouldn't show up too close to each other (yes, I realize they can be
 * when you're looking from the side).  Make sure there is some separation.
 *
 * @param l1      left x of one post
 * @param r1      right x of the same post
 * @param l2      left x of the other post
 * @param r2      right x of the other post
 * @return        true when there is enough separation.
 */
bool ObjectFragments::secondPostFarEnough(point <int> left1, point <int> right1,
                                          point <int> left2, point <int> right2,
                                          int post) {
    if (SANITY) {
        cout << "Separations " << (dist(left1.x, left1.y, right2.x, right2.y))
             << " " << (dist(left2.x, left2.y, right1.x, right1.y)) << endl;
    }
    // cout << left1.x << " " << left2.x << " " << right1.x << " "
    //      << right2.x << endl;
    if ((post == RIGHT && right2.x > left1.x) ||
        (post == LEFT && left2.x < right1.x)) {
        if (SANITY) {
            cout << "Second post is on the wrong side!" << endl;
        }
        return false;
    }
    if (dist(left1.x, left1.y, right2.x, right2.y) > MIN_POST_SEPARATION &&
        dist(left2.x, left2.y, right1.x, right1.y) > MIN_POST_SEPARATION) {
        if (dist(left1.x, left1.y, left2.x, left2.y) > MIN_POST_SEPARATION &&
            dist(right2.x, right2.y, right1.x, right1.y) > MIN_POST_SEPARATION){
            return true;
        }
    }
    return false;
}


/* When we process blobs we start them with BADVALUE such that we can easily
 * tell if whatever processing we did worked out.  Here we make that check.
 * @param b    the blob we worked on.
 * @return     true when the processing worked, false otherwise
 */
bool ObjectFragments::blobOk(Blob b) {
    if (b.getLeftTopX() > BADVALUE && b.getLeftBottomX() > BADVALUE)
        return true;
    return false;
}

/*  When we have two candidate posts we don't want one to be huge and the other
 * tiny.  So we need to make
 * sure that the size ratios are within reason.
 * TODO:  THis needs to be rethought and redone for the Naos.  It is very much
 * a product of the Aibos.
 * @param spanX    the width of one post
 * @param spanY    its height
 * @param spanX2   the width of the other post
 * @param spanY2   its height
 * @return         are the ratios reasonable?
 */

bool ObjectFragments::relativeSizesOk(int spanX, int spanY, int spanX2,
									  int spanY2, int t1, int t2, int fudge)
{
    const int SECOND_IS_TALL = 100;
	const int SPAN_MULTIPLIER = 3;
	const float SPAN_PERCENT = 0.25f;
	const float SPAN2_PERCENT = 0.33f;
	const int SECOND_IS_TALL2 = 70;
	const int SMALL_POST = 10;

    if (spanY2 > SECOND_IS_TALL) return true;
    if (spanY2 > SPAN_MULTIPLIER * spanY * SPAN_PERCENT) return true;
    // we need to get the "real" offset
    int f = max(yProject(0, t1, spanY), yProject(IMAGE_WIDTH - 1, t1,
												 IMAGE_WIDTH - spanY));
    if (abs(t1 - t2) > SPAN_MULTIPLIER * min(spanY, spanY2) * SPAN_PERCENT + f)
	{
        if (SANITY) {
            cout << "Bad top offsets" << endl;
        }
        return false;
    }
    if (spanY2 > SECOND_IS_TALL2) return true;
    if (spanX2 > 2
		&& (spanY2 > spanY / 2 || spanY2 > BIGPOST ||
			( (spanY2 > spanY * SPAN2_PERCENT && spanX2 > SMALL_POST) &&
			  (spanX2 <= spanX / 2 || fudge != 0)) ) &&
		(spanX2 > spanX * SPAN_PERCENT))  {
        return true;
    }
    if (t1 < 1 && t2 < 1) return true;
    if (SANITY) {
        cout << "Bad relative sizes" << endl;
        cout << spanX << " "  << spanY << " " << spanX2 << " " << spanY2 << endl;
    }
    return false;
}

/*  Is the ratio of width to height ok for the second post?
 * We use a different criteria here than for
 * the first post because we have lots of other ways to verify if this is a good
 * post.
 * @param ratio     the height/width ratio
 * @return          is it a legal value?
 */
bool ObjectFragments::postRatiosOk(float ratio) {
    return ratio < GOODRAT;
}


/* Misc. routines
 */


/* Calculate the horizontal distance between two objects
 * (the end of one to the start of the other).
 * @param x1    left x of one object
 * @param x2    right x of the object
 * @param x3    left x of the other
 * @param x4    right x of the other
 * @return      the distance between the objects in the x dimension
 */
int ObjectFragments::distance(int x1, int x2, int x3, int x4) {
    if (x2 < x3)
        return x3 - x2;
    if (x1 > x4)
        return x1 - x4;
    return 0;
}

/*
 * The next group of functions are for debugging only.  They are set up so that
 * debugging information will only appear when processing is done off-line.
 */

/*  Print debugging information for a field object.
 * @param objs     the object in question
 */
void ObjectFragments::printObject(VisualFieldObject * objs) {
#if defined OFFLINE
    cout << objs->getLeftTopX() << " " << objs->getLeftTopY() << " "
         << objs->getRightTopX() << " " << objs->getRightTopY() << endl;
    cout << objs->getLeftBottomX() << " " << objs->getLeftBottomY() << " "
         << objs->getRightBottomX() << " " << objs->getRightBottomY() << endl;
    cout << "Height is " << objs->getHeight() << " Width is "
         << objs->getWidth() << endl;
    if (objs->getIDCertainty() == _SURE)
        cout << "Very sure" << endl;
    else
        cout << "Not sure" << endl;
    distanceCertainty dc = objs->getDistanceCertainty();
    switch (dc) {
    case BOTH_SURE:
        cout << "Distance should be good" << endl;
        break;
    case HEIGHT_UNSURE:
        cout << "Heights are not to be trusted" << endl;
        break;
    case WIDTH_UNSURE:
        cout << "Widths are not to be trusted" << endl;
        break;
    case BOTH_UNSURE:
        cout << "Neither height nor width should be trusted" << endl;
        break;
    }
#endif
}

/* Print debugging information for any field object currently found.
 */
void ObjectFragments::printObjs() {
#if defined OFFLINE
    if (PRINTOBJS) {
        if (vision->bglp->getWidth() >  0) {
            cout << "Vision found left blue post " << endl;
            printObject(vision->bglp);
        }
        if (vision->bgrp->getWidth() >  0) {
            cout << "Vision found right blue post " << endl;
            printObject(vision->bgrp);
        }
        if (vision->yglp->getWidth() >  0) {
            cout << "Vision found left yellow post " << endl;
            printObject(vision->yglp);
        }
        if (vision->ygrp->getWidth() >  0) {
            cout << "Vision found right yellow post " << endl;
            printObject(vision->ygrp);
        }
        if (vision->bgCrossbar->getWidth() >  0) {
            cout << "Vision found blue backstop " << endl;
            //printObject(vision->bgCrossbar);
        }
        if (vision->ygCrossbar->getWidth() >  0) {
            cout << "Vision found yellow backstop " << endl;
            //printObject(vision->ygCrossbar);
        }
        cout << "Done with frame" << endl;
    }
#endif
}

/* Print debugging information for a blob.
 * @param b    the blob
 */
void ObjectFragments::printBlob(Blob b) {
#if defined OFFLINE
    cout << "Outputting blob" << endl;
    cout << b.getLeftTopX() << " " << b.getLeftTopY() << " " << b.getRightTopX() << " "
         << b.getRightTopY() << endl;
    cout << b.getLeftBottomX() << " " << b.getLeftBottomY() << " " << b.getRightBottomX()
         << " " << b.getRightBottomY() << endl;
#endif
}

/* Debugging method used to show where things were processed on the image.
 * Paints a verticle stripe corresponding to a "run" of color.
 *
 * @param x     x coord
 * @param y     y coord
 * @param h     height
 * @param c     the color to paint
 */
void ObjectFragments::paintRun(int x, int y, int h, int c){
    vision->drawLine(x,y+1,x,y+h+1,c);
}

/*  More or less the same as the previous method, but with different parameters.
 * @param run     a run of color
 * @param c       the color to paint
 */
void ObjectFragments::drawRun(const run& run, int c) {
    vision->drawLine(run.x,run.y+1,run.x,run.y+run.h+1,c);
}

/*  Draws a "+" on the screen at the specified location with the specified color.
 * @param x     x coord
 * @param y     y coord
 * @param c     the color to paint
 */
void ObjectFragments::drawPoint(int x, int y, int c) {
#ifdef OFFLINE
    thresh->drawPoint(x, y, c);
#endif
}

/*  Draws the outline of a rectangle in the specified color.
 * @param b    the rectangle
 * @param c    the color to paint
 */
void ObjectFragments::drawRect(int x, int y, int w, int h, int c) {
#ifdef OFFLINE
    thresh->drawRect(x, y, w, h, c);
#endif
}

/*  Draws the outline of a blob in the specified color.
 * @param b    the blob
 * @param c    the color to paint
 */
void ObjectFragments::drawBlob(Blob b, int c) {
#ifdef OFFLINE
    thresh->drawLine(b.getLeftTopX(), b.getLeftTopY(),
                     b.getRightTopX(), b.getRightTopY(),
                     c);
    thresh->drawLine(b.getLeftTopX(), b.getLeftTopY(),
                     b.getLeftBottomX(), b.getLeftBottomY(),
                     c);
    thresh->drawLine(b.getLeftBottomX(), b.getLeftBottomY(),
                     b.getRightBottomX(), b.getRightBottomY(),
                     c);
    thresh->drawLine(b.getRightTopX(), b.getRightTopY(),
                     b.getRightBottomX(), b.getRightBottomY(),
                     c);
#endif
}

/* Draws a line on the screen of the specified color.
 * @param x    x value of point 1
 * @param y    y value of point 1
 * @param x1   x value of point 2
 * @param y1   y value of point 2
 * @param c    the color to paint the line.
 */
void ObjectFragments::drawLine(int x, int y, int x1, int y1, int c) {
#ifdef OFFLINE
    thresh->drawLine(x, y, x1, y1, c);
#endif
}
