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
static const float NORMALPOST = 0.6f;
static const float QUESTIONABLEPOST = 0.85f;
static const int DIST_POINT_FUDGE = 5;

#ifdef OFFLINE
static const bool PRINTOBJS = false;
static const bool POSTDEBUG = false;
static const bool POSTLOGIC = false;
static const bool TOPFIND = false;
static const bool CORNERDEBUG = false;
static const bool BACKDEBUG = false;
static const bool SANITY = false;
static const bool CORRECT = false;
static const bool OPENFIELD = false;
#else
static const bool PRINTOBJS = false;
static const bool POSTDEBUG = false;
static const bool POSTLOGIC = false;
static const bool TOPFIND = false;
static const bool CORNERDEBUG = false;
static const bool BACKDEBUG = false;
static const bool SANITY = false;
static const bool CORRECT = false;
static const bool OPENFIELD = false;
#endif

//previous constants inserted from .h class


ObjectFragments::ObjectFragments(Vision* vis, Threshold* thr, int _color)
    : vision(vis), thresh(thr), color(_color), runsize(1)
{
    init(0.0);
    allocateColorRuns();
#ifdef OFFLINE
    PRINTOBJS = false;
    POSTDEBUG = false;
    POSTLOGIC = false;
    TOPFIND = false;
    CORNERDEBUG = false;
    BACKDEBUG = false;
    SANITY = false;
    CORRECT = false;
    OPENFIELD = false;
#endif
}


/* Initialize the data structure.
 * @param s     the slope corresponding to the robot's head tilt
 */
void ObjectFragments::init(float s)
{
    slope = s;
    biggestRun = 0;
    maxHeight = IMAGE_HEIGHT;
    maxOfBiggestRun = 0L;
    numberOfRuns = 0;
    indexOfBiggestRun = 0;
    for (int i = 0; i < IMAGE_WIDTH; i++)
        shoot[i] = true;
}


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
    // depending on the color we have more or fewer runs available
    switch (color) {
    case YELLOW:
    case BLUE:
        run_num = IMAGE_WIDTH * RUNS_PER_SCANLINE;
        runsize = IMAGE_WIDTH * RUNS_PER_LINE;
        break;
    }
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
    // depending on the color we have more or fewer runs available
    switch (color) {
    case YELLOW:
    case BLUE:
        run_num = IMAGE_WIDTH * RUNS_PER_SCANLINE;
        runsize = IMAGE_WIDTH * RUNS_PER_LINE;
        break;
    }
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

        if (h > biggestRun) { // tracking largest run
            biggestRun = h;
            maxOfBiggestRun = y;
            indexOfBiggestRun = numberOfRuns * RUN_VALUES;
        }
        if (y < maxHeight) { // we're counting backwards
            maxHeight = y;
        }
        numberOfRuns++;
    }else{
        if(color == ORANGE) {
            print("WARNING!!!: INSUFFICIENT MEMORY ALLOCATED ORANGE RUNS");
        }
        //cout << "Too many runs " << color << endl;
    }
}


/* The next group of methods has to do with scanning along axis parallel
 * dimensions in order to create objects without blobbing.
 */

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
    // go until we hit enough bad pixels
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
    // go until we hit enough bad pixels
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
 * them to find the true edge.
 * @param top         the topmost estimated point
 * @param bottom      the bottommost estimated point
 * @param c           the primary color of the structure
 * @param c2          the secondary color of the structure
 * @param left        whether the open space is to the left or the right
 */

int ObjectFragments::findTrueLineVertical(point <int> top, point <int> bottom,
                                          int c, int c2, bool left)
{
    const int POST_DIVISOR = 3;

    int spanY = bottom.y - top.y;
    int count = 0;
    int good = spanY;
    int j = 0;
    int dir = 1;
    if (left)
        dir = -1;
    // go until you hit enough bad pixels or some strong reason to stop
    for (j = 1; count < spanY / POST_DIVISOR && top.x + dir * j >= 0
             && top.x + dir * j < IMAGE_WIDTH && good > spanY / 2 ; j++) {
        count = 0;
        good = 0;
        for (int i = top.y; count < spanY / POST_DIVISOR && i <= bottom.y; i++)
		{
            int _spot = top.x + dir * j;
            /*if (checkEdge(spot, i, _spot - dir, i)) {
              count++;
              }*/
            int curcol = thresh->thresholded[i][_spot];
            if (curcol == c || curcol == c2) {
                good++;
            }
        }
    }
    return top.x + dir * j - dir;
}

/*
 * Given two points that should define an edge of a structure, try and improve
 * them to find the true edge. Exactly like the previous function except that it
 * allows for a slope to the camera angle.
 *
 * @param top         the topmost estimated point
 * @param bottom      the bottommost estimated point
 * @param c           the primary color of the structure
 * @param c2          the secondary color of the structure
 * @param left        whether the open space is to the left or the right
 */

void ObjectFragments::findTrueLineVerticalSloped(point <int>& top,
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
    int minGood = max(1, (spanY / 2) / increment);
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
        //cout << "Actual y is " << actualY << endl;
        if (actualY < 1) atTop = true;
        //cout << "Actual y is " << actualY << endl;
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
                //if (checkEdge(theSpot, i, theSpot - dir, i)) {
                //count++;
                //}
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
                        // cout << "Init run " << initRun << " " << top.y << " "
                        //      << minRun << endl;
                    }
                    if (curcol != ORANGE && curcol != WHITE)
                        run++;
                    //if (run == 5)
                    //drawPoint(theSpot, i, BLACK);
                }
            }
        }
        if (good + fake < minGood || good < 1) {
            badLines++;
        }
        // cout << good << " " << minGood << " " << count << " " << initRun << " "
        //      << i << endl;
        //if (!left)
        //drawPoint(theSpot, i, BLACK);
    }

    // ok so we did the basic thing.  The problem is that sometimes we ended
	// early because we hit a screen edge and weren't really done.  Try and 
	// check for those situations.
    int temp = top.x;
    //drawPoint(top.x, top.y, BLACK);
    top.x = top.x + dir * (j - badLines) - dir;
    top.y = yProject(temp, top.y, top.x);
    //drawPoint(top.x, top.y, RED);
    bottom.y = top.y + spanY;
    bottom.x = xProject(top.x, top.y, top.y + spanY);
    //cout << "Checking " << top.x << " " << top.y << endl;
    if (top.x < 2 || top.x > IMAGE_WIDTH - IMAGE_DIFFERENCE) {
        //cout << "In upward scan" << endl;
        for (j = 1; count < minCount && bottom.x + dir * j >= 0
                 && bottom.x + dir * j < IMAGE_WIDTH
                 && good > minGood && run < minRun; j+=increment) {
            //count = 0;
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
                        //if (run == 5)
                        //drawPoint(theSpot, i, BLACK);
                    }
                }
            }
        }
        temp = bottom.x;
        //drawPoint(top.x, top.y, BLACK);
        bottom.x = bottom.x + dir * (j - increment) - dir;
        bottom.y = yProject(temp, bottom.y, bottom.x);
        //drawPoint(top.x, top.y, RED);
        top.y = bottom.y - spanY;
        top.x = xProject(bottom.x, bottom.y, bottom.y - spanY);
    }
}

/*
 * Given two points that should define an edge of a structure, try and improve
 * them to find the true edge.
 * @param left        the leftmost estimated point
 * @param right       the rightmost estimated point
 * @param c           the primary color of the structure
 * @param c2          the secondary color of the structure
 * @param up          whether the open space is up or down
 */

void ObjectFragments::findTrueLineHorizontalSloped(point <int>& left,
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

    // loop until we can't expand anymore
    for (j = 1; count < minCount && left.y + dir * j >= 0
             && left.y + dir * j < IMAGE_HEIGHT && badLines < 2
             && greens < max(minRun, maxgreen); j++) {
        //count = 0;
        good = 0;
        run = 0;
        greens = 0;
        fakegood = 0;
        int actualX = xProject(left.x, left.y, left.y + dir * j);
		// count up the good and bad pixels in this scanline
        for (int i = actualX; count < minCount && i <= actualX + spanX
                 && greens < maxgreen; i++) {
            theSpot = yProject(actualX, left.y + dir * j, i);
            //drawPoint(i, theSpot, RED);
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
                        //if (greens > 1)
                        //cout << "Greens " << greens << " " << theSpot << endl;
                    }
                }
            }
        }
        good += fakegood / 2;
        if (good < minGood)
            badLines++;
    }
    // cout << good << " " << minGood << " " << run << " " << minRun << endl;
    // if we had to stop because we hit the left edge, then see if we can go
    // farther by using the bottom
    int temp = left.y;
    left.y =  left.y + dir * (j - badLines) - dir;
    left.x = xProject(left.x, temp, left.y);
    right.x = left.x + spanX;
    right.y = left.y + spanY;

    if( right.x > IMAGE_WIDTH) {
        //cout << "right.x > IMAGE_WIDTH, value is: " << right.x << endl;
        right.x = IMAGE_WIDTH;
    }

    if( left.x < 0) {
        //cout << "left.x < 0, value is: " << left.x << endl;
        left.x = 0;
    }


    if (!up && thresh->getVisionHorizon() > left.y) {
        // for the heck of it let's scan down
        int found = left.y;
        for (int d = left.y; d < thresh->getVisionHorizon(); d+=1) {
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
            findTrueLineHorizontalSloped(left, right, c, c2, up);
        }
    }
}

/*
 * Given two points that should define an edge of a structure, try and improve
 * them to find the true edge. This is just like the previous function except
 * that it assumes a perfectly level plane.  Which of course makes it a
 * lot easier.  C'est la vie.
 *
 * @param left        the leftmost estimated point
 * @param right       the rightmost estimated point
 * @param c           the primary color of the structure
 * @param c2          the secondary color of the structure
 * @param up          whether the open space is up or down
 */

int ObjectFragments::findTrueLineHorizontal(point <int> left, point <int> right,
                                            int c, int c2, bool up)
{
    const int WIDTH_DIVISOR = 3;

    int spanX = right.x - left.x + 1;
    int count = 0;
    int good = spanX;
    int j = 0;
    int dir = 1;
    if (up)
        dir = -1;
    for (j = 1; count < spanX / WIDTH_DIVISOR && left.y + dir * j >= 0
             && left.y + dir * j < IMAGE_HEIGHT && good > spanX / 2; j++) {
        count = 0;
        good = 0;
        for (int i = left.x; count < spanX / WIDTH_DIVISOR && i <= right.x; i++)
		{
            int theSpot = left.y + dir * j;
            if (checkEdge(i, theSpot, i, theSpot - dir)) {
                count++;
            }
            int curcol = thresh->thresholded[theSpot][i];
            if (curcol == c || curcol == c2) {
                good++;
            }
        }
    }
    return left.y + dir * j - dir;
}

/*  In theory our pose information tells us the slant of an object.  In practice
	it doesn't always get it for a vareity of reasons.  This is an attempt to
	correct for the errors in the process.  At this point
    it is basically a rough draft of a good methodology.
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

/*  Routine to find a general square goal.
 * We start with a point.  We scan up from the point and down from the point
 * looking for a strip of the right color.  That serves as our starting point.
 * Then we try expanding the sides outward.  The we try expanding the top and
 * bottom in a similar fashion.
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

    // so we can check for failure
	int count = 0;
	bool looping = false;
    obj.setLeftTopX(BADVALUE); obj.setLeftTopY(BADVALUE);
	stop scan;
	do {
		// first we try going up
		vertScan(x, y, -1,  ERROR_TOLERANCE, c, c2, scan);
		int h = scan.good;
		// at this point we have a very rough idea of how tall the square is
		int top = scan.y;
		int topx = scan.x;
		// now go down
		vertScan(x, y, 1,  ERROR_TOLERANCE, c, c2, scan);
		h += scan.good;
		if (h < 2) return;
		int bottom = scan.y;
		int bottomx = scan.x;
		//drawPoint(topx, top, RED);
		//drawPoint(bottomx, bottom, RED);
		obj.setLeftTopX(topx);
		obj.setLeftTopY(top);
		obj.setRightTopX(topx);
		obj.setRightTopY(top);
		obj.setLeftBottomX(bottomx);
		obj.setLeftBottomY(bottom);
		obj.setRightBottomX(bottomx);
		obj.setRightBottomY(bottom);
		point <int> temp1 = obj.getLeftTop();
		point <int> temp2 = obj.getLeftBottom();
		//int spanY = obj.getLeftBottomY() - obj.getLeftTopY();
		findTrueLineVerticalSloped(temp1, temp2, c, c2, true);
		obj.setLeftTop(temp1);
		obj.setLeftBottom(temp2);
		temp1 = obj.getRightTop();
		temp2 = obj.getRightBottom();
		findTrueLineVerticalSloped(temp1, temp2, c, c2, false);
		obj.setRightTop(temp1);
		obj.setRightBottom(temp2);
		temp1 = obj.getLeftTop();
		temp2 = obj.getRightTop();
		findTrueLineHorizontalSloped(temp1, temp2, c, c2, true);
		obj.setLeftTop(temp1);
		obj.setRightTop(temp2);
		temp1 = obj.getLeftBottom();
		temp2 = obj.getRightBottom();
		findTrueLineHorizontalSloped(temp1, temp2, c, c2, false);
		obj.setLeftBottom(temp1);
		obj.setRightBottom(temp2);
		float newSlope = correct(obj, c, c2);
		if (newSlope != 0.0) {
			if (CORRECT)
				cout << "Old slope was " << slope << " " << newSlope << endl;
			slope = newSlope;
			looping = true;
		}
		count++;
	} while (count < 2 && looping);
    //drawBlob(obj, ORANGE);
}

/*  Our "main" methods.  Entry points into just about everything else.
 */


/* This is the entry  point from object recognition in Threshold.cc  For now it
 * is only called on yellow, blue and orange.
 * In each case we search for objects that have the corresponding colors.
 * @param  c            color we are processing
 * @return              always 0
 */

void ObjectFragments::createObject(int c) {
    // these are in the relative order that they should be called
    switch (color) {
    case BLUE:
        // either we should see a marker or a goal
        blue(c);
        break;
    case YELLOW:
        // either we should see a marker or a goal
        yellow(c);
        break;
    }
}


int ObjectFragments::horizonAt(int x) {
    return yProject(0, thresh->getVisionHorizon(), x);
}

/* Process yellow:
 * First we scan for beacons, based on the firstBlueYellow in the RLE as a
 * starting point looking for posts.
 * @param bigGreen      green horizon value at x == 0
 */
void ObjectFragments::yellow(int bigGreen) {
    goalScan(vision->yglp, vision->ygrp, vision->ygCrossbar, YELLOW,
             ORANGEYELLOW, false, bigGreen);
}

/* Process blue:
 * Same as for yellow except we theoretically don't need to worry about finding
 * beacons.
 * @param bigGreen       field horizon at x == 0
 */

void ObjectFragments::blue(int bigGreen) {

    goalScan(vision->bglp, vision->bgrp, vision->bgCrossbar, BLUE, BLUEGREEN,
             false, bigGreen);
    // Now that we're done, print out information on all of the objects that
    // we've found.
    printObjs();

}

/* A collection of miscelaneous methods used in processing goals.
 */


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
int ObjectFragments::getBigRun(int left, int right, int hor) {
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
            // hack for pictures with tape measure
            // && nextY < hor) {
            maxRun = nextH;
            index = i;
        }
    }
    return index;
}


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
    //cout << "Got an object" << endl;
    // before we do this let's make sure that the object is really our color
  const float BLUEPOST = 0.75f;
  float perc = NORMALPOST;
  if (_certainty != _SURE && two.height() < 40 && color == BLUE) {
    //cout << "uppint the anty on blue" << endl;
    perc = BLUEPOST;
  }
    if (rightBlobColor(two, perc)) {
        one->updateObject(&two, _certainty, _distCertainty);
        return true;
    } else {
      //cout << "Screening object for low percentage of real color" << endl;
        return false;
    }
}

/* Here we are trying to figure out how confident we are about our values with
 * regard to how they might be used for distance calculations.  Basically if
 * an object is too near one of the screen edges, or if we have some evidence
 * that it is occluded then we note uncertainty in that dimension.
 *
 * @param left     the leftmost point of the object
 * @param right    the rightmost point of the object
 * @param top      the topmost point of the object
 * @param bottom   the bottommost point of the object
 * @return         a constant indicating where the uncertainties (if any) lie
 */
distanceCertainty ObjectFragments::checkDist(int left, int right, int top,
                                             int bottom, Blob pole)
{
    const int ERROR_TOLERANCE = 6;

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
int ObjectFragments::crossCheck(Blob b)
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

int ObjectFragments::checkIntersection(Blob post) {

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
    @param post    the post we have id'd
    @return        the id of the post (or lack of id)
 */

int ObjectFragments::checkCorners(Blob post)
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

/*  Sometimes we process posts differently depending on how big they are.
 * This just characterizes a post's size such that we can make that
 * determination.
 *
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

/* Shooting stuff */

/* Determines shooting information. Basically scans down from backstop and looks
 * for occlusions.
 * Sets the information found within the backstop data structure.
 * @param one the backstop
 */

void ObjectFragments::setShot(VisualCrossbar* one)
{
    const int INTERSECTIONS = 3;   // number of line intersections
    const int CROSSINGS = 3;       // max number of intersections allowed
    const int MINIMUM_PIXELS = 5;  // how many pixels constitute a run
    const int LINE_WIDTH = 10;     // nearness to intersection and still on line
    const int GREY_MAX = 15;       // how many undefined pixels are ok
    const int MAX_RUN_SIZE = 10;   // how many pixels constitute a block
    const int WIDTH_DIVISOR = 5;   // gives a percent of the width

    int pix, bad, white, grey, run, greyrun;
    int ySpan = IMAGE_HEIGHT - one->getLeftBottomY();
    bool colorSeen = false;
    int lx = one->getLeftTopX(), ly = one->getLeftTopY(),
        rx = one->getRightTopX(), ry = one->getRightTopY();
    int bx = one->getLeftBottomX(), brx = one->getRightBottomX();
    int intersections[INTERSECTIONS];
    int crossings = 0;
    bool lineFound = false;
    // now let's see if our backstop is "shootable" and where
    for (int i = max(min(lx, bx), 0); i < min(max(rx, brx), IMAGE_WIDTH - 1);
         i++) {
        bad = 0; white = 0; grey = 0; run = 0; greyrun = 0; colorSeen = false;
        // first - determine if any lines intersection this plumbline and where
        point <int> plumbLineTop, plumbLineBottom, line1start, line1end;
        plumbLineTop.x = i; plumbLineTop.y = ly;
        plumbLineBottom.x = i; plumbLineBottom.y = IMAGE_HEIGHT;
        const vector <VisualLine>* lines = vision->fieldLines->getLines();
        crossings = 0;
        for (vector <VisualLine>::const_iterator k = lines->begin();
             k != lines->end(); k++) {
            pair<int, int> foo = Utility::
                plumbIntersection(plumbLineTop, plumbLineBottom,
                                  k->start, k->end);
            if (foo.first != NO_INTERSECTION && foo.second != NO_INTERSECTION) {
                intersections[crossings] = foo.second;
                crossings++;
                if (crossings == CROSSINGS) {
                    shoot[i] = false;
                    break;
                }
            }
        }

        int strip = 0;
        for (int j = min(ly, ry); j < IMAGE_HEIGHT && shoot[i]; j++) {
            pix = thresh->thresholded[j][i];
            if (pix == color) {
                strip++;
                if (strip > MINIMUM_PIXELS)
                    colorSeen = true;
            }
            if (colorSeen && (pix == RED || pix == NAVY)) {
                bad++;
                run++;
                lineFound = false;
            } else if (colorSeen && pix == WHITE) {
                if (!lineFound) {
                    for (int k = 0; k < crossings; k++) {
                        //cout << "Crassing at " << intersections[k] << endl;
                        if (intersections[k] - j < LINE_WIDTH &&
                            intersections[k] - j > 0) {
                            lineFound = true;
                        }
                    }
                }
                if (lineFound) {
                    //cout << "Intersection at " << j << endl;
                    run = 0;
                    greyrun = 0;
                } else {
                    //cout << "No Intersection at " << j << endl;
                    white++;
                    run++;
                }
            } else if ( colorSeen && (pix == GREY || pix == BLACK) ) {
                grey++;
                greyrun++;
            } else if (pix == GREEN || pix == BLUEGREEN) {
                run = 0;
                greyrun = 0;
                lineFound = false;
            }
            if (greyrun > GREY_MAX) {
                //shoot[i] = false;
                if (BACKDEBUG) {
                    //drawPoint(i, j, RED);
                }
            }
            if (run > MAX_RUN_SIZE && (pix == NAVY || pix == RED)) {
                shoot[i] = false;
                if (BACKDEBUG)
                    drawPoint(i, j, RED);
            }
            if (run > MAX_RUN_SIZE) {
                shoot[i] = false;
                if (BACKDEBUG) {
                    drawPoint(i, j, RED);
                }
            }
        }
        if (bad > ySpan / WIDTH_DIVISOR) {
            shoot[i] = false;
        }
    }
    // now find the range of shooting
    int r1 = IMAGE_WIDTH / 2;
    int r2 = IMAGE_WIDTH / 2;
    for ( ;r1 < brx && r1 >= bx && shoot[r1]; r1--) {}
    for ( ;r2 > bx && r2 <= rx && shoot[r2]; r2++) {}
    if (r2 - r1 < MINSHOTWIDTH || abs(r1 - IMAGE_WIDTH / 2) < MINSHOTWIDTH / 2||
        abs(r2 - IMAGE_WIDTH / 2) < MINSHOTWIDTH) {
        one->setShoot(false);
        one->setBackLeft(-1);
        one->setBackRight(-1);
    } else {
        one->setShoot(true);
        if (BACKDEBUG) {
            drawLine(r1, ly, r1, IMAGE_HEIGHT - 1, RED);
            drawLine(r2, ly, r2, IMAGE_HEIGHT - 1, RED);
        }
    }
    one->setBackLeft(r1);
    one->setBackRight(r2);

    if (BACKDEBUG) {
        drawPoint(r1, ly, RED);
        drawPoint(r2, ly, BLACK);
    }
    // now figure out the optimal direction
    int left = 0, right = 0;
    for (int i = lx; i < IMAGE_WIDTH / 2; i++) {
        if (shoot[i]) left++;
    }
    for (int i = IMAGE_WIDTH / 2; i < rx; i++) {
        if (shoot[i]) right++;
    }
    one->setLeftOpening(left);
    one->setRightOpening(right);

    if (left > right)
        one->setBackDir(MOVELEFT);
    else if (right > left)
        one->setBackDir(MOVERIGHT);
    else if (right == 0)
        one->setBackDir(ALLBLOCKED);
    else
        one->setBackDir(EITHERWAY);
    if (BACKDEBUG) {
        cout << "Crossbar info: Left Col: " << r1 << " Right Col: " << r2
             << " Dir: " << one->getBackDir();
        if (one->shotAvailable())
            cout << " Take the shot!" << endl;
        else
            cout << " Don't shoot!" << endl;
    }
}


void ObjectFragments::bestShot(VisualFieldObject* left,
                               VisualFieldObject* right,
                               VisualCrossbar* middle)
{
    const int bigSize = 10;
	const int topBuff = 20;
	const int newHeight = 20;

    // start by setting boundaries
    int leftb = 0, rightb = IMAGE_WIDTH - 1, bottom = 0;
    int rl = 0, rr = 0;
    bool screen[IMAGE_WIDTH];
    for (int i = 0; i < IMAGE_WIDTH; i++) {
        screen[i] = false;
    }
    if (left->getDistance() != 0) {
        if (left->getIDCertainty() != _SURE) return;
        leftb = left->getRightBottomX();
        bottom = left->getLeftBottomY();
        rightb = min(rightb, left->getRightBottomX() + (int)left->getHeight());
    }
    if (right->getDistance() != 0) {
        rightb = right->getLeftBottomX();
        bottom = right->getRightBottomY();
        if (leftb == 0) {
            leftb = max(0, rightb - (int)right->getHeight());
        }
    }
	//cout << "Boundary " << leftb << " " << rightb << endl;
	middle->setLeftTopX(leftb);
	middle->setLeftBottomX(leftb);
	middle->setRightTopX(rightb);
	middle->setRightBottomX(rightb);
	middle->setLeftBottomY(bottom);
	middle->setRightBottomY(bottom);
	middle->setLeftTopY(bottom - 10);
	middle->setRightTopY(bottom - 10);
	middle->setWidth(rightb - leftb + 1);
	middle->setHeight(10);
	setShot(middle);
	/*
    if (vision->red1->getDistance() > 0) {
        rl = vision->red1->getLeftBottomX();
        rr = vision->red1->getRightBottomX();
        if (rr >= leftb && rl <= rightb) {
            for (int i = rl; i <= rr; i++) {
                screen[i] = true;
            }
        }
    }
    if (vision->red2->getDistance() > 0) {
        rl = vision->red2->getLeftBottomX();
        rr = vision->red2->getRightBottomX();
        if (rr >= leftb && rl <= rightb) {
            for (int i = rl; i <= rr; i++) {
                screen[i] = true;
            }
        }
    }
    if (vision->navy1->getDistance() > 0) {
        rl = vision->navy1->getLeftBottomX();
        rr = vision->navy1->getRightBottomX();
        if (rr >= leftb && rl <= rightb) {
            for (int i = rl; i <= rr; i++) {
                screen[i] = true;
            }
        }
    }
    if (vision->navy2->getDistance() > 0) {
        rl = vision->navy2->getLeftBottomX();
        rr = vision->navy2->getRightBottomX();
        if (rr >= leftb && rl <= rightb) {
            for (int i = rl; i <= rr; i++) {
                screen[i] = true;
            }
        }
    }
    // now find the biggest swatch
    int run = 0, index = -1, indexr = -1, big = 0;
    for (int i = leftb; i <= rightb; i++) {
        if (!screen[i]) {
            run++;
        } else {
            if (run > big) {
                indexr = i - 1;
                index = indexr - run;
                big = run;
            }
            run = 0;
        }
    }
    if (run > big) {
        indexr = rightb;
        index = indexr - run;
        big = run;
    }
    if (big > bigSize) {
        int bot = max(horizonAt(index), bottom);
        middle->setLeftTopX(index);
        middle->setLeftTopY(bot - topBuff);
        middle->setLeftBottomX(index);
        middle->setLeftBottomY(bot);
        middle->setRightTopX(indexr);
        middle->setRightTopY(bot - topBuff);
        middle->setRightBottomX(indexr);
        middle->setRightBottomY(bot);
        middle->setX(index);
        middle->setY(bot);
        middle->setWidth( static_cast<float>(big) );
        middle->setHeight(newHeight);
        middle->setCenterX(middle->getLeftTopX() + ROUND2(middle->getWidth() /
                                                          2));
        middle->setCenterY(middle->getRightTopY() + ROUND2(middle->getHeight() /
                                                           2));
        middle->setDistance(1);

		}*/
}

/*
 * Determines what is the most open part of the field.  Basically scans up and
 * looks for occlusions.
 */
void ObjectFragments::openDirection(int horizon, NaoPose *pose)
{
    const int INTERSECTIONS = 5;
    const int SCAN_DIVISION = 10;
	const int BOUND = 60;
	const int MAX_CROSSINGS = 5;
	const int INTERSECTION_NEARNESS = 10;
	const int GREY_MAX = 15;
	const int Y_BUFFER = 15;
	const int RUN_BUFFER = 10;
	const int MAX_RUN_SIZE = 10;
	const int MAX_BAD_PIXELS = 3;
	const int JUMP_MAX = 11;
	const int BOTTOM_BUFFER= 10;
	const int HORIZON_TOLERANCE = 20;
	const int DOWNMAX = 10;
	const int IMAGE_SECTIONS = 15;
	const int X_AXIS_SECTIONS = 6;
	const int LINE_DIVIDER = 3;

    int pix, bad, white, grey, run, greyrun;
    int intersections[INTERSECTIONS];
    int crossings = 0;
    bool lineFound = false;
    int y;
    int open[IMAGE_WIDTH / SCAN_DIVISION];
    int open2[IMAGE_WIDTH / SCAN_DIVISION];
    //cout << "In open direction " << endl;
    open[0] = horizon;
    open2[0] = horizon;
    int lastd = 0;
    int sixty = IMAGE_HEIGHT - 1;
    for (int i = IMAGE_HEIGHT - 1; i > horizon; i--) {
        estimate d = pose->pixEstimate(IMAGE_WIDTH / 2, i, 0.0);
        //cout << "Distances " << i << " " << d.dist << endl;
        if (d.dist > BOUND && lastd < BOUND) {
            if (OPENFIELD) {
                drawPoint(IMAGE_WIDTH / 2, i, MAROON);
            }
            sixty = i;
        }
        lastd = (int)d.dist;
    }
    const vector <VisualLine>* lines = vision->fieldLines->getLines();
    for (int x = SCAN_DIVISION; x < IMAGE_WIDTH - 1; x += SCAN_DIVISION) {
        bad = 0; white = 0; grey = 0; run = 0; greyrun = 0;
        open[(int)(x / SCAN_DIVISION)] = horizon;
        open2[(int)(x / SCAN_DIVISION)] = horizon;
        // first - determine if any lines intersection this plumbline and where
        point <int> plumbLineTop, plumbLineBottom, line1start, line1end;
        plumbLineTop.x = x; plumbLineTop.y = 0;
        plumbLineBottom.x = x; plumbLineBottom.y = IMAGE_HEIGHT;
        crossings = 0;
        for (vector <VisualLine>::const_iterator k = lines->begin();
             k != lines->end(); k++) {
            pair<int, int> foo = Utility::
                plumbIntersection(plumbLineTop, plumbLineBottom,
                                  k->start, k->end);
            if (foo.first != NO_INTERSECTION && foo.second != NO_INTERSECTION) {
                intersections[crossings] = foo.second;
                crossings++;
                if (crossings == MAX_CROSSINGS) {
                    break;
                }
            }
        }
        int maxH = max(0, horizonAt(x));
        //cout << "Got lines " << maxH << endl;
        for (y = IMAGE_HEIGHT - 1; y > maxH; y--) {
            pix = thresh->thresholded[y][x];
            if ((pix == RED || pix == NAVY)) {
                bad++;
                run++;
                lineFound = false;
            } else if (pix == WHITE) {
                if (!lineFound) {
                    for (int k = 0; k < crossings; k++) {
                        //cout << "Crassing at " << intersections[k] << endl;
                        if (intersections[k] - y < INTERSECTION_NEARNESS &&
                            intersections[k] - y > 0) {
                            lineFound = true;
                        }
                    }
                }
                if (lineFound) {
                    //cout << "Intersection at " << j << endl;
                    run = 0;
                    greyrun = 0;
                } else {
                    //cout << "No Intersection at " << j << endl;
                    white++;
                }
            } else if (pix == GREY || pix == BLACK) {
                grey++;
                greyrun++;
            } else if (pix == GREEN || pix == BLUEGREEN || pix == BLUE ||
                       pix == ORANGE) {
                run = 0;
                greyrun = 0;
                lineFound = false;
            }
            if (greyrun == GREY_MAX) {
                //shoot[i] = false;
                if (open[(int)x / SCAN_DIVISION] == horizon) {
                    open[(int)x / SCAN_DIVISION] = y + Y_BUFFER;
                }
                open2[(int)x / SCAN_DIVISION] = y + Y_BUFFER;
                //drawPoint(x, y, RED);
                //drawPoint(x - 1, y, RED);
                //drawPoint(x + 1, y, RED);
                y = 0;
            }
            if (run == RUN_BUFFER) {
                if (open[(int)x / SCAN_DIVISION] == horizon) {
                    open[(int)x / SCAN_DIVISION] = y + RUN_BUFFER;
                }
                if (bad == RUN_BUFFER) {
                    open2[(int)x / SCAN_DIVISION] = y + RUN_BUFFER;
                    y = 0;
                }
                //drawPoint(x, y, RED);
                //drawPoint(x - 1, y, RED);
                //drawPoint(x + 1, y, RED);
            }
            if (run > MAX_RUN_SIZE && (bad > MAX_BAD_PIXELS || y < sixty)) {
                open2[(int)x / SCAN_DIVISION] = y + run;
                y = 0;
            }
        }
    }
    // OK let's see if we can say anything about how blocked we were
    // left side first
    int index1 = 0, index2, index3, i, index12 = 0, index22, index32;
    int longs = 0, longsize = 0, longIndex = 0, minsize = horizon;
    int jumpdown = -1, lastone = horizon;
    bool vert = false;
    for (i = 0; i < IMAGE_WIDTH / SCAN_DIVISION; i++) {
        if (i - jumpdown < JUMP_MAX && !vert && open[i] >
			IMAGE_HEIGHT - BOTTOM_BUFFER && jumpdown != -1) {
            vert = true;
        }
        if (open[i] > horizon + HORIZON_TOLERANCE &&
			lastone < horizon + HORIZON_TOLERANCE && i != 0) {
            jumpdown = i;
            vert = false;
        }
        if (vert && lastone > horizon + HORIZON_TOLERANCE  && open[i] <
			horizon + HORIZON_TOLERANCE) {
            //cout << "Testing for vertical " << jumpdown << " " << i << endl;
            if (i - jumpdown < DOWNMAX && jumpdown != -1) {
                point<int> midTop(jumpdown * SCAN_DIVISION,
								  IMAGE_HEIGHT - horizon /2);
                point<int> midBottom(i * SCAN_DIVISION,IMAGE_HEIGHT- horizon/2);
                bool intersects = vision->fieldLines->
                    intersectsFieldLines(midTop,midBottom);
                if (intersects) {
                    if (OPENFIELD) {
                        cout << "VERTICAL LINE DETECTED BY OPEN FIELD*********"
                             << endl;
                    }
                    for (int k = jumpdown; k < i; k++) {
                        open[k] = horizon;
                        open2[k] = horizon;
                    }
                }
            }
        }
        lastone = open[i];
        if (open[i] - MAX_RUN_SIZE <= horizon) {
            longs++;
            if (longs > longsize) {
                longIndex = i - longs;
                longsize = longs;
            }
        } else {
            longs = 0;
        }
        if (open[i] > minsize) {
            minsize = open[i];
        }
    }
    for (i = 1; i < IMAGE_WIDTH / IMAGE_SECTIONS; i++) {
        if (open[i] > open[index1]) {
            index1 = i;
        }
        if (open2[i] > open2[index12]) {
            index12 = i;
        }
    }
    index2 = i; index22 = i;
    for (i++ ; i < 2 * IMAGE_WIDTH / IMAGE_SECTIONS; i++) {
        if (open[i] > open[index2]) {
            index2 = i;
        }
        if (open2[i] > open2[index22]) {
            index22 = i;
        }
    }
    index3 = i; index32 = i;
    for (i++ ; i < (IMAGE_WIDTH / SCAN_DIVISION) ; i++) {
        if (open[i] > open[index3]) {
            index3 = i;
        }
        if (open2[i] > open2[index32]) {
            index32 = i;
        }
    }
    // All distance estimates are to the HARD values
    estimate e;
    e = pose->pixEstimate(IMAGE_WIDTH/X_AXIS_SECTIONS, open2[index12], 0.0);
    vision->fieldOpenings[0].soft = open[index1];
    vision->fieldOpenings[0].hard = open2[index12];
    vision->fieldOpenings[0].horizonDiffSoft = open[index1] - horizon;
    vision->fieldOpenings[0].horizonDiffHard = open2[index12] - horizon;
    vision->fieldOpenings[0].dist = e.dist;
    vision->fieldOpenings[0].bearing = e.bearing;
    vision->fieldOpenings[0].elevation = e.elevation;

    e = pose->pixEstimate(IMAGE_WIDTH/2, open2[index22],0.0);
    vision->fieldOpenings[1].soft = open[index2];
    vision->fieldOpenings[1].hard = open2[index22];
    vision->fieldOpenings[1].horizonDiffSoft = open[index2] - horizon;
    vision->fieldOpenings[1].horizonDiffHard = open2[index22] - horizon;
    vision->fieldOpenings[1].dist = e.dist;
    vision->fieldOpenings[1].bearing = e.bearing;
    vision->fieldOpenings[1].elevation = e.elevation;

    e = pose->pixEstimate(SCAN_DIVISION*IMAGE_WIDTH/X_AXIS_SECTIONS,
						  open2[index32],0.0);
    vision->fieldOpenings[2].soft = open[index3];
    vision->fieldOpenings[2].hard = open2[index32];
    vision->fieldOpenings[2].horizonDiffSoft = open[index3] - horizon;
    vision->fieldOpenings[2].horizonDiffHard = open2[index32] - horizon;
    vision->fieldOpenings[2].dist = e.dist;
    vision->fieldOpenings[2].bearing = e.bearing;
    vision->fieldOpenings[2].elevation = e.elevation;


    if (OPENFIELD) {
        cout << "Obstacle 1 Dist:" << vision->fieldOpenings[0].dist << endl
             << "Obstacle 2 Dist:" << vision->fieldOpenings[1].dist << endl
             << "Obstacle 3 Dist:" << vision->fieldOpenings[2].dist << endl;
        // drawLine(0, open[index1], IMAGE_WIDTH / 3, open[index1], PINK);
        // drawLine(IMAGE_WIDTH / 3, open[index2], 2 * IMAGE_WIDTH / 3,
        //          open[index2], PINK);
        // drawLine(2 * IMAGE_WIDTH / 3, open[index3], IMAGE_WIDTH  - 1,
        //          open[index3], PINK);
        // drawLine(0, open[index1] + 1, IMAGE_WIDTH / 3, open[index1] + 1,
        //          MAROON);
        // drawLine(IMAGE_WIDTH / 3, open[index2] + 1, 2 * IMAGE_WIDTH / 3,
        //          open[index2] + 1, PINK);
        // drawLine(2 * IMAGE_WIDTH / 3, open[index3] + 1, IMAGE_WIDTH  - 1,
        //          open[index3] + 1, PINK);
        drawLine(0, open2[index12], IMAGE_WIDTH / LINE_DIVIDER, open2[index12],
				 MAROON);
        drawLine(IMAGE_WIDTH / LINE_DIVIDER, open2[index22], 2 * IMAGE_WIDTH /
				 LINE_DIVIDER,
                 open2[index22], MAROON);
        drawLine(2 * IMAGE_WIDTH / LINE_DIVIDER, open2[index32],
				 IMAGE_WIDTH  - 1,
                 open2[index32], MAROON);
        drawLine(0, open2[index12] - 1, IMAGE_WIDTH / LINE_DIVIDER,
				 open2[index12] - 1,
                 MAROON);
        drawLine(IMAGE_WIDTH / LINE_DIVIDER, open2[index22] - 1,
				 2 * IMAGE_WIDTH / LINE_DIVIDER,
                 open2[index22] - 1, MAROON);
        drawLine(2 * IMAGE_WIDTH / LINE_DIVIDER, open2[index32] - 1,
				 IMAGE_WIDTH  - 1,
                 open2[index32] - 1, MAROON);
        if (open2[index12] != open2[index22]) {
            drawLine(IMAGE_WIDTH / LINE_DIVIDER, open2[index12],
					 IMAGE_WIDTH / LINE_DIVIDER,
                     open2[index22], MAROON);
        }
        if (open2[index32] != open2[index22]) {
            drawLine(2 * IMAGE_WIDTH / LINE_DIVIDER, open2[index32],
					 2 * IMAGE_WIDTH / LINE_DIVIDER,
                     open2[index22], MAROON);
        }
        if (open2[index12] <  open2[index22] &&
            open2[index12] < open2[index32]) {
            for (i = IMAGE_WIDTH / LINE_DIVIDER; open[i / SCAN_DIVISION] <=
					 open2[index12]; i++){
            }
            drawMore(i, open2[index12], PINK);
        }
        else if (open2[index22] <  open2[index12] &&
                 open2[index22] < open2[index32]) {
            for (i = IMAGE_WIDTH / LINE_DIVIDER; open[i / SCAN_DIVISION] <=
					 open2[index22]; i--){
            }
            drawLess(i, open2[index22], PINK);
            for (i = 2 * IMAGE_WIDTH / LINE_DIVIDER; open[i / SCAN_DIVISION] <=
					 open2[index22];
                 i++) {}
            drawMore(i, open2[index22], PINK);
        }
        else if (open2[index32] <  open2[index22] &&
                 open2[index32] < open2[index12]) {
            for (i = 2 * IMAGE_WIDTH / LINE_DIVIDER; open[i / SCAN_DIVISION] <=
					 open2[index32];
                 i--) {}
            drawLess(i, open2[index32], PINK);
        }
        else if (open2[index22] ==  open2[index12] &&
                 open2[index22] < open2[index32]) {
            for (i = 2 * IMAGE_WIDTH / LINE_DIVIDER; open[i / SCAN_DIVISION] <=
					 open2[index22];
                 i++) {}
            drawMore(i, open2[index22], PINK);
        }
        else if (open2[index22] < open2[index12] &&
                 open2[index22] == open2[index32]) {
            for (i = IMAGE_WIDTH / LINE_DIVIDER; open[i / SCAN_DIVISION] <=
					 open2[index22]; i--){
            }
            drawLess(i, open2[index22], PINK);
        } else if (open2[index12] < open2[index22] &&
                   open2[index12] == open2[index32]) {
            // vertical line?
            cout << "Vertical line?" << endl;
        }
        // drawMore(longIndex * SCAN_DIVISION + longsize * SCAN_DIVISION +
		// SCAN_DIVISION, horizon,
        //          PINK);
        cout << "Estimate soft is " << open[index1] << " " << open[index2]
             << " " << open[index3] << endl;
        cout << "Estimate hard is " << open2[index12] << " " << open2[index22]
             << " " << open2[index32] << endl;
    }
}

/* Try and find the biggest post left on the screen.  We start by looking for
 * our longest "run" of the current color.
 * We then call squareGoal to expand that into a post.  Later
 * we will check if it actually meets the criteria for a good post.
 * @param c       current color
 * @param c2      secondary color
 * @param horizon green horizon
 * @param left    leftmost limit to look
 * @param right   rightmost limit to look
 * @param         indication of whether we found a decent candidate
 */

int ObjectFragments::grabPost(int c, int c2, int horizon, int left,
							  int right, Blob & obj) {
    int maxRun = 0, maxY = 0, maxX = 0, index = 0;
    // find the biggest Run
    index = getBigRun(left, right, horizon);
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
    return LEFT; // Just return something other than NOPOST
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

int ObjectFragments::checkOther(int left, int right, int height, int horizon)
{
    const int HORIZON_TOLERANCE = 10;    // our other post should be near horizon
	const int MIN_OTHER_THRESHOLD = 20;  // how big does it have to be?

    int largel = 0;
    int larger = 0;
    //int mind = max(MIN_POST_SEPARATION, height / 2);
	int mind = MIN_POST_SEPARATION + (right - left) / 2;
    for (int i = 0; i < numberOfRuns; i++) {
        int nextX = runs[i].x;
        int nextY = runs[i].y;
        int nextH = runs[i].h;
        //int nextB = nextY + nextH;
        // meanwhile collect some information on which post we're looking at
        if (nextH > 0) {
            if (nextX < left - mind && nextH > MIN_GOAL_HEIGHT &&
                nextY < horizonAt(nextX) &&
                nextY + nextH > horizonAt(nextX) - HORIZON_TOLERANCE) {
                if (nextH > largel)
                    largel = nextH;
                //drawPoint(nextX, nextY, ORANGE);
                //cout << largel << endl;
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


/* Main routine for classifying posts (Aibos).  We have a variety of methods to
 * classify posts in our tool box.  The idea is to start with the best ones and
 * keep trying until one produces an answer.
 *
 * @param horizon        the green horizon (y value)
 * @param c              color of the post
 * @param c2             secondary color
 * @param beaconFound      did we find a beacon in this image?
 * @param left           field object to send out if we find a left post
 * @param right          ditto for right post
 * @param mid            ditto for backstop
 * @return               classification
 */

int ObjectFragments::classifyFirstPost(int horizon, int c,int c2,
                                       bool beaconFound,
                                       VisualFieldObject* left,
                                       VisualFieldObject* right,
                                       VisualCrossbar* mid, Blob pole)
{
    const int MIN_BLOB_SIZE = 10;

    // ok now we're going to try and figure out which post it is and where the
    //other one might be
    int trueLeft = min(pole.getLeftTopX(), pole.getLeftBottomX());  // leftmost value
    int trueRight = max(pole.getRightTopX(), pole.getRightBottomX()); // rightmost value
    int trueTop = min(pole.getLeftTopY(), pole.getRightTopY());       // topmost value
    int trueBottom = max(pole.getLeftBottomY(), pole.getRightBottomY()); // bottommost
    int ly = pole.getLeftTopY();                         // to look for the crossbar
    // These variables are used to figure out whether we are looking at a right
	// or left post before proclaiming this a post, let's make sure its boundaries
	// are in reasonable places
    int horizonLeft = horizonAt(trueLeft);
    int fakeBottom = max(trueBottom, horizonLeft);
    //int spanX = rx - lx + 1;
    int spanY = pole.getLeftBottomY() - ly;
    // do some sanity checking - this one makes sure the blob is ok
    if (!locationOk(pole, horizon)) {
		return NOPOST;
    }
    if (spanY + 1 == 0) return BADVALUE;
    // first characterize the size of the possible pole
    int howbig = characterizeSize(pole);
    int post = RIGHT;
    int stopp = spanY / 2;
    if (howbig == LARGE)
        stopp = spanY;

    // start the long process of figuring out which post we've got - fortunately
    // with the Naos it is easier
    post = checkOther(trueLeft, trueRight, fakeBottom - trueTop, horizon);
    if (post != NOPOST) {
        if (POSTLOGIC)
            cout << "Found from checkOther" << endl;
        return post;
    }

    post = crossCheck(pole);        // look for the crossbar
    if (post != NOPOST) {
        if (POSTLOGIC) {
            cout << "Found crossbar " << post << endl;
        }
        return post;
    }

    post = checkIntersection(pole);
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
 * @param beaconFound   did we find a beacon?
 * @param horizon     the green field horizon
 */
// Look for posts and goals given the runs we've collected
void ObjectFragments::goalScan(VisualFieldObject* left,
                               VisualFieldObject* right,
                               VisualCrossbar* mid, int c, int c2,
                               bool beaconFound, int horizon)
{
    const int IMAGE_EDGE = 3;
	const int NEAR_DISTANCE = 10;
	const int POST_NEAR_DIST = 5;
	const int MAX_Y_VALUE = 10;

	//cout << horizon << " " << slope << endl;
    // if we don't have any runs there is nothing to do
    if (numberOfRuns <= 1) return;

    int nextX = 0;
    //int nextY = 0;
    int nextH = 0;
    distanceCertainty dc = BOTH_UNSURE;

	Blob pole;
    int isItAPost = grabPost(c, c2, horizon, IMAGE_WIDTH, -1, pole);
    //drawBlob(obj, ORANGE);
    // make sure we're looking at something big enough to be a post
    if (isItAPost == NOPOST) {
        if (POSTDEBUG) {
            cout << "Not a post" << endl;
        }
        return;
    }
    //drawBlob(obj, PINK);
    //printBlob(obj);
    // ok now we're going to try and figure out which post it is and where
	// the other one might be
    int trueLeft = min(pole.getLeftTopX(), pole.getLeftBottomX());    // leftmost value
    int trueRight = max(pole.getRightTopX(), pole.getRightBottomX()); // rightmost value
    int trueTop = min(pole.getLeftTopY(), pole.getRightTopY());       // topmost
    int trueBottom = max(pole.getLeftBottomY(), pole.getRightBottomY()); // bottommost
    int lx = pole.getLeftTopX();               // save these values in case we need
    int ly = pole.getLeftTopY();               // to look for the crossbar
    int rx = pole.getRightTopX();
    // before proclaiming this a post, let's make sure its boundaries
	// are in reasonable places
    int horizonLeft = horizonAt(trueLeft);
    //if (slope < 0)
    //horizonLeft = yProject(IMAGE_WIDTH - 1, horizon, trueLeft);
    int fakeBottom = max(trueBottom, horizonLeft);
    int spanX = rx - lx + 1;
    int spanY = pole.getLeftBottomY() - ly;
    int pspanY = fakeBottom - trueTop;
    //cout << "Spans " << spanY << " " << pspanY << endl;
    //int ySpan = pole.getLeftBottomY() - pole.getLeftTopY() + 1;
    // do some sanity checking - this one makes sure the blob is ok
    if (!locationOk(pole, horizon)) {

        if (POSTLOGIC)
            cout << "Bad location on post" << endl;
        return;
    }
    if (spanY + 1 == 0) return;
    float rat = (float)(spanX) / (float)(spanY);
    if (!postRatiosOk(rat) && spanY < IMAGE_HEIGHT / 2 && spanX < 30) {
        return;
    }

    dc = checkDist(trueLeft, trueRight, trueTop, trueBottom, pole);
    // first characterize the size of the possible pole
    int howbig = characterizeSize(pole);
    int post = classifyFirstPost(horizon, c, c2, beaconFound, left, right,
								 mid, pole);
    if (post == LEFT) {
        updateObject(right, pole, _SURE, dc);
    } else if (post == RIGHT) {
        updateObject(left, pole, _SURE, dc);
    } else if (post == BACKSTOP) {
    } else if (pole.getRightTopX() - pole.getLeftTopX() > IMAGE_WIDTH - IMAGE_EDGE) {
    } else {
        if (howbig == LARGE)
            updateObject(right, pole, NOT_SURE, dc);
        if (POSTLOGIC)
            cout << "Post not classified" << endl;
        return;
    }
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
    int trueLeft2 = 0;
    int trueRight2 = 0;
    int trueBottom2 = 0;
    int trueTop2 = 0;
    int second = 0;
    int spanX2 = 0, spanY2 = 0;
    isItAPost = grabPost(c, c2, horizon, trueLeft - POST_NEAR_DIST,
						 trueRight + POST_NEAR_DIST, pole);
    if (isItAPost == NOPOST) {
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
        trueLeft2 = min(pole.getLeftTopX(), pole.getLeftBottomX());
        trueRight2 = max(pole.getRightTopX(), pole.getRightBottomX());
        trueTop2 = min(pole.getLeftTopY(), pole.getRightTopY());
        trueBottom2 = max(pole.getLeftBottomY(), pole.getRightBottomY());
        spanX2 = pole.getRightTopX() - pole.getLeftTopX() + 1;
        spanY2 = pole.getLeftBottomY() - pole.getLeftTopY() + 1;
        dc = checkDist(trueLeft2, trueRight2, trueTop2, trueBottom2, pole);
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
        //drawBlob(pole, GREEN);
        // if things look ok, then we have ourselves a second post
        int fudge = 0;
        if (trueLeft < 1 || trueRight > IMAGE_WIDTH - 2) {
            fudge = spanX / 2;
        }
        if (locationOk(pole, horizon) && ratOk && goodSecondPost &&
            secondPostFarEnough(leftP, rightP, pole.getLeftTop(),
								pole.getRightTop(), post) &&
			relativeSizesOk(spanX, pspanY, spanX2, spanY2, trueTop, trueTop2, fudge)) {
            if (post == LEFT) {
                second = 1;
                updateObject(left, pole, _SURE, dc);
                // make sure the certainty was set on the other post
                right->setIDCertainty(_SURE);
            } else {
                second = 1;
                updateObject(right, pole, _SURE, dc);
                left->setIDCertainty(_SURE);
            }
        } else {
            if (SANITY) {
                drawBlob(pole, ORANGE);
            }
            if (locationOk(pole, horizon) && ratOk && goodSecondPost &&
				pole.getLeftTopX() > trueRight) {
                // maybe it really is a side-goal situation
                if (abs(trueTop - trueTop2) < MAX_Y_VALUE &&
					qualityPost(pole, c)) {
                    if (post == LEFT) {
                        second = 1;
                        updateObject(left, pole, _SURE, dc);
                        // make sure the certainty was set on the other post
                        right->setIDCertainty(_SURE);
                    } else {
                        second = 1;
                        updateObject(right, pole, _SURE, dc);
                        left->setIDCertainty(_SURE);
                    }
                }
            } else if (!locationOk(pole, horizon) && secondPostFarEnough(leftP,
																		 rightP,
																		 pole.getLeftTop(),
																		 pole.getRightTop(),
																		 post)) {
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

void ObjectFragments::transferBlob(Blob from, Blob & to) {
    to.setLeftTopX(from.getLeftTopX());
    to.setLeftTopY(from.getLeftTopY());
    to.setRightTopX(from.getRightTopX());
    to.setRightTopY(from.getRightTopY());
    to.setRightBottomX(from.getRightBottomX());
    to.setRightBottomY(from.getRightBottomY());
    to.setLeftBottomX(from.getLeftBottomX());
    to.setLeftBottomY(from.getLeftBottomY());
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

    if (b.getRightBottomY() >= IMAGE_HEIGHT - 1 || b.getLeftBottomY() >= IMAGE_HEIGHT-1)
		return true;
    if (b.getRightTopX() - b.getLeftTopX() > IMAGE_WIDTH / 2) return true;
    int w = b.getRightBottomX() - b.getLeftBottomX() + 1;
    int y = 0;
    int x = b.getLeftBottomX();
	stop scan;
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
    int spanX = tempobj.getRightTopX() - tempobj.getLeftTopX(); 
    int spanY = tempobj.getLeftBottomY() - tempobj.getLeftTopY();
    if (spanX < 1 || spanY < 1) return false;
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
    if (percent > minpercent) {
        return true;
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

/* Combines several sanity checks into one.  Checks that the bottom of the
 * object is ok and the top too.
 * Also, just makes sure that the object is in fact an object.
 * @param b        the potential post
 * @param hor      the green horizon
 * @return         true if it is reasonably located, false otherwise
 */

bool ObjectFragments::locationOk(Blob b, int hor)
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
    if (hor < MIN_HORIZON) {
        if (POSTLOGIC) {
            cout << "Horizon too high" << endl;
        }
        return false;
    }
    int trueLeft = min(b.getLeftTopX(), b.getLeftBottomX());       // leftmost value
    int trueRight = max(b.getRightTopX(), b.getRightBottomX());    // rightmost value
    int trueTop = min(b.getLeftTopY(), b.getRightTopY());          // topmost value
    int trueBottom = max(b.getLeftBottomY(), b.getRightBottomY()); // bottommost value
    int horizonLeft = yProject(0, hor, trueLeft);          // horizon at left
    int horizonRight = yProject(0, hor, trueRight);        // horizon at right
    //drawPoint(trueLeft, horizonLeft, RED);
    //if (slope < 0) {
    //  horizonLeft = yProject(IMAGE_WIDTH - 1, hor, trueLeft);
    //  horizonRight = yProject(IMAGE_WIDTH - 1, hor, trueRight);
    //}
    //drawPoint(trueLeft, horizonLeft, YELLOW);
    //drawPoint(trueRight, horizonRight, YELLOW);
    int spanX = b.getRightTopX() - b.getLeftTopX() + 1;
    int spanY = b.getLeftBottomY() - b.getLeftTopY();
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
	if (trueTop < 1) return true;
    //if (trueRight - trueLeft > IMAGE_WIDTH - 10) return true;
    return horizonTopOk(trueTop, max(horizonAt(trueLeft),
                                     horizonAt(trueRight)));
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
    // if we're fudging then make sure there is green somewhere
	// in other words watch out for occluded beacons
    if (top == 0 && bottom + BOTTOM_FUDGE_FACTOR < minHeight) {
        //int x = left + spanX / 2;
        //int y = bottom + 5 + spanX;
        //vertScan(x, y, 1, 3, WHITE, ORANGEYELLOW);
        //drawPoint(x, y, RED);
        /*if (scan.good > MIN_GOOD) {
            if (SANITY)
                cout << "Occluded beacon?" << endl;
            return false;
			}*/
    }
    return true;
}

/* The top of objects need to be above the horizon.  Make sure they are.
 * Note:  we had to futz with this in Atlanta because of the wonky field
 * conditions.
 * @param top      the top of the post
 * @param hor      the green field horizon
 * @return         true when the horizon is below the top of the object
 */
bool ObjectFragments::horizonTopOk(int top, int hor)
{
    const int DEBUG_X = 100;

    //if (hor <= 0) return false;
    if (top < 0) return true;
    if (top + MIN_GOAL_HEIGHT / 2 > hor && hor > 0) {
        if (SANITY) {
            drawPoint(DEBUG_X, top, RED);
            drawPoint(DEBUG_X, hor, BLACK);
            cout << "Top is " << top << " " << hor << endl;
            cout << "Problems at top" << endl;
        }
        return false;
    }
    if (SANITY)
        cout << "Horizon top is ok " << top << " " << hor << endl;

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

void ObjectFragments::drawLess(int x, int y, int c)
{
    const int lineBuff = 10;

#ifdef OFFLINE
    thresh->drawLine(x, y, x + lineBuff, y - lineBuff, c);
    thresh->drawLine(x, y, x + lineBuff, y + lineBuff, c);
    thresh->drawLine(x + 1, y, x + lineBuff + 1, y - lineBuff, c);
    thresh->drawLine(x + 1, y, x + lineBuff + 1, y + lineBuff, c);
#endif
}

void ObjectFragments::drawMore(int x, int y, int c)
{
    const int lineBuff = 10;

#ifdef OFFLINE
    thresh->drawLine(x, y, x - lineBuff, y - lineBuff, c);
    thresh->drawLine(x, y, x - lineBuff, y + lineBuff, c);
    thresh->drawLine(x - 1, y, x - lineBuff - 1, y - lineBuff, c);
    thresh->drawLine(x - 1, y, x - lineBuff - 1, y + lineBuff, c);
#endif
}
