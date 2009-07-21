
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
	debugFieldEdge = true;
#else
	debugHorizon = false;
	debugFieldEdge = false;
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
 */

int Field::findGreenHorizon(int pH) {
	const int MIN_PIXELS_INITIAL = 2;
	const int SCAN_INTERVAL_X = 10;
	const int SCAN_INTERVAL_Y = 4;
    // we're more demanding of Green because there is so much
	const int MIN_GREEN_SIZE = 10;
	const int MIN_PIXELS_PRECISE = 20;

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


