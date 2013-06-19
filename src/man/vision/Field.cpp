/**
 * This file processes vision with respect to the field.
 * The primary objectives of this processing are to determine where
 * in the visual field the field is and, if possible, where the edges
 * of the field are.  The former is achieved in the method
 * findGreenHorizon where the latter is done in findFieldEdges.
 *
 * findGreenHorizon works by starting at the pose calculated horizon.
 * Then it does some horizontal scanning every 4th line looking for the
 * first place where there is a threshold amount of green.	From there
 * it just tightens up the bound.
 *
 * findFieldEdges uses information found during thresholding - namely
 * the highest point in each column where green was seen during thresholding.
 * Its goal is basically to construct lines in much the same manner as
 * in field lines.	However, this problem is more highly constrained
 * and therefore is considerably simpler.  Since we get all of our
 * "line points" in order and don't have to worry about horizontal/
 * vertical issues we can avoid a lot of the checking necessary for
 * normal lines.  Essentially we scan through the points attempting to
 * grow line segments.	We stop any given segment when new points are
 * not part of the segment.  Once we have our segments we then analyze
 * them to see if they can be combined (e.g. a robot occluded a line)
 * and also whether they are truly at the field edge.  To do this we
 * project the line to each end of the image and make sure that none
 * of our other points lie "above" our projected line.    Normally the
 * end result should be no more than two lines (and a corner at their
 * intersection).
 */

#include "Common.h"

#include <math.h>
#include <assert.h>
#include <boost/shared_ptr.hpp>

#include "Field.h"
#include "VisualFieldEdge.h"
#include "Utility.h"

using namespace std;
using boost::shared_ptr;

namespace man {
namespace vision {

// Constructor for Field class. passed an instance of Vision and Pose
Field::Field(Vision* vis, Threshold * thr)
  : vision(vis), thresh(thr)
{
	// NOTE: leave this in please, else I will "git blame" and cut off your
	// funding. - chown
#ifdef OFFLINE
	debugFieldEdge = false;
	debugDrawFieldEdge = true;
	debugHorizon = false;
#endif
}

/* As part of finding the convex hull, we need to know where the
   top points of green are at each scanline.  We calculate them
   here.  Just start at the horizon in each scanline and scan
   down until we find enough green.  Then store the result in
   the convex array.
   @param pH    the horizon found by findGreenHorizon
*/
void Field::initialScanForTopGreenPoints(int pH) {
	int good, ok, top;
	unsigned char pixel;
	int topGreen = 0;
	int greenRun = 0;
    float possible = 0.0f;
	int lastGreen;
    const float BUFFER = 200.0f; // other fields should be farther than this
	// we need a better criteria for what the top is
	for (int i = 0; i < HULLS; i++) {
		good = 0;
		ok = 0;
		int poseProject = thresh->yellow->yProject(0, pH, i * SCANSIZE);
		if (poseProject <= 0) {
			poseProject = 0;
		} else if (pH == 0) {
			poseProject = 0;
		}
		topGreen = IMAGE_HEIGHT - 1;
		greenRun = 0;
		lastGreen = 0;
		for (top = max(poseProject, 0);
				good < RUNSIZE && top < IMAGE_HEIGHT; top++) {
			// scan until we find a run of green pixels
			int x = i * SCANSIZE;
			if (i == HULLS - 1) {
				x--;
			}
			pixel = thresh->getColor(x, top);
            // watch out for patches of green off the field
            if (topGreen != IMAGE_HEIGHT - 1 &&
				top - lastGreen  >  5 &&
                possible - thresh->getPixDistance(top) > BUFFER / 2) {
				if (debugFieldEdge) {
					cout << "Detected bad patch " << x << " " << top << " " << lastGreen << endl;
				}
                topGreen = IMAGE_HEIGHT - 1;
            }
			//pixel = thresh->thresholded[top][x];
			if (Utility::isGreen(pixel)) {
				lastGreen = top;
				good++;
				greenRun++;
				if (greenRun > 3 && topGreen == IMAGE_HEIGHT - 1) {
					topGreen = top - greenRun;
                    possible = thresh->getPixDistance(topGreen);
					if (debugFieldEdge) {
						cout << "Setting topGreen " << x << " " << topGreen << endl;
						vision->drawPoint(x, topGreen, BLUE);
					}
				}
                // before we finish make sure we haven't seen another field
                if (good == RUNSIZE) {
                    float topDist = thresh->getPixDistance(topGreen);
                    float newDist = thresh->getPixDistance(top);
                    if (topDist > BUFFER) {
						int check = top;
						int greens = 0;
						int check2 = top;
						int whites = 0;
						bool found = false;
						while ((thresh->getPixDistance(check) > BUFFER ||
								check < IMAGE_HEIGHT / 2) && !found && check < IMAGE_HEIGHT - 1) {
							check++;
							pixel = thresh->getColor(x, check);
							greens = 0;
							while (Utility::isGreen(pixel) && check < IMAGE_HEIGHT - 1 &&
								   greens < 6) {
								check++;
								greens++;
								pixel = thresh->getColor(x, check);
							}
							if (greens >= 6) {
								check2 = check;
								found = true;
							} else if (greens > 2) {
								check2 = check;
							}
							while (!Utility::isGreen(pixel) && check < IMAGE_HEIGHT - 1) {
								check++;
								if (Utility::isWhite(pixel)) {
									whites++;
								}
								pixel = thresh->getColor(x, check);
							}
							if (thresh->getPixDistance(check2) - thresh->getPixDistance(check)
								> BUFFER / 2 && check - check2 > 5 && check - check2 - whites > 4) {
								if (debugFieldEdge) {
									cout << "Unsetting top green " <<
										(thresh->getPixDistance(check2) - thresh->getPixDistance(check))
										 << endl;
								}
								topGreen = IMAGE_HEIGHT - 1;
								top = check;
								good = 1;
								greenRun = 1;
								check = IMAGE_HEIGHT - 1;
							}
						}
                    }
                }
			} else if (Utility::isOrange(pixel) || Utility::isWhite(pixel)) {
				//good++;
				greenRun = 0;
			} else if (Utility::isUndefined(pixel)) {
				ok++;
				if (ok > SCANNOISE) {
					good = 0;
					ok = 0;
				}
				greenRun = 0;
			} else {
				good = 0;
				ok = 0;
				greenRun = 0;
			}
		}
		if (good == RUNSIZE) {
			convex[i] = point<int>(i * SCANSIZE, topGreen);
			if (poseProject < 0 && topGreen < 10) {
				convex[i] = point<int>(i * SCANSIZE, 0);
            }
        } else {
            convex[i] = point<int>(i * SCANSIZE, IMAGE_HEIGHT);
        }
        if (debugFieldEdge) {
            vision->drawPoint(i * SCANSIZE, convex[i].y, MAROON);
        }
    }
    // look for odd spikes and quell them
    /*if (poseHorizon > -100) {
        for (good = 4; good < HULLS - 4; good++) {
            if (convex[good-1].y - convex[good].y > 15 && convex[good+1].y -
                convex[good].y > 15) {
                if (debugFieldEdge) {
                    cout << "Spike at " << convex[good].x << " " << convex[good].y <<
                        endl;
                }
                convex[good].y = convex[good-1].y;
            }
        }
		// special case for the edges
		if (convex[HULLS - 2].y - convex[HULLS - 1].y > 15) {
			convex[HULLS - 1].y = convex[HULLS - 2].y;
		}
		if (convex[1].y - convex[0].y > 15) {
			convex[0].y = convex[1].y;
		}
		}*/
    for (good = 0; convex[good].y == IMAGE_HEIGHT && good < HULLS; good++) {}
    if (good < HULLS) {
        for (int i = good-1; i > -1; i--) {
            convex[i].y = convex[i+1].y;
        }
        for (good = HULLS - 1; convex[good].y == IMAGE_HEIGHT && good > 0; good--) {}
        for (int i = good + 1; i < HULLS; i++) {
            convex[i].y = convex[i-1].y;
        }
    }
    if (convex[0].y - convex[2].y > 10) {
        convex[0].y = convex[2].y;
        convex[1].y = convex[2].y;
    }
    if (convex[HULLS - 1].y - convex[HULLS - 3].y > 10) {
        convex[HULLS - 1].y = convex[HULLS - 3].y;
        convex[HULLS - 2].y = convex[HULLS - 3].y;
    }
}

/* At this point we have found our convex hull as defined for the scanlines.
   We want to extend this information to all possible x values.  So here we
   calculate all of those values, doing some simple interpolating to find
   most of them.
   @param M     the number of sides of the convex hull
*/
void Field::findTopEdges(int M) {
    // interpolate the points in the hull to determine values for every scanline
    topEdge[0] = convex[0].y;
    float maxPix = 0.0f;
    estimate e;
	peak = -1;
	float maxLine = 1000.0f;
    for (int i = 1; i <= M; i++) {
        int diff = convex[i].y - convex[i-1].y;
        float step = 0.0f;
        if (convex[i].x != convex[i-1].x) {
            step = (float)diff / (float)(convex[i].x - convex[i-1].x);
        }
        float cur = static_cast<float>(convex[i].y);
        for (int j = convex[i].x; j > convex[i-1].x; j--) {
            cur -= step;
            topEdge[j] = (int)cur;
			if (cur < maxLine) {
				peak = j;
				maxLine = cur;
			}
            if (debugDrawFieldEdge) {
                if (j < convex[i].x - 2) {
                    vision->drawPoint(j, (int)cur, BLUE);
                } else {
                    vision->drawPoint(j, (int)cur, RED);
                }
            }
        }
        if (debugDrawFieldEdge) {
            vision->drawLine(convex[i-1].x, convex[i-1].y, convex[i].x,
                             convex[i].y, ORANGE);
        }
    }
    // calculate the distance to the edge of the field at three key points
    const int quarter = IMAGE_WIDTH / 4;
    const int TOPPING = 30;
    float qDist = 1000.0f, hDist = 1000.0f, tDist = 1000.0f;
    if (topEdge[quarter] > TOPPING) {
        e = vision->pose->pixEstimate(quarter, topEdge[quarter], 0.0f);
        qDist = e.dist;
    }
    if (topEdge[quarter * 2] > TOPPING) {
        e = vision->pose->pixEstimate(quarter * 2, topEdge[quarter * 2], 0.0f);
        hDist = e.dist;
    }
    if (topEdge[quarter * 3] > TOPPING) {
        e = vision->pose->pixEstimate(quarter * 3, topEdge[quarter * 3], 0.0f);
        tDist = e.dist;
    }
    vision->fieldEdge->setDistances(qDist, hDist, tDist);
    if (debugFieldEdge) {
        cout << "Distances are " << qDist << " " << hDist << " " << tDist <<
               " there are " << M << " points." << endl;
        cout << "Max dist is " << maxPix << endl;
    }
}

/* Provides a rough guidline to whether the horizon slopes to the right or left
   or is roughly horizontal;
 */
int Field::findSlant() {
	if (topEdge[0] > topEdge[IMAGE_HEIGHT - 1] + 20) {
		return 1;
	} else if (topEdge[IMAGE_HEIGHT - 1] > topEdge[0] + 20) {
		return -1;
	}
	return 0;
}

/** Find the convex hull of the field.    We've already found the point of maximal
    green.    Now scan down from that point every SCANLINES lines and find the
    highest point of green in each of those scanlines.     We collect those points
    and perform a Graham-Scan to find their convex hull (see wikipedia.org).
    The convex hull in turn forms our usable horizon at any given scanline.
    @param ph     the horizon determined by findGreenHorizon
*/

void Field::findConvexHull(int pH) {
    //point<int> convex[HULLS];
    initialScanForTopGreenPoints(pH);
    // now do the Graham scanning algorithm
    int M = 2;
    for (int i = 2; i < HULLS; i++) {
        while (ccw(convex[M-1], convex[M], convex[i]) <= 0 && M >= 1) {
            M--;
        }
        M++;
        point<int> temp = convex[M];
        convex[M] = convex[i];
        convex[i] = temp;
    }
    // when we apply Graham scanning there can be minor problems at each end
    int diffy = convex[2].y - convex[1].y;
    int diffx = convex[2].x - convex[1].x;
    float steps = 0.0f;
    // this shouldn't happen, but let's be sure
    if (diffx != 0) {
        steps = (float)diffy / (float)diffx;
    }
    int diffx2 = convex[1].x - convex[0].x;
    float project = (float)convex[1].y - (float)diffx2 * steps;
    if (convex[1].x < 50 && convex[0].y - (int)project > 5) {
        convex[0].y = (int)project;
    }
    // do the same for the right edge
    if (M > 3) {
        diffx = convex[M-1].x - convex[M-2].x;
        diffy = convex[M-1].y - convex[M-2].y;
        if (diffx != 0) {
            steps = (float)diffy / (float)diffx;
        } else {
            steps = 0.0f;
        }
        diffx2 = convex[M].x - convex[M-1].x;
        project = (float)convex[M-1].y + (float)diffx2 * steps;
        if (convex[M-1].x > IMAGE_WIDTH - 1 - 50 && convex[M].y - (int)project > 5) {
            convex[M].y = (int)project;
        }
    }
    findTopEdges(M);

}

int Field::ccw(point<int> p1, point<int> p2, point<int> p3) {
    return (p2.x - p1.x)*(p3.y - p1.y) - (p2.y - p1.y)*(p3.x - p1.x);
}

/* We start getting the convex hull by getting a good estimate of the field
   horizon.  The first thing we do is get the post estimated horizon and
   use it as an initial estimate.  Here we take that estimate and try to
   improve it.  Normally the real field horizon will be below the estimated
   horizon.  So we just do some simple scanning down from the pose estimate
   until we find enough evidence of the field.  The provides our
   initial estimate.
   @param pH     the post estimate horizon
   @return       a new estimate of the horizon line
*/
int Field::getInitialHorizonEstimate(int pH) {
    const int MIN_PIXELS_INITIAL = 8;
    const int MIN_PIXELS_HARDER = 20;
    const int SCAN_INTERVAL_X = 10;
    const int SCAN_INTERVAL_Y = 4;
    //variable definitions
    int run, greenPixels, scanY;
    register int i, j;
    unsigned char pixel; //, lastPixel;
	int best = -1;

    int pixelsNeeded = MIN_PIXELS_HARDER;
    if (pH < -100) {
        pixelsNeeded = MIN_PIXELS_INITIAL;
    }
    horizon = -1;             // our calculated horizon
    run = 0;                 // how many consecutive green pixels have I seen?
    greenPixels = 0;         // count for any given line
    scanY = 0;                 // which line are we scanning
    int firstpix = 0;
	/*int find = IMAGE_HEIGHT - 1;
	// see if there are any gaps that are relatively far away
	while (thresh->getPixDistance(find) < 250 && find > 0) {
		find--;
	}
	if (find > 0) {
	}*/
    // we're going to do this backwards of how we used to - we start at the pose
    // horizon and scan down.  This will provide an initial estimate
    for (j = max(0, pH); j < IMAGE_HEIGHT && horizon == -1; j+=SCAN_INTERVAL_Y) {
        // reset values for each scan
        greenPixels = 0;
        run = 0;
        scanY = 0;
        // we do a scan based on the slope provided by pose
        // and we only look at every 10th pixel
        for (i = 0; i < IMAGE_WIDTH && scanY < IMAGE_HEIGHT && scanY > -1
                 && greenPixels <= pixelsNeeded; i+= SCAN_INTERVAL_X) {
            //pixel = thresh->thresholded[scanY][i];
            pixel = thresh->getColor(i, scanY);
            // project the line to get the next y value
            scanY = thresh->yellow->yProject(0, j, i);
            if (Utility::isGreen(pixel)) {
                greenPixels++;
                // since green pixels are likely to be next to other ones
                i -= SCAN_INTERVAL_X;
                i++;
            }
        }
        // once we see enough green we're done
        if (greenPixels > pixelsNeeded) {
			return j;
		}
    }
    return j;
}

/* Once we have a decent estimate of the field horizon we can work a
   little harder to improve it.  We do that here.  This is pretty much
   the same as the previous method except that we do it more precisely.
   @param  horizon    the estimate of the horizon
   @return            the new estimate
*/
int Field::getImprovedEstimate(int horizon) {
    // we're more demanding of Green because there is so much
    const int MIN_GREEN_SIZE = 10;
    const int MIN_PIXELS_PRECISE = 20;
    //variable definitions
    int run, greenPixels, scanY, firstpix = 0;
    register int i, j;
    unsigned char pixel; //, lastPixel;
    // we should have a base estimate, let's move it up
    int k, l, minpix = IMAGE_WIDTH, minpixrow = -1;
    // scan back toward the pose estimated horizon
    for (k = min(horizon, IMAGE_HEIGHT - 2); k > -1; k-=4) {
        // this is basically identical to the initial method except we're going up
        // Also, since we're trying to be precise we scan more and
        // have to worry about where the projection of the line goes
        greenPixels = 0;
        run = 0;
        scanY = 0;
        int maxRun = 0;
        for (l = max(0, firstpix - 5), firstpix = -1; l < IMAGE_WIDTH && scanY <
                 IMAGE_HEIGHT && scanY > -1 && run < MIN_GREEN_SIZE &&
                 (greenPixels < MIN_PIXELS_PRECISE || maxRun < 5); l+=3) {
            if (debugHorizon) {
                vision->drawPoint(l, scanY, BLACK);
            }
            unsigned char newPixel = thresh->getColor(l, scanY);
            //int newPixel = thresh->thresholded[scanY][l];
            if (Utility::isGreen(newPixel)) {
                // firstpix tracks where we saw the first green pixel
                if (firstpix == -1) {
                    firstpix = l;
                    // now because of slopes the min location could be anywhere
                    if (l <= minpix) {
                        minpix = l;
                        minpixrow = k;
                    }
                }
                run++;
                if (run > maxRun) {
                    maxRun = run;
                }
                greenPixels++;
            } else {
                run = 0;
            }
            // next Y value in this scan
            scanY = thresh->yellow->yProject(0, k, l);
        }
        // now check how we did in this scanline - remember now we are
        // looking for the first line where we DON'T have lots of green
        if (run < MIN_GREEN_SIZE && (greenPixels < MIN_PIXELS_PRECISE ||
                                     maxRun < 5)) {
            // first make sure we didn't get fooled by firstpix
            run = 0;
            scanY = firstpix;
            for (j = firstpix - 1; j >= 0; j--) {
                if (scanY < 0) {
                    scanY = 0;
                }
                if (scanY > IMAGE_HEIGHT) {
                    scanY = IMAGE_HEIGHT;
                }
                if (debugHorizon) {
                    vision->drawPoint(j, scanY, BLACK);
                }
                pixel = thresh->getColor(j, scanY);
                if (Utility::isGreen(pixel)) {
                    run++;
                    greenPixels++;
                    firstpix = j;
                }
                scanY = thresh->yellow->yProject(0, k, j);
            }
            // if we still meet the criteria then we are done
            if (run < MIN_GREEN_SIZE && greenPixels < MIN_PIXELS_PRECISE) {
                if (debugHorizon) {
                    // cout << "Found horizon " << k << " " << run << " "
                    //      << greenPixels << endl;
                    vision->drawPoint(100, k + 1, BLACK);
                    vision->drawLine(minpix, minpixrow, firstpix, k + 2, RED);
                }
                horizon = k + 2;
                return horizon;
            }
        }
    }
    return 0;
}

/*
 *    Our goal is to determine the basic shape of the field.
 *    We start with a guess provided by our pose estimate.  We use
 * this as a baseline and try to improve it by doing a series of
 * scans going down until we find the field.
 *
 * This method also serves to initialize new field information for
 * the current image.
 * @param pH    the pose estimated horizon
 * @param sl    the pose estimated slope
 * @return      the new horizon estimate
 */

int Field::findGreenHorizon(int pH, float sl) {
    // re init shooting info
    for (int i = 0; i < IMAGE_WIDTH; i++) {
        topEdge[i] = 0;
        shoot[i] = true;
    }
    // store field pose
    poseHorizon = pH;
    /*if (pH < -100) {
      horizon = 0;
      return 0;
      }*/
    /*estimate e = vision->pose->pixEstimate(IMAGE_WIDTH / 2, pH, 0.0f);
      cout << "Dist is " << e.dist << " " << pH << endl;
      if (e.dist > 1000.0) {
      while (e.dist > 1000.0) {
      pH = pH + 5;
      e = vision->pose->pixEstimate(IMAGE_WIDTH / 2, pH, 0.0f);
      }
      cout << "new Ph is " << pH << endl;
      }*/
    // get an initial estimate
    int initialEstimate = getInitialHorizonEstimate(pH);
    if (debugHorizon) {
        cout << "initial estimate is " << initialEstimate << " " << pH << endl;
    }
    // improve the estimate
    horizon = getImprovedEstimate(initialEstimate);
    // calculate the convex hull
    findConvexHull(horizon);
    return horizon;
}

/* Calculate where in the goal the most open space is.  A work in perpetual
   progress.
*/
void Field::bestShot(VisualFieldObject* left,
                     VisualFieldObject* right,
                     VisualCrossbar* middle, int color)
{
    const int bigSize = 10;
    const int topBuff = 20;
    const int newHeight = 20;

    // start by setting boundaries
    int leftb = 0, rightb = IMAGE_WIDTH - 1, bottom = 0;
    int rl = 0, rr = 0;
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
    middle->setLeftTopX(leftb);
    middle->setLeftBottomX(leftb);
    middle->setRightTopX(rightb);
    middle->setRightBottomX(rightb);
    middle->setLeftBottomY(bottom);
    middle->setRightBottomY(bottom);
    middle->setLeftTopY(bottom - 10);
    middle->setRightTopY(bottom - 10);
    middle->setWidth(static_cast<float>(rightb - leftb + 1));
    middle->setHeight(10);
}

/* The horizon at the given x value.  Eventually we'll be changing this to
 * return a value based upon the field edges.
 * @param x        column to find the horizon in
 * @return        projected value
 */

int Field::horizonAt(int x) {
    if (thresh->usingTopCamera) {
        if (x < 0 || x >= IMAGE_WIDTH) {
            if (debugHorizon) {
                cout << "Problem in horizon " << x << endl;
            }
            if (x < 0) {
                return topEdge[0];
            } else {
                return topEdge[IMAGE_WIDTH - 1];
            }
        }
        return topEdge[x];
    }
    else
        return 0;
}

/* Project a line given a start coord and a new y value - note that this is
 * dangerous depending on how you do the projection.
 *
 * @param startx   the x point to start at
 * @param starty   the y point to start at
 * @param newy       the y point to end at
 * @return           the corresponding x point
 */
int Field::xProject(int startx, int starty, int newy)
{
    //slope is a float representing the slope of the horizon.
    return startx - ROUND2(slope * (float)(newy - starty));
}

/* Project a line given a start coord and a new y value - note that this is
 * dangerous depending on how you do the projection.
 *
 * @param point       the point to start at
 * @param newy       the y point to end at
 * @return           the corresponding x point
 */
int Field::xProject(point <int> point, int newy) {
    //slope is a float representing the slope of the horizon.
    return point.x - ROUND2(slope * (float)(newy - point.y));
}

/* Project a line given a start coord and a new x value
 * @param startx   the x point to start at
 * @param starty   the y point to start at
 * @param newx       the x point to end at
 * @return           the corresponding y point
 */
int Field::yProject(int startx, int starty, int newx)
{
    return starty + ROUND2(slope * (float)(newx - startx));
}

/* Project a line given a start coord and a new x value
 * @param point       the point to start at
 * @param newx       the x point to end at
 * @return           the corresponding y point
 */
int Field::yProject(point <int> point, int newx)
{
    return point.y + ROUND2(slope * (float)(newx - point.x));
}


void Field::drawLess(int x, int y, int c)
{
    const int lineBuff = 10;

#ifdef OFFLINE
    vision->drawLine(x, y, x + lineBuff, y - lineBuff, c);
    vision->drawLine(x, y, x + lineBuff, y + lineBuff, c);
    vision->drawLine(x + 1, y, x + lineBuff + 1, y - lineBuff, c);
    vision->drawLine(x + 1, y, x + lineBuff + 1, y + lineBuff, c);
#endif
}

void Field::drawMore(int x, int y, int c)
{
    const int lineBuff = 10;

#ifdef OFFLINE
    vision->drawLine(x, y, x - lineBuff, y - lineBuff, c);
    vision->drawLine(x, y, x - lineBuff, y + lineBuff, c);
    vision->drawLine(x - 1, y, x - lineBuff - 1, y - lineBuff, c);
    vision->drawLine(x - 1, y, x - lineBuff - 1, y + lineBuff, c);
#endif
}

}
}
