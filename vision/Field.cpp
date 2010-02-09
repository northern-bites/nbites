
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
 * This file processes vision with respect to the field.
 * The primary objectives of this processing are to determine where
 * in the visual field the field is and, if possible, where the edges
 * of the field are.  The former is achieved in the method
 * findGreenHorizon where the latter is done in findFieldEdges.
 *
 * findGreenHorizon works by starting at the pose calculated horizon.
 * Then it does some horizontal scanning every 4th line looking for the
 * first place where there is a threshold amount of green.  From there
 * it just tightens up the bound.
 *
 * findFieldEdges uses information found during thresholding - namely
 * the highest point in each column where green was seen during thresholding.
 * Its goal is basically to construct lines in much the same manner as
 * in field lines.  However, this problem is more highly constrained
 * and therefore is considerably simpler.  Since we get all of our
 * "line points" in order and don't have to worry about horizontal/
 * vertical issues we can avoid a lot of the checking necessary for
 * normal lines.  Essentially we scan through the points attempting to
 * grow line segments.  We stop any given segment when new points are
 * not part of the segment.  Once we have our segments we then analyze
 * them to see if they can be combined (e.g. a robot occluded a line)
 * and also whether they are truly at the field edge.  To do this we
 * project the line to each end of the image and make sure that none
 * of our other points lie "above" our projected line.  Normally the
 * end result should be no more than two lines (and a corner at their
 * intersection).
 */

#include "ifdefs.h"
#include "Common.h"

#include <math.h>
#include <assert.h>
#if ROBOT(NAO_SIM)
#  include <aldefinitions.h>
#endif
#include <boost/shared_ptr.hpp>

#include "Field.h"
#include "debug.h"

using namespace std;
using boost::shared_ptr;

// Constructor for Field class. passed an instance of Vision and Pose
Field::Field(Vision* vis, Threshold * thr)
    : vision(vis), thresh(thr)
{
#ifdef OFFLINE
	debugHorizon = false;
	debugFieldEdge = false;
	openField = false;
	debugShot = false;
#else
	debugHorizon = false;
	debugFieldEdge = false;
	openField = false;
	debugShot = false;
#endif
}

/*
 */

void Field::findFieldEdges() {
	// build field edge segments
	const int DISCONTINUOUS = 5;          // max difference between points
	const int MIN_FIELD_SEG = 25;         // minimum line segment size
	const int CLOSE_SLOPE = 5;            // fudge factor for jaggy lines
	const float CLOSE_LINE_SLOPE = 0.15f; // threshold for combining lines

	int startSeg = 0;
	int width, firstTrend = -1, currentTrend = -1, pairs = 0, points = 0;
	int noise = 0;
	point<int> linePoints[20];
	int trends[10];
	float percentGreen;

	for (int i = 5; i < IMAGE_WIDTH; i++) {
		// check for a discontinuity - note may need to upgrade this for horizontalish
		// edges
		if (abs(thresh->greenEdgePoint(i) - thresh->greenEdgePoint(i-1)) >
			DISCONTINUOUS || i == IMAGE_WIDTH - 1 ||
			thresh->greenEdgePoint(i) > IMAGE_HEIGHT - 3 ||
			thresh->greenEdgePoint(i) < 2) {
			// ends current line segment
			width = i - 1 - startSeg;
			if (width > MIN_FIELD_SEG &&
				(i > IMAGE_HEIGHT - 3 ||
				 thresh->greenEdgePoint(i) > thresh->greenEdgePoint(i-1))) {

				// create the line segment - store end points
				linePoints[points++] = point<int>(startSeg,
												  thresh->greenEdgePoint(startSeg));
				linePoints[points++] = point<int>(i - 1, thresh->greenEdgePoint(i-1));
				if (debugFieldEdge) {
					cout << "Adding " << linePoints[points-2] << " " <<
						linePoints[points-1] << endl;
				}
				trends[pairs++] = firstTrend; // tracks slope
				startSeg = i;
				noise = 0;
			} else {
				// there was no viable segment, so start fresh
				startSeg = i;
				noise = 0;
			}
		} else if (i - startSeg >= 10) {
			// get slope of last 10 points
			currentTrend = thresh->greenEdgePoint(i) - thresh->greenEdgePoint(i-10);
			// check green unless we're at the top of the image
			if (thresh->greenEdgePoint(startSeg) > 3 || thresh->greenEdgePoint(i) > 3) {
				percentGreen = vision->fieldLines->percentColorBetween(
					startSeg, thresh->greenEdgePoint(startSeg), i,
					max(0, thresh->greenEdgePoint(i) - 2), GREEN);
			} else percentGreen = 0.0f;

			// slopes must be close and there can't be a lot of green between
			if (abs(currentTrend - firstTrend > CLOSE_SLOPE) || percentGreen > 50.0f) {
				width = i - 1 - startSeg;
				if (width > MIN_FIELD_SEG) {
					// the last segment was viable
					linePoints[points++] = point<int>(startSeg,
													  thresh->greenEdgePoint(startSeg));
					linePoints[points++] = point<int>(i - 5, thresh->greenEdgePoint(i-5));
					if (debugFieldEdge) {
						cout << "Adding " << linePoints[points-2] << " "
							 << linePoints[points-1] << endl;
					}
					trends[pairs++] = firstTrend;
					startSeg = i;
				} else {
					// we allow a little bit of noise - but only one point
					if (noise > 0) {
						startSeg = i - 2;
						noise = 0;
					} else noise++;
				}
			} else if (i - startSeg == 10) {
				// set the value for the first 10 points in the new segment
				firstTrend = thresh->greenEdgePoint(i) - thresh->greenEdgePoint(startSeg);
				noise = 0;
			}
		}
	}

	// now go through the line segments and see if we can combine them
	float slope1, slope2;
	for (int i = 1; i < pairs; i++) {
		if (abs(trends[i] - trends[i-1]) < CLOSE_SLOPE) {
			// we need to project the lines to check if they are parallel
			// so first we need slopes
			slope1 = (float)(linePoints[(i-1)*2+1].y - linePoints[(i-1)*2].y) /
				(float)(linePoints[(i-1)*2+1].x - linePoints[(i-1)*2].x);
			// now project the line to the current column
			int yproject = linePoints[(i-1)*2].y +
				ROUND2(slope1 * (float)(linePoints[i*2].x - linePoints[(i-1)*2].x));
			// if the projection is close then the lines may be the same
			if (abs(yproject - linePoints[i*2].y) < 4) {
				// ok so they intersect, now we need to know if they are a corner
				// get the slope of the 2d line
				slope2 = (float)(linePoints[(i)*2+1].y - linePoints[(i)*2].y) /
					(float)(linePoints[(i)*2+1].x - linePoints[(i)*2].x);
				if (fabs(slope1 - slope2) < CLOSE_LINE_SLOPE) {
					// combine them
					linePoints[i*2] = linePoints[(i-1)*2];
					linePoints[(i-1)*2] = point<int>(-1, -1);
				} else {
					// at this point it is important to note the word "potential"
					// there is still a lot of checking to do
					if (debugFieldEdge) {
						cout << "Potential corner detected " << endl;
						thresh->drawPoint(linePoints[i*2].x-1,
										  linePoints[i*2].y, ORANGE);
					}
				}
			}
		}
	}

	// now do some checking to see if these are reasonable field edges
	// first project out the line and see if any serious green is above it anywhere
	for (int i = 0; i < pairs; i++) {
		// skip lines that were subsumed by other lines
		if (linePoints[i*2].x != -1) {
			// We assume that large lines are ok unless they are high in the image
			if (linePoints[i*2+1].x - linePoints[i*2].x < IMAGE_WIDTH / 2 ||
				horizon < 50) {

				// get the slope of the line
				slope1 = (float)(linePoints[(i)*2+1].y - linePoints[(i)*2].y) /
					(float)(linePoints[(i)*2+1].x - linePoints[(i)*2].x);
				// project the line to the left edge and scan back to the start
				for (int j = 0; j < linePoints[i*2].x && linePoints[i*2].x != -1; j++) {
					int y0 = linePoints[i*2].y + ROUND2(slope1 *
														(float)(j - linePoints[i*2].x)) - 5;
					// if the green in this column is above our line then it isn't a field edge
					if (y0 > thresh->greenEdgePoint(j) &&
						thresh->greenEdgePoint(j) < IMAGE_HEIGHT - 2) {
						// mark it as an unusable segment
						linePoints[i*2].x = -1;
					}
				}
				// do the same thing on the right side of the line
				for (int j = linePoints[i*2].x; j < IMAGE_WIDTH && linePoints[i*2].x != -1;
					 j++) {

					// project the y value of the line in this column
					int y0 = linePoints[i*2].y + ROUND2(slope1 *
														(float)(j - linePoints[i*2].x)) - 5;
					// check it against the highest value seen
					if (y0 > thresh->greenEdgePoint(j) &&
						thresh->greenEdgePoint(j) < IMAGE_HEIGHT - 2) {
						// unusable segment
						linePoints[i*2].x = -1;
					}
				}
			}
		}
	}

	// Display the detected edges
	if (debugFieldEdge) {
		for (int i = 0; i < pairs; i++) {
			if (linePoints[i*2].x != -1) {
				thresh->drawLine(linePoints[i*2].x, linePoints[i*2].y,
								 linePoints[i*2+1].x, linePoints[i*2+1].y, BLACK);
				thresh->drawLine(linePoints[i*2].x, linePoints[i*2].y+1,
								 linePoints[i*2+1].x, linePoints[i*2+1].y+1, BLACK);
			}
		}
	}
}

/*
 *  Our goal is to determine the basic shape of the field.
 *  We start with a guess provided by our pose estimate.  We use
 * this as a baseline and try to improve it by doing a series of
 * scans going down until we find the field.
 *
 * This method also serves to initialize new field information for
 * the current image.
 */

int Field::findGreenHorizon(int pH, float sl) {
	const int MIN_PIXELS_INITIAL = 2;
	const int SCAN_INTERVAL_X = 10;
	const int SCAN_INTERVAL_Y = 4;
    // we're more demanding of Green because there is so much
	const int MIN_GREEN_SIZE = 10;
	const int MIN_PIXELS_PRECISE = 20;

	slope = sl;
	// re init shooting info
    for (int i = 0; i < IMAGE_WIDTH; i++)
        shoot[i] = true;
    //variable definitions
    int run, greenPixels, scanY;
    register int i, j;
    unsigned char pixel; //, lastPixel;

    horizon = -1;            // our calculated horizon
    run = 0;                 // how many consecutive green pixels have I seen?
    greenPixels = 0;         // count for any given line
    scanY = 0;               // which line are we scanning
    int firstpix = 0;
    // we're going to do this backwards of how we used to - we start at the pose
    // horizon and scan down.  This will provide an initial estimate
    for (j = pH; j < IMAGE_HEIGHT && horizon == -1; j+=SCAN_INTERVAL_Y) {
		// reset values for each scan
        greenPixels = 0;
        run = 0;
        scanY = 0;
		// we do a scan based on the slope provided by pose
		// and we only look at every 10th pixel
        for (i = 0; i < IMAGE_WIDTH && scanY < IMAGE_HEIGHT && scanY > -1
                 && greenPixels < 3; i+= SCAN_INTERVAL_X) {
            pixel = thresh->thresholded[scanY][i];
            if (pixel == GREEN) {
                greenPixels++;
            }
			// project the line to get the next y value
            scanY = thresh->blue->yProject(0, j, i);
        }
		// once we see enough green we're done
        if (greenPixels > MIN_PIXELS_INITIAL) {
            break;
        }
    }
    // we should have a base estimate, let's move it up
    int k, l, minpix = IMAGE_WIDTH, minpixrow = -1;
	if (debugHorizon) {
		cout << "initial estimate is " << j << " " << pH << endl;
	}
	// set our initial estimate
    horizon = j;
	// now scan back toward the pose estimated horizon
    for (k = min(horizon, IMAGE_HEIGHT - 2); k > -1; k-=4) {
		// this is basically identical to above except we're going up
		// Also, since we're trying to be precise we scan more and
		// have to worry about where the projection of the line goes
        greenPixels = 0;
        run = 0;
        scanY = 0;
        for (l = max(0, firstpix - 5), firstpix = -1; l < IMAGE_WIDTH && scanY <
                 IMAGE_HEIGHT && scanY > -1 && run < MIN_GREEN_SIZE &&
                 greenPixels < MIN_PIXELS_PRECISE; l+=3) {
			if (debugHorizon) {
				thresh->drawPoint(l, scanY, BLACK);
			}
            int newPixel = thresh->thresholded[scanY][l];
            if (newPixel == GREEN) {
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
                greenPixels++;
            } else {
                run = 0;
            }
			// next Y value in this scan
            scanY = thresh->blue->yProject(0, k, l);
        }
		// now check how we did in this scanline - remember now we are
		// looking for the first line where we DON'T have lots of green
        if (run < MIN_GREEN_SIZE && greenPixels < MIN_PIXELS_PRECISE) {
            // first make sure we didn't get fooled by firstpix
            run = 0;
            scanY = firstpix;
            for (j = firstpix - 1; j >= 0; j--) {
                if (scanY < 0) {
                    //cout << "scanY < 0, value is: " << scanY << endl;
                    scanY = 0;
                }
                if (scanY > IMAGE_HEIGHT) {
                    //cout << "scanY > IMAGE_HEIGHT, value is: " << scanY << endl;
                    scanY = IMAGE_HEIGHT;
                }

                int newPixel = thresh->thresholded[scanY][j];
				if (debugHorizon) {
					thresh->drawPoint(j, scanY, BLACK);
				}
                if (newPixel == GREEN) {
                    run++;
                    greenPixels++;
                    firstpix = j;
                }
                scanY = thresh->blue->yProject(0, k, j);
            }
			// if we still meet the criteria then we are done
            if (run < MIN_GREEN_SIZE && greenPixels < MIN_PIXELS_PRECISE) {
				if (debugHorizon) {
					cout << "Found horizon " << k << " " << run << " "
						 << greenPixels << endl;
					thresh->drawPoint(100, k + 1, BLACK);
					thresh->drawLine(minpix, minpixrow, firstpix, k + 2, RED);
				}
                horizon = k + 2;
                return horizon;
            }
        }
    }
    horizon = 0;
	return horizon;
}

/* Shooting stuff */

/* Determines shooting information. Basically scans down from backstop and looks
 * for occlusions.
 * Sets the information found within the backstop data structure.
 * @param one the backstop
 */

void Field::setShot(VisualCrossbar* one, int color)
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
        const vector <boost::shared_ptr<VisualLine> >* lines = vision->fieldLines->getLines();
        crossings = 0;
        for (vector < shared_ptr<VisualLine> >::const_iterator k = lines->begin();
             k != lines->end(); k++) {
            pair<int, int> foo = Utility::
                plumbIntersection(plumbLineTop, plumbLineBottom,
                                  (*k)->start, (*k)->end);
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
                if (debugShot) {
                    //drawPoint(i, j, RED);
                }
            }
            if (run > MAX_RUN_SIZE && (pix == NAVY || pix == RED)) {
                shoot[i] = false;
                if (debugShot)
                    thresh->drawPoint(i, j, RED);
            }
            if (run > MAX_RUN_SIZE) {
                shoot[i] = false;
                if (debugShot) {
                    thresh->drawPoint(i, j, RED);
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
        if (debugShot) {
            thresh->drawLine(r1, ly, r1, IMAGE_HEIGHT - 1, RED);
            thresh->drawLine(r2, ly, r2, IMAGE_HEIGHT - 1, RED);
        }
    }
    one->setBackLeft(r1);
    one->setBackRight(r2);

    if (debugShot) {
        thresh->drawPoint(r1, ly, RED);
        thresh->drawPoint(r2, ly, BLACK);
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
    if (debugShot) {
        cout << "Crossbar info: Left Col: " << r1 << " Right Col: " << r2
             << " Dir: " << one->getBackDir();
        if (one->shotAvailable())
            cout << " Take the shot!" << endl;
        else
            cout << " Don't shoot!" << endl;
    }
}


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
	setShot(middle, color);
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
void Field::openDirection(int horizon, NaoPose *pose)
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
            if (openField) {
                thresh->drawPoint(IMAGE_WIDTH / 2, i, MAROON);
            }
            sixty = i;
        }
        lastd = (int)d.dist;
    }
    const vector <boost::shared_ptr<VisualLine> >* lines = vision->fieldLines->getLines();
    for (int x = SCAN_DIVISION; x < IMAGE_WIDTH - 1; x += SCAN_DIVISION) {
        bad = 0; white = 0; grey = 0; run = 0; greyrun = 0;
        open[(int)(x / SCAN_DIVISION)] = horizon;
        open2[(int)(x / SCAN_DIVISION)] = horizon;
        // first - determine if any lines intersection this plumbline and where
        point <int> plumbLineTop, plumbLineBottom, line1start, line1end;
        plumbLineTop.x = x; plumbLineTop.y = 0;
        plumbLineBottom.x = x; plumbLineBottom.y = IMAGE_HEIGHT;
        crossings = 0;
        for (vector <shared_ptr<VisualLine> >::const_iterator k = lines->begin();
             k != lines->end(); k++) {
            pair<int, int> foo = Utility::
                plumbIntersection(plumbLineTop, plumbLineBottom,
                                  (*k)->start, (*k)->end);
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
                    if (openField) {
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


    if (openField) {
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
        thresh->drawLine(0, open2[index12], IMAGE_WIDTH / LINE_DIVIDER, open2[index12],
				 MAROON);
        thresh->drawLine(IMAGE_WIDTH / LINE_DIVIDER, open2[index22], 2 * IMAGE_WIDTH /
				 LINE_DIVIDER,
                 open2[index22], MAROON);
        thresh->drawLine(2 * IMAGE_WIDTH / LINE_DIVIDER, open2[index32],
				 IMAGE_WIDTH  - 1,
                 open2[index32], MAROON);
        thresh->drawLine(0, open2[index12] - 1, IMAGE_WIDTH / LINE_DIVIDER,
				 open2[index12] - 1,
                 MAROON);
        thresh->drawLine(IMAGE_WIDTH / LINE_DIVIDER, open2[index22] - 1,
				 2 * IMAGE_WIDTH / LINE_DIVIDER,
                 open2[index22] - 1, MAROON);
        thresh->drawLine(2 * IMAGE_WIDTH / LINE_DIVIDER, open2[index32] - 1,
				 IMAGE_WIDTH  - 1,
                 open2[index32] - 1, MAROON);
        if (open2[index12] != open2[index22]) {
            thresh->drawLine(IMAGE_WIDTH / LINE_DIVIDER, open2[index12],
					 IMAGE_WIDTH / LINE_DIVIDER,
                     open2[index22], MAROON);
        }
        if (open2[index32] != open2[index22]) {
            thresh->drawLine(2 * IMAGE_WIDTH / LINE_DIVIDER, open2[index32],
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

/* The horizon at the given x value.  Eventually we'll be changing this to
 * return a value based upon the field edges.
 * @param x     column to find the horizon in
 * @return      projected value
 */

int Field::horizonAt(int x) {
    return yProject(0, horizon, x);
}

/* Project a line given a start coord and a new y value - note that this is
 * dangerous depending on how you do the projection.
 *
 * @param startx   the x point to start at
 * @param starty   the y point to start at
 * @param newy     the y point to end at
 * @return         the corresponding x point
 */
int Field::xProject(int startx, int starty, int newy)
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
int Field::xProject(point <int> point, int newy) {
    //slope is a float representing the slope of the horizon.
    return point.x - ROUND2(slope * (float)(newy - point.y));
}

/* Project a line given a start coord and a new x value
 * @param startx   the x point to start at
 * @param starty   the y point to start at
 * @param newx     the x point to end at
 * @return         the corresponding y point
 */
int Field::yProject(int startx, int starty, int newx)
{
    return starty + ROUND2(slope * (float)(newx - startx));
}

/* Project a line given a start coord and a new x value
 * @param point    the point to start at
 * @param newx     the x point to end at
 * @return         the corresponding y point
 */
int Field::yProject(point <int> point, int newx)
{
    return point.y + ROUND2(slope * (float)(newx - point.x));
}


void Field::drawLess(int x, int y, int c)
{
    const int lineBuff = 10;

#ifdef OFFLINE
    thresh->drawLine(x, y, x + lineBuff, y - lineBuff, c);
    thresh->drawLine(x, y, x + lineBuff, y + lineBuff, c);
    thresh->drawLine(x + 1, y, x + lineBuff + 1, y - lineBuff, c);
    thresh->drawLine(x + 1, y, x + lineBuff + 1, y + lineBuff, c);
#endif
}

void Field::drawMore(int x, int y, int c)
{
    const int lineBuff = 10;

#ifdef OFFLINE
    thresh->drawLine(x, y, x - lineBuff, y - lineBuff, c);
    thresh->drawLine(x, y, x - lineBuff, y + lineBuff, c);
    thresh->drawLine(x - 1, y, x - lineBuff - 1, y - lineBuff, c);
    thresh->drawLine(x - 1, y, x - lineBuff - 1, y + lineBuff, c);
#endif
}
