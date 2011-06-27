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
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
// GNU Lesser Public License for more details.
//
// You should have received a copy of the GNU General Public License
// and the GNU Lesser Public License along with Man.  If not, see
// <http://www.gnu.org/licenses/>.


/*
 * Object Fragment class - a ChownDawg production
 * ObjectFragments is no longer the major vision class. Instead it is
 * now simply the place where we process goals.	 We no longer do any
 * run-length-encoding here.  We still use the color "run" collected
 * in Threshold.cpp, but now we simply use them to look for likely
 * posts.  Once we have identified a likely location then we  do
 * active scanning to determine the outline of the post (if it is
 * indeed a post).	After that we do lots of sanity checking to
 * make sure that the post is reasonable.  In addition we try and
 * figure out whether we are looking at a right or left post.  There
 * are a number of ways to do this (is another post around?	 do we
 * see the crossbar?  the goalbox? etc.).
 */

#include <iostream>
#include "ObjectFragments.h"
#include "debug.h"
#include "FieldConstants.h"
#include "Utility.h"
#include <vector>
#include <boost/shared_ptr.hpp>

using namespace std;

static const float CLOSE_DIST = 150.0f;  // box corner to post distance
static const float FAR_DIST = 180.0f;    // corner of field to post
static const float BOX_FUDGE = 10.0f;          // allow for errors

//here are defined the lower bounds on the sizes of goals, posts, and balls
//IMPORTANT: they are only guesses right now.

#define MIN_GOAL_HEIGHT	35
#define MIN_GOAL_WIDTH	4

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

//previous constants inserted from .h class


ObjectFragments::ObjectFragments(Vision* vis, Threshold* thr, Field* fie,
                                 Context* con, unsigned char _color)
    : vision(vis), thresh(thr), field(fie), context(con), color(_color),
      runsize(1)
{
    init(0.0);
    allocateColorRuns();
}


/* Initialize the data structure.
 * @param s		the slope corresponding to the robot's head tilt
 */
void ObjectFragments::init(float s)
{
    slope = s;
    numberOfRuns = 0;
}


/*	Our "main" methods.	 Entry points into just about everything else.
 */


/* This is the entry  point from object recognition in Threshold.cc	 For now it
 * is only called on yellow, blue and orange.
 * In each case we search for objects that have the corresponding colors.
 * @param  c			color we are processing
 * @return				always 0
 */

void ObjectFragments::createObject() {
    // these are in the relative order that they should be called
    switch (color) {
    case BLUE_BIT:
        lookForFirstPost(vision->bglp, vision->bgrp,
                         vision->bgCrossbar, BLUE_BIT);
        break;
    case YELLOW_BIT:
        lookForFirstPost(vision->yglp, vision->ygrp,
                         vision->ygCrossbar, YELLOW_BIT);
        break;
    }
}


/*	Methods that have to do with the processing of "runs"
 */

/* Set the primary color.  Depending on the color, we have different space needs
 * @param c		   the color
 */
void ObjectFragments::setColor(unsigned char c)
{
    const int RUN_VALUES = 3;			  // x, y, and h
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
    const int RUN_VALUES = 3;			// x, y and h
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

   @param x		x value of run
   @param y		y value of top of run
   @param h		height of run
*/
void ObjectFragments::newRun(int x, int y, int h)
{
    const int RUN_VALUES = 3;	   // x, y, and h of course

    //cout << "Run " << x << " " << y << " " << h << endl;
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


/* Find the index of the biggest run left.	The "run" is our basic currency in
 * vision.	Our idea in looking for the goal is to find the biggest run we can
 * and assume that it is part of a goal beacon.	 We look from there for a big
 * square of the right color.  This method is very simple, it just scans all of
 * the runs between "left" and "right" and picks out the bigest one.  Once done
 * it returns the corresponding index.
 *
 * @param left	  the left boundary of legal runs to consider
 * @param right	  the right boundary of legal runs to consider
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
        if (nextH > maxRun && (nextX < left || nextX > right) &&
			nextX > 5 && nextX < IMAGE_WIDTH - 5) {
            maxRun = nextH;
            index = i;
        }
    }
    return index;
}

/* Identical to the last method except it allows a wider latitude
   of runs to consider.  Basically when our first attempt failed
   we expand our search parameters to include more runs (at the
   edges).  Also, will exclude the failed area
 *
 * @param left	  the left boundary of legal runs to consider
 * @param right	  the right boundary of legal runs to consider
 * @param prev    the run returned by getBigRun that didn't work
 * @return index  the x value of the largest run that meets the criteria
 */
int ObjectFragments::getBigRunExpanded(int left, int right, int prev) {
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
        if (nextH > maxRun && (nextX < left || nextX > right) &&
			(abs (nextX - prev) > 3)) {
            maxRun = nextH;
            index = i;
        }
    }
    return index;
}

/* The next group of methods has to do with scanning along axis parallel
 * dimensions in order to create objects without blobbing.
 */


/*	Find the visual horizon at the indicated x value.
 * that uses the edge of the field for a better estimate.
 * @param x		column where we'd like to know the horizon
 * @return		field horizon at that point
 */
int ObjectFragments::horizonAt(int x) {
    return field->horizonAt(x);
}

/* Project a line given a start coord and a new y value - note that this is
 * dangerous depending on how you do the projection.
 *
 * @param startx   the x point to start at
 * @param starty   the y point to start at
 * @param newy	   the y point to end at
 * @return		   the corresponding x point
 */
int ObjectFragments::xProject(int startx, int starty, int newy)
{
    //slope is a float representing the slope of the horizon.
    return startx - ROUND2(slope * (float)(newy - starty));
}

/* Project a line given a start coord and a new y value - note that this is
 * dangerous depending on how you do the projection.
 *
 * @param point	   the point to start at
 * @param newy	   the y point to end at
 * @return		   the corresponding x point
 */
int ObjectFragments::xProject(point <int> point, int newy) {
    //slope is a float representing the slope of the horizon.
    return point.x - ROUND2(slope * (float)(newy - point.y));
}

/* Project a line given a start coord and a new x value
 * @param startx   the x point to start at
 * @param starty   the y point to start at
 * @param newx	   the x point to end at
 * @return		   the corresponding y point
 */
int ObjectFragments::yProject(int startx, int starty, int newx)
{
    return starty + ROUND2(slope * (float)(newx - startx));
}

/* Project a line given a start coord and a new x value
 * @param point	   the point to start at
 * @param newx	   the x point to end at
 * @return		   the corresponding y point
 */
int ObjectFragments::yProject(point <int> point, int newx)
{
    return point.y + ROUND2(slope * (float)(newx - point.x));
}

/* Scan from the point along the line until you have hit "stopper" points that
 * aren't color "c" return the last good point found and how many good and bad
 * points seen.	 Though this is a void function it actually "returns"
 * information in the scan variable. scan.x and scan.y represent the finish
 * points of the line (last point of appropriate color) and bad and good
 * represent how many bad and good pixels (pixels that are of the right color
 * or not) along the way.
 * @param x		   the x point to start at
 * @param y		   the y point to start at
 * @param dir	   the direction of the scan (positive or negative)
 * @param stopper  how many incorrectly colored pixels we can live with
 * @param c		   color we are most interested in
 * @param c2	   soft color that could also work
 */
void ObjectFragments::vertScan(int x, int y, int dir, int stopper,
							   unsigned char c,
							   stop &scan)
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
    unsigned char pixel;
	int hor = horizonAt(x);
    // go until we hit enough bad pixels or are at a screen edge
    for ( ; x > -1 && y > -1 && x < width && y < height && bad < stopper; ) {
        //cout << "Vert scan " << x << " " << y << endl;
        // if it is the color we're looking for - good
        if (Utility::colorsEqual(c, GREEN))
            pixel = thresh->getColor(x, y);
        else
            pixel = thresh->getExpandedColor(x, y, c);
        if (Utility::colorsEqual(pixel, c)) {
			// when we're below the horizon ignore bluegreen
			if (c == BLUE_BIT && dir == 1 && y > hor && Utility::isGreen(pixel)) {
				bad++;
				run = 0;
			} else {
				good++;
				bad--;
				run++;
				if (run > 1) {
					scan.x = x;
					scan.y = y;
				}
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
 * @param x			 the x point to start at
 * @param y			 the y point to start at
 * @param dir		 the direction of the scan (positive or negative)
 * @param stopper	 how many incorrectly colored pixels we can live with
 * @param c			 color we are most interested in
 * @param c2		 soft color that could also work
 * @param leftBound	 furthest left we can go
 * @param rightBound further right we can go
 */
void ObjectFragments::horizontalScan(int x, int y, int dir, int stopper,
									 unsigned char c,
									 int leftBound, int rightBound,
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
    unsigned char pixel;
    // go until we hit enough bad pixels or are at a screen edge
    for ( ; x > leftBound && y > -1 && x < rightBound && x < IMAGE_WIDTH
              && y < height && bad < stopper; ) {
        if (Utility::colorsEqual(c, GREEN))
            pixel = thresh->getColor(x, y);
        else
            pixel = thresh->getExpandedColor(x, y, c);
        if (Utility::colorsEqual(pixel, c)) {
            // if it is either of the colors we're looking for - good
            good++;
            run++;
            bad--;
            int temp = pixel;
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
}

/*
 * TODO: Check this edge value
 * TODO: This isn't actually used.	But it would be nice to break free
 * of the standard color approach.
 * Given two points determine if they constitute an "edge".	 For now our
 * definition of an edge is a difference in Y values of 30 (sort of a standard
 * approach in our league).	 This is a place for potential improvements in the
 * future.
 *
 * @param x		   the x value of the first point
 * @param y		   the y value of the first point
 * @param x2	   the x value of the second point
 * @param y2	   the y value of the second point
 * @return		   was an edge detected?
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

/* Given an array of values pick the Nth biggest one.  Basically we just
   scan the array repeatedly and pick the biggest each time stopping at N.
   @param values		the array
   @param n				the "n" in "nth"
   @param s				the size of the array
   @return				the nth largest integer
*/
int ObjectFragments::pickNth(int values[], int n, int s) {
    // we don't really need to sort the values
    int best = 0, index = 0;
    for (int j = 0; j < n; j++) {
        best = 0; index = 0;
        for (int i = 0; i < s; i++) {
            if (values[i] > best) {
                best = values[i];
                index = i;
            }
        }
        values[index] = 0;
    }
    return best;
}


/*
 * Given two points that should define an edge of a structure, try and improve
 * them to find the true edge. The idea is pretty simply.  The passed in points
 * form a potential edge to our goal.  However, they are just a starting point.
 * WHat we want to do is to move the edge until it hits the real edge of the
 * object.	We scan out in three places to see how far the object goes.	 Then
 * we take the middle value (Olympic scoring - throw out high and low values).
 *
 * @param top		  the topmost estimated point
 * @param bottom	  the bottommost estimated point
 * @param c			  the primary color of the structure
 * @param c2		  the secondary color of the structure
 * @param left		  whether the open space is to the left or the right
 */

void ObjectFragments::findVerticalEdge(point <int>& top,
									   point <int>& bottom,
									   unsigned char c, bool left, bool correct)
{
    const int NUMSCANS = 5;
    const int WHICH = 3;
    int values[NUMSCANS];
    int spanY = bottom.y - top.y;
    int dir = 1;
    if (left) {
        dir = -1;
    }
    int shortSpan = spanY / NUMSCANS;
    int qy = top.y + shortSpan;
    int qx = xProject(top.x, top.y, qy);
    stop scan;

    for (int i = 0; i < NUMSCANS; i++) {
        // scan out a 1/4 of the way down the edge
        horizontalScan(qx, qy, dir, 4, c, 0, IMAGE_WIDTH - 1, scan);
        values[i] = abs(qx - scan.x);	// how far did we go?
        // set up the next scan
        qy += shortSpan;
        qx = xProject(top.x, top.y, qy);
    }

  int qs = pickNth(values, WHICH, NUMSCANS);
  if (qs == 0 && correct) {
	  // reset in case our edge is too far out
	  qy = top.y + shortSpan;
	  qx = xProject(top.x, top.y, qy);
	  for (int i = 0; i < NUMSCANS; i++) {
		  bool found = false;
		  values[i] = 0;
		  int tempx = qx;
		  int tempy = qy;
		  while (!found) {
			  if (Utility::colorsEqual(color,
									   thresh->getThresholded(tempy, tempx))) {
				  found = true;
			  } else {
				  values[i]++;
			  }
			  tempx -=dir;
			  tempy = yProject(tempx - 1 * dir, tempy, tempx);
			  if (tempx < 0 || tempx >= IMAGE_WIDTH || tempy < 0 ||
															   tempy >= IMAGE_HEIGHT ||
					  values[i] > 15) {
				  found = true;
			  }
		  }
		  // set up the next scan
		  qy += shortSpan;
		  qx = xProject(top.x, top.y, qy);
	  }
	  qs = pickNth(values, 2, NUMSCANS);
	  if (qs > 0) {
		  if (CORRECT) {
			  cout << "Squeezing " << qs << " " << dir << endl;
		  }
	  }
	  dir = -dir;
  }

  // reset the edge
  int te = top.x;
  top.x = top.x + dir * qs;
  top.y = yProject(te, top.y, top.x);
  bottom.y = min(top.y + spanY, IMAGE_HEIGHT -1);
  bottom.x = min(max(0, xProject(top.x, top.y, top.y + spanY)), IMAGE_WIDTH-1);
  top.x = min(max(0, top.x), IMAGE_WIDTH-1);
  top.y = max(0, top.y);
}

/*
 * Given two points that should define an edge of a structure, try and improve
 * them to find the true edge.	The idea is pretty simply.	The passed in points
 * form a potential edge to our goal.  However, they are just a starting point.
 * See comments for previous method.
 * @param left		  the leftmost estimated point
 * @param right		  the rightmost estimated point
 * @param c			  the primary color of the structure
 * @param c2		  the secondary color of the structure
 * @param up		  whether the open space is up or down
 */

void ObjectFragments::findHorizontalEdge(point <int>& left,
					 point <int>& right, unsigned char c,
					 bool up)
{
    const int NUMSCANS = 4;
    const int WHICH = 2;
    int values[NUMSCANS];
    int spanX = right.x - left.x;
    int dir = 1;
    if (up) {
        dir = -1;
    }
    int shortSpan = spanX / 4;
    int qx = left.x + shortSpan;
    int qy = yProject(left.x, right.y, qx);
    stop scan;

    // scan out a 1/4 of the way to the right the edge
    for (int i = 0; i < NUMSCANS; i++) {
        vertScan(qx, qy, dir, 4, c, scan);
        values[i] = abs(qy - scan.y);	// how far did we go?
        qx += shortSpan;
        qy = yProject(left.x, left.y, qx);
    }

    int qs = pickNth(values, WHICH, NUMSCANS);
	if (qs == 0) {
		// reset in case our edge is too far out
		int qx = left.x + shortSpan;
		int qy = yProject(left.x, right.y, qx);
		for (int i = 0; i < NUMSCANS; i++) {
			bool found = false;
			values[i] = 0;
			int tempx = qx;
			int tempy = qy;
			while (!found) {
				if (Utility::colorsEqual(color,
										 thresh->getThresholded(tempy, tempx))) {
					found = true;
				} else {
					values[i]++;
				}
				tempy -=dir;
				tempx = xProject(tempx, tempy - dir, tempy);
				if (tempx < 0 || tempx >= IMAGE_WIDTH || tempy < 0 ||
																 tempy >= IMAGE_HEIGHT ||
						values[i] > 15) {
					found = true;
				}
			}
			// set up the next scan
			qx += shortSpan;
			qy = yProject(left.x, left.y, qx);
		}
		qs = pickNth(values, 2, NUMSCANS);
		if (qs > 0) {
			if (CORRECT) {
				cout << "Squeezing horizontally" << qs << " " << dir << endl;
			}
		}
		dir = -dir;
	}
    // reset the edge
    int te = left.y;
    left.y = left.y + dir * qs;
    left.x = xProject(left.x, te, left.y);
    right.x = min(left.x + spanX, IMAGE_WIDTH-1);
    right.y = min(max(0, yProject(left.x, left.y, left.x + spanX)),
                  IMAGE_HEIGHT-1);
    left.y = min(max(0, left.y), IMAGE_HEIGHT-1);
    left.x = min(max(0, left.x), IMAGE_WIDTH-1);
}


/*	In theory our pose information tells us the slant of an object.	 In practice
	it doesn't always get it for a vareity of reasons.	This is an attempt to
	correct for the errors in the process.	At this point
	it is basically a rough draft of a good methodology, but it does seem to
	work pretty well.

	Here's what we do:	We scan out from the corners of our blob.  For each corner
	we check how much extra stuff of the same color as the goal is around.
	If opposite corners both have extra stuff then it is taken as evidence
	that the post is slanted differently than our pose estimate.  We then use
	this evidence to generate a new slope estimate that we feed back to our
	goal scanning routine.	It then uses that slope instead of the pose estimate
	to find the post.
	@param post	   the blob we're examining
	@param c	   primary color
	@param c2	   secondary color
*/

float ObjectFragments::correct(Blob & b, unsigned char color) {
    const float GOOD_SLOPE = 0.25f;

    int points[3];
    int corrections[3];
	int directions[3];
	for (int i = 0; i < 3; i++) {
		corrections[i] = 0;
		directions[i] = 0;
	}
    int diffy = (b.getLeftBottomY() - b.getLeftTopY()) / 4;
    int midy = b.getLeftTopY();
    int midsy = 0, bottomy = 0, topy = 0;
    bool correct = false;
    int midx, count;
	unsigned char col;
    // loop to the right and to the left
    for (int k = 1; k > -2; k = k - 2) {
        // loop at 1/4 2/4 and 3/4
        for (int i = 1; i < 4; i++) {
            corrections[i-1] = 0;
            midy = midy + diffy;
            if (k > 0) {
                midx = b.getLeft();
            } else {
                midx = b.getRight();
            }
            if (i == 1) {
                topy = midy;
            } else if (i == 2) {
                midsy = midy;
            } else bottomy = midy;
            col = GREY_BIT;
            count = 0;
            // we should be at the current edge - loop until we find real thing
            while (col != color && midx < IMAGE_WIDTH && midx > -1) {
                // normally we assume we are outside the real edge
                midx+= k;
                if (Utility::colorsEqual(thresh->getExpandedColor(midx, midy, color),
										 color)) {
                    col = color;
                    if (count == 0) {
                        // this is the case where the edge is outside our blob
                        while (midx >= 0 &&
                               Utility::colorsEqual(thresh->getExpandedColor(midx,
																			 midy,
																			 color),
													color)) {
                            midx-= k;
                            count++;
							directions[i-1]--;
                        }
                    }
                } else {
                    count++;
                    corrections[i-1]++;
					directions[i-1]++;
                }
                if (count >	 2) {
					correct = true;
				}
            }
            points[i-1] = midx;
        }
        if (corrections[0] > 0 && corrections[1] > 0 && corrections[2] > 0) {
            if (CORRECT) {
                cout << "All negative corrections " << corrections[0] <<
                    " " << corrections[1] << " " <<
                    corrections[2] << " " << endl;
            }
            int magnitude = pickNth(corrections, 3, 3);
            if (k > 0) {
                // left side is too far out
                int oldx = b.getLeftTopX();
                b.setLeftTopX(oldx + magnitude);
                b.setLeftTopY(max(0, yProject(oldx, b.getLeftTopY(),
                                              b.getLeftTopX())));
                oldx = b.getLeftBottomX();
                b.setLeftBottomX(oldx + magnitude);
                b.setLeftBottomY(min(IMAGE_HEIGHT - 1,
                                     yProject(oldx, b.getLeftBottomY(),
                                              b.getLeftBottomX())));
            } else {
                // right side is too far out
                int oldx = b.getRightTopX();
                b.setRightTopX(oldx - magnitude);
                b.setRightTopY(max(0, yProject(oldx, b.getRightTopY(),
                                               b.getRightTopX())));
                oldx = b.getRightBottomX();
                b.setRightBottomX(oldx - magnitude);
                b.setRightBottomY(min(IMAGE_HEIGHT - 1,
                                      yProject(oldx, b.getRightBottomY(),
                                               b.getRightBottomX())));
            }
        }
        float newSlope1 = -(float)(points[2] - points[1]) /
            (float)(bottomy - midsy);
        float newSlope2 = -(float)(points[1] - points[0]) /
            (float)(midsy - topy);
        float newSlope3 = -(float)(points[2] - points[0]) /
            (float)(bottomy - topy);
        if (CORRECT) {
            cout << "Slopes " << newSlope1 << " " << newSlope2 << " " <<
                newSlope3 << " " << correct << endl;
            if (correct) {
                cout << "Correct is true" << endl;
            } else {
                cout << "Correct is false" << endl;
            }
        }
        if (correct && abs(newSlope1 - newSlope2) < GOOD_SLOPE &&
            b.getLeft() > 5 && b.getRight() < IMAGE_WIDTH - 4 &&
            abs(newSlope2 - newSlope3) < GOOD_SLOPE &&
            abs(newSlope3 - slope) > 0.1) {
			// determine direction and magnitude of correction
			/*int correctMagnitude = pickNth(corrections, 1, 3);
			if (k == 1) {
				if (directions[0] > 0 && directions[2] < 0) {
					// correcting to right
					int yChange = yProject(0, 0, correctMagnitude);
					b.shift(correctMagnitude, yChange);
				} else {
					int yChange = yProject(0, 0, -correctMagnitude);
					b.shift(-correctMagnitude, yChange);
				}
			}
			drawBlob(b, GREEN);*/
            return newSlope3;
        }
        midy = b.getRightTopY();
    }
    return 0.0f;

}

/*	Routine to find a general rectangular goal.
 * We start with a point.  We scan up from the point and down from the point
 * looking for a strip of the right color.	That serves as our starting point.
 * Then we try expanding the sides outward.	 The we try expanding the top and
 * bottom in a similar fashion.	 Once we have our goal we call the correction
 * routine to see if we could get a better rectangle by using a different slope.
 *
 * @param x			x value of our starter point
 * @param y			y value of our starter point
 * @param c			the primary color
 * @param c2		the secondary color
 * @param obj		blob to store the data in
 */
void ObjectFragments::squareGoal(int x, int y, int left, int right, int minY,
				 int maxY, unsigned char c, Blob & obj)
{
    const int ERROR_TOLERANCE = 5;

    // set bad values so we can check for failure
    obj.setLeftTopX(BADVALUE); obj.setLeftTopY(BADVALUE);
    stop scan;
    int top = minY;
    int spanY = maxY - minY;
	int bottom = top + spanY;
    int topx = xProject(left, maxY, minY);
    int rightx = topx + (right - left);
    int topry = yProject(topx, top, rightx);
    point <int> leftTop = point<int>(topx, top);
    point <int> rightTop = point<int>(rightx, topry);
    point <int> leftBottom = point<int>(left, maxY);
    point <int> rightBottom = point<int>(right, topry + spanY);
    for (int i = 0; i < 2; i++) {
        // now expand the top and bottom
        findHorizontalEdge(leftTop, rightTop, c, true);
        findHorizontalEdge(leftBottom, rightBottom, c, false);
        findVerticalEdge(leftTop, leftBottom, c, true, false);
        findVerticalEdge(rightTop, rightBottom, c, false, false);
        // now expand the top and bottom
        findHorizontalEdge(leftTop, rightTop, c, true);
        findHorizontalEdge(leftBottom, rightBottom, c, false);
        // store the information into our blob
        obj.setBlob(leftTop, rightTop, leftBottom, rightBottom);
        // check if our estimated slope from pose is not right for this post
        float newSlope = 0.0f;
        if (i == 0) {
            newSlope = correct(obj, c);
            // if we detected that the post was leaning then redo
            leftTop = obj.getLeftTop();
            rightTop = obj.getRightTop();
            leftBottom = obj.getLeftBottom();
            rightBottom = obj.getRightBottom();
            if (newSlope != 0.0) {
                if (CORRECT) {
                    cout << "Old slope was " << slope << " " << newSlope <<
                        endl;
                }
				bool right = false;
				if (newSlope < slope) {
					right = false;
				}
				slope = newSlope;
                // we need to be very careful about placement here
                // determine the center of the blob
                int midTopx = (leftTop.x + rightTop.x) / 2;
                int midBottomx = (leftBottom.x + rightBottom.x) / 2;
                int midY = (leftTop.y + leftBottom.y) / 2;
                int newx = xProject((midBottomx + midTopx) / 2, midY,
                                    obj.getTop());
                if (CORRECT) {
                    vision->drawPoint(newx, midY, RED);
                    cout << "New start " << newx << " " << midY << endl;
                }
				// use the center to rotate all of the corners
				int mag;
				int projx = (xProject(newx, midY, top));
				mag = projx - newx;
				leftTop.x += mag;
				rightTop.x += mag;
				leftBottom.x -= mag;
				rightBottom.x -= mag;

                //leftTop = point<int>(newx, obj.getTop());
                //rightTop = point<int>(newx, obj.getTop());
                //newx = xProject((midBottomx, +midTopx) / 2, midY,
				//              obj.getBottom());
                //leftBottom = point<int>(newx, obj.getBottom());
                //rightBottom = point<int>(newx, obj.getBottom());
                // repeat the process fresh
                findVerticalEdge(leftTop, leftBottom, c, true, true);
                findVerticalEdge(rightTop, rightBottom, c, false, true);
                findHorizontalEdge(leftTop, rightTop, c, true);
                findHorizontalEdge(leftBottom, rightBottom, c, false);
            }
        }
    }
}

/* A collection of miscelaneous methods used in processing goals.
 */


/*	Revised to basically do nothing but transfer the contents of the blob.
 * @param one			  the field object we'd like to update
 * @param two			  the blob that contains the information we need
 * @param certainty		  how certain are we of its ID?
 * @param distCertainty	  how certain are we of how big we think it is?
 */
bool ObjectFragments::updateObject(VisualFieldObject* one, Blob two,
								   certainty _certainty,
								   distanceCertainty _distCertainty) {
    one->updateObject(&two, _certainty, _distCertainty);
    // update the context variables too
    if (!_certainty) {
        if (color == BLUE_BIT) {
            context->setUnknownBluePost();
        } else {
            context->setUnknownYellowPost();
        }
    } else {
        switch (one->getID()) {
        case BLUE_GOAL_LEFT_POST:
            context->setLeftBluePost();
            break;
        case BLUE_GOAL_RIGHT_POST:
            context->setRightBluePost();
            break;
        case YELLOW_GOAL_LEFT_POST:
            context->setLeftYellowPost();
            break;
        case YELLOW_GOAL_RIGHT_POST:
            context->setRightYellowPost();
            break;
        case YELLOW_GOAL_POST:
            context->setUnknownYellowPost();
            break;
        case BLUE_GOAL_POST:
            context->setUnknownBluePost();
            break;
        default:
            break;
        }
    }
    return true;
}

/* Here we are trying to figure out how confident we are about our values with
 * regard to how they might be used for distance calculations.	Basically if
 * an object is too near one of the screen edges, or if we have some evidence
 * that it is occluded then we note uncertainty in that dimension.
 *
 * @param pole	   the object we are checking
 * @return		   a constant indicating where the uncertainties (if any) lie
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
            dc =	WIDTH_UNSURE;
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
        vertScan(nextX, nextY, 1,  ERROR_TOLERANCE, GREEN, scan);
        nextX = nextX + 2;
        nextY = yProject(pole.getLeftBottom(), nextX);
    } while (nextX <= pole.getRightBottomX() && scan.good < 2);
    if (scan.good > 1)
        return dc;
    else if (dc == WIDTH_UNSURE)
        return BOTH_UNSURE;
    return HEIGHT_UNSURE;
}

/*	Sometimes we process posts differently depending on how big they are.
 * This just characterizes a post's size such that we can make that
 * determination.
 *
 * TODO: These sizes are almost meaningless for the NAOs.  They have been
 * updated some, but do not reflect the true size of the Nao goals.
 * @param b	  the post in question
 * @return	  a constant indicating size - SMALL, MEDIUM, or LARGE
 */
// EXAMINED: change these constants
int ObjectFragments::characterizeSize(Blob b) {
    int w = b.getRightTopX() - b.getLeftTopX() + 1;
    int h = b.getLeftBottomY() - b.getLeftTopY() + 1;
    const int largePostHeight = 100;
    const int smallPostHeight = 50;
    const int smallPostWidth = 10;
    const int midPostHeight = 30;
    const int midPostWidth = 15;
    if (h > largePostHeight) {
        return LARGE;
    }
    if (h < smallPostHeight || w < smallPostWidth) {
        return SMALL;
    }
    if (h < midPostHeight || w < midPostWidth) {
        return MEDIUM;
    }
    return LARGE;
}

/* Sets a standard of proof for a post.	 In this case that the blob comprising
 * the post is at least 60% correct color.
 * Note:  this is actually not the greatest idea when the angle of the head is
 * significantly off horizontal.
 * TODO:  This should use slopes to scan the actual blob.
 *
 * @param b	  the post in question
 * @return	  a constant indicating size - SMALL, MEDIUM, or LARGE
 */

bool ObjectFragments::qualityPost(Blob b, unsigned char c)
{
    const float PERCENT_NEEDED = 0.6f;  // percent of pixels that must be right

    int good = 0;
    //bool soFar;
    for (int i = b.getLeftTopX(); i < b.getRightTopX(); i++) {
        for (int j = b.getLeftTopY(); j < b.getLeftBottomY(); j++) {
            if (Utility::colorsEqual(thresh->getExpandedColor(i, j, c), c)) {
                good++;
            }
        }
    }

    if (static_cast<float>(good) <
        static_cast<float>(b.getArea()) * PERCENT_NEEDED) {
        return false;
    }

    return true;
}

/* Provides a kind of sanity check on the size of the post.	 Essentially we are
 * looking for cases where we don't have a post, but are looking at a backstop.
 * Also just let's us know how good the size estimate is.
 * This needs lots of beefing up.
 * TODO: This needs to be updated for the Naos.	 It was written for the Aibos
 * and has not been updated.
 *
 * @param b	  the post in question
 * @return	  a constant indicating size - SMALL, MEDIUM, or LARGE
 */

bool ObjectFragments::checkSize(Blob b, unsigned char c)
{
    const int ERROR_TOLERANCE = 6;	 // how many bad pixels to scan over
    const int FAR_ENOUGH = 10;		 // how far to scan to be sure
    const int PROBLEM_THRESHOLD = 5;	 // haw many pixels presents a problem

    int midY = b.getLeftTopY() + (b.getLeftBottomY() - b.getLeftTopY()) / 2;
    stop scan;
    horizontalScan(b.getLeftTopX(), midY, -1,	 ERROR_TOLERANCE, c, 0,
                   b.getLeftTopX() + 1, scan);
    int leftMid = scan.good;
    horizontalScan(b.getRightTopX(), midY, 1, ERROR_TOLERANCE, c,
                   b.getRightTopX() - 1, b.getRightTopX() + FAR_ENOUGH, scan);
    if (leftMid > PROBLEM_THRESHOLD && scan.good > PROBLEM_THRESHOLD) {
        return false;
    }
    return true;
}

/*
 */
void ObjectFragments::lookForPost(int index, Blob & obj) {
    int maxRun = runs[index].h,  maxY = runs[index].y,  maxX = runs[index].x;

    int need = max(10, min(30, maxRun / 3));
    int left, right, smallY = maxY + maxRun / 2, bigY = smallY;
    for (left = index -1; left > -1 && runs[left].h > need &&
             runs[left].x > runs[left+1].x - 3; left--) {
        if (runs[left].y < smallY) {
            smallY = runs[left].y;
        } else if (runs[left].y+runs[left].h > bigY) {
            bigY = runs[left].y+runs[left].h;
        }
    }
    for (right = index+1; right < numberOfRuns && runs[right].h > need &&
             runs[right].x < runs[right-1].x + 3; right++) {
        if (runs[right].y < smallY) {
            smallY = runs[right].y;
        } else if (runs[right].y+runs[right].h > bigY) {
            bigY = runs[right].y+runs[right].h;
        }
    }

    // Try and figure out the true axis-parallel post dimensions - we're going
    // to try and start right in the middle
    int startX = maxX;
    int startY = maxY + maxRun / 2;
    // starts a scan in the middle of the tallest run.
    squareGoal(startX, startY, runs[left+1].x, runs[right - 1].x,
               smallY, bigY, color, obj);
}

/* Try and find the biggest post left on the screen.  We start by looking for
 * our longest "run" of the current color.
 * We then call squareGoal to expand that into a post.	Later
 * we will check if it actually meets the criteria for a good post.
 * @param c		  current color
 * @param left	  leftmost limit to look
 * @param right	  rightmost limit to look
 * @param		  indication of whether we found a decent candidate
 */

int ObjectFragments::grabPost(unsigned char c, int leftx,
				  int rightx, Blob & obj) {
    int index = 0;
    // find the biggest Run
    index = getBigRun(leftx, rightx);
    if (index == BADVALUE) {
		// try again
		index = getBigRunExpanded(leftx, rightx, -100);
		if (index == BADVALUE) {
			return NOPOST;
		} else if (POSTDEBUG) {
			cout << "Expanded big run used to generate post" << endl;
		}
    }
	lookForPost(index, obj);
    // make sure we're looking at something big enough to be a post
    if (!postBigEnough(obj)) {
		// try again
		index = getBigRunExpanded(leftx, rightx, runs[index].x);
		if (index != BADVALUE) {
			if (POSTDEBUG) {
				cout << "First post was too small, trying again" << endl;
				drawBlob(obj, ORANGE);
			}
			lookForPost(index, obj);
			if (!postBigEnough(obj)) {
				if (POSTDEBUG) {
					cout << "Post was too small" << endl;
					printBlob(obj);
					drawBlob(obj, ORANGE);
				}
				return NOPOST;
			}
		} else {
			if (POSTDEBUG) {
				cout << "Post was too small first" << endl;
				drawBlob(obj, ORANGE);
			}
			return NOPOST;
		}
    }
    // check how big it is versus how big we think it should be
    if (badDistance(obj)) {
        if (POSTDEBUG) {
            cout << "Threw post out for bad distance estimate " << endl;
        }
        return NOPOST;
    }
    return LEFT; // Just return something other than NOPOST
}

/*
 * Post recognition for NAOs
 */

/*	Look for a crossbar once we think we've found a goalpost.  The idea is
 * pretty simple, if we	 have done a good job with the square post,
 * then the crossbar should either shoot out of the left or right top corner.
 * So try scanning from those positions.  The tricky part, or course involves
 * being able to definitively say that what we see is in fact a bar.  I'd say
 * that right now this probably needs more work.
 *
 *	@param b   the square post
 *	@return	  either RIGHT or LEFT if a crossbar found, or NOPOST if not
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
    if (y < 3 && w > DISTANTPOST) {
        return NOPOST;
    }

    // scan the left side to see how far out we can go seeing post
    for (int i = 0; i < h / HEIGHT_DIVISOR && biggest < need; i+=1) {
        int tx = xProject(x, y, y + i);
        horizontalScan(tx, y + i, -1, ERROR_TOLERANCE, color,
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
        horizontalScan(tx, y + i, 1, ERROR_TOLERANCE, color,
                       x - 1, IMAGE_WIDTH - 1, scan);
        if (scan.good > biggest2) {
            biggest2 = scan.good;
        }
    }

    if (POSTLOGIC) {
        cout << "Cross check " << biggest << " " << biggest2 << endl;
        if (biggest > need) {
            vision->drawRect(b.getLeftTopX() - biggest, b.getLeftTopY(), biggest,
                     DEBUG_DRAW_SIZE, ORANGE);
        }
        if (biggest2 > need) {
            vision->drawRect(x, y, biggest2, DEBUG_DRAW_SIZE, ORANGE);
        }
    }

    if (biggest > need && biggest > 2 * biggest2) {
        // do one last sanity check
        x = b.getLeftTopX();
        y = b.getLeftTopY();
        for (int i = h / 2; i <  h / 2 + h / HEIGHT_DIVISOR; i+=1) {
            int tx = xProject(x, y, y + i);
            horizontalScan(tx, y + i, -1, ERROR_TOLERANCE, color,
                           max(0, x - 2 * w), IMAGE_WIDTH - 1, scan);
            if (scan.good > biggest / 2) {
                return NOPOST;
            }
        }
        return RIGHT;
    }
    if (biggest2 > need && biggest2 > 2 * biggest) {
        for (int i = h / 2; i <  h / 2 + h / HEIGHT_DIVISOR; i+=1) {
            int tx = xProject(x, y, y + i);
            horizontalScan(tx, y + i, 1, ERROR_TOLERANCE, color,
                           max(0, x - 2 * w), IMAGE_WIDTH - 1, scan);
            if (scan.good > biggest2 / 2) {
                return NOPOST;
            }
        }
        return LEFT;
    }

    return NOPOST;
}

/*
 */
int ObjectFragments::classifyByInnerL(Blob post, VisualCorner & corner) {
	int x = corner.getX();
	int y = corner.getY();
	bool right = corner.doesItPointRight();
	float distant = 0;
	// check if this corner is at the edge
	const vector < boost::shared_ptr<VisualLine> > * lines =
		vision->fieldLines->getLines();
	for (vector < boost::shared_ptr<VisualLine> >::const_iterator i =
			 lines->begin();
		 i != lines->end(); ++i) {
		distant = max((*i)->getDistance(), distant);
	}
	// check that the post isn't too far away
	// basically the goalie view - the post to one side, corner in view
	if (distant > corner.getLine1()->getDistance() &&
		distant > corner.getLine2()->getDistance()) {
		if (x > post.getLeft()) {
			return LEFT;
		} else {
			return RIGHT;
		}
	}
	float diff = realDistance(x, y, post.getLeftBottomX(),
							  post.getLeftBottomY());
	float further = max(corner.getLine1()->getDistance(),
						corner.getLine2()->getDistance());
	// field corner
	if (distant <= further) {
		if (diff < FIELD_WHITE_HEIGHT / 2) {
			if (x > post.getLeft()) {
				return RIGHT;
			} else {
				return LEFT;
			}
		}
	}
	estimate e = vision->pose->pixEstimate(x, y, 0.0);
	if (e.dist < FIELD_WHITE_HEIGHT / 2) {
		if (x > post.getLeft()) {
			return RIGHT;
		} else {
			return LEFT;
		}
	}

	// if we can't see the bottom of the post it is too dangerous
	if (y < post.getBottom() && post.getBottom() < IMAGE_HEIGHT - 2) {
		// roughly how far away is it?
		// if it is in the right position we can figure out which post
		if (POSTLOGIC) {
			cout << "Checking a corner " << x << " " <<
				post.getLeftBottomX() << " corner points ";
			if (right) {
				cout << "right" << endl;
			} else {
				cout << "left" << endl;
			}
			cout << "Distances: " << diff << " " << e.dist << endl;
		}
		if (x <= post.getLeftBottomX()) {
			if (right) {
				return cornerClassifier(diff, e.dist,
										post.getLeftBottomX(),
										post.getLeftBottomX(),
										LEFT, RIGHT, true);
			} else {
				return cornerClassifier(diff, e.dist,
										post.getLeftBottomX(),
										post.getLeftBottomX(),
										LEFT, RIGHT, false);
			}
		} else {
			if (right) {
				return cornerClassifier(diff, e.dist,
										post.getRightBottomX(),
										post.getRightBottomY(), RIGHT,
										LEFT, false);
			} else {
				return cornerClassifier(diff, e.dist,
										post.getRightBottomX(),
										post.getRightBottomY(), RIGHT,
										LEFT, true);
			}
		}
	}
	return NOPOST;
}

/* Try to use Outer L corners to decide which post we're looking at.
 */

int ObjectFragments::classifyByOuterL(Blob post, VisualCorner & corner) {
	// Determine which line of the corner is the shortest
	const point<int> end1 = corner.getLine1()->getEndpoint();
	const point<int> end2 = corner.getLine1()->getStartpoint();
	float l1 = realDistance(end1.x, end1.y, end2.x, end2.y);
	const point<int> endl1 = corner.getLine2()->getEndpoint();
	const point<int> endl2 = corner.getLine2()->getStartpoint();
	float l2 = realDistance(endl1.x, endl1.y, endl2.x, endl2.y);
	int x = corner.getX();
	float dist = realDistance(post.getLeft(), post.getBottom(),
							  corner.getX(), corner.getY());
	// sometimes side Ts turn up as Ls
	// annika/yellowgoal/63.FRM
	if (dist > 250 || (l1 > 150 && l2 > 150)) {
		return NOPOST;
	}
	// if one line is long enough we can determine its relationship
	if (POSTLOGIC) {
		cout << "Checking outer L corner " << l1 << " " << l2 << endl;
	}
	if (abs(corner.getOrientation()) < 90) {
		if (l1 > l2 && l1 > GOALBOX_DEPTH + 20.0f) {
			if (endl1.y < end2.y) {
				if (endl1.x > post.getRight()) {
					return RIGHT;
				} else {
					return LEFT;
				}
			} else if (endl2.x > post.getRight()) {
				return RIGHT;
			} else {
				return LEFT;
			}
		} else if (l2 > l1 && l2 > GOALBOX_DEPTH + 20.0f) {
			if (end1.y < end2.y) {
				if (end1.x > post.getRight()) {
					return RIGHT;
				} else {
					return LEFT;
				}
			} else if (end2.x > post.getRight()) {
				return RIGHT;
			} else {
				return LEFT;
			}
		}
	}
	// perhaps it is a field corner
	if (post.getBottom() - corner.getY() > 20 &&
		corner.getDistance() < MIDFIELD_X) {
		if (POSTLOGIC) {
			cout << "Checking for field corner" << endl;
		}
		if (x > post.getLeft() + post.width() / 2) {
			if (corner.doesItPointRight()) {
				return RIGHT;
			}
		} else if (corner.doesItPointLeft()) {
			return LEFT;
		}
	}

	return NOPOST;
}


/* Try to use T Corners to decide which post we're looking at.
   This is actually considerably easier with the large goal boxes as the only
   times you'll see corners reasonably near a post is when they are on the
   same side as the post.
*/

int ObjectFragments::classifyByTCorner(Blob post) {

	const int MAXIMUM_Y_DIFF = 30;	 // minimum height post

	// TODO: check if this should be the same standard minHeight for a post
	if (post.height() < MAXIMUM_Y_DIFF) {
		return NOPOST;
	}
	list <VisualCorner>* corners = vision->fieldLines->getCorners();
	int spanx = post.width();
	int spany = post.height();
	// iterate through all of the corners looking for T Corners
	for (list <VisualCorner>::iterator k = corners->begin();
		 k != corners->end(); k++) {
		if (k->getShape() == T) {
			int x = k->getX();
			int y = k->getY();
			bool closeEnough = false;
            // Check the distance - if it is really far, then it is
            // a side T
			if (y < post.getLeftBottomY() + spany) {
				closeEnough = true;
			}
			float diff = realDistance(x, y, post.getLeftBottomX(),
									  post.getLeftBottomY());
			bool adjacent = true;
			if (diff > GOALBOX_OVERAGE * 2.0f) {
				adjacent = false;
			}
			// if the T is higher in the visual field than the bottom of the post
			// then we have an easy job - we can decide based on which side its on
			if (closeEnough) {
				if (POSTLOGIC) {
					cout << "Got a T that was close enough " << endl;
				}
				int side;
                if (k->doesItPointUp() && y < post.getLeftBottomY()) {
                    if (k->doesItPointLeft()) {
                        side = LEFT;
                    } else {
                        side =  RIGHT;
                    }
                } else if (k->doesItPointUp() && y > post.getLeftBottomY()) {
                    if (k->doesItPointLeft()) {
                        side = RIGHT;
                    } else {
                        side = LEFT;
                    }
                }
				else if (x <= post.getLeftBottomX() + spanx / 2) {
					side = LEFT;
				} else {
					side = RIGHT;
				}
				if (!adjacent) {
					if (diff < GOALBOX_OVERAGE * 3.0) {
						return NOPOST;
					}
					if (POSTLOGIC) {
						float dis = context->realLineDistance(k->getTStem());
						cout << "T is far from post " << diff << " " << dis
							 << endl;
					}
					// The T may actually be a center T - should be easy
					if (context->realLineDistance(k->getTStem()) >
						GOALBOX_DEPTH * 1.5f || diff > CROSSBAR_CM_WIDTH +
						GOALBOX_OVERAGE * 2) {
						if (POSTLOGIC) {
							cout << "T is a side T" << endl;
						}
						// naturally it isn't always this simple - see
						// watson_11/spock/pink_robot_far/8.NBFRM
						return NOPOST;
					} else if (side == LEFT) {
						return RIGHT;
					} else {
						return LEFT;
					}
				}
				return side;
			}
		}
	}
	return NOPOST;
}

/*	We have a post and wonder which one it is.	This method looks for the nearby
	corners. Not as good as T Corners, but still a decent way to ID the goal.
	@param post	   the post we have id'd
	@return		   the id of the post (or lack of id)
*/

int ObjectFragments::classifyByCheckingCorners(Blob post)
{
    // get all of the corners
    list <VisualCorner>* corners = vision->fieldLines->getCorners();
    int spanx = post.width();
    int spany = post.height();
    // iterate through all of the corners, skipping all of the T Corners
	int classification;
    for (list <VisualCorner>::iterator k = corners->begin();
         k != corners->end(); k++) {
        // we already processed T Corners so skip them, skip others too
        if (k->getShape() == INNER_L) {
            classification = classifyByInnerL(post, *k);
			if (classification != NOPOST) {
				return classification;
			}
        } else if (k->getShape() == OUTER_L) {
			classification = classifyByOuterL(post, *k);
			if (classification != NOPOST) {
				return classification;
			}
		}
    }
    return NOPOST;
}

/* We have found a corner and its distance to the post, so use that information
 * to identify the post.
 * @param diff         the distance from the post to the corner
 * @param dist         the distance from the observor to the corner
 * @param x            the x coord of the bottom corner of the post
 * @param y            the y coord of the bottom corner of the post
 * @param class1       post classification based on the side of the post
 * @param class2       the other post classification
 * @return             the id of the post
 */
int ObjectFragments::cornerClassifier(float diff, float dist, int x, int y,
                                      int class1, int class2, bool goal) {

    // if the post is close to the corner, then we know it we are good
    if (goal && diff < POST_CORNER * 1.5) {
        return class1;
    }
    if (!goal && diff < (FIELD_WHITE_HEIGHT - CROSSBAR_CM_WIDTH) * 0.6) {
        return class1;
    }
    if (dist < CLOSE_DIST) {
        return class1;
    } else if (dist > FAR_DIST) {
        // the corner is far away - how far away is the post?
        estimate p = vision->pose->pixEstimate(x, y, 0.0);
        if (p.dist < 40.0f) {
            return class1;
        } else if (p.dist < 140.0f) {
            if (POSTLOGIC) {
                cout << "Dangerous corner classification" << endl;
            }
            return NOPOST;
        } else {
            return class1;
        }
    }
    return NOPOST;
}


/*	We have a post and wonder which one it is.	This method looks for the nearby
	goal box line.	Basically if we see one post and a line in front of it, if the
	line has a long length in either direction it can help us ID the post.
	@param post	   the post we have id'd
	@return		   the id of the post (or lack of id)
*/

int ObjectFragments::classifyByCheckingLines(Blob post)
{
    const int MAXIMUM_Y_DIFFERENCE = 30;	// max offset between corner and post
    const float MINDIST = 300.0f;			// we might see the midline
    const int NO_INTERSECTION = -13591295;

    if (post.getRightBottomY() - post.getRightTopY() < MAXIMUM_Y_DIFFERENCE) {
        return NOPOST;
    }
    estimate e = vision->pose->pixEstimate(post.getLeftBottomX(),
                                           post.getLeftBottomY(), 0.0);
    if (e.dist > MINDIST) {
        return NOPOST;
    }
    point <int> plumbLineTop, plumbLineBottom, line1start, line1end;
    plumbLineTop.x = post.getLeft(); plumbLineTop.y = min(IMAGE_HEIGHT - 1,
                                                          post.getBottom() + 10);
    plumbLineBottom.x = post.getLeft(); plumbLineBottom.y = IMAGE_HEIGHT;
    const vector <boost::shared_ptr<VisualLine> >* lines =
        vision->fieldLines->getLines();
    for (vector <boost::shared_ptr<VisualLine> >::const_iterator k =
             lines->begin();
         k != lines->end(); k++) {
        // Check if a plumb line from the bottom of the post intersects the line
        pair<int, int> foo = Utility::
            plumbIntersection(plumbLineTop, plumbLineBottom,
                              (*k)->getLeftEndpoint(), (*k)->getRightEndpoint());
        point<int> left = (*k)->getLeftEndpoint();
        point<int> right = (*k)->getRightEndpoint();
        if (foo.first != NO_INTERSECTION && foo.second != NO_INTERSECTION) {
            int classification = classifyByGoalBoxFrontline(foo, left, right);
            if (classification != NOPOST) {
                return classification;
            }
        } else {
            // is the line the endline?
            left = post.getLeftBottom();
            left.x = max(0, left.x - 15);
            right = post.getRightBottom();
            right.x = min(right.x + 15, IMAGE_WIDTH - 1);
            point<int> leftT = post.getLeftTop();
            leftT.x = max(0, leftT.x - 15);
            point<int> rightT = post.getRightTop();
            rightT.x = min(IMAGE_WIDTH - 1, rightT.x + 15);
            if (Utility::intersectProp(right, rightT,
                                       (*k)->getStartpoint(),
                                       (*k)->getEndpoint()) ||
                Utility::intersectProp(left, leftT,
                                       (*k)->getStartpoint(),
                                       (*k)->getEndpoint())) {
                int classification = classifyByGoalline((*k)->getLeftEndpoint(),
                                                        (*k)->getRightEndpoint(),
                                                        left, right);
                if (classification != NOPOST) {
                    return classification;
                }
            }
        }
    }
    return NOPOST;
}

/* This is called when we have a line that runs into the goal box and its
 * estimated distance suggests it may be the goalbox line.  We can sometimes
 * use this to classify the post.
 * @param y                the bottom of the post
 * @param diff             the difference between the bottom of the post, line
 * @param dist             the actual distance to the line
 * @param classification   the classification based on the side
 */
int ObjectFragments::classifyGoalBoxLineThatAbutsPost(int y, float diff, float dist,
                                                   int classification) {
	return NOPOST;
    // can we see the bottom of the post?
    /*if (y < IMAGE_HEIGHT - 1) {
        if (POSTLOGIC) {
            cout << "Perpindicular line detected, distance is " << diff << endl;
        }
        if (diff < GOALBOX_OVERAGE + 2 * BOX_FUDGE ) {
            return classification;
        } else {
            // ToDo:	The logic for this could still be worked out
            return NOPOST;
        }
    } else {
        // if the distance is short enough, then we can still classify
        if (dist < GOALBOX_OVERAGE + 2 * BOX_FUDGE) {
            return classification;
        } else {
            return NOPOST;
        }
		}*/
}

/* Returns true if the given x value is within margin of either image edge.
 * @param x      the value to check
 * @param margin how much tolerance
 * @return       whether or not x is within the margin
 */
bool ObjectFragments::withinEdgeMargin(int x, int margin) {
    if (x < margin || x > IMAGE_WIDTH - margin - 1) {
        return true;
    }
    return false;
}

/* Returns true if the given x value is within margin of either image edge.
 * @param x      the value to check
 * @param margin how much tolerance
 * @return       whether or not x is within the margin
 */
bool ObjectFragments::withinVerticalEdgeMargin(int y, int margin) {
    if (y < margin || y > IMAGE_HEIGHT - margin - 1) {
        return true;
    }
    return false;
}

/* We have a nice long line that intersects the post.  We usually can use
 * it to id the post by looking at how long it is on either side.  The
 * idea is that we have already checked corners at this point, so we know
 * we don't have any useful ones.  So a long line segment would normally
 * be the one that goes into the goal box (else we'd have seen the T).
 * @param dist     the length of the line segment
 * @param x        the endpoint x value of the line
 * @param y        the endpoint y value of the line
 * @param class1   the normal classification based on the goal box
 * @param class2   an alternative based on the edge of the field
 * @return         the classification (if we can do it)
 */
int ObjectFragments::classifyByLengthOfGoalline(float dist, int x, int y,
                                                int class1, int class2) {
    // watch out for occluded Ts
    if (dist < GOALBOX_OVERAGE + 2.5 * BOX_FUDGE && withinEdgeMargin(x, 10)) {
        return NOPOST;
    }
    if (POSTLOGIC) {
        cout << "Got a big goalline to the side of the post " << dist <<
            " " << x << " " << y << endl;
        if (class1 == RIGHT) {
            cout << "On right side" << endl;
        } else {
            cout << "On left side" << endl;
        }
    }
    // under some conditions we miss the T next to the post - look out for that
    // if the line ends in the middle of the frame and is near the horizon line
    if (!withinEdgeMargin(x, 10) && horizonAt(x) > 5 && y - horizonAt(x) < 20) {
        return class1;
    } else {
        if (POSTLOGIC) {
            cout << "Swapping classification " << x << " " << y << endl;
        }
        // this seems to get fouled up a lot
        //return class2;
        return NOPOST;
    }
}

/* Given the endpoints of a line that intersects the post, try to classify
 * the post.
 * @param linel     left endpoint of line
 * @param liner     right endpoint of line
 * @param left      left bottom of post
 * @param right     right bottom of post
 * @return          possible classification of post
 */

int ObjectFragments::classifyByGoalline(const point<int> linel,
                                        const point<int> liner,
                                        point<int> left, point<int> right) {
    // check if the Y component of the intersection is wildly off
    estimate e = vision->pose->pixEstimate(left.x, left.y, 0.0);
    estimate e2 = vision->pose->pixEstimate(liner.x, liner.y, 0.0);
    estimate e3 = vision->pose->pixEstimate(linel.x, linel.y, 0.0);
    float diffr = e2.dist - e.dist;
    float diffl = e3.dist - e.dist;
    // if line ends at left side of post it may be a goal box line
    if (liner.x < right.x && diffr > 20.0f) {
        return classifyGoalBoxLineThatAbutsPost(right.y, diffr,
                                             e2.dist, LEFT);
    } else if (linel.x > left.x && diffl > 20.0f) {
        return classifyGoalBoxLineThatAbutsPost(left.y, diffl,
                                             e3.dist, RIGHT);
    }
    // too dangerous in practice
    // if either of the endpoints are far enough away we can classify the line
    /*float linedist1 = realDistance(right.x, right.y, liner.x, liner.y);
    float linedist2 = realDistance(left.x, left.y, linel.x, linel.y);
    if (linedist1 > linedist2) {
        if (linedist1 > GOALBOX_OVERAGE + 2 * BOX_FUDGE) {
            return classifyByLengthOfGoalline(linedist1, liner.x, liner.y,
                                              RIGHT, LEFT);
        }
    } else if (linedist2 > GOALBOX_OVERAGE + 2 * BOX_FUDGE) {
        return classifyByLengthOfGoalline(linedist2, linel.x, linel.y,
                                          LEFT, RIGHT);
                                          }*/
    return NOPOST;
}

/* We have identified the front line of the goal box.  Based on its
 * relationship to the post we may be able to classify the post.
 * @param foo     where a a vertical line from the post intersects the line
 * @param left    the left endpoint of the line
 * @param right   the right endpoint of the line
 * @return        a possible classification
 */
int ObjectFragments::classifyByGoalBoxFrontline(pair<int, int> foo,
                                                point<int> left,
                                                point<int> right) {

    int leftsize = foo.first - left.x;
    int rightsize = right.x - foo.first;
    // which side has the longer line segment?
    float lineAngle = Utility::getAngle(left.x, left.y, right.x, right.y);
    // when the angle gets bigger than this the problem gets harder
    if (abs(lineAngle) < 15.0f) {
        if (leftsize > rightsize) {
            return getFrontlineClassification(left, foo, RIGHT);
        } else {
            return getFrontlineClassification(right, foo, LEFT);
        }
    } else {
        // ToDo: we should be able to handle this case too with some geometry
    }
    return NOPOST;
}

/* We have identified the front line of the goal box.  Based on its
 * relationship to the post we may be able to classify the post.
 * @param post           nearby bottom point on the post
 * @param foo            where a a vertical line from the post intersects
 * @param classiication  classification based upon post side
 * @return               a possible classification
 */
int ObjectFragments::getFrontlineClassification(point<int> post,
                                                pair<int, int> foo,
                                                int classification) {
    float linedist = realDistance(post.x, post.y, foo.first, foo.second);
    if (POSTLOGIC) {
        cout << "Goal box frontline distance was " << linedist << endl;
    }
    if (linedist > GOALBOX_OVERAGE + BOX_FUDGE) {
        return classification;
    }
    return NOPOST;
}


/* Another post classification method.	In this one we look left and right of
 * the post trying to find a really long run of the same color.	 If we find one
 * this is good evidence for the post we're looking for.  This will not work
 * if we are over to the side of the goal as the two posts will be too close
 * together.
 *
 * may correspond to the matching post of this color.
 * @param left		 leftmost value (x dim) of the post
 * @param right		 rightmost value
 * @param height	 the height of the post
 * @return			 potential classification
 */

int ObjectFragments::classifyByOtherRuns(int left, int right, int height)
{
    const int HORIZON_TOLERANCE = 10;	   // our other post should be near horizon
    const int MIN_OTHER_THRESHOLD = 20;  // how big does it have to be?

    int largel = 0;
    int larger = 0;
	int indexr = 0;
	int indexl = 0;
    int mind = min(100, height / 2 + (right - left) / 2);
    for (int i = 0; i < numberOfRuns; i++) {
        int nextX = runs[i].x;
        int nextY = runs[i].y;
        int nextH = runs[i].h;
        int horX = horizonAt(nextX);
        // meanwhile collect some information on which post we're looking at
        if (nextH > MIN_GOAL_HEIGHT && nextY < horX &&
            nextX > 5 && nextX < IMAGE_WIDTH - 5 &&
            nextY + nextH > horX - HORIZON_TOLERANCE) {
            if (nextX < left - mind) {
                if (nextH > largel) {
                    largel = nextH;
					indexl = i;
                }
            } else if (nextX > right + mind) {
                if (nextH > larger) {
                    larger = nextH;
					indexr = i;
                }
            }
        }
    }
    if ((larger > height / 2 || larger > MIN_OTHER_THRESHOLD) && larger >
        largel) {
		// watch out for tiny swatches
		int count = 0;
		for (int i = indexr + 1; i < numberOfRuns && runs[i].x - runs[i-1].x < 3;
			 i++) {
			count++;
		}
		for (int i = indexr - 1; i >= 0 && runs[i+1].x - runs[i].x < 3; i++) {
			count++;
		}
		if (count > 4) {
			if (POSTLOGIC) {
				cout << "Larger " << left << " " << right << " " << larger <<
					" " << count << endl;
			}
			return LEFT;
		}
    } else if (largel > MIN_OTHER_THRESHOLD || largel > height / 2) {
		int count = 0;
		for (int i = indexl + 1; i < numberOfRuns && runs[i].x - runs[i-1].x < 3;
			 i++) {
			count++;
		}
		for (int i = indexl - 1; i >= 0 && runs[i+1].x - runs[i].x < 3; i++) {
			count++;
		}
		if (count > 4) {
			if (POSTLOGIC) {
				cout << "Largel " << left << " " << right << " " << largel <<
					" " << count << endl;
			}
			return RIGHT;
		}
    }
    if (POSTLOGIC) {
        cout << "Large R " << larger << " " << largel << " " << endl;
    }
    return NOPOST;
}


/* Main routine for classifying posts.	We have a variety of methods to
 * classify posts in our tool box.	The idea is to start with the best ones and
 * keep trying until one produces an answer.
 *
 * @param c				 color of the post
 * @param c2			 secondary color
 * @param beaconFound	   did we find a beacon in this image?
 * @param left			 field object to send out if we find a left post
 * @param right			 ditto for right post
 * @param mid			 ditto for backstop
 * @return				 classification
 */

int ObjectFragments::classifyFirstPost(unsigned char c, Blob pole)
{
    const int MIN_BLOB_SIZE = 10;

    int horizonLeft = horizonAt(pole.getLeft());
    int fakeBottom = max(pole.getBottom(), horizonLeft);

    // start the process of figuring out which post we've got - fortunately
    // with the Naos it is easier

    // Our first test is whether we see a big blob of the same color
    // somewhere else
    int post = classifyByOtherRuns(pole.getLeft(), pole.getRight(),
                                   fakeBottom - pole.getTop());
    if (post != NOPOST) {
        if (POSTLOGIC) {
            cout << "Found from checkOther" << endl;
        }
        return post;
    }

    post = classifyByCrossbar(pole);		  // look for the crossbar
    if (post != NOPOST) {
        if (POSTLOGIC) {
            cout << "Found crossbar " << post << endl;
        }
        return post;
    }

    //  Corners are an excellent way to ID posts
    int tCorners = context->getTCorner();
    int lCorners = context->getLCorner();
    if (POSTLOGIC) {
        cout << "Corners: " << tCorners << " " << lCorners << endl;
    }

    if (tCorners > 0) {
        post = classifyByTCorner(pole);
        if (post != NOPOST) {
            if (POSTLOGIC) {
                cout << "Found from T Intersection" << endl;
            }
            return post;
        }
    }

    if (lCorners > 0) {
        post = classifyByCheckingCorners(pole);
        if (post != NOPOST) {
            if (POSTLOGIC) {
                cout << "Found from Corners" << endl;
            }
            return post;
        }
    }
    /*
    post = classifyByCheckingLines(pole);
    if (post != NOPOST) {
        if (POSTLOGIC) {
            cout << "Found from lines" << endl;
        }
        return post;
        }*/

    return post;
}

/* Checks is a perspective goal post has reasonable size and shape parameters.
 * @param post    the post to check
 * @return        whether it passed the tests
 */
bool ObjectFragments::isPostReasonableSizeShapeAndPlace(Blob post) {
    // before proclaiming this a post, let's make sure its boundaries
    // are in reasonable places
    int horizonLeft = horizonAt(post.getLeft());
    int fakeBottom = max(post.getBottom(), horizonLeft);
    // do some sanity checking - this one makes sure the blob is ok
    if (!locationOk(post)) {
        if (POSTDEBUG)
            cout << "Bad location on post" << endl;
        return false;
    }
    // make sure we have some size to our post
    if (post.height() == 0) {
        return false;
    }

    if (POSTDEBUG) {
        drawBlob(post, BLACK);
    }
    // make sure that the ratio of height to width is reasonable
    float rat = (float)(post.width()) / (float)(post.height());
    if (!postRatiosOk(rat) && post.height() < IMAGE_HEIGHT / 2 &&
        post.getTop() > 0) {
        return false;
    }

    return true;

}

/* Resets the run data after we've identified our first post.  Clears out the
 * runs that are associated with the first post, or are close to it.  Makes
 * room to find the next post.
 * @param pole     the post we identified
 * @param post     its identification - which allows us to clear other stuff
 */
void ObjectFragments::updateRunsAfterFirstPost(Blob pole, int post) {
    const int NEAR_DISTANCE = 10;
    int nextX = 0, nextH = 0;
    int trueLeft = pole.getLeft();
    int trueRight = pole.getRight();
    // first get rid of all the color that corresponds to this post
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
}

/* Look for goal posts.
 * The basic idea is pretty simple.  We start by looking for the
 * biggest run of color we can find.
 * We then build a rectangular blob out of that color.	From there we need to
 * determine if the rectangular blob
 * is a post, and if so, then which post it is.	 If that all goes well we look
 * to see if there is a second post,
 * and potentially a backstop.
 * @param left		  the left goal post
 * @param right		  the right post
 * @param mid		  the backstop
 * @param c			  the color we're processing
 * @param c2		  the soft color closest to it (e.g. bluegreen for blue)
 */
// Look for posts and goals given the runs we've collected
void ObjectFragments::lookForFirstPost(VisualFieldObject* left,
							   VisualFieldObject* right,
							   VisualCrossbar* mid, unsigned char c)
{
    // if we don't have any runs there is nothing to do
    if (numberOfRuns <= 1) {
        return;
    }
    distanceCertainty dc = BOTH_UNSURE;
    Blob pole;
	float saveSlope = slope;
    int isItAPost = grabPost(c, IMAGE_WIDTH - 3, 2, pole);
	// restore slope for 2d post
	slope = saveSlope;
    // make sure we're looking at something big enough to be a post
    if (isItAPost == NOPOST) {
        return;
    } else if (POSTDEBUG) {
        cout << "We have a good candidate" << endl;
    }
    if (!isPostReasonableSizeShapeAndPlace(pole)) {
        if (POSTDEBUG) {
            cout << "Post unreasonable size or shape" << endl;
        }
        return;
    }

    dc = checkDist(pole);
    // first characterize the size of the possible post
    int howbig = characterizeSize(pole);
    // now see if we can figure out whether it is a right or left post
    int post = classifyFirstPost(c, pole);
    //setContext(post);
    // based on those results update the proper data structure
    if (post == LEFT) {
        updateObject(right, pole, _SURE, dc);
    } else if (post == RIGHT) {
        updateObject(left, pole, _SURE, dc);
    } else {
        // if we have a big post and no idea what it is, then stop
        // we'll mark it as uncertain for the localization system
        updateObject(right, pole, NOT_SURE, dc);
        if (POSTLOGIC) {
            cout << "Post not classified" << endl;
        }
        return;
    }
	if (PRINTOBJS) {
		printBlob(pole);
	}
    lookForSecondPost(pole, post, left, right, mid, c);
}

/* We have found one good post, here we look for a second.  The procedure
 * is similar to the methodology for the first post except that we
 * don't have to worry about classification, and we have some extra
 * sanity checks having to do with the posts relative to each other.
 * @param pole        the first post
 * @param post        the first post's classification
 * @param left		  the left goal post
 * @param right		  the right post
 * @param mid		  the backstop
 * @param c			  the color we're processing
 * @param c2		  the soft color closest to it (e.g. bluegreen for blue)
 */
void ObjectFragments::lookForSecondPost(Blob pole, int post,
                                        VisualFieldObject* left,
                                        VisualFieldObject* right,
                                        VisualCrossbar* mid, unsigned char c) {
    const int POST_NEAR_DIST = 5;
    // at this point we have a post and it is normally classified
    // if we feel pretty good about this, then prepare for the next post
    updateRunsAfterFirstPost(pole, post);
    // find the other post if possible - the process is basically identical to
    // the first post
    distanceCertainty dc = checkDist(pole);
    int howbig = characterizeSize(pole);
    bool questions = howbig == SMALL;
    Blob secondPost;
    // ready to grab the potential post
    bool isItAPost = grabPost(c, pole.getLeft() - POST_NEAR_DIST,
                         pole.getRight() + POST_NEAR_DIST, secondPost);
    if (isItAPost == NOPOST || !isPostReasonableSizeShapeAndPlace(secondPost)) {
        // we didn't get one
        // before returning make sure we don't need to screen the previous post
        if (questions) {
            if (post == LEFT) {
                if (right->getIDCertainty() != _SURE) {
                    right->init();
                }
            } else {
                if (left->getIDCertainty() != _SURE) {
                    left->init();
                }
            }
        }
        return;
    } else {
        // we managed to grab a second post - lets do more sanity checks
        dc = checkDist(secondPost);
        float rat = (float)secondPost.width() / (float)secondPost.height();
        bool ratOk = postRatiosOk(rat) || (!greenCheck(secondPost) &&
                                           rat < SQUATRAT);
        bool goodSecondPost = checkSize(secondPost, c);
        // extra sanity checks: post must be reasonably placed with regard
        // to the field, and must not be too close to the other post
        // also its size relative to the other post must be ok
        if (ratOk && goodSecondPost && secondPostFarEnough(pole, secondPost,
                                                           post) &&
            relativeSizesOk(pole, secondPost)) {
            if (post == LEFT) {
                updateObject(left, secondPost, _SURE, dc);
                // make sure the certainty was set on the other post
                right->setIDCertainty(_SURE);
            } else {
                updateObject(right, secondPost, _SURE, dc);
                left->setIDCertainty(_SURE);
            }
        } else {
            // we failed at least one sanity check
            if (SANITY) {
                drawBlob(secondPost, ORANGE);
            }
        }
    }
}

/* We misidentified the first post.	 Now that we've figured that out we need to
 *	switch it to the correct post.	Just transfer the information and reinit the
 *	previously IDd post.
 * @param p1	the correct post
 * @param p2	the wrong one
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

/*	When we're looking for posts it is helpful if they are surrounded by green.
 * The best place to look is underneath.  So let's do that.
 * @param b	   the potential post
 * @return	   did we find some green?
 */
bool ObjectFragments::greenCheck(Blob b)
{
    const int ERROR_TOLERANCE = 5;
    const int EXTRA_LINES = 10;
    const int MAX_BAD_PIXELS = 4;

    // if the bottom of the post is at the bottom of the screen default to true
    if (b.getRightBottomY() >= IMAGE_HEIGHT - 1 || b.getLeftBottomY() >=
        IMAGE_HEIGHT-1) {
        return true;
    }
    // for huge posts default to true
    if (b.width() > IMAGE_WIDTH / 2) {
        return true;
    }
    int w = b.width();
    int y = 0;
    int x = b.getLeftBottomX();
    stop scan;
    // do the actual scanning under the blob
    for (int i = 0; i < w; i+= 2) {
        y = yProject(x, b.getLeftBottomY(), x + i);
        vertScan(x + i, y, 1, ERROR_TOLERANCE, GREEN, scan);
        if (scan.good > 1) {
            return true;
        }
    }
    // try one more in case its a white line
    int bad = 0;
    for (int i = 0; i < EXTRA_LINES && bad < MAX_BAD_PIXELS; i++) {
        x = max(0, xProject(x, b.getLeftBottomY(), b.getLeftBottomY() + i));
        unsigned char pix = thresh->getThresholded(min(IMAGE_HEIGHT - 1,
                                             b.getLeftBottomY() + i),x);
        if (Utility::colorsEqual(pix, GREEN)) {
            return true;
        }
        if (!Utility::colorsEqual(pix, WHITE)) {
            bad++;
        }
    }
    return false;
}

/* Checks out how much of the blob is of the right color.
 * If it is enough returns true, if not false.
 * @param tempobj	  the blob we're checking (usually a post)
 * @param minpercent  how good it needs to be
 * @return			  was it good enough?
 */
bool ObjectFragments::rightBlobColor(Blob tempobj, float minpercent) {
    int x = tempobj.getLeftTopX();
    int y = tempobj.getLeftTopY();
    int spanX = tempobj.width();
    int spanY = tempobj.height();
    int goal = static_cast<int>(static_cast<float>(spanX * spanY) * minpercent);
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
                if (Utility::colorsEqual(thresh->getExpandedColor(nx, ny, color),
										 color)) {
                    good++;
                    if (good > goal) {
                        return true;
                    }
                }
            }
        }
    }
    float percent = (float)good / (float) (total);
    cout << "Color check " << percent << " " << minpercent << endl;
    if (percent > minpercent) {
        return true;
    }
    if (POSTDEBUG) {
        cout << "Percentages " << percent << " " << minpercent << endl;
    }
    return false;
}

/* Checks if a potential post meets our size requirements.
 * @param b		the post
 * @return		true if its big enough, false otherwise
 */
bool ObjectFragments::postBigEnough(Blob b) {
    if (b.getLeftTopX() == BADVALUE) {
        return false;
    }
    if (b.width() <MIN_GOAL_WIDTH) {
        return false;
    }
    if (b.height() < MIN_GOAL_HEIGHT) {
		// before tossing it, check for occlusion
		int gap = horizonAt(b.getLeft()) - b.getBottom();
		if (b.height() + gap > MIN_GOAL_HEIGHT) {
			return true;
		}
        if (b.getTop() > 5) {
            return false;
        }
        if (color == BLUE_BIT && b.width() < 20) {
            return false;
		}
		if (b.height() < MIN_GOAL_HEIGHT / 2) {
			return false;
		}
    }
    return true;
}

/** Compare the pixEstimated distance with the distance we get from
 *	widths and/or height if possible.  If they are off by too much
 *	then punt this object
 */

bool ObjectFragments::badDistance(Blob b) {
    if (b.height() < MIN_GOAL_HEIGHT + 25 ||
		(vision->pose->getHorizonY(0) < 0 && color == BLUE_BIT)) {
        int x = b.getLeftBottomX();
        int y = b.getLeftBottomY();
        int bottom = b.getBottom();
        estimate e = vision->pose->pixEstimate(x, y, 0.0);
        distanceCertainty dc = checkDist(b);
        float disth = thresh->getGoalPostDistFromHeight(static_cast<float>
                                                        (b.height()));
        float distw = thresh->getGoalPostDistFromWidth(static_cast<float>
                                                       (b.width()));
        float choose = 0.0f;
        if (b.getTop() < 5 || y > IMAGE_HEIGHT - 5) {
            if (x < 5 || b.getRight() > IMAGE_WIDTH - 5) {
                choose = min(disth, distw);
            } else {
                choose = distw;
            }
        } else if (x < 5 || b.getRight() > IMAGE_WIDTH - 5) {
            choose = distw;
        } else {
            choose = disth;
        }

        float diste = e.dist;
        if (diste > 0.0f && (choose * 2 < diste || diste * 2 < choose) &&
			choose > 150.0f) {
            if (POSTDEBUG) {
                cout << "Throwing out post.	 Distance estimate is " << e.dist
                     << endl;
                cout << "Dist from height width " << disth << " " << distw
                     << endl;
                cout << "Post at " << x << " " << y << endl;
            }
            return true;
        }
		if (vision->pose->getHorizonY(0) < -100 && color == BLUE_BIT &&
			(choose > 200.0f || choose > 2 * diste)) {
			if (POSTDEBUG) {
				cout << "Throwing away questionable blue post" <<
					choose << " " << diste << " " <<
					vision->pose->getHorizonY(0) << endl;
			}
			return true;
		}
		if (b.getTop() > IMAGE_HEIGHT / 3 && diste < 100.0f) {
			if (POSTDEBUG) {
				cout << "Close post, but in bottom of image " << diste << endl;
			}
			return true;
		}
		if (POSTDEBUG) {
			cout << "Distance estimates pix first: " << diste << " " <<
				choose << endl;
		}

    }
    return false;
}

/* Combines several sanity checks into one.	 Checks that the bottom of the
 * object is ok and the top too.
 * Also, just makes sure that the object is in fact an object.
 * @param b		   the potential post
 * @return		   true if it is reasonably located, false otherwise
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
    int trueLeft = b.getLeft();		// leftmost value
    int trueRight = b.getRight();
    int trueTop = b.getTop();
    int trueBottom = b.getBottom();
    int horizonLeft = horizonAt(b.getLeftBottomX());
    int horizonRight = horizonAt(b.getRightBottomX());
    int spanX = b.width();
    int spanY = b.height();
    int mh = min(horizonLeft, horizonRight);
    // file this one under "very specific sanity checks"
    if (color == BLUE_BIT && spanY < TALL_POST && trueTop > IMAGE_HEIGHT / 2) {
        if (SANITY) {
            cout << "Screening blue post that is uniform-like" << endl;
        }
        return false;
    }
	if (trueTop > mh && trueTop > 3) {
		if (SANITY) {
			cout << "Top was less than horizon " << trueTop << " " << mh << endl;
		}
		return false;
	}
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
					drawBlob(b, RED);
                }
                return false;
            }
        } else {
        }
    }
    return true;
}

/* Objects need to be at or below the horizon.	We get the basic shape of the
 * object and either the horizon
 * or the projected horizon.
 * @param spanX		 how wide the post is
 * @param spanY		 how tall it is
 * @param minHeight	 the normal minimum value for where the bottom could be
 * @param left		 the leftmost x value of the object
 * @param right		 the rightmost x value of the object
 * @param bottom	 the biggest y value of the object
 * @param top		 the smallest y value of the object
 * @return			 true if it seems reasonable, false otherwise
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
    if (spanY > TALL_POST) {
        return true;
    }
    if (color == BLUE_BIT) {
        if (bottom + BOTTOM_FUDGE_FACTOR < minHeight) {
            if (SANITY)
                cout << "Removed risky blue post" << endl;
            return false;
        }
    }
    if (bottom + BOTTOM_FUDGE_FACTOR + 5 * spanX < minHeight) {
        if (SANITY) {
            cout << "Bad height " << bottom << " " << spanX <<
				" " << minHeight << endl;
        }
        return false;
    }
    // when we're at the edges of the image make the rules a bit more stringent
    if (bottom + BOTTOM_FUDGE_FACTOR < minHeight &&
        (left < IMAGE_EDGE || right > IMAGE_WIDTH - IMAGE_EDGE ||
         top < MAX_Y_VALUE)
        && (spanY < MIN_WIDTH)) {
        if (SANITY)
            cout << "Bad Edge Information" << endl;
        return false;
    }
    return true;
}


/*	Posts shouldn't show up too close to each other (yes, I realize they can be
 * when you're looking from the side).	Make sure there is some separation.
 *
 * @param l1	  left x of one post
 * @param r1	  right x of the same post
 * @param l2	  left x of the other post
 * @param r2	  right x of the other post
 * @return		  true when there is enough separation.
 */
bool ObjectFragments::secondPostFarEnough(Blob post1, Blob post2, int post) {

    point <int> left1 = post1.getLeftBottom();
    point <int> right1 = post1.getRightBottom();
    point <int> left2 = post2.getLeftBottom();
    point <int> right2 = post2.getRightBottom();
    if (SANITY) {
        cout << "Separations " << (dist(left1.x, left1.y, right2.x, right2.y))
             << " " << (dist(left2.x, left2.y, right1.x, right1.y)) << endl;
    }
    // cout << left1.x << " " << left2.x << " " << right1.x << " "
    //	  << right2.x << endl;
    if ((post == RIGHT && right2.x > left1.x) ||
        (post == LEFT && left2.x < right1.x)) {
        if (SANITY) {
            cout << "Second post is on the wrong side!" << endl;
        }
        return false;
    }
	if ((right2.x >= left1.x - 2 && right2.x <= right1.x + 2) ||
		(right1.x >= left2.x - 2 && right1.x <= right2.x + 2)) {
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
 * @param b	   the blob we worked on.
 * @return	   true when the processing worked, false otherwise
 */
bool ObjectFragments::blobOk(Blob b) {
    if (b.getLeftTopX() > BADVALUE && b.getLeftBottomX() > BADVALUE)
        return true;
    return false;
}

bool ObjectFragments::withinMargin(float n, float n2, float margin) {
    if (n > n2 - margin && n < n2 + margin) {
        return true;
    }
    return false;
}

bool ObjectFragments::withinMarginInt(int n, int n2, int margin) {
    if (n > n2 - margin && n < n2 + margin) {
        return true;
    }
    return false;
}

/*	When we have two candidate posts we don't want one to be huge and the other
 * tiny.  So we need to make
 * sure that the size ratios are within reason. Now that we have decent pixEstimates
 * we can do it that way.
 * @param post1    the original post
 * @param post2    the potential new one
 * @return		   are the ratios reasonable?
 */

bool ObjectFragments::relativeSizesOk(Blob post1, Blob post2) {
    const float fudge = 40.0f;
    int x1 = post1.getMidBottomX();
    int y1 = post1.getMidBottomY();
    int x2 = post2.getMidBottomX();
    int y2 = post2.getMidBottomY();
    // if posts are the same basic shape and size, let's just be done with it
    if (withinMarginInt(post1.height(), post2.height(), post1.height() / 3) &&
        withinMarginInt(post1.width(), post2.width(), min(post1.width(),
                                                          post2.width()))) {
        return true;
    }
    if (!withinVerticalEdgeMargin(post1.getBottom(), 3) &&
        !withinVerticalEdgeMargin(post2.getBottom(), 3)) {
        // both posts are in full view - just check the distance between them
        float dist = realDistance(x1, y1, x2, y2);
        if (withinMargin(dist, CROSSBAR_CM_WIDTH, 2 * fudge)) {
            return true;
        }
        // if we're far away our pixEstimate may be off
        estimate e = vision->pose->pixEstimate(x1, y1, 0.0);
        estimate e1 = vision->pose->pixEstimate(x2, y2, 0.0);
        if (e.dist > 400.0f || e1.dist > 400.0f) {
            if (withinMargin(dist, CROSSBAR_CM_WIDTH, fudge * 3)) {
                return true;
            }
        }
		if (abs(e.dist - e1.dist) < 100.0f && abs(x1 -x2) > IMAGE_WIDTH / 4 &&
			dist < CROSSBAR_CM_WIDTH + fudge * 3) {
			return true;
		}
        if (SANITY) {
            cout << "Failed relative size check 1 dist was " << dist <<
                " " << x1 << " " << y1 << " " << x2 << " " << y2 <<
                " " << e.dist << " " << e1.dist << endl;
        }
        return false;
    }
    // one post is very close, the other should be relatively close too
    if (withinEdgeMargin(post1.getBottom(), 2)) {
        estimate e1 = vision->pose->pixEstimate(x2, y2, 0.0);
        if (withinMargin(e1.dist, CROSSBAR_CM_WIDTH, fudge * 3)) {
            return true;
        }
    } else {
        estimate e1 = vision->pose->pixEstimate(x1, y1, 0.0);
        if (withinMargin(e1.dist, CROSSBAR_CM_WIDTH, fudge * 3)) {
            return true;
        }
    }
    if (SANITY) {
        cout << "Failed relative size check" << endl;
    }
    return false;
}

/*	Is the ratio of width to height ok for the second post?
 * We use a different criteria here than for
 * the first post because we have lots of other ways to verify if this is a good
 * post.
 * @param ratio		the height/width ratio
 * @return			is it a legal value?
 */
bool ObjectFragments::postRatiosOk(float ratio) {
    return ratio < GOODRAT;
}


/* Misc. routines
 */


/* Calculate the horizontal distance between two objects
 * (the end of one to the start of the other).
 * @param x1	left x of one object
 * @param x2	right x of the object
 * @param x3	left x of the other
 * @param x4	right x of the other
 * @return		the distance between the objects in the x dimension
 */
int ObjectFragments::distance(int x1, int x2, int x3, int x4) {
    if (x2 < x3) {
        return x3 - x2;
    }
    if (x1 > x4) {
        return x1 - x4;
    }
    return 0;
}

/*	Calculate the actual distance between two points.  Uses functions
	from NaoPose.cpp in Noggin
	@param x1	x coord of object 1
	@param y1	y coord of object 1
	@param x2	x coord of object 2
	@param y2	y coord of object 2
	@return		the distance in centimeters
 */

float ObjectFragments::realDistance(int x1, int y1, int x2, int y2) {
	estimate r = vision->pose->pixEstimate(x1, y1, 0.0);
	estimate l = vision->pose->pixEstimate(x2, y2, 0.0);
	return vision->pose->getDistanceBetweenTwoObjects(l, r);
}

/*
 * The next group of functions are for debugging only.	They are set up so that
 * debugging information will only appear when processing is done off-line.
 */

/*	Print debugging information for a field object.
 * @param objs	   the object in question
 */
void ObjectFragments::printObject(VisualFieldObject * objs) {
#if defined OFFLINE
    cout << objs->getLeftTopX() << " " << objs->getLeftTopY() << " "
         << objs->getRightTopX() << " " << objs->getRightTopY() << endl;
    cout << objs->getLeftBottomX() << " " << objs->getLeftBottomY() << " "
         << objs->getRightBottomX() << " " << objs->getRightBottomY() << endl;
    cout << "Height is " << objs->getHeight() << " Width is "
         << objs->getWidth() << endl;
    if (objs->getIDCertainty() == _SURE) {
        cout << "Very sure" << endl;
    } else {
        cout << "Not sure" << endl;
    }
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
        if (vision->bglp->getWidth() >	0) {
            cout << "Vision found left blue post " << endl;
            printObject(vision->bglp);
        }
        if (vision->bgrp->getWidth() >	0) {
            cout << "Vision found right blue post " << endl;
            printObject(vision->bgrp);
        }
        if (vision->yglp->getWidth() >	0) {
            cout << "Vision found left yellow post " << endl;
            printObject(vision->yglp);
        }
        if (vision->ygrp->getWidth() >	0) {
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
 * @param b	   the blob
 */
void ObjectFragments::printBlob(Blob b) {
#if defined OFFLINE
    cout << "Outputting blob" << endl;
    cout << b.getLeftTopX() << " " << b.getLeftTopY() << " " << b.getRightTopX()
         << " "
         << b.getRightTopY() << endl;
    cout << b.getLeftBottomX() << " " << b.getLeftBottomY() << " " <<
        b.getRightBottomX()
         << " " << b.getRightBottomY() << endl;
#endif
}


/*	Draws the outline of a blob in the specified color.
 * @param b	   the blob
 * @param c	   the color to paint
 */
void ObjectFragments::drawBlob(Blob b, int c) {
#ifdef OFFLINE
    vision->drawLine(b.getLeftTopX(), b.getLeftTopY(),
                     b.getRightTopX(), b.getRightTopY(),
                     c);
    vision->drawLine(b.getLeftTopX(), b.getLeftTopY(),
                     b.getLeftBottomX(), b.getLeftBottomY(),
                     c);
    vision->drawLine(b.getLeftBottomX(), b.getLeftBottomY(),
                     b.getRightBottomX(), b.getRightBottomY(),
                     c);
    vision->drawLine(b.getRightTopX(), b.getRightTopY(),
                     b.getRightBottomX(), b.getRightBottomY(),
                     c);
#endif
}
