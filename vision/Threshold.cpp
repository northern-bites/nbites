
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


/**
 * This file and a related file (ObjectFragments.cc) do the bulk of the Vision
 * processing in a given frame. The algorithm used is (relatively)
 * straightforward - the visual signal comes in as a 2D array of YUV values.
 *
 * In principle the algorithm would work like this:
 *   1) convert each YUV value to a color (this is called thresholding)
 *   2) find groups of connected pixels of similar colors (run length encoding)
 *   3) extract objects from the blobs found
 *
 * We need to be faster and smarter than that so 2) kind of blends into 1)
 * and 3).
 * Our version of vision scans the image starting at the bottom left corner
 * and doing vertical slices up and across the image.  Each pixel is checked
 * against a color table to do a basic thresholding.  If there are multiple
 * pixels in a row of the same color they are collected together into a "run"
 * and sent to the object detection module.  There are variations of this basic
 * theme in accordance with the structure of the field and the importance of
 * the potential object (i.e. balls are extra important).
 *
 * Once the scan is complete then run length encoding is done on each color
 * object separately and potential field objects are extracted.
 *
 * These are then sanity checked based on things like where the horizon of the
 * field is, the structure of the field (i.e. posts can't be too close to
 * goals), etc.
 */

#include "ifdefs.h"
#include "Common.h"

#include <math.h>
#include <assert.h>
#if ROBOT(NAO_SIM)
#  include <aldefinitions.h>
#endif
#include <boost/shared_ptr.hpp>

#include "Threshold.h"
#include "debug.h"

using namespace std;
using boost::shared_ptr;
#define PRINT_VISION_INFO

// Constructor for Threshold class. passed an instance of Vision and Pose
Threshold::Threshold(Vision* vis, shared_ptr<NaoPose> posPtr)
    : inverted(false), vision(vis), pose(posPtr)
{

    // storing locally
#ifdef OFFLINE
    visualHorizonDebug = false;
	debugSelf = true;
#endif

    // loads the color table on the MS into memory
#if ROBOT(NAO_RL)
# if ! defined WEBOTS_BACKEND && ! defined OFFLINE
    initTable("/opt/naoqi/modules/etc/table.mtb");
# elif defined WEBOTS_BACKEND
    initTable(string(string(getenv("WEBOTS_HOME"))+
                  "/projects/contests/nao_robocup/" +
                     "controllers/nao_soccer_player_red/etc/table.mtb"));

# endif

#elif ROBOT(NAO_SIM)
#  error Compressed table not set up for simulator Nao yet
#else
#  error Undefined robot type
#endif // OFFLINE
    // Set up object recognition object pointers
	field = new Field(vision, this);
    blue = shared_ptr<ObjectFragments>(new ObjectFragments(vision, this,
                                                           field, BLUE));
    yellow = shared_ptr<ObjectFragments>(new ObjectFragments(vision, this,
                                                             field, YELLOW));
    navyblue = new Robots(vision, this, field, NAVY);
	red = new Robots(vision, this, field, RED);
    orange = new Ball(vision, this, field, ORANGE);
    cross = new Cross(vision, this, field);
    for (int i = 0; i < IMAGE_WIDTH; i++) {
      lowerBound[i] = IMAGE_HEIGHT - 1;
    }
}

/* Main vision loop, called by Vision.cc
 */
void Threshold::visionLoop() {

    // threshold image and create runs
    thresholdAndRuns();


    // do line recognition (in FieldLines.cc)
    // This will form all lines and all corners. After this call, fieldLines
    // will be able to supply information about them through getLines() and
    // getCorners().
	PROF_ENTER(vision->profiler, P_LINES);
    vision->fieldLines->lineLoop();
    PROF_EXIT(vision->profiler, P_LINES);
    // do recognition
    PROF_ENTER(vision->profiler, P_OBJECT);
    objectRecognition();
    PROF_EXIT(vision->profiler, P_OBJECT);

    vision->fieldLines->afterObjectFragments();
	// For now we don't set shooting information
    if (vision->bgCrossbar->getWidth() > 0) {
        //blue->setShot(vision->bgCrossbar);
    }
    if (vision->ygCrossbar->getWidth() > 0) {
        //yellow->setShot(vision->ygCrossbar);
    }
	// for now we also don't use open field information
    //field->openDirection(horizon, pose.get());

#ifdef OFFLINE
    if (visualHorizonDebug) {
        drawVisualHorizon();
    }
    transposeDebugImage();
#endif
}

/*
 * Threshold and runs.  The goal here is to scan the image and collect up "runs"
 * of color.
 * The ones we're particularly interested in are blue, yellow, orange and green
 * (also red and dark blue).
 * Then we send the runs off to the object recognition system.  We scan the
 * image from bottom to top. Along the way we keep track of things like: where
 * we saw blue-yellow and yellow-blue transitions, where the green horizon
 * line is, etc.
 */
void Threshold::thresholdAndRuns() {
    PROF_ENTER(vision->profiler, P_THRESHRUNS); // profiling

    // Perform image thresholding
    PROF_ENTER(vision->profiler, P_THRESHOLD);
    threshold();
    PROF_EXIT(vision->profiler, P_THRESHOLD);

	initColors();

    // Determine where the field horizon is
    PROF_ENTER(vision->profiler, P_FGHORIZON);
    horizon = field->findGreenHorizon(pose->getHorizonY(0),
									  pose->getHorizonSlope());
    PROF_EXIT(vision->profiler, P_FGHORIZON);

    // 'Run' up the image to find color-grouped pixel sequences
    PROF_ENTER(vision->profiler, P_RUNS);
    runs();
    PROF_EXIT(vision->profiler, P_RUNS);

    PROF_EXIT(vision->profiler, P_THRESHRUNS);
}


/* Thresholding.  Since there's no real benefit (and in fact can it can be a
 * detriment with compiler optimizations on) to combine the thresholding and
 * the runs loops, I (Jeremy) have split out the thresholding into it's own
 * method here.  This also allows for my incredibly complex unrolled loops for
 * each robot to be separated from other, clearer, code.
 */
void Threshold::threshold() {
    // My loop variables
    int m;
    unsigned char *tPtr, *tEnd, *tOff; // pointers into thresholded array
    const unsigned char *yPtr, *uPtr, *vPtr; // pointers into image array

    // My loop variable initializations
    yPtr = &yplane[0];
    uPtr = &uplane[0];
    vPtr = &vplane[0];

    tPtr = &thresholded[0][0];
    tEnd = &thresholded[IMAGE_HEIGHT-1][IMAGE_WIDTH-1] + 1;

#if ROBOT(NAO_SIM)
    m = (IMAGE_WIDTH * IMAGE_HEIGHT) % 8;

    // number of non-unrolled offset from beginning of row
    tOff = tPtr + m;

    // here is non-unrolled loop
    while (tPtr < tOff)
        *tPtr++ = bigTable[*yPtr++>>YSHIFT][*yPtr++>>USHIFT][*yPtr++>>VSHIFT];

    // here is the unrolled loop
    while (tPtr < tEnd) {
        // Eight unrolled table lookups
        *tPtr++ = bigTable[*yPtr++>>YSHIFT][*yPtr++>>USHIFT][*yPtr++>>VSHIFT];
        *tPtr++ = bigTable[*yPtr++>>YSHIFT][*yPtr++>>USHIFT][*yPtr++>>VSHIFT];
        *tPtr++ = bigTable[*yPtr++>>YSHIFT][*yPtr++>>USHIFT][*yPtr++>>VSHIFT];
        *tPtr++ = bigTable[*yPtr++>>YSHIFT][*yPtr++>>USHIFT][*yPtr++>>VSHIFT];
        *tPtr++ = bigTable[*yPtr++>>YSHIFT][*yPtr++>>USHIFT][*yPtr++>>VSHIFT];
        *tPtr++ = bigTable[*yPtr++>>YSHIFT][*yPtr++>>USHIFT][*yPtr++>>VSHIFT];
        *tPtr++ = bigTable[*yPtr++>>YSHIFT][*yPtr++>>USHIFT][*yPtr++>>VSHIFT];
        *tPtr++ = bigTable[*yPtr++>>YSHIFT][*yPtr++>>USHIFT][*yPtr++>>VSHIFT];
    }

#elif ROBOT(NAO_RL)
    m = (IMAGE_WIDTH * IMAGE_HEIGHT) % 8;

    // number of non-unrolled offset from beginning of row
    tOff = tPtr + m;

    // due to YUV422 data, we can only increment u & v every two assigments
    // thus, we need to do different stuff if we start off with even or odd
    // remainder.  However, we won't get odd # of pixels in 422 (not valid), so
    // lets ignore that

    // here is non-unrolled loop (unrolled by 2, actually)
    while (tPtr < tOff) {
        // we increment Y by 2 every time, and U and V by 4 every two times
        *tPtr++ = bigTable[*yPtr>>YSHIFT][*uPtr>>USHIFT][*vPtr>>VSHIFT];
        yPtr+=2;
        *tPtr++ = bigTable[*yPtr>>YSHIFT][*uPtr>>USHIFT][*vPtr>>VSHIFT];
        yPtr+=2; uPtr+=4; vPtr+=4;
    }

    // here is the unrolled loop
    while (tPtr < tEnd) {
        // Eight unrolled table lookups
        *tPtr++ = bigTable[*yPtr>>YSHIFT][*uPtr>>USHIFT][*vPtr>>VSHIFT];
        yPtr+=2;
        *tPtr++ = bigTable[*yPtr>>YSHIFT][*uPtr>>USHIFT][*vPtr>>VSHIFT];
        yPtr+=2; uPtr+=4; vPtr+=4;
        *tPtr++ = bigTable[*yPtr>>YSHIFT][*uPtr>>USHIFT][*vPtr>>VSHIFT];
        yPtr+=2;
        *tPtr++ = bigTable[*yPtr>>YSHIFT][*uPtr>>USHIFT][*vPtr>>VSHIFT];
        yPtr+=2; uPtr+=4; vPtr+=4;
        *tPtr++ = bigTable[*yPtr>>YSHIFT][*uPtr>>USHIFT][*vPtr>>VSHIFT];
        yPtr+=2;
        *tPtr++ = bigTable[*yPtr>>YSHIFT][*uPtr>>USHIFT][*vPtr>>VSHIFT];
        yPtr+=2; uPtr+=4; vPtr+=4;
        *tPtr++ = bigTable[*yPtr>>YSHIFT][*uPtr>>USHIFT][*vPtr>>VSHIFT];
        yPtr+=2;
        *tPtr++ = bigTable[*yPtr>>YSHIFT][*uPtr>>USHIFT][*vPtr>>VSHIFT];
        yPtr+=2; uPtr+=4; vPtr+=4;
    }

#endif // ROBOT(...)
}

/* Image runs.  As explained in the comments for the threshold() method, I
 * (Jeremy) have split the thresholdAndRuns() method into parts.  This also
 * helped with working out the slow sections of code.
 * We are now so far from pure run-length encoding that we do this a whole lot
 * differently.  First, we figure out the top and bottom borders of the field.
 * We get a convex hull for the top, and look out for our own body parts for the
 * bottom.  The we scan a bit more intelligently for field objects (e.g.
 * balls will only be in the confines of the field).
 */
void Threshold::runs() {
  //detectSelf();
    // split up the loops
    for (int i = 0; i < IMAGE_WIDTH; i += 1) {
		int topEdge = max(0, field->horizonAt(i));
		findBallsCrosses(i, topEdge);
		findGoals(i, topEdge);
    }

}

/** Ideally goals will be either right at the field edge, or will have part above
 * and part below.  So we scan up from the edge and also scan down.  All we're doing
 * is trying to collect big runs of either BLUE or YELLOW.  We tolerate some noise.
 * To Do:  Figure out the right amount of noise to tolerate.
 * @param column     the current vertical scanline
 * @param topEdge    the top of the field in that scanline
 */

void Threshold::findGoals(int column, int topEdge) {
	const int BADSIZE = 5;
	// scan up for goals
	int bad = 0, blues = 0, yellows = 0, blueGreen = 0;
	int firstBlue = topEdge, firstYellow = topEdge, lastBlue = topEdge, lastYellow = topEdge;
	topEdge = min(topEdge, lowerBound[column]);
	for (int j = topEdge; bad < BADSIZE && j >= 0; j--) {
		// get the next pixel
		unsigned char pixel = thresholded[j][column];
		// otherwise, do stuff according to color
		switch (pixel) {
		case BLUE:
			lastBlue = j;
			blues++;
			break;
		case YELLOW:
			lastYellow = j;
			yellows++;
			break;
		case BLUEGREEN:
			blueGreen++;
			break;
		default:
			bad++;
		}
	}
	// now do the same going down from the horizon
	bad = 0;
	for (int j = topEdge + 1; bad < BADSIZE && j < lowerBound[column]; j++) {
		// get the next pixel
		unsigned char pixel = thresholded[j][column];
		// otherwise, do stuff according to color
		switch (pixel) {
		case BLUE:
			firstBlue = j;
			blues++;
			break;
		case YELLOW:
			firstYellow = j;
			yellows++;
			break;
		case BLUEGREEN:
			blueGreen++;
			break;
		case GREEN:
			bad += 2;
			break;
		default:
			bad++;
		}
	}
	if (blues > 10) {
		blue->newRun(column, lastBlue, firstBlue - lastBlue);
	} else if (yellows > 10) {
		yellow->newRun(column, lastYellow, firstYellow - lastYellow);
	}
}

/* Balls and field crosses can only be on the actual field.  So we scan
 * down from the top edge to the bottom of the image (or where we would
 * see ourselves).  During the scan we collect up connected runs of
 * ORANGE or WHITE.  We pass them to the relevant Object structures to
 * be processed there.
 * To Do:  Put robot detection back in.
 * @param column     the current vertical scanline
 * @param topEdge    the top of the field in that scanline
 */

void Threshold::findBallsCrosses(int column, int topEdge) {
	// scan down finding balls and crosses
	unsigned char lastPixel = GREEN;
	int currentRun = 0;
	int bound = lowerBound[column];
	// if a ball is in the middle of the boundary, then look a little lower
	if (bound < IMAGE_HEIGHT - 1) {
		while (bound < IMAGE_HEIGHT && thresholded[bound][column] == ORANGE) {
			bound++;
		}
	}
	// scan down the column looking for ORANGE and WHITE
	for (int j = bound; j >= topEdge; j--) {
		// get the next pixel
		unsigned char pixel = thresholded[j][column];
		// for simplicity treat ORANGERED as ORANGE - we'll look
		// more carefully when we check whether or not it is a ball
		if (pixel == ORANGERED) {
			pixel = ORANGE;
		}

		// check thresholded point with last thresholded point.
		// if the same, increment the current run
		if (lastPixel == pixel) {
			currentRun++;
		}
		// otherwise, do stuff according to color
		if (lastPixel != pixel || j == topEdge) { // end of column
			// switch for last thresholded pixel
			switch (lastPixel) {
			case ORANGE:
				// add to Ball data structure
				if (currentRun > 2) {
					orange->newRun(column, j, currentRun);
				}
				break;
			case WHITE:
				// add to the cross data structure
				if (currentRun > 2) {
					cross->newRun(column, j, currentRun);
				}
				break;
			}
			// since this loop runs when a run ends, restart # pixels in run counter
			currentRun = 1;
		}
		lastPixel = pixel;
	}
}

/** Given two lines defined by "detectSelf" set the lower bounds.  We have
 * detected a part of ourself, so we don't want to process it or we might
 * mistake ourself for a cross or a ball.
 */

void Threshold::setBoundaryPoints(int x1, int y1, int x2, int y2, int x3, int y3) {
	float step = (float)(y1 - y2) / (float) (x2 - x1);
	float start = (float)y1;
	for (int i = x1; i < x2 && i < IMAGE_WIDTH; i++) {
		if (i >= 0 && start < IMAGE_HEIGHT) {
			int temp = max(0, (int)start);
#ifdef OFFLINE
			if (debugSelf) {
				drawPoint(i, temp, BLACK);
			}
#endif
			lowerBound[i] = temp;
		}
		start -= step;
	}
	step = (float)(y3 - y2) / (float) (x3 - x2);
	start = (float)y2;
	for (int i = x2; i < x3 && i < IMAGE_WIDTH; i++) {
		if (i >= 0 && start < IMAGE_HEIGHT) {
			int temp = max(0, (int)start);
			lowerBound[i] = temp;
#ifdef OFFLINE
			if (debugSelf) {
				drawPoint(i, temp, BLACK);
			}
#endif
		}
		start += step;
	}
}

/** Detect the shoulders of the robot and ignore them.  Our goal is to set lower
	bounds for all of the vertical scanlines.
	To Do:  Detect feet and hands.
*/
void Threshold::detectSelf() {
    // To do:  This boundary could be much better.
	// To do:  Detect feet and arms too
    const int pixInImageLeft = getPixelBoundaryLeft();
    const int pixInImageRight = getPixelBoundaryRight();
    const int pixInImageUp = getPixelBoundaryUp();
	const int MIDY = 40;
	const int MIDX = 90;
	const int LOWBOUND = -200;
	const int RIGHTBAL = 571;
	const int RIGHTFUDGE = 20;
	const int UPTRANSLATE = 85;
	const int HEIGHT = 200;
	const int TOPRIGHT = 250;
	const int HIGHBOUND = -50;
	const int COMPENSATION = 20;

	//cout << "Values " << pixInImageLeft << " " << pixInImageRight << " " << pixInImageUp << endl;

	for (int i = 0; i < IMAGE_WIDTH; i++) {
		lowerBound[i] = IMAGE_HEIGHT - 1;
	}
	int up = pixInImageUp;
	if (pixInImageUp < HIGHBOUND) {
		up -= COMPENSATION;
	} else if (pixInImageUp > -HIGHBOUND) {
		up += COMPENSATION;
	}
	if (pixInImageRight < 150) {
		up -= COMPENSATION;
	}
	if (pixInImageLeft > 170) {
		up -= COMPENSATION;
	}
	int xp = -1, yp = -1;
	if (pixInImageRight > 0 && pixInImageRight < IMAGE_WIDTH + RIGHTFUDGE) {
		xp = pixInImageRight - RIGHTFUDGE;
		yp = IMAGE_HEIGHT  + (up - UPTRANSLATE);
		setBoundaryPoints(xp, yp, xp + 100, yp - 130,
						  pixInImageRight + TOPRIGHT, yp - HEIGHT);
		if (pixInImageRight + TOPRIGHT < IMAGE_WIDTH -1) {
			for (int i = pixInImageRight + TOPRIGHT; i < IMAGE_WIDTH; i++) {
				lowerBound[i] = max(0, yp - HEIGHT);
			}
		}
	}
	if (pixInImageLeft > 0 && pixInImageLeft < IMAGE_WIDTH) {
		xp = pixInImageLeft;
		yp = IMAGE_HEIGHT + (up - UPTRANSLATE);
		setBoundaryPoints(pixInImageLeft - TOPRIGHT, yp - HEIGHT,
						  xp - 100, yp - 130,
						  xp, yp);
		if (pixInImageLeft > TOPRIGHT) {
			for (int i = 0; i < pixInImageLeft - TOPRIGHT; i++) {
				lowerBound[i] = max(0, yp - HEIGHT);
			}
		}
	}

	// screen out the robot's belly - note this still does not cover feet
	if (pixInImageUp < LOWBOUND) {
		int offset = (pixInImageRight - RIGHTBAL) / 4;
		int lefty = IMAGE_HEIGHT + pixInImageUp - LOWBOUND - offset;
		int righty = IMAGE_HEIGHT + pixInImageUp - LOWBOUND + offset;
		setBoundaryPoints(0, lefty, IMAGE_WIDTH - 1, righty, -1, -1);
	}
}

int Threshold::greenEdgePoint(int x) {
	return greenEdge[x];
}

int Threshold::getRobotTop(int x, int c) {
	if (c == RED) {
		return redTops[x];
	}
	return navyTops[x];
}

int Threshold::getRobotBottom(int x, int c) {
	if (c == RED) {
		return redBottoms[x];
	}
	return navyBottoms[x];
}


/*  Makes the calls to the vision system to recognize objects.  Then performs some extra
 * sanity checks to make sure we don't have weird cases like 2 beacons.
 */

void Threshold::objectRecognition() {
#ifdef JOHO_DEBUG
    print("   Theshold::objectRecognition");
#endif
    // Chown-RLE
    initObjects();
    // now get the posts and goals
    yellow->createObject();
    blue->createObject();
	cross->createObject();
	/* Shut off for now
    red->robot(horizon);
    navyblue->robot(horizon); */

    bool ylp = vision->yglp->getWidth() > 0;
    bool yrp = vision->ygrp->getWidth() > 0;
    bool blp = vision->bglp->getWidth() > 0;
    bool brp = vision->bgrp->getWidth() > 0;

	// HACK:  This will have to do until we can properly model the shoulders
	// for blue posts that run down into our shoulder boundary, disallow
	// some based on the ratio of the height to how much is below the boundary
	if (brp) {
		int x = max(0, vision->bgrp->getLeftBottomX());
		int y = min(vision->bgrp->getLeftBottomY(), IMAGE_HEIGHT -1);
		int x1 = min(IMAGE_WIDTH - 1, vision->bgrp->getRightBottomX());
		int y1 = min(IMAGE_HEIGHT - 1, vision->bgrp->getRightBottomY());
		int h = (int)vision->bgrp->getHeight();
		if ((y  > lowerBound[x] && lowerBound[x] < IMAGE_HEIGHT - 1 &&
				y - lowerBound[x] > h /2)
			|| (y1 > lowerBound[x1] && lowerBound[x1] < IMAGE_HEIGHT - 1 &&
				y1 - lowerBound[x1] > h / 2)) {
			vision->bgrp->init();
			//cout << "Screen " << x << " " << y << " " << x1 << " " << y1 << " " << h << endl;
			brp = false;
		}
	}
	if (blp) {
		int x = max(0, vision->bglp->getLeftBottomX());
		int y = min(vision->bglp->getLeftBottomY(), IMAGE_HEIGHT -1);
		int x1 = min(IMAGE_WIDTH - 1, vision->bglp->getRightBottomX());
		int y1 = min(IMAGE_HEIGHT - 1, vision->bglp->getRightBottomY());
		int h = (int)vision->bglp->getHeight();
		if ((y  > lowerBound[x] && lowerBound[x] < IMAGE_HEIGHT - 1 &&
				y - lowerBound[x] > h / 2)
			|| (y1 > lowerBound[x1] && lowerBound[x1] < IMAGE_HEIGHT - 1 &&
				y1 - lowerBound[x1] > h / 2)) {
			vision->bglp->init();
			//cout << "Screen1 " << x << " " << y << endl;
			blp = false;
		}
	}


    if ((ylp || yrp) && (blp || brp)) {
		// we see one of each, so pick the biggest one
		float ylpw = vision->yglp->getWidth();
		float yrpw = vision->ygrp->getWidth();
		float blpw = vision->bglp->getWidth();
		float brpw = vision->bgrp->getWidth();
		if (ylpw > yrpw) {
			if (blpw > brpw) {
				if (ylpw > blpw) {
					vision->bglp->init();
					vision->bgrp->init();
				}
				else {
					vision->yglp->init();
					vision->ygrp->init();
				}
			} else {
				if (ylpw > brpw) {
					vision->bgrp->init();
					vision->bglp->init();
				}
				else {
					vision->yglp->init();
					vision->ygrp->init();
				}
			}
		} else {
			if (blpw > brpw) {
				if (yrpw > blpw) {
					vision->bglp->init();
					vision->bgrp->init();
				}
				else {
					vision->ygrp->init();
					vision->yglp->init();
				}
			} else {
				if (yrpw > brpw) {
					vision->bglp->init();
					vision->bgrp->init();
				}
				else {
					vision->yglp->init();
					vision->ygrp->init();
				}
			}
			//}
	    }
    }


    // throw blue goal objects through a filter to eliminate noise in corners
    //reset these bools, incase we changed them above
    ylp = vision->yglp->getWidth() > 0;
    yrp = vision->ygrp->getWidth() > 0;
    blp = vision->bglp->getWidth() > 0;
    brp = vision->bgrp->getWidth() > 0;

    // make sure we don't see a false crossbar when looking at the other goal
    if (ylp || yrp) {
        vision->bgCrossbar->init();
    }
    if (blp || brp) {
        vision->ygCrossbar->init();
    }

    if (horizon < IMAGE_HEIGHT)
        orange->createBall(horizon);
    else
		orange->createBall(pose->getHorizonY(0));

    if (ylp || yrp) {
        field->bestShot(vision->ygrp, vision->yglp, vision->ygCrossbar, YELLOW);
    }
    if (blp || brp) {
        field->bestShot(vision->bgrp, vision->bglp, vision->bgCrossbar, BLUE);
    }

    storeFieldObjects();

}

//right post

/*
  RLE Helper Methods
*/

/* Calculates and stores all landmark field objects info.
 */
void Threshold::storeFieldObjects() {

    setFieldObjectInfo(vision->yglp);
    setFieldObjectInfo(vision->ygrp);
    setFieldObjectInfo(vision->bglp);
    setFieldObjectInfo(vision->bgrp);
	setVisualCrossInfo(vision->cross);
    vision->ygCrossbar->setFocDist(0.0); // sometimes set to 1.0 for some reason
    vision->bgCrossbar->setFocDist(0.0); // sometimes set to 1.0 for some reason
    vision->ygCrossbar->setDistance(0.0); // sometimes set to 1.0 for some reason
    vision->bgCrossbar->setDistance(0.0); // sometimes set to 1.0 for some reason

#if ROBOT(NAO)
    setVisualRobotInfo(vision->red1);
    setVisualRobotInfo(vision->red2);
    setVisualRobotInfo(vision->navy1);
    setVisualRobotInfo(vision->navy2);
#endif

}

/* Figures out center x,y, angle x,y, and foc/body dists for field objects.
 * @param objPtr    the field object to study
 */
void Threshold::setFieldObjectInfo(VisualFieldObject *objPtr) {
    // if the object is on screen, basically
    if (objPtr->getHeight() > 0) {
        // set center x,y
        objPtr->setCenterX(objPtr->getX() + ROUND(objPtr->getWidth()/2));
        objPtr->setCenterY(objPtr->getY() + ROUND(objPtr->getHeight()/2));

        // find angle x/y (relative to camera)
        objPtr->setAngleX( static_cast<float>(HALF_IMAGE_WIDTH -
											  objPtr->getCenterX() ) /
						   MAX_BEARING_RAD );
        objPtr->setAngleY(static_cast<float>(HALF_IMAGE_HEIGHT -
											 objPtr->getCenterY() )/
                          MAX_ELEVATION_RAD);

        // if object is a goal post
        if (objPtr == vision->yglp ||
            objPtr == vision->ygrp ||
            objPtr == vision->bglp ||
            objPtr == vision->bgrp) {
            //print("we've got a post!");
            float dist = 0.0;
            float width = objPtr->getWidth(); float height = objPtr->getHeight();
            distanceCertainty cert = objPtr->getDistanceCertainty();
            float distw = getGoalPostDistFromWidth(width);
            float disth = getGoalPostDistFromHeight(height);

			const int bottomLeftX = objPtr->getLeftBottomX();
			const int bottomRightX = objPtr->getRightBottomX();
			const int bottomLeftY = objPtr->getLeftBottomY();
			const int bottomRightY = objPtr->getRightBottomY();

			const float bottomOfObjectX = static_cast<float>(bottomLeftX +
															 bottomRightX) * 0.5f;
			const float bottomOfObjectY = static_cast<float>(bottomLeftY
															 + bottomRightY) * 0.5f;

			const float poseDist = pose->pixEstimate(
				static_cast<int>(bottomOfObjectX),
				static_cast<int>(bottomOfObjectY), 0.0f).dist;

			dist = chooseGoalDistance(cert, disth, distw, poseDist,
									  static_cast<int>(bottomOfObjectY));
	    #if defined OFFLINE && defined PRINT_VISION_INFO

            //print("{%g,%g},", poseDist, dist);

            //print("goal post dist: %g %g %g\n", dist, distw, disth);
	    #endif

            // sanity check: throw ridiculous distance estimates out
            // constants in Threshold.h
            if (dist < POST_MIN_FOC_DIST ||
                dist > POST_MAX_FOC_DIST) {
                dist = 0.0;
            }
            objPtr->setDistance(dist);
        } else { // don't know what object it is
            //print("setFieldObjectInfo: unrecognized FieldObject");
            return;
        }

        // sets focal distance of the field object
        objPtr->setFocDist(objPtr->getDistance());
        // convert dist + angle estimates to body center
        estimate obj_est = pose->bodyEstimate(objPtr->getCenterX(),
                                              objPtr->getCenterY(),
                                              objPtr->getDistance());
        objPtr->setDistanceWithSD(obj_est.dist);
        objPtr->setBearingWithSD(obj_est.bearing);
        objPtr->setElevation(obj_est.elevation);
    }
    else {
        objPtr->setFocDist(0.0);

        objPtr->setDistanceWithSD(0.0);
        objPtr->setBearingWithSD(0.0);
        objPtr->setElevation(0.0);
    }
}
/** Choose the best way to measure goal distance based on our certainty about
 * the sizes of width and height.
 */

float Threshold::chooseGoalDistance(distanceCertainty cert, float disth,
									float distw, float poseDist, int bottom) {
	float dist = 0.0f;
	switch (cert) {
	case HEIGHT_UNSURE:
		dist = distw;
		if (distw < 200 && bottom < IMAGE_HEIGHT - 2) {
			dist = poseDist;
		}
		break;
	case WIDTH_UNSURE:
		dist = disth;
		break;
	case BOTH_UNSURE:
		// We choose the min distance here, since that means more pixels
		if (bottom < IMAGE_HEIGHT - 4)
			dist = min( poseDist, min(disth, distw));
		else
			dist = min( disth, distw);
		break;
	case BOTH_SURE:
		dist = disth;
		break;
	}
	return dist;
}

/* Figures out center x,y, angle x,y, and foc/body dists for field objects.
 * @param objPtr    the field object to study
 */
void Threshold::setVisualRobotInfo(VisualRobot *objPtr) {
    // if the object is on screen, basically
    if (objPtr->getHeight() > 0) {
        // set center x,y
        objPtr->setCenterX(objPtr->getX() + ROUND(objPtr->getWidth()/2));
        objPtr->setCenterY(objPtr->getY() + ROUND(objPtr->getHeight()/2));

        // find angle x/y (relative to camera)
        objPtr->setAngleX( static_cast<float>(HALF_IMAGE_WIDTH -
											  objPtr->getCenterX() ) /
						   MAX_BEARING_RAD );
        objPtr->setAngleY( static_cast<float>(HALF_IMAGE_HEIGHT -
											  objPtr->getCenterY() ) /
						   MAX_ELEVATION_RAD );

        // sets focal distance of the field object
        objPtr->setFocDist(objPtr->getDistance());
        // convert dist + angle estimates to body center
        estimate obj_est = pose->bodyEstimate(objPtr->getCenterX(),
                                              objPtr->getCenterY(),
                                              objPtr->getDistance());
        objPtr->setDistanceWithSD(obj_est.dist);
        objPtr->setBearingWithSD(obj_est.bearing);
        objPtr->setElevation(obj_est.elevation);
    } else {
        objPtr->setFocDist(0.0);
        objPtr->setDistanceWithSD(0.0);
        objPtr->setBearingWithSD(0.0);
        objPtr->setElevation(0.0);
    }
}

/* Figures out center x,y, angle x,y, and foc/body dists for field objects.
 * @param objPtr    the field object to study
 */
void Threshold::setVisualCrossInfo(VisualCross *objPtr) {
    // if the object is on screen, basically
    if (objPtr->getHeight() > 0) {
        // set center x,y
        objPtr->setCenterX(objPtr->getX() + ROUND(objPtr->getWidth()/2));
        objPtr->setCenterY(objPtr->getY() + ROUND(objPtr->getHeight()/2));

        // find angle x/y (relative to camera)
        objPtr->setAngleX( static_cast<float>(HALF_IMAGE_WIDTH -
											  objPtr->getCenterX() ) /
						   MAX_BEARING_RAD );
        objPtr->setAngleY( static_cast<float>(HALF_IMAGE_HEIGHT -
											  objPtr->getCenterY() ) /
						   MAX_ELEVATION_RAD );

        // convert dist + angle estimates to body center
        estimate obj_est = pose->pixEstimate(objPtr->getCenterX(),
                                             objPtr->getCenterY(),
                                             0.0);
        obj_est = pose->bodyEstimate(objPtr->getCenterX(),
                                     objPtr->getCenterY(),
                                     obj_est.dist);
		if (obj_est.dist > 1500.0f) { // pose problem which happens rarely
			objPtr->setFocDist(0.0);
			objPtr->setDistanceWithSD(0.0);
			objPtr->setBearingWithSD(0.0);
			objPtr->setElevation(0.0);
		} else {
			objPtr->setDistanceWithSD(obj_est.dist);
			objPtr->setBearingWithSD(obj_est.bearing);
			objPtr->setElevation(obj_est.elevation);
			// now let's see if we can id this guy
			// at this point we've sorted out all of the goal post info
			bool ylp = vision->yglp->getDistance() > 0.0f;
			bool yrp = vision->ygrp->getDistance() > 0.0f;
			bool blp = vision->bglp->getDistance() > 0.0f;
			bool brp = vision->bgrp->getDistance() > 0.0f;
			if (ylp || yrp) {
				float dist = 0.0f;
				// get the relevant distances
				if (ylp) {
					dist = vision->yglp->getDistance();
					if (yrp)
						dist = min(dist, vision->ygrp->getDistance());
				} else {
					dist = vision->ygrp->getDistance();
				}
				// compare the distances
				//cout << "Compare dist " << obj_est.dist << " " << dist << endl;
				if (fabs(obj_est.dist - dist) < 300.0) {
					objPtr->setID(YELLOW_GOAL_CROSS);
				} else
					objPtr->setID(BLUE_GOAL_CROSS);
			} else if (blp || brp) {
				float dist = 0.0f;
				if (blp) {
					dist = vision->bglp->getDistance();
					if (brp)
						dist = min(dist, vision->bgrp->getDistance());
				} else {
					dist = vision->bgrp->getDistance();
				}
				//cout << "Compare dist " << obj_est.dist << " " << dist << endl;
				if (fabs(obj_est.dist - dist) < 300.0)
					objPtr->setID(BLUE_GOAL_CROSS);
				else
					objPtr->setID(YELLOW_GOAL_CROSS);
			} else {
				objPtr->setID(ABSTRACT_CROSS);
			}
		}
    } else {
        objPtr->setFocDist(0.0);
        objPtr->setDistanceWithSD(0.0);
        objPtr->setBearingWithSD(0.0);
        objPtr->setElevation(0.0);
    }
}

/* Looks up goal post height in pixels to focal distance function.
 * @param height     the height of the post in pixels
 * @return           the distance to the post in centimeters
 */
float Threshold::getGoalPostDistFromHeight(float height) {
#if ROBOT(NAO_SIM)
    return 17826*pow((double) height,-1.0254);
#else
    // return pose->pixHeightToDistance(height, GOAL_POST_CM_HEIGHT);
    return 32880.0f/height - 11.8597f;
#endif
}

/* Looks up goal post width in pixels to focal distance function.
 * @param width     the width of the post
 * @return          the distance to the post
 */
float Threshold::getGoalPostDistFromWidth(float width) {
#if ROBOT(NAO_SIM)
    //floor distance, seems to be best for the width
    //camera dist - 2585.4*pow(width,-1.0678);//OLD return 100.0*13.0/width;
    return 2360.1*pow((double) width,-1.0516);
#else
    // return pose->pixWidthToDistance(width, GOAL_POST_CM_WIDTH);
    return 3116.59f/width + 21.75f;
#endif
}

/*
 * Sets all the object information back to zero.
 */
void Threshold::initObjects(void) {

    // yellow goal objs
    vision->ygrp->init();
    vision->yglp->init();
    vision->ygCrossbar->init();

    // blue goal objs
    vision->bgrp->init();
    vision->bglp->init();
    vision->bgCrossbar->init();

    // robots
    vision->red1->init();
    vision->red2->init();
    vision->navy1->init();
    vision->navy2->init();
    // balls
    vision->ball->init();

	// crosses
	vision->cross->init();
} // initObjects

/*
 * Initializes all the color data structures.
 */
void Threshold::initColors() {

    orange->init(pose->getHorizonSlope());
    blue->init(pose->getHorizonSlope());
    yellow->init(pose->getHorizonSlope());
    red->init();
    navyblue->init();
	cross->init();
}

/* This function loads a table file with the given file name
 * into memory(the big Table array)
 * for example, filename can be "/MS/merged.mtb".
 * it means the merged.mtb file in the root directory of the Memory stick
 * @param filename      the file to load
 */
void Threshold::initTable(std::string filename) {

    FILE* fp;
    //cout << filename << endl;
    fp = fopen(filename.c_str(), "r");   //open table for reading
    if (fp == NULL) {
        print("initTable() FAILED to open filename: %s", filename.c_str());
#ifdef OFFLINE
        exit(0);
#else
        return;
#endif
    }

    //actually read the table into memory
    for(int i=0; i< YMAX; i++)
        for(int j=0; j<UMAX; j++){
            fread(bigTable[i][j], sizeof(unsigned char), VMAX, fp);
        }

#ifndef OFFLINE
    print("Loaded colortable %s",filename.c_str());
#endif

    fclose(fp);
}


void Threshold::initTableFromBuffer(byte * tbfr)
{

    byte* source = tbfr;
    for(int i=0; i< YMAX; i++)
        for(int j=0; j<UMAX; j++){
            //pointer to beginning of row:
            byte* dest = bigTable[i][j];
            //copy over a whole row into big table from the buffer
            memcpy(dest,source,YMAX);
            source+=YMAX;//advance the source bugger
        }
}

/* This function loads a table file with the given file name
 * into memory(the big Table array)
 * for example, filename can be "/MS/merged.mtb".
 * it means the merged.mtb file in the root directory of the Memory stick
 * @param filename      the file to load
 */
void Threshold::initCompressedTable(std::string filename){
#ifndef NO_ZLIB
    FILE* fp;
    //cout << filename << endl;
    fp = fopen(filename.c_str(), "r");   //open table for reading
    if (fp == NULL) {
        printf("initTable() FAILED to open filename: %s , exiting",
               filename.c_str());
#ifdef OFFLINE
        exit(0);
#else
        // crash
        int x = 0;
        x = 1/x;
#endif
    }

    unsigned char *fileData = NULL;
    int length;
    fileData = Zlib::readCompressedFile(fp, length);
    if(!fileData) {
        //crash
        print("something went wrong with decompression\n");
#ifdef OFFLINE
        exit(0);
#else
        // crash
        int x = 0;
        x = 1/x;
#endif
    }
    else
        printf("everything went fine\n");


    unsigned char *fileTraverse = fileData;


    for(int i=0; i< YMAX; i++) {
        //printf("vtoro");
        for(int j=0; j<UMAX; j++){
            //64 bytes per chunk
            //fread(bigTable[i][j], sizeof(unsigned char), vMax, fp);
            for(int k=0; k<VMAX; k++) {
                bigTable[i][j][k] = *fileTraverse;
                fileTraverse++;
            }
        }
    }

    print("Loaded colortable %s",filename.c_str());
    free(fileData);

    fclose(fp);
#endif /* NO_ZLIB */
}

const uchar* Threshold::getYUV() {
    return yuv;
}

/* I haven't a clue what this method is for.
 * @param newyuv     presumably a new yuv value in bytes or something
 */
void Threshold::setYUV(const uchar* newyuv) {

    yuv = newyuv;
    yplane = yuv;

    if (!inverted) {
#if ROBOT(NAO_RL)
        // I've reversed the U and V planes, in addition to offsetting them, as
        // the color table format is still reversed
        uplane = yplane + 3; // normally, is 1, but with reversed tables, is 1
        vplane = yplane + 1; // normally, is 3, but with reversed tables, is 1
#elif ROBOT(NAO_SIM)
#else
#    error Undefined robot type
#endif

    }else {
        // inverted
#if ROBOT(NAO)
        // this is actually the correct (non-inverted) settings, but again, with
        // our color tables, inverted=non-inverted and non-inverted=inverted
        uplane = yplane + 1;
        vplane = yplane + 3;
#elif ROBOT(NAO_SIM)
#else
#    error Undefined robot type
#endif
    }
}

/* Calculate the distance between two objects (x distance only).
 * @param x1   one end point of the first object
 * @param x2   the other end point
 * @param x3   one end point of the second object
 * @param x4   the other end point
 * @return     the distance between them
 */

int Threshold::distance(int x1, int x2, int x3, int x4) {
    if (x2 < x3)
        return x3 - x2;
    if (x1 > x4)
        return x1 - x4;
    return 0;
}

/*  Returns the euclidian distance between two points.
 * @param coord1    the first point
 * @param coord2    the second point
 * @return          the distance between them
 */
float Threshold::getEuclidianDist(point <int> coord1, point <int> coord2) {
    return std::sqrt( std::pow( static_cast<float>(coord2.y-coord1.y), 2) +
					  std::pow( static_cast<float>(coord2.x-coord1.x), 2) );
}

int Threshold::getPixelBoundaryLeft() {
    const float headYaw = pose->getHeadYaw();
    const float angleInImageLeft = HORIZONTAL_SHOULDER_THRESH_LEFT - headYaw;
    return static_cast<int>(-angleInImageLeft * RAD_TO_PIX_X) + IMAGE_WIDTH / 2;
}

int Threshold::getPixelBoundaryRight() {
    const float headYaw = pose->getHeadYaw();
    const float angleInImageRight = HORIZONTAL_SHOULDER_THRESH_RIGHT - headYaw;
    return static_cast<int>(-angleInImageRight * RAD_TO_PIX_X) + IMAGE_WIDTH / 2;
}

int Threshold::getPixelBoundaryUp() {
    const float headPitch = pose->getHeadPitch();
    const float angleInImageUp = VERTICAL_SHOULDER_THRESH - headPitch;
    return static_cast<int>(angleInImageUp * RAD_TO_PIX_Y) + IMAGE_HEIGHT / 2;
}

/*  A bunch of methods for offline debugging.  Basically we create an extra image
 * array so that we can draw on it without disturbing the real image.  After we're
 * done processing then we can overlay the drawing array.
 */


/*  Reset the debugging array back to empty.
 */
void Threshold::initDebugImage(){
#ifdef OFFLINE
    for(int x = 0 ; x < IMAGE_WIDTH;x++)
        for(int y = 0; y < IMAGE_HEIGHT;y++)
            debugImage[y][x] = GREY;
#endif
}

/* For any pixels in the drawing image that have been modified, transfer them
 * to the real image.
 */
void Threshold::transposeDebugImage(){
#if defined OFFLINE && defined DEBUG_IMAGE
    for(int x = 0 ; x < IMAGE_WIDTH;x++)
        for(int y = 0; y < IMAGE_HEIGHT;y++)
            if(debugImage[y][x]!=GREY){
                thresholded[y][x] = debugImage[y][x];}
#endif
}

/* Draw a box in the fake image.
 * @param left     x value of left edge
 * @param right    x value of right edge
 * @param bottom   y value of bottom
 * @param top      y value of top
 * @param c        the color we'd like to draw
 */
void Threshold::drawBox(int left, int right, int bottom, int top, int c) {


#ifdef OFFLINE
    if (left < 0) {
        left = 0;
    }
    if (top < 0) {
        top = 0;
    }
    int width = right-left;
    int height = bottom-top;

    for (int i = left; i < left + width; i++) {
        if (top >= 0 &&
            top < IMAGE_HEIGHT &&
            i >= 0 &&
            i < IMAGE_WIDTH) {
            debugImage[top][i] = static_cast<unsigned char>(c);
        }
        if ((top + height) >= 0 &&
            (top + height) < IMAGE_HEIGHT &&
            i >= 0 &&
            i < IMAGE_WIDTH) {
            debugImage[top + height][i] = static_cast<unsigned char>(c);
        }
    }
    for (int i = top; i < top + height; i++) {
        if (i >= 0 &&
            i < IMAGE_HEIGHT &&
            left >= 0 &&
            left < IMAGE_WIDTH) {
            debugImage[i][left] = static_cast<unsigned char>(c);
        }
        if (i >= 0 &&
            i < IMAGE_HEIGHT &&
            (left+width) >= 0 &&
            (left+width) < IMAGE_WIDTH) {
            debugImage[i][left + width] = static_cast<unsigned char>(c);
        }
    }
#endif
} // drawBox


/* Draw a rectangle in the fake image.
 * @param left     x value of left edge
 * @param right    x value of right edge
 * @param bottom   y value of bottom
 * @param top      y value of top
 * @param c        the color we'd like to draw
 */
void Threshold::drawRect(int left, int top, int width, int height, int c) {
#ifdef OFFLINE
    if (left < 0) {
        width += left;
        left = 0;
    }
    if (top < 0) {
        height += top;
        top = 0;
    }

    for (int i = left; i < left + width; i++) {
        if (top >= 0 && top < IMAGE_HEIGHT && i >= 0 && i < IMAGE_WIDTH) {
            debugImage[top][i] = static_cast<unsigned char>(c);
        }
        if ((top + height) >= 0 &&
            (top + height) < IMAGE_HEIGHT &&
            i >= 0 &&
            i < IMAGE_WIDTH) {
            debugImage[top + height][i] = static_cast<unsigned char>(c);
        }
    }
    for (int i = top; i < top + height; i++) {
        if (i >= 0 &&
            i < IMAGE_HEIGHT &&
            left >= 0 &&
            left < IMAGE_WIDTH) {
            debugImage[i][left] = static_cast<unsigned char>(c);
        }
        if (i >= 0 &&
            i < IMAGE_HEIGHT &&
            (left+width) >= 0 &&
            (left+width) < IMAGE_WIDTH) {
            debugImage[i][left + width] = static_cast<unsigned char>(c);
        }
    }
#endif
} // drawRect

void Threshold::drawLine(const point<int> start, const point<int> end,
                         const int color) {
    drawLine(start.x, start.y, end.x, end.y, color);
}

/* Draw a line in the fake image.
 * @param x       start x
 * @param y       start y
 * @param x1      finish x
 * @param y1      finish y
 * @param c       color we'd like to draw
 */
void Threshold::drawLine(int x, int y, int x1, int y1, int c) {

#ifdef OFFLINE
    float slope = static_cast<float>(y - y1) / static_cast<float>(x - x1);
    int sign = 1;
    if ((abs(y - y1)) > (abs(x - x1))) {
        slope = 1.0f / slope;
        if (y > y1) sign = -1;
        for (int i = y; i != y1; i += sign) {
            int newx = x +
				static_cast<int>(slope * static_cast<float>(i - y) );

            if (newx >= 0 && newx < IMAGE_WIDTH && i >= 0 && i < IMAGE_HEIGHT)
                debugImage[i][newx] = static_cast<unsigned char>(c);
        }
    } else if (slope != 0) {
        //slope = 1.0 / slope;
        if (x > x1) sign = -1;
        for (int i = x; i != x1; i += sign) {
            int newy = y +
				static_cast<int>(slope * static_cast<float>(i - x) );

            if (newy >= 0 && newy < IMAGE_HEIGHT && i >= 0 && i < IMAGE_WIDTH)
                debugImage[newy][i] = static_cast<unsigned char>(c);
        }
    }
    else if (slope == 0) {
        int startX = min(x, x1);
        int endX = max(x, x1);
        for (int i = startX; i <= endX; i++) {
            if (y >= 0 && y < IMAGE_HEIGHT && i >= 0 && i < IMAGE_WIDTH) {
                debugImage[y][i] = static_cast<unsigned char>(c);
            }
        }
    }
#endif
}

// Draws the visual horizon on the image
void Threshold::drawVisualHorizon() {
    drawLine(0, horizon, IMAGE_WIDTH, horizon, VISUAL_HORIZON_COLOR);
}

/* drawPoint()
 * Draws a crosshair or a 'point' on the fake image at some given x, y, and with
 * a given color.
 * @param x       center of the point
 * @param y       center y value
 * @param c       color to draw
 */
void Threshold::drawPoint(int x, int y, int c) {

#ifdef OFFLINE
    if (y > 0 && x > 0 && y < (IMAGE_HEIGHT) && x < (IMAGE_WIDTH)) {
        debugImage[y][x] = static_cast<unsigned char>(c);
    }if (y+1 > 0 && x > 0 && y+1 < (IMAGE_HEIGHT) && x < (IMAGE_WIDTH)) {
        debugImage[y+1][x] = static_cast<unsigned char>(c);
    }if (y+2 > 0 && x > 0 && y+2 < (IMAGE_HEIGHT) && x < (IMAGE_WIDTH)) {
        debugImage[y+2][x] = static_cast<unsigned char>(c);
    }if (y-1 > 0 && x > 0 && y-1 < (IMAGE_HEIGHT) && x < (IMAGE_WIDTH)) {
        debugImage[y-1][x] = static_cast<unsigned char>(c);
    }if (y-2 > 0 && x > 0 && y-2 < (IMAGE_HEIGHT) && x < (IMAGE_WIDTH)) {
        debugImage[y-2][x] = static_cast<unsigned char>(c);
    }if (y > 0 && x+1 > 0 && y < (IMAGE_HEIGHT) && x+1 < (IMAGE_WIDTH)) {
        debugImage[y][x+1] = static_cast<unsigned char>(c);
    }if (y > 0 && x+2 > 0 && y < (IMAGE_HEIGHT) && x+2 < (IMAGE_WIDTH)) {
        debugImage[y][x+2] = static_cast<unsigned char>(c);
    }if (y > 0 && x-1 > 0 && y < (IMAGE_HEIGHT) && x-1 < (IMAGE_WIDTH)) {
        debugImage[y][x-1] = static_cast<unsigned char>(c);
    }if (y > 0 && x-2 > 0 && y < (IMAGE_HEIGHT) && x-2 < (IMAGE_WIDTH)) {
        debugImage[y][x-2] = static_cast<unsigned char>(c);
    }
#endif
}

// Prerequisite - point is within bounds of screen
void Threshold::drawX(int x, int y, int c) {
#ifdef OFFLINE
    // Mid point
    debugImage[y-2][x-2] = static_cast<unsigned char>(c);
    debugImage[y-1][x-1] = static_cast<unsigned char>(c);
    debugImage[y][x] = static_cast<unsigned char>(c);
    debugImage[y+1][x+1] = static_cast<unsigned char>(c);
    debugImage[y+2][x+2] = static_cast<unsigned char>(c);

    debugImage[y-2][x+2] = static_cast<unsigned char>(c);
    debugImage[y-1][x+1] = static_cast<unsigned char>(c);

	debugImage[y+1][x-1] = static_cast<unsigned char>(c);
    debugImage[y+2][x-2] = static_cast<unsigned char>(c);

#endif

}



const char* Threshold::getShortColor(int _id) {
    switch (_id) {
    case WHITE: return "W";
    case ORANGE: return "O";
    case BLUE: return "B";
    case GREEN: return "G";
    case YELLOW: return "Y";
    case BLACK: return "b";
    case RED: return "R";
    case NAVY: return "n";
    case GREY: return "U";
    case YELLOWWHITE: return "YW";
    case BLUEGREEN: return "BG";
    case PINK: return "P";
    default: return "No idea what thresh color you have, mate";
    }
}

