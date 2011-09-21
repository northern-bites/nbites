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
 * This is where we do all major ball related processing.  For now we
 * still do run length encoding to find candidate balls.  What we do
 * is collect up all of the candidate blobs and run them through a
 * series of initial sanity checks to whittle the list down to a smaller
 * number.	From that list we then start with the biggest candidate
 * ball and run more sanity checks until we find a ball that we
 * are satisfied with.	Basically our sanity checks revolve around:
 * color - the ball should be mostly orange, red is dangerous
 * shape - the ball should be pretty round
 * location - the ball should be on the field
 * distance - the perceived distance of the ball should match where
 *			  it is in our field of view
 */

#include <iostream>
#include "Ball.h"
#include "debug.h"
#include <vector>

using namespace std;

// Ball constants
// EXAMINED: look at this switch - SMALLBALLDIM
static const int SMALLBALLDIM = 3; // below this size balls are considered small
static const int SMALLBALL = SMALLBALLDIM * SMALLBALLDIM;
// ratio of width/height worse than this is a very bad sign
static const float BALLTOOFAT = 1.5f;
// ditto
static const float BALLTOOTHIN = 0.75f;
// however, if the ball is occluded we can go thinner
static const float OCCLUDEDTHIN = 0.2f;
// or fatter
static const float OCCLUDEDFAT = 4.0f;
static const float MIDFAT = 3.0f;
static const float MIDTHIN = 0.3f;
// at least this much of the blob should be orange normally
static const float MINORANGEPERCENTSMALL = 0.44f;
static const float MINORANGEPERCENT = 0.5f;
static const float MINGOODBALL = 0.5f;
static const float MAXGOODBALL = 3.0f;
static const int BIGAREA = 400;
static const int BIGGERAREA = 600;
static const float FATBALL = 2.0f;
static const float THINBALL = 0.5f;
static const int DIAMETERMISMATCH = 5;
static const int EDGEMISMATCH = 10;
static const int EDGECENTERMISMATCH = 16;

static const int DIST_POINT_FUDGE = 5;

//previous constants inserted from .h class


Ball::Ball(Vision* vis, Threshold* thr, Field* fie, Context* con,
		   unsigned char _color)
    : vision(vis), thresh(thr), field(fie), context(con), color(_color),
      runsize(1)
{
	blobs = new Blobs(MAX_BALLS);
	init(0.0);
	allocateColorRuns();
#ifdef OFFLINE
    BALLDEBUG = false;
    BALLDISTDEBUG = false;
#endif
}


/* Initialize the data structure.
 * @param s		the slope corresponding to the robot's head tilt
 */
void Ball::init(float s)
{
	blobs->init();
	slope = s;
	biggestRun = 0;
	maxHeight = IMAGE_HEIGHT;
	maxOfBiggestRun = 0L;
	numberOfRuns = 0;
	indexOfBiggestRun = 0;
	numPoints = 0;
}

/* This is the entry  point ball recognition in Threshold.cc
 * @param  h			field horizon
 * @return				always 0
 */

void Ball::createBall(int h) {
    // start by building blobs
	if (numberOfRuns > 1) {
		for (int i = 0; i < numberOfRuns; i++) {
			// search for contiguous blocks
			int nextX = runs[i].x;
			int nextY = runs[i].y;
			int nextH = runs[i].h;
			blobs->blobIt(nextX, nextY, nextH, true);
		}
	}
	balls(h, vision->ball);
    setFramesOnAndOff(vision->ball);
}

/* Are the dimensions of a candidate blob worthy of further study?
   @param w     the blob's width
   @param h     the blob's height
   @return      whether it meets the minimum criteria
 */
bool Ball::blobIsBigEnoughToBeABall(int w, int h) {
    return !(w < 3 || h < 3);
}

/*  Before we just take the biggest blob, we do some initial screening
    based upon basic blob information.  The main things we look at here
    are the color of the blob and its size.  When we find a blob that
    doesn't meet our criteria we simply re-initialize it so it will
    no longer be considered.
 */
void Ball::preScreenBlobsBasedOnSizeAndColor() {
    const int MIN_AREA = 12;
    const int MAX_AREA = 1000;
    float minpercent = MINORANGEPERCENT;

	// pre-screen blobs that don't meet our criteria
	for (int i = 0; i < blobs->number(); i++) {
        int ar = blobs->get(i).getArea();
        float perc = static_cast<float>(blobs->get(i).getPixels()) /
			static_cast<float>(ar);
		// the getPixels approach doesn't work well for small balls
        int w = blobs->get(i).width();
        int h = blobs->get(i).height();
		int x = blobs->get(i).getLeft();
		int y = blobs->get(i).getTop();
        int diam = max(w, h);
		if (w < 10 || (w < 20 && perc > 0.25)) {
			int count = 0;
			for (int j = x; j < x + w; j++) {
				for (int k = y; k < y + h; k++) {
					if (Utility::isOrange(thresh->getThresholded(k, j)) &&
						!Utility::isRed(thresh->getThresholded(k, j))) {
						count++;
					}
				}
			}
			perc = static_cast<float>(count) / static_cast<float>(ar);
		}
        // For now we are going to allow very small balls to be a bit less orange
        // obviously this is dangerous, so we'll have to keep an eye on it.
        if (ar < MIN_AREA * 3) {
            minpercent = MINORANGEPERCENTSMALL;
        } else {
            minpercent = MINORANGEPERCENT;
        }
        if (!blobIsBigEnoughToBeABall(w, h)) {
            blobs->init(i);
        } else if (ar > 0) {
            if (blobs->get(i).getBottom() + diam <
                horizonAt(blobs->get(i).getLeft())) {
                blobs->init(i);
                if (BALLDEBUG) {
                    cout << "Screened one for horizon problems " << endl;
                    drawBlob(blobs->get(i), WHITE);
                }
            } else if (ar > MIN_AREA && perc >= minpercent) {
                if (BALLDEBUG) {
                    cout << "Candidate ball " << endl;
                    printBlob(blobs->get(i));
                }
            } else if (ar > MAX_AREA &&
                       rightHalfColor(blobs->get(i)) > minpercent)
            {
                if (BALLDEBUG) {
                    cout << "Candidate ball2 " << endl;
                    printBlob(blobs->get(i));
                }
            } else {
                if (BALLDEBUG) {
                    drawBlob(blobs->get(i), BLACK);
                    printBlob(blobs->get(i));
                    if (perc < minpercent) {
                        cout << "Screened one for not being orange enough: "
                             << perc << "%" << endl;
                    } else {
                        cout << "Screened one for being too small - its area is "
                             << ar << endl;
                    }
                }
                blobs->init(i);
            }
        }
    }
}

/* Do more sanity checks on the ball returning false if any fail.
   @return          whether the ball passes the sanity checks
 */
bool Ball::sanityChecks(int w, int h, estimate e, VisualBall * thisBall) {
    const float DISTANCE_MISMATCH = 50.0f;
	const float PIXACC = 300;
	const int HORIZON_THRESHOLD = 30;

    float distanceDifference = fabs(e.dist - focalDist.dist);
    int horb = horizonAt(topBlob->getLeftBottomX());

    if (!ballIsReasonablySquare(topBlob->getLeftTopX(), topBlob->getLeftTopY(),
                                w, h)) {
		if (BALLDEBUG) {
			drawBlob(*topBlob, BLACK);
            float ratio = (float)w / (float) h;
			cout << "Screening for ratios " << ratio << endl;
		}
        thisBall->init();
        topBlob->init();
        return false;
    } else if (roundness(*topBlob) != 0) {
        if (BALLDEBUG) {
            cout << "Screening for roundness " << endl;
        }
        topBlob->init();
        thisBall->init();
        return false;
    } else if (badSurround(*topBlob)) {
        if (BALLDEBUG) {
            drawBlob(*topBlob, BLACK);
            cout << "Screening for lack of green and bad surround" << endl;
        }
        topBlob->init();
        thisBall->init();
        return false;
    } else if (distanceDifference > DISTANCE_MISMATCH &&
               (e.dist *2 <  focalDist.dist ||
                focalDist.dist * 2 < e.dist)
               && e.dist < PIXACC && e.dist > 0 && w < 12) {
        if (BALLDEBUG) {
            cout << "Screening due to distance mismatch " << e.dist <<
                " " << focalDist.dist << endl;
			drawBlob(*topBlob, BLACK);
        }
        thisBall->init();
        topBlob->init();
        return false;
    } else if (w < SMALLBALLDIM || h < SMALLBALLDIM) {
        // small balls should be near the horizon - this check makes extra sure
        if (topBlob->getLeftBottomY() > horb + HORIZON_THRESHOLD) {
            if (BALLDEBUG) {
                cout << "Screening small ball for horizon" << endl;
            }
            thisBall->init();
            topBlob->init();
            return false;
        }
    }
    return true;
}


/* See if there is a ball onscreen.	 Basically we get all of the orange blobs
 * and test them for viability.	 Once we've screened all of the obviously bad
 * ones we then pick the biggest one and check it some more.
 *
 * @param  horizon	 the horizon intercept
 * @param  thisBall	 the ball object
 * @return			 we always return 0 - an artifact of other methods
 */
int Ball::balls(int horizon, VisualBall *thisBall)
{
	const int PIX_EST_DIV = 3;
	occlusion = NOOCCLUSION;
	int w, h;	// width and height of potential ball
	estimate e; // pix estimate of ball's distance

    preScreenBlobsBasedOnSizeAndColor();
    // loop through the blobs from biggest to smallest until we find a ball
	do {
		topBlob = blobs->getTopAndMerge(horizon);
        // the conditions when we know we don't have a ball
		if (topBlob == NULL || !blobOk(*topBlob) || topBlob->getArea() == 0) {
            return 0;
        }
        if (BALLDEBUG) {
            cout << endl << "Examining next top blob " << endl;
        }
        w = topBlob->width();
        h = topBlob->height();
        //if (abs(w - h) > DIAMETERMISMATCH && !nearEdge(*topBlob)) {
		if (!nearEdge(*topBlob)) {
            adjustBallDimensions();
            w = topBlob->width();
            h = topBlob->height();
        }
		const float BALL_REAL_HEIGHT = 6.5f;
		e = vision->pose->pixEstimate(topBlob->getLeftTopX() + (w / 2),
									  topBlob->getLeftTopY() + 2 * h / PIX_EST_DIV,
									  ORANGE_BALL_RADIUS);
        // SORT OUT BALL INFORMATION
        setOcclusionInformation();
        setBallInfo(w, h, thisBall, e);
        if (thisBall->getHeight() > 0) thisBall->setOn(true);
    } while (!sanityChecks(w, h, e, thisBall));

    // last second adjustment for non-square balls
    if (ballIsClose(thisBall) && ballIsNotSquare(h, w)) {
        checkForReflections(h, w, thisBall, e);
    }
    if (BALLDEBUG) {
        cout << "Vision found ball " << endl;
        cout << topBlob->getLeftTopX() << " " << topBlob->getLeftTopY()
             << " " <<
            w << " " << h << " " << e.dist << endl;
    }
	if (BALLDISTDEBUG) {
		estimate es;
		es = vision->pose->pixEstimate(topBlob->getLeftTopX() + topBlob->width() /
									   2, topBlob->getLeftTopY() + 2
									   * topBlob->height() / PIX_EST_DIV,
									   ORANGE_BALL_RADIUS);
		cout << "Distance is " << thisBall->getDistance() << " " <<
				thisBall->getFocDist() << " " << es.dist << endl;
		cout<< "Radius"<<thisBall->getRadius()<<endl;
	}
	return 0;
}

/* Determines on which side the ball is obviously occluded.
 */
void Ball::setOcclusionInformation() {
    const int OCCLUSION_MARGIN = 2;
    if (nearImageEdgeY(topBlob->getLeft(), OCCLUSION_MARGIN)) {
        occlusion = BOTTOMOCCLUSION;
    }
    if (nearImageEdgeY(topBlob->getTop(), OCCLUSION_MARGIN)) {
        occlusion *= TOPOCCLUSION;
    }
    if (nearImageEdgeX(topBlob->getLeft(), OCCLUSION_MARGIN)) {
        occlusion *= LEFTOCCLUSION;
    }
    if (nearImageEdgeX(topBlob->getRight(), OCCLUSION_MARGIN)) {
        occlusion *= RIGHTOCCLUSION;
    }
}

/* From a given coordinate scan out in a given direction until the apparent
   edge of the ball is found.  We check for the edge using the difference
   in the V dimension.
   @param x        x coord
   @param y        y coord
   @param dir      direction of scanning (1 or -1)
   @return         x value of the edge
 */
int Ball::findBallEdgeX(int x, int y, int dir) {
    int lastv = thresh->getV(x,y);
    int midv = lastv;
    int newx = x;
    int changex = topBlob->getLeft();
    if (dir > 0) {
        changex = topBlob->getRight();
    }
    for (int i = -3; i < 4; i++) {
        newx = x;
        if (y + i >= 0 && y + i < IMAGE_HEIGHT) {
            for (bool done = false; !done && newx >= 0 && newx < IMAGE_WIDTH;
                 newx+=dir) {
                int newv = thresh->getV(newx, y + i);
                if (abs(newv - lastv) > EDGEMISMATCH
                    || abs(newv - midv) > EDGECENTERMISMATCH) {
                    done = true;
                }
                lastv = newv;
            }
            if (dir < 0) {
                if (newx + dir < changex) {
                    changex = newx + dir;
                }
            } else {
                if (newx + dir > changex) {
                    changex = newx + dir;
                }
            }
        }
    }
    return min(max(0,changex), IMAGE_WIDTH);
}

/* From a given coordinate scan out in a given direction until the apparent
   edge of the ball is found.  We check for the edge using the difference
   in the V dimension.
   @param x        x coord
   @param y        y coord
   @param dir      direction of scanning (1 or -1)
   @return         y value of the edge
 */
int Ball::findBallEdgeY(int x, int y, int dir) {
    int lastv = thresh->getV(x,y);
    int midv = lastv;
    int newy = y;
    int changey = topBlob->getTop();
    if (dir > 0) {
        changey = topBlob->getBottom();
    }
    for (int i = -3; i < 4; i++) {
        newy = y;
        if (x + i >= 0 && x + i < IMAGE_WIDTH) {
            for (bool done = false; !done && newy >= 0 && newy < IMAGE_HEIGHT;
                 newy+=dir) {
                int newv = thresh->getV(x + i,newy);
                if (abs(newv - lastv) > EDGEMISMATCH
                    || abs(newv - midv) > EDGECENTERMISMATCH) {
                    done = true;
                }
                lastv = newv;
            }
            if (dir < 0) {
                if (newy + dir < changey) {
                    changey = newy + dir;
                }
            } else {
                if (newy + dir > changey) {
                    changey = newy + dir;
                }
            }
        }
    }
    return changey;
}

/* We'd like the ball blob to be square.  It often isn't.  Usually
   its width is a little too small compared to the height due to the
   camera and the way we scan the image.  Sometimes the differences
   are more pronounced.  In those cases we do some extra scanning to
   see if we can fix the smaller dimension.  We do that here.
 */
void Ball::adjustBallDimensions() {
    int w = topBlob->width();
    int h = topBlob->height();
    if (w > h) {
        int x = topBlob->getLeft() + w / 2;
        int y = topBlob->getTop() + h / 5;
        int newtop = findBallEdgeY(x, y, -1);
        y = topBlob->getBottom() - h / 5;
        int newbottom = findBallEdgeY(x, y, 1);
        int change = topBlob->getTop() - newtop +
            newbottom - topBlob->getBottom();
        if (abs(change - (w - h)) < DIAMETERMISMATCH) {
            if (BALLDEBUG) {
                cout << "Adjusting height of blob" << endl;
            }
            topBlob->setTop(newtop);
            topBlob->setBottom(newbottom);
        }
    } else {
        int x = topBlob->getLeft() + w / 5;
        int y = topBlob->getTop() + h / 2;
        int newleft = findBallEdgeX(x, y, -1);
        x = topBlob->getRight() - w / 5;
        int newright = findBallEdgeX(x, y, 1);
        int change = newright - topBlob->getRight() +
            topBlob->getLeft() - newleft;
        if (abs(change - (h - w)) < DIAMETERMISMATCH) {
            if (BALLDEBUG) {
                cout << "Adjusting width of blob " << change << endl;
            }
            topBlob->setLeft(newleft);
            topBlob->setRight(newright);
			printBlob(*topBlob);
        }
    }
}

/* Sometimes we get a reflection off a post or other robot and it skews how
   we see the ball.  A good clue is that the ball is close and isn't very
   square.  So when that happens we see if we can improve on its basic
   squareness.  We do that here.
   @param h        the ball height
   @param w        the ball width
   @param thisBall the ball
   @param e        pixEstimated distance to ball
 */
void Ball::checkForReflections(int h, int w, VisualBall * thisBall,
                               estimate e) {
    // we probably have misidentified the distance see if we can fix it.
    if (BALLDISTDEBUG) {
        cout << "Detected bad ball distance - trying to fix " << w <<
            " " << h << endl;
    }
    // generally the reflections are over or under the ball
    if (h > w) {
        // scan the sides to find the real sides
        int count = -2;
        if (topBlob->getRightTopX() - h > 0) {
            for (int i = topBlob->getRightTopX() - h; i < IMAGE_WIDTH - 1;
                 i++) {
                for (int j = topBlob->getLeftTopY();
                     j < topBlob->getLeftBottomY(); j++) {
                    if (Utility::isOrange(thresh->getThresholded(j,i))) {
                        topBlob->setRightTopX(i);
                        j = IMAGE_HEIGHT;
                        i = IMAGE_WIDTH;
                    }
                }
                count++;
            }
        }
        if (topBlob->getLeftTopX() + h < IMAGE_WIDTH) {
            for (int i = topBlob->getLeftTopX() + h; i > -1; i--) {
                for (int j = topBlob->getLeftTopY();
                     j < topBlob->getLeftBottomY(); j++) {
                    if (Utility::isOrange(thresh->getThresholded(j,i))) {
						topBlob->setRightTopX(i);
                        j = IMAGE_HEIGHT;
                        i = -1;
                    }
                }
                count++;
            }
        }
        if (count > 1) {
            if (BALLDISTDEBUG) {
                cout << "Resetting ball dimensions.	 Count was " << count
                     << endl;
            }
            setBallInfo(w, w, thisBall, e);
        }
    }
    thisBall->setDistanceEst(e);
}

/* Returns true when the ball is close (3/4 of a meter).
   @param thisBall       the ball
   @return               whether it is within the prescribed distance
 */

bool Ball::ballIsClose(VisualBall * thisBall) {
    return thisBall->getDistance() < 75.0f;
}

/* Returns true when the height and width are not a good match.
   @param h      ball height
   @param w      ball width
   @return       whether the ball is square or not
 */
bool Ball::ballIsNotSquare(int h, int w) {
    return abs(h - w) > 3;
}

/* Set the primary color.  Depending on the color, we have different space needs
 * @param c		   the color
 */
void Ball::setColor(unsigned char c)
{
	const int RUN_VALUES = 3;			// x, y, and h
	const int RUNS_PER_LINE = 5;
	const int RUNS_PER_SCANLINE = 15;

	runsize = 1;
	int run_num = RUN_VALUES;
	color = c;
	runsize = BALL_RUNS_MALLOC_SIZE; //max number of runs
	run_num = runsize * RUN_VALUES;
	runs = (run*)malloc(sizeof(run) * run_num);
}


/* Allocate the required amount of memory dependent on the primary color
 */
void Ball::allocateColorRuns()
{
	const int RUN_VALUES = 3;		  // x, y and h
	const int RUNS_PER_SCANLINE = 15;
	const int RUNS_PER_LINE = 5;

	int run_num = RUN_VALUES;
	// depending on the color we have more or fewer runs available
	runsize = BALL_RUNS_MALLOC_SIZE; //max number of runs
	run_num = runsize * RUN_VALUES;
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
void Ball::newRun(int x, int y, int h)
{
	const int RUN_VALUES = 3;	 // x, y, and h of course

	if (numberOfRuns < runsize) {
		int last = numberOfRuns - 1;
		// skip over noise --- jumps over two pixel noise currently.
		//HW--added CONSTANT for noise jumps.
		if (last > 0 && runs[last].x == x &&
				(runs[last].y - (y + h) <= NOISE_SKIPS)) {
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

/* Returns the field horizon at the given x coordinate
   @param x     x value we want to check
   @return      y value of horizon
 */
int Ball::horizonAt(int x) {
	return field->horizonAt(x);
}


/*	Normally we want our balls to be orange and can just check the number of
 * pixels within the blob
 * that are orange.	 However, sometimes the balls are occluded.
 * If we have a nice big orange blob,
 * but it doesn't seem orange enough it might be occluded.	So we look at
 * different halves of the blob
 * to see if one of them is properly orange.
 * @param tempobj	   the current ball candidate
 * @return			   the best percentage we found
 */
// only called on really big orange blobs
float Ball::rightHalfColor(Blob tempobj)
{
	const float COLOR_THRESH = 0.15f;
	const float POOR_VALUE = 0.10f;

	int x = tempobj.getLeftTopX();
	int y = tempobj.getLeftTopY();
	int spanY = tempobj.height();
	int spanX = tempobj.width();
	int good = 0, good1 = 0, good2 = 0;
	unsigned char pix;
	for (int i = spanY / 2; i < spanY; i++) {
		for (int j = 0; j < spanX; j++) {
			pix = thresh->getThresholded(y + i,x + j);
			if (y + i > -1 && x + j > -1 && (y + i) < IMAGE_HEIGHT &&
				x + j < IMAGE_WIDTH && Utility::isOrange(pix)) {
				good++;
			}
		}
	}
	for (int i = 0; i < spanY; i++) {
		for (int j = 0; j < spanX / 2; j++) {
			pix = thresh->getThresholded(y + i,x + j);
			if (y + i > -1 && x + j > -1 && (y + i) < IMAGE_HEIGHT &&
				x + j < IMAGE_WIDTH && Utility::isOrange(pix)) {
				good1++;
			}
		}
	}
	for (int i = 0; i < spanY; i++) {
		for (int j = spanX / 2; j < spanX; j++) {
			pix = thresh->getThresholded(y + i,x + j);
			if (y + i > -1 && x + j > -1 && (y + i) < IMAGE_HEIGHT &&
				x + j < IMAGE_WIDTH && Utility::isOrange(pix)) {
				good2++;
			}
		}
	}
	if (BALLDEBUG) {
		cout << "Checking half color " << good << " " << good1 << " " <<
				good2 << " " << (spanX * spanY / 2) << endl;
	}
	float percent = (float)max(max(good, good1), good2) /
			(float) (spanX * spanY / 2);
	return percent;
}

/* Check is the blob representing the ball is basically square.  Sometimes
   it isn't square because it is occluded, so be carefull about those cases.
   @param    x        x value of upper left corner of blob
   @param    y        y value of upper left corner of blob
   @param    w        width of blob
   @param    h        height of blob
   @return            whether the blob is reasonably square or not
 */

bool Ball::ballIsReasonablySquare(int x, int y, int w, int h) {
	const int IMAGE_EDGE = 3;
	const int BIG_ENOUGH = 4;
	const int TOO_BIG_TO_CHECK = 20;
	const int WIDTH_AT_SCREEN_BOTTOM = 15;
	float ratio = static_cast<float>(w) / static_cast<float>(h);
    int margin = IMAGE_EDGE;

	if ((h < SMALLBALLDIM && w < SMALLBALLDIM && ratio > BALLTOOTHIN &&
			ratio < BALLTOOFAT)) {
        return true;
	} else if (ratio > THINBALL && ratio < FATBALL) {
        return true;
	} else if (ratio < BALLTOOFAT && ratio > OCCLUDEDTHIN) {
		// check for robot occlusion
		if (robotOccludesIt(x, y, w, h)) {
			return true;
		}
	}
	if (nearImageEdgeX(x, margin) || nearImageEdgeX(x+w, margin) ||
               nearImageEdgeY(y, margin) || nearImageEdgeY(y+h, margin)) {
        bool nearX = nearImageEdgeX(x, margin) || nearImageEdgeX(x+w, margin);
        bool nearY = nearImageEdgeY(y, margin) || nearImageEdgeY(y+h, margin);
		// we're on an edge so allow for streching
		if (h > BIG_ENOUGH && w > BIG_ENOUGH && nearY &&
            ratio < MIDFAT && ratio > 1) {
            return true;
			// then sides
		} else if (h > BIG_ENOUGH && w > BIG_ENOUGH && nearX
                   && ratio > MIDTHIN && ratio < 1) {
            return true;
        } else if (h > TOO_BIG_TO_CHECK && nearX && ratio > OCCLUDEDTHIN &&
            ratio < 1.0f) {
            return true;
        } else if (w > TOO_BIG_TO_CHECK && nearY && ratio < OCCLUDEDFAT &&
            ratio > 1.0f) {
            return true;
			// when we have big slivers then allow for extra
        } else if (nearY && w > WIDTH_AT_SCREEN_BOTTOM) {
            return true;
			// the bottom is a really special case
		} else {
            return false;
		}
	} else {
		return false;
	}
    return true;
}

/*
 */
bool Ball::robotOccludesIt(int x, int y, int w, int h) {
	if (BALLDEBUG) {
		cout << "Checking for occluded ball" << endl;
	}
	int count = 0;
	if (x > 2) {
		for (int i = y; i < y + h; i++) {
			for (int j = x - 2; j < x; j++) {
				if (Utility::isWhite(thresh->getThresholded(i, j))) {
					count++;
				}
			}
		}
	}
	if (count > h) {
		return true;
	}
	count = 0;
	if (x < IMAGE_WIDTH - 2) {
		for (int i = y; i < y + h; i++) {
			for (int j = x + w + 1; j < x + w + 3; j++) {
				if (Utility::isWhite(thresh->getThresholded(i, j))) {
					count++;
				}
			}
		}
	}
	if (count > h) {
		return true;
	}
	return false;
}

/* Returns true is the blob abuts any image edge
   @param b      the blob to check
   @return       true when the blob is near an edge
 */
bool Ball::nearEdge(Blob b) {
    return nearImageEdgeX(b.getLeft(), 1) || nearImageEdgeX(b.getRight(), 1) ||
        nearImageEdgeY(b.getTop(), 1) || nearImageEdgeY(b.getBottom(), 2);
}

/* Is the x value near the image edge?
   @param x      the x value
   @param margin how close we allow
   @return       whether it is close enough
 */
bool Ball::nearImageEdgeX(int x, int margin) {
    return x < margin || x > IMAGE_WIDTH - margin - 1;
}

/* Is the y value near the image edge?
   @param y      the y value
   @param margin how close we allow
   @return       whether it is close enough
 */
bool Ball::nearImageEdgeY(int y, int margin) {
    return y < margin || y > IMAGE_HEIGHT - margin - 1;
}

/*	It probably goes without saying that the ideal ball is round.  So let's see
 * how round our current candidate is.	Among other things we check its
 * height/width ratio (should be about 1) and where the orange is (shouldn't
 * be in the corners, should be in the middle)
 * TODO:  This needs LOTS of work.	Especially if we move to nontraditional
 * colors or multi-colored balls.
 * TODO:  Handle cases where screen occludes ball
 * @param b		 the candidate ball
 * @return		 a constant result - BAD_VALUE, or 0 for round
 */

int	 Ball::roundness(Blob b)
{
    if (nearEdge(b)) {
        return 0;
    }
	int w = b.width();
	int h = b.height();
	if (w * h > SMALLBALL) {
		// now make some scans through the blob - horizontal, vertical, diagonal
        pair<int, int> diagonal = scanDiagonalsForRoundnessInformation(b);
        pair<int, int> midlines = scanMidlinesForRoundnessInformation(b);
        int goodPix = diagonal.first + midlines.first;
        int badPix = diagonal.second + midlines.second;
		if (BALLDEBUG) {
			cout << "Roundness: Good " << goodPix << " " << badPix << endl;
        }
        badPix = 0;
		// if more than 20% or so of our pixels tested are bad, then we toss it out
		if (goodPix < badPix * 5) {
			return BAD_VALUE;
		}
	}
	return 0;
}

/*  As part of our roundness checking we scan the midlines of the blob.
    In principle every pixel should be the right color.
    @param b        candidate blob
    @return         a pair containing the number of good and bad pixels
 */
pair<int, int> Ball::scanMidlinesForRoundnessInformation(Blob b) {
	int w = b.width();
	int h = b.height();
	int x = b.getLeftTopX();
	int y = b.getLeftTopY();
    unsigned char pix;
    int goodPix = 0, badPix = 0;
    for (int i = 0; i < h; i++) {
        pix = thresh->getThresholded(y+i,x + w/2);
		if (Utility::isOrange(pix)) {
            goodPix++;
        } else if (!Utility::isUndefined(pix))
            badPix++;
    }
    for (int i = 0; i < w; i++) {
        pix = thresh->getThresholded(y+h/2,x + i);
		if (Utility::isOrange(pix)) {
            goodPix++;
        } else if (!Utility::isUndefined(pix)) {
            badPix++;
        }
    }
    pair<int, int> info;
    info.first = goodPix;
    info.second = badPix;
    return info;
}

/* As part of roundness checking we scan the diagonals of the blob.
   We know that there is a predictible transition point from no ball
   to ball and categorize each pixel accordingly
   @param b      the blob
   @return       a pair containing the number of good pixels, and bad ones
 */
pair<int, int> Ball::scanDiagonalsForRoundnessInformation(Blob b) {
	const float CORNER_CHUNK_DIV = 6.0f;
	int w = b.width();
	int h = b.height();
	int x = b.getLeftTopX();
	int y = b.getLeftTopY();
    unsigned char pix;
    int goodPix = 0, badPix = 0;
    int d = ROUND2(static_cast<float>(std::max(w, h)) /
                   CORNER_CHUNK_DIV);
    int d3 = min(w, h);
    pair<int, int> info;
    for (int i = 0; i < d3; i++) {
        pix = thresh->getThresholded(y+i,x+i);
        if (i < d || (i > d3 - d)) {
			if (Utility::isOrange(pix)) {
                badPix++;
            } else {
                goodPix++;
            }
        } else {
			if (Utility::isOrange(pix)) {
                goodPix++;
            } else if (!Utility::isUndefined(pix)) {
                badPix++;
            }
        }
        pix = thresh->getThresholded(y+i,x+w-i);
        if (i < d || (i > d3 - d)) {
			if (Utility::isOrange(pix)) {
                badPix++;
            }
            else {
                goodPix++;
            }
        } else if (Utility::isOrange(pix)) {
            goodPix++;
        } else if (Utility::isUndefined(pix)) {
            badPix++;
        }
    }
    info.first = goodPix;
    info.second = badPix;
    return info;
}

/*	Check the information surrounding the ball and look to see if it might be a
 * false ball.	Since our main candidate for false balls is the red/pink uniform, the
 * main thing we worry about is a preponderance of red.  In many ways this is the
 * key sanity check.
 *
 * @param b	   our ball candidate
 * @return	   true if the surround looks bad, false if its ok
 */

bool Ball::badSurround(Blob b) {
	// basically check around the blob and see if it is ok - ideally we'd have
	// some green, worrisome would be lots of RED
	static const int SURROUND = 12;

	const float GREEN_PERCENT = 0.1f;

	int x = b.getLeftTopX();
	int y = b.getLeftTopY();
	int w = b.width();
	int h = b.height();
	int surround = min(SURROUND, w/2);
	int greens = 0, orange = 0, red = 0, borange = 0, realred = 0,
			yellows = 0;
	unsigned char pix;

	// now collect information on the area surrounding the ball and the ball
	x = max(0, x - surround);
	y = max(0, y - surround);
	w = w + surround * 2;
	h = h + surround * 2;
	for (int i = 0; i < w && x + i < IMAGE_WIDTH; i++) {
		for (int j = 0; j < h && y + j < IMAGE_HEIGHT; j++) {
			pix = thresh->getThresholded(y + j,x + i);
			if (Utility::isOrange(pix)) {
				orange++;
                if (x + i >= b.getLeft() && x + i <= b.getRight() &&
                    y + j >= b.getTop() && y + j <= b.getBottom()) {
                    borange++;
                }
			}
			else if (Utility::isRed(pix)) {
				realred++;
			}
			if (Utility::isRed(pix) && Utility::isOrange(pix)) {
				red++;
			}
			if (Utility::isGreen(pix)) {
				greens++;
			}
			if (Utility::isYellow(pix) && j < surround) {
				yellows++;
            }
		}
	}
	if (BALLDEBUG) {
		cout << "Surround information " << red << " " << realred << " "
				<< orange << " " << borange << " " << greens << " "
				<< yellows << endl;
	}
	if (realred > borange) {
		if (BALLDEBUG) {
			cout << "Too much real red" << endl;
		}
		return true;
	}
	if (realred > greens * 2 && w * h < 2000 && b.getBottom() < IMAGE_HEIGHT - 5) {
		if (BALLDEBUG) {
			cout << "Too much real red versus green" << endl;
		}
		return true;
	}
	if (realred > borange && realred > orange) {
		if (BALLDEBUG) {
			cout << "Too much real red vs borange" << endl;
		}
		return true;
	}
	if (orange - borange > borange * 0.3 && orange - borange > 10) {
		if (BALLDEBUG) {
			cout << "Too much orange outside of the ball" << endl;
		}
		// We can run into this problem with reflections - let's see if
		// we're up against a post or something
		if (yellows > w * 3) {
			if (BALLDEBUG) {
				cout << "But lots of yellow, doing nothing " << endl;
			}
			return false;
		} else {
			return true;
		}
	}
	if ((red > orange) &&
			(static_cast<float>(greens) <
					(static_cast<float>(w * h) * GREEN_PERCENT))) {
		if (BALLDEBUG) {
			cout << "Too much real orangered without enough green" << endl;
		}
		return true;
	}
	if (red > orange || (realred > greens && realred > 2 * w &&
            realred > borange * 0.1))  {
		if (BALLDEBUG) {
			cout << "Too much real red - doing more checking" << endl;
		}
		x = b.getLeftTopX();
		y = b.getLeftBottomY();
        if (nearImageEdgeX(x, 2) || nearImageEdgeX(x+b.width(), 2) ||
            nearImageEdgeY(y, 2)) {
			if (BALLDEBUG) {
				cout << "Dangerous corner location detected " << x << " "
                     << y << " " << w << endl;
			}
			return true;
		}
		return roundness(b) == BAD_VALUE;
	}
	return false;
}

/* Once we have determined a ball is a blob we want to set it up for
   the rest of the world (localization, behavior, etc.).
   @param w         ball width
   @param h         ball height
   @param thisBall  the ball
   @param e         pixEstimate to ball
 */

void Ball::setBallInfo(int w, int h, VisualBall *thisBall, estimate e) {

	const float radDiv = 2.0f;
	// x, y, width, and height. Not up for debate.
	thisBall->setX(topBlob->getLeftTopX());
	thisBall->setY(topBlob->getLeftTopY());

	thisBall->setWidth( static_cast<float>(w) );
	thisBall->setHeight( static_cast<float>(h) );
	thisBall->setRadius( std::max(static_cast<float>(w)/radDiv,
								  static_cast<float>(h)/radDiv ) );
	int amount = h / 2;
	if (w > h) {
		amount = w / 2;
	}

	if (occlusion == LEFTOCCLUSION) {
		thisBall->setCenterX(topBlob->getRightTopX() - amount);
		thisBall->setX(topBlob->getRightTopX() - amount * 2);
	} else {
		thisBall->setCenterX(topBlob->getLeftTopX() + amount);
	}
	if (occlusion != TOPOCCLUSION) {
		thisBall->setCenterY(topBlob->getLeftTopY() + amount);
	} else {
		thisBall->setCenterY(topBlob->getLeftBottomY() - amount);
	}
	thisBall->setConfidence(SURE);
	thisBall->findAngles();
	focalDist = vision->pose->sizeBasedEstimate(thisBall->getCenterX(),
												thisBall->getCenterY(),
												ORANGE_BALL_RADIUS,
												thisBall->getRadius(),
												ORANGE_BALL_RADIUS);
	if (occlusion == NOOCCLUSION || e.dist > 600) {
		thisBall->setFocalDistanceFromRadius();
		//trust pixest to within 300 cm
		if (e.dist <= 300) {
			thisBall->setDistanceEst(e);
		}
		else {
			thisBall->setDistanceEst(focalDist);
		}
	} else {
		// use our super swell updated pix estimate to do the distance
		thisBall->setDistanceEst(e);
		if (BALLDISTDEBUG) {
			thisBall->setFocalDistanceFromRadius();
		}
	}
	/*cout<<"pixest "<<e.dist<<"size "<<vision->pose->sizeBasedEstimate(
	  thisBall->getCenterX(),
	  thisBall->getCenterY(),
	  ORANGE_BALL_RADIUS,
	  thisBall->getRadius(),
	  ORANGE_BALL_RADIUS).dist<<endl;*/
}

/*
 * Sets frames on/off to the correct number.
 */
void Ball::setFramesOnAndOff(VisualBall *objPtr) {
   if (objPtr->isOn()) {
        objPtr->setFramesOn(objPtr->getFramesOn()+1);
        objPtr->setFramesOff(0);
    }
    else {
        objPtr->setFramesOff(objPtr->getFramesOff()+1);
        objPtr->setFramesOn(0);
    }
 }


/* Misc. routines
 */

/* When we process blobs we start them with BAD_VALUE such that we can easily
 * tell if whatever processing we did worked out.  Here we make that check.
 * @param b	   the blob we worked on.
 * @return	   true when the processing worked, false otherwise
 */
bool Ball::blobOk(Blob b) {
	if (b.getLeftTopX() > BAD_VALUE && b.getLeftBottomX() > BAD_VALUE &&
			b.width() > 2)
		return true;
	return false;
}


/* Print debugging information for a blob.
 * @param b	   the blob
 */
void Ball::printBlob(Blob b) {
#if defined OFFLINE
	cout << "Blob Top Left Corner " << b.getLeftTopX() << " " << b.getLeftTopY()
         << endl;
    cout << "Width/height " << b.width() << " " << b.height();
    cout << " Amount of orange " << b.getPixels() << endl;
#endif
}

/* Prints a bunch of ball information about the best ball candidate (or any one).
 * @param b	   the candidate ball
 * @param c	   how confident we are its a ball
 * @param p	   how many occlusions
 * @param o	   what the occlusions are if any
 * @param bg   where around the ball there is green
 */
void Ball::printBall(Blob b, int c, float p, int o) {
#ifdef OFFLINE
	if (BALLDEBUG) {
		cout << "Ball info: " << b.getLeftTopX() << " " << b.getLeftTopY()
					 << " " << b.width() << " " << b.height() << endl;
		cout << "Confidence: " << c << " Orange Percent: " << p
				<< " Occlusions: ";
		if (o == NOOCCLUSION) cout <<  "none";
		if (o % LEFTOCCLUSION == 0) cout << "left ";
		if (o % RIGHTOCCLUSION == 0) cout << "right ";
		if (o % TOPOCCLUSION == 0) cout << "top ";
		if (o % BOTTOMOCCLUSION == 0) cout << "bottom ";
		cout << endl;
	}
#endif
}

/* Debugging method used to show where things were processed on the image.
 * Paints a verticle stripe corresponding to a "run" of color.
 *
 * @param x		x coord
 * @param y		y coord
 * @param h		height
 * @param c		the color to paint
 */
void Ball::paintRun(int x, int y, int h, int c){
	vision->drawLine(x,y+1,x,y+h+1,c);
}

/*	More or less the same as the previous method, but with different parameters.
 * @param run	  a run of color
 * @param c		  the color to paint
 */
void Ball::drawRun(const run& run, int c) {
	vision->drawLine(run.x,run.y+1,run.x,run.y+run.h+1,c);
}

/*	Draws a "+" on the screen at the specified location with the specified color.
 * @param x		x coord
 * @param y		y coord
 * @param c		the color to paint
 */
void Ball::drawPoint(int x, int y, int c) {
#ifdef OFFLINE
	vision->drawPoint(x, y, c);
#endif
}

/*	Draws the outline of a rectangle in the specified color.
 * @param b	   the rectangle
 * @param c	   the color to paint
 */
void Ball::drawRect(int x, int y, int w, int h, int c) {
#ifdef OFFLINE
	vision->drawRect(x, y, w, h, c);
#endif
}

/*	Draws the outline of a blob in the specified color.
 * @param b	   the blob
 * @param c	   the color to paint
 */
void Ball::drawBlob(Blob b, int c) {
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

/* Draws a line on the screen of the specified color.
 * @param x	   x value of point 1
 * @param y	   y value of point 1
 * @param x1   x value of point 2
 * @param y1   y value of point 2
 * @param c	   the color to paint the line.
 */
void Ball::drawLine(int x, int y, int x1, int y1, int c) {
#ifdef OFFLINE
	vision->drawLine(x, y, x1, y1, c);
#endif
}

