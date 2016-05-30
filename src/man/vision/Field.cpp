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

using namespace std;
using boost::shared_ptr;

namespace man {
namespace vision {

// Constructor for Field class.
	Field::Field(int w, int h, FieldHomography *hom)
		:width(w),
		 height(h),
		 homography(hom)
	{
		topCamera = true;
		currentX = 0;
		currentY = 0;
#ifdef OFFLINE
		debugFieldEdge = false;
		debugDrawFieldEdge = false;
		debugHorizon = false;
		drawCameraHorizon = false;
		debugRansac = false;
#endif
	}

	void Field::setDebugImage(DebugImage * di) {
		debugDraw =  *di;
		//cout << "Set debug " << (int)di << endl;
	}

	void Field::setImages(ImageLiteU8 white, ImageLiteU8 green, ImageLiteU8 orange,
		ImageLiteU16 yImg) {
		whiteImage = white;
		greenImage = green;
		orangeImage = orange;
		yImage = yImg;
	}


	void Field::getColor(int x, int y) {
		currentX = x;
		currentY = y;
	}

	bool Field::isGreen() {
		if (*(greenImage.pixelAddr(currentX, currentY)) > 138) {
			return true;
		}
		return false;
	}

	bool Field::isWhite() {
		if (*(whiteImage.pixelAddr(currentX, currentY)) > 128 &&
			*(yImage.pixelAddr(currentX, currentY)) < 350) {
			return true;
		}
		return true;
	}

	bool Field::isOrange() {
		if (*(orangeImage.pixelAddr(currentX, currentY)) > 128) {
			return true;
		}
		return true;
	}

	bool Field::isUndefined() {
		if (!isOrange() && !isWhite() && !isGreen()) {
			return true;
		}
		return false;
	}

	float Field::getPixDistance(int x) {
		double x1, x2, y1, y2;
		// field coords wants things specified relative to the center of the image
		int offset = height / 2 - x;
		y1 = static_cast<double>(offset);
		if (homography->fieldCoords(0.0, y1, x1, y2)) {
			//cout << "Calculating distance " << x << " " << y2 << endl;
			return static_cast<float>(y2);
		}
		return 100000.0f;
	}

	bool Field::isNavy() {
		return false;
	}

	void Field::drawPoint(int x, int y, int c) {
		debugDraw.drawPoint(x, y, c);
	}

	void Field::drawLine(int x, int y, int x1, int x2, int c) {
		debugDraw.drawLine(x, y, x1, x2, c);
	}

	void Field::drawDot(int x, int y, int c) {
		debugDraw.drawDot(x, y, c);
	}

/* As part of finding the convex hull, we need to know where the
   top points of green are at each scanline.  We calculate them
   here.  Just start at the horizon in each scanline and scan
   down until we find enough green.  Then store the result in
   the convex array.
   Note: I (Chown) have long wanted to change this to use the results
   of the last vision frame.
   Note: While this works very well, it seems absurdly complicated for
   what ought to be a reasonably easy thing to do. The main problems that
   complicate it are: other fields in the distance, misc. green stuff,
   dark portions of the field that aren't "green", and occluding robots.
   @param pH    the horizon found by findGreenHorizon
*/
void Field::initialScanForTopGreenPoints(int pH) {
	int good, ok, top;
	unsigned char pixel;
	int topGreen = 0;         // topmost green Y value
	int greenRun = 0;         // connected green pixels
    float possible = 0.0f;
	int lastGreen;            // y value of last seen green pixel
    const float BUFFER = 100.0f; // other fields should be farther than this
	// we need a better criteria for what the top is
	for (int i = 0; i < HULLS; i++) {
		good = 0;      // good == # of green pixels
		ok = 0;        // ok == # of pixels that could be green (e.g. orange, white)

		// based on our pose we scan differently (tilt) - determine highest pixel
		//int poseProject = thresh->yellow->yProject(0, pH, i * SCANSIZE);
		int poseProject = pH;
		if (poseProject <= 0) {
			poseProject = 0;
		} else if (pH == 0) {
			poseProject = 0;
		}
		topGreen = height - 1;
		greenRun = 0;
		lastGreen = 0;
		// scan from the top point down
		for (top = max(poseProject, poseHorizon);
				good < RUNSIZE && top < height; top++) {
			// scan until we find a run of green pixels
			int x = i * SCANSIZE;           // scan column
			if (i == HULLS - 1) {
				x--;
			}
			if (debugFieldEdge) {
				if (top == max(poseProject, 0)) {
					drawPoint(x, top, BLUE);
				}
			}
			//pixel = getColor(x, top);
			getColor(x, top);
            // watch out for patches of green off the field
            if (topGreen != height - 1 &&
				top - lastGreen  >  5 &&
                possible - getPixDistance(top) > BUFFER / 2) {
				if (debugFieldEdge) {
					cout << "Detected bad patch " << x << " " << top << " " << lastGreen << endl;
				}
                topGreen = height - 1;
            }
			//pixel = thresh->thresholded[top][x];
			if (isGreen()) {
				// we're looking at a good pixel, figure out if we have enough evidence
				lastGreen = top;
				good++;
				greenRun++;
				if ((greenRun > 3 || good == RUNSIZE) && topGreen == height - 1) {
					topGreen = top - greenRun;
                    possible = getPixDistance(topGreen);
					if (debugFieldEdge) {
						//cout << "Setting topGreen " << x << " " << topGreen << endl;
						drawPoint(x, topGreen, BLUE);
					}
				}
                // before we finish make sure we haven't seen another field
                if (good == RUNSIZE && topGreen != height - 1) {
                    float topDist = getPixDistance(topGreen);
                    float newDist = getPixDistance(top);
                    if (topDist > BUFFER) {
						int check = top;
						int greens = 0;
						int check2 = top;
						int whites = 0;
						int blues = 0;
						bool found = false;
						while ((getPixDistance(check) > BUFFER ||
								check < height / 2) && !found && check < height - 1) {
							check++;
							//pixel = getColor(x, check);
							getColor(x, check);
							greens = 0;
							while (isGreen() && check < height - 1) {
								check++;
								greens++;
								//pixel = getColor(x, check);
								getColor(x, check);
							}
							if (greens >= 15) {
								check2 = check;
								found = true;
							} else if (greens > 2) {
								check2 = check;
							}
							while (!isGreen() && check < height - 1) {
								check++;
								greens++;
								if (isWhite()) {
									whites++;
								}
								// NEWVISION
								/*if (isBlue()) {
									blues++;
									}*/
								//pixel = getColor(x, check);
								getColor(x, check);
							}
							if (getPixDistance(check2) - getPixDistance(check)
								> BUFFER / 2 && check - check2 > 5 &&
								(check - check2 - whites > 4 || blues > 6) && !found) {
								if (debugFieldEdge) {
									cout << "Unsetting top green " <<
										(getPixDistance(check2) - getPixDistance(check))
										 << endl;
								}
								topGreen = height - 1;
								top = check;
								good = 1;
								greenRun = 1;
								check = height - 1;
							} else {
								//good += greens;
							}
						}
                    }
                }
			} else if (isOrange() || isWhite()) {
				//good++;
				greenRun = 0;
			} else if (isUndefined()) {
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
		if (good >= RUNSIZE) {
			convex[i] = point<int>(i * SCANSIZE, topGreen);
			if (poseProject < 0 && topGreen < 10) {
				convex[i] = point<int>(i * SCANSIZE, 0);
            }
        } else {
            convex[i] = point<int>(i * SCANSIZE, height);
        }
        if (debugFieldEdge) {
            drawPoint(i * SCANSIZE, convex[i].y, MAROON);
        }
    }
    // look for odd spikes and quell them - e.g. someone wearing green shirt
    if (poseHorizon > -100) {
		const int BARRIER = 15;
        for (good = 4; good < HULLS - 4; good++) {
            if (convex[good-1].y - convex[good].y > BARRIER &&
				convex[good+1].y - convex[good].y > BARRIER &&
				convex[good-2].y - convex[good].y > BARRIER &&
				convex[good+2].y - convex[good].y > BARRIER) {
                if (debugFieldEdge) {
                    cout << "Spike at " << convex[good].x << " " << convex[good].y <<
                        endl;
                }
                convex[good].y = convex[good-1].y;
            }
            if (-convex[good-1].y + convex[good].y > BARRIER &&
				-convex[good+1].y + convex[good].y > BARRIER &&
				-convex[good-2].y + convex[good].y > BARRIER &&
				-convex[good+2].y + convex[good].y > BARRIER) {
                if (debugFieldEdge) {
                    cout << "Dip at " << convex[good].x << " " << convex[good].y <<
                        endl;
                }
                convex[good].y = convex[good-1].y;
            }
        }
	}
	// prepare for the convex hull algorithm
    for (good = 0; convex[good].y == height && good < HULLS; good++) {}
    if (good < HULLS) {
        for (int i = good-1; i > -1; i--) {
            convex[i].y = convex[i+1].y;
        }
        for (good = HULLS - 1; convex[good].y == height && good > 0; good--) {}
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

/* This function takes in an x value in field coordinates and calculates
 * the relative field edge y point (also in field coordinates) if it can be
 * determined.
 * @param x   value in global coordinates
 * @param y   back the field edge in global coordinates (if possible)
 * @return    if calculation was possible, false otherwise
 */
bool Field::onField(double x, double & y) {
    for (int i = 1; i <= numberOfHulls; i++) {
		if (convexWorld[i-1].x < x && convexWorld[i].x > x) {
			// interpolate the y's
			double diff = convexWorld[i].x - convexWorld[i-1].x;
			double diffy = convexWorld[i].y - convexWorld[i-1].y;
			double stepPercent = (x - convexWorld[i-1].x) / diff ;
			double finalDiffY = diffy * stepPercent;
			y = convexWorld[i-1].y + finalDiffY;
			return true;
		}
	}
	y = 0;
	return false;
}

/* At this point we have found our convex hull as defined for the scanlines.
   We want to extend this information to all possible x values.  So here we
   calculate all of those values, doing some simple interpolating to find
   most of them. Now we also determine where the field edge is occluded
   @param M     the number of sides of the convex hull
*/
void Field::findTopEdges(int M) {
    // interpolate the points in the hull to determine values for every scanline
	numberOfHulls = M;
    topEdge[0] = convex[0].y;
	topBlock[0] = blockages[0].y;
    float maxPix = 0.0f;
    //estimate e;
	peak = -1;
	float maxLine = 1000.0f;
	for (int i = 0; i <= M; i++) {
		double x1, x2, y1, y2;
		// field coords wants things specified relative to the center of the image
		int offsety = height / 2 - convex[i].y;
		int offsetx = convex[i].x - width / 2;
		y1 = static_cast<double>(offsety);
		x1 = static_cast<double>(offsetx);
		// convert the convex hull in image coordinates to field coordinates
		homography->fieldCoords(x1, y1, convexWorld[i].x, convexWorld[i].y);
		if (debugFieldEdge) {
			cout << "Convex Hull: " << convex[i].x << " " << convex[i].y << " "
				 << convexWorld[i].x << " " << convexWorld[i].y <<
				" " << offsety << " " << offsetx << endl;
		}
	}
	// This loop does the horizon calculations given the convex hull info
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
                    drawPoint(j, (int)cur, BLUE);
                } else {
                    drawPoint(j, (int)cur, RED);
                }
            }
        }
        if (debugDrawFieldEdge) {
			cout << "Calling drawline" << endl;
            drawLine(convex[i-1].x, convex[i-1].y, convex[i].x,
					 convex[i].y, ORANGE);
        }
    }
	// this loop determines where the field edge is occluded - or where there are obstacles
    for (int i = 1; i < HULLS; i++) {
		int greens = 0;
		// do a little extra checking to be sure a robot arm didn't partially occlude it
		for (int j = topEdge[blockages[i].x]; j < blockages[i].y; j++) {
            //unsigned char pixel = getColor(blockages[i].x, j);
			getColor(blockages[i].x, j);
            // project the line to get the next y value
            if (isGreen() && !isNavy()) {
				greens++;
				if (greens > 4) {
					blockages[i].y = j;
					break;
				}
			} else {
				greens--;
			}
		}
        int diff = blockages[i].y - blockages[i-1].y;
        float step = 0.0f;
        if (blockages[i].x != blockages[i-1].x) {
            step = (float)diff / (float)(blockages[i].x - blockages[i-1].x);
        }
        float cur = static_cast<float>(blockages[i].y);
        for (int j = blockages[i].x; j > blockages[i-1].x; j--) {
            cur -= step;
            topBlock[j] = (int)cur;
            if (debugDrawFieldEdge) {
				drawDot(j, (int)cur, WHITE);
            }
        }
        if (debugDrawFieldEdge) {
			drawLine(blockages[i-1].x, blockages[i-1].y, blockages[i].x,
					 blockages[i].y, BLACK);
        }
    }
}

/* Provides a rough guidline to whether the horizon slopes to the right or left
   or is roughly horizontal;
 */
int Field::findSlant() {
	if (topEdge[0] > topEdge[height - 1] + 30) {
		return 1;
	} else if (topEdge[height - 1] > topEdge[0] + 30) {
		return -1;
	}
	return 0;
}

/* Find the field edges if possible and turn them into lines.
   Heavily borrowed from the UNSW implementation of the same.
 */

void Field::findFieldEdgeLines(unsigned int *seed) {
	// Set up some constants based on the camera
	const int COLS = width;
	const int ROWS = height;
	int density = SCANSIZE;
	int cols = width;

	/* some magic constants (see RANSAC documentation) */
	static const unsigned int k = 40;
	static const float        e = 8.0;
	static const unsigned int n = 8; //(140 / density);

	/* If there are n or greater green tops, which usually fall below
	 * the field edge, above a field line, invalidate it
	 */
	static const int invalidateIfNTops  = 40  / density;

	unsigned int i;

	std::vector<PointI> *edgePoints;       // points where green found
	std::vector<PointI> edgePoints2;       // subset for possible 2d line
	edgePoints = &edgePointsTop;

	std::vector<bool> *cons, consBuf[2];
	consBuf[0].resize(cols * 2);           // which points are in line
	consBuf[1].resize(cols * 2);           // points in 2d line

	std::vector<RANSACLine> lines(1);      // holds two potential lines
	RANSACLine result;

	RANSAC::Generator<RANSACLine> g;       // generates possible lines
	RANSAC::Ransac<RANSACLine> ransac;     // tests possibilities

	int minX = 0;// = min(lines[i].p1.x(), lines[i].p2.x());
	int maxX = width;// = max(lines[i].p1.x(), lines[i].p2.x());

	// Check if we have a legal ransac line for this image
	if (ransac(g, *edgePoints, &cons, lines[0], k, e, n, consBuf, seed)) {

		/**
		 * Line found, now try to find a second line. We could see a corner
		 * First remove points belonging to the first line
		 * Then run RANSAC again
		 */
		for (i = 0; i < edgePoints->size(); ++i) {
			if (! (*cons)[i]) {
				edgePoints2.push_back(edgePoints->at(i));
			} else {
				minX = min(edgePoints->at(i).x(), minX);
				maxX = max(edgePoints->at(i).x(), maxX);
			}
		}

		lines.resize(2);  // make room in vector for 2d line
		if (! ransac(g, edgePoints2, &cons, lines[1], k, e, n, consBuf, seed)) {
			lines.resize(1);  // on failure trim vector
		}

	} else {
		// no line found
		lines.resize(0);
	}

	// Sanity checks

	/* Invalidate any line with too much green above it */
	std::vector<RANSACLine>::iterator line;
	for (line = lines.begin(); line != lines.end(); ++ line) {
		int n;
		if (line->t1 == 0) {
			n = greenTops[HULLS - 1];
		} else {
			/* Solve for x when y = SALIENCY_DENSITY. At y = 0 aliased green
			 * pixels from a legitimate field edge can be along the top of the
			 * image
			 *
			 * t1x + t2y + t3 = 0
			 * x = -(t3 + t2*SALIENCY_DENSITY) / t1
			 */
			int x;
			//if (top) {
			x = -(line->t3 + line->t2 * density) / line->t1;
			//} else {
			//   x = -(line->t3 + line->t2*(density+ROWS)) /line->t1;
			//}

			/* Convert to saliency image resolution */
			x /= density;

			if (x < 0 || x >= HULLS) {
				n = greenTops[HULLS - 1];
			} else {
				bool positiveSlope = (line->t1 >= 0) == (line->t2 >= 0);
				if (positiveSlope) {
					n = greenTops[x];
				} else {
					n = greenTops[HULLS - 1] - greenTops[x];
				}
			}
		}
		if (n >= invalidateIfNTops) {
			if (debugRansac) {
				cout << "Ransac line invalidated for too much green above " <<
					n << " "  << endl;
			}
			line = lines.erase(line) - 1;
		}
	}

	// possible intersection point
	float x_intercept;
	float y_intercept;

	if (lines.size() == 2) {
		if (debugRansac) {
			cout << "Two possible lines" << endl;
		}
		/**
		 * begin sanity checks, sort lines by gradient
		 */
		RANSACLine *l1;
		RANSACLine *l2;

		if (abs(lines[0].t2 * lines[1].t1) < abs(lines[1].t2 * lines[0].t1)) {
			l1 = &lines[0];
			l2 = &lines[1];
		} else {
			l1 = &lines[1];
			l2 = &lines[0];
		}

		/**
		 * sanity check 1: avoid lines that intersect outside the image
		 */
		//llog(DEBUG2) << "sanity check 1: intersect inside frame" << endl;
		x_intercept = (l1->t3*l2->t2 - l2->t3*l1->t2) /
			(float)(l1->t2*l2->t1 - l1->t1*l2->t2);
		y_intercept = (l1->t3*l2->t1 - l2->t3*l1->t1)/
			(float)(l1->t1*l2->t2 - l2->t1*l1->t2);

		if (debugRansac) {
			cout << "intersection is at " << x_intercept << " " << y_intercept << endl;
		}

		if (x_intercept < 0 || x_intercept > COLS
			|| y_intercept < 0 || y_intercept > (2*ROWS))
		{
			lines.pop_back();
		}

		if (lines.size() == 2) {
			// TODO: write a working version of this
			/**
			 * sanity check 2: avoid lines that are close to being parallel
			 */
			//llog(DEBUG2) << "sanity check 2: angle between lines" << endl;
			/*RANSACLine rrl1 = convRR->convertToRRLine(*l1);
			  RANSACLine rrl2 = convRR->convertToRRLine(*l2);
			  float m1 = -rrl1.t1 / (float) rrl1.t2;
			  float m2 = -rrl2.t1/ (float) rrl2.t2;
			  float theta = atan(abs((m1 - m2) / (1 + m1 * m2)));
			  // float deviation = M_PI / 2 - theta;
			  // if (deviation > MAX_EDGE_DEVIATION_FROM_PERPENDICULAR)
			  const int MIN_ANGLE_BETWEEN_EDGE_LINES = 10;
			  if (theta < MIN_ANGLE_BETWEEN_EDGE_LINES)
			  {
			  lines.pop_back();
			  //llog(DEBUG2) << "Failed sanity check 2" << std::endl;
			  //llog(DEBUG2) << "m1: " << m1 << ", m2: " << m2 << ", theta: " << RAD2DEG(theta) << std::endl;
			  }*/
		}
	}

	int smaller = 1;
	if (lines.size() == 2) {
		int minX1 = min(lines[0].p1.x(), lines[0].p2.x());
		int minX2 = min(lines[1].p1.x(), lines[1].p2.x());
		if (minX1 < minX2) {
			smaller = 0;
		}
	}

	for (i = 0; i < lines.size(); ++ i) {
		if (debugRansac) {
			cout << "Outputting ransac line " << " " << lines[i].p1.x() << " " <<
				lines[i].p1.y() << " " << lines[i].p2.x() << " " <<
				lines[i].p2.y() << endl;
		}
		// Extend the line to include the bounds of the consensus set
		int size = edgePoints->size();
		if (i == 1) {
			size = edgePoints2.size();
		}
		int count = 0;
		if (lines.size() == 2) {
			if (i == smaller) {
				maxX = x_intercept;
				minX = 0;
			} else {
				minX = x_intercept;
				maxX = width - 1;
			}
		} else {
			/*for (int k = 0; k < consBuf[1].size(); k++) {
				if (consBuf[1].at(k)) {
					cout << "Got " << k << endl;
				}
			}
			for (int k = 0; k < size; k++) {
				if (consBuf[i].at(k)) {
					count++;
					int curX = edgePoints->at(k).x();
					if (i > 0) {
						curX = edgePoints2.at(k).x();
					}
					minX = min(minX, curX);
					maxX = max(maxX, curX);
				}
				} */
		}
		// next we just make sure our new line covers as much ground as possible
		if (debugRansac) {
			cout << "Min and max " << minX << " " << maxX << " " << count << endl;
		}
		int left = min(lines[i].p1.x(), lines[i].p2.x());
		int right = lines[i].p1.x();
		int leftY = lines[i].p1.y();
		int rightY = lines[i].p2.y();
		if (left == right) {
			right = lines[i].p2.x();
		} else {
			rightY = lines[i].p1.y();
			leftY = lines[i].p2.y();
		}
		int range = right - left;
		int lift = rightY - leftY;
		float slope = (float)(lift) / (float)(range);

		int extendLeft = left - minX;
		int extendRight = maxX - right;
		float minY = leftY - (float)(extendLeft) * slope;
		float maxY = rightY + (float)(extendRight) * slope;
		if (debugRansac) {
			drawLine(minX, (int)(minY), maxX, (int)(maxY), ORANGE);
		}

	}
}

/* Find a rough outline of the edge of the field. We are going to scan
   down from the camera horizon and look for the first clump of green that we
   can find. This will be a point for our ransac algorithm. We then leave it
   to ransac to actually fit the points to a good line or lines.
   @param pH     the pose estimated horizon
 */

void Field::findFieldEdge(int pH) {
	int consecutiveGreen = 0;   // how many green pixels we've seen
	int greenCount = 0;         // how much green overall
	int whiteCount = 0;         // how much white
	const int MINRUN = 6;       // minimimum size run to call our point
	edgePointsTop.clear();      // start with an empty list of points

	for (int i = 0; i < HULLS; i++) {    // we'll track absolute top green points
		greenTops[i] = 0;
	}

	for (int i = 0; i < HULLS; i++) {
		// initialize for this scanline
		if (i != 0) {
			greenTops[i] = greenTops[i-1];
		}
		greenCount = 0;
		whiteCount = 0;
		consecutiveGreen = -1;
		int x = SCANSIZE * i;

		// now scan down from the pose horizon
		for (int j = pH; j < height; j++) {
			// grab the next pixel and check its color
			getColor(x, j);
			// if its green, then go to work
			if (isGreen()) {
				greenCount++;
				consecutiveGreen++;
				whiteCount = 0;

				// when we've seen enough green
				if (greenCount == MINRUN) {
					// find the top point
					j = j - consecutiveGreen;
					// the top of the screen is a special case
					if (j != 0) {
						if (debugRansac) {
							//cout << "Found ransac point " << x << " " << j << endl;
							drawPoint(x, j, WHITE);
						}
						edgePointsTop.push_back(PointI(x, j));
					} else {
						if (debugRansac) {
							cout << "Incrementing green tops " << i << endl;
						}
						++ greenTops[i];
					}
					// we're done with this scan
					break;
				} else if (greenCount > SCANSIZE) {
					j = height;
				}
			} else if (whiteCount < 1 && greenCount > 0) {
				consecutiveGreen = 0;
				whiteCount = 1;
			} else {
				greenCount = 0;
				whiteCount = 0;
				consecutiveGreen = -1;
			}
		}
	}
	// now that we have the ransac points, let's actually find the edges
	unsigned int seed = static_cast<unsigned int>(pH);
	findFieldEdgeLines(&seed);
}

/** Find the convex hull of the field.    We've already found the point of maximal
    green.    Now scan down from that point every SCANLINES lines and find the
    highest point of green in each of those scanlines.     We collect those points
    and perform a Graham-Scan to find their convex hull (see wikipedia.org).
    The convex hull in turn forms our usable horizon at any given scanline.
    @param ph     the horizon determined by findGreenHorizon
*/

void Field::findConvexHull(int pH) {
	findFieldEdge(pH);
    initialScanForTopGreenPoints(pH);

	// save the points we calculated to use for other things such as robot detection
	for (int i = 0; i < HULLS; i++) {
		blockages[i].x = convex[i].x;
		blockages[i].y = convex[i].y;
	}
    // now do the Graham scanning algorithm

	// intersect the bottom of the screen
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
        if (convex[M-1].x > width - 1 - 50 && convex[M].y - (int)project > 5) {
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

    // we're going to do this backwards of how we used to - we start at the pose
    // horizon and scan down.  This will provide an initial estimate
    for (j = max(0, pH); j < height && horizon == -1; j+=SCAN_INTERVAL_Y) {
        // reset values for each scan
        greenPixels = 0;
        run = 0;
        scanY = 0;
        // we do a scan based on the slope provided by pose
        // and we only look at every 10th pixel
        for (i = 0; i < width && scanY < height && scanY > -1
                 && greenPixels <= pixelsNeeded; i+= SCAN_INTERVAL_X) {
            //pixel = thresh->thresholded[scanY][i];
            //pixel = getColor(i, scanY);
			getColor(i, scanY);
            // project the line to get the next y value
			// NEWVISION
            //scanY = thresh->yellow->yProject(0, j, i);
			scanY = j;
            if (isGreen()) {
                greenPixels++;
                // since green pixels are likely to be next to other ones
                i -= SCAN_INTERVAL_X;
                i++;
            }
        }
        // once we see enough green we're done
        if (greenPixels > pixelsNeeded) {
			if (debugHorizon) {
				cout << "Initial horizon: " << j << endl;
			}
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
	int firstHorizon = -1;
    register int i, j;
    unsigned char pixel; //, lastPixel;
    // we should have a base estimate, let's move it up
    int k, l, minpix = width, minpixrow = -1;
    // scan back toward the pose estimated horizon
    for (k = min(horizon, height - 2); k > -1 && k > poseHorizon; k-=4) {
        // this is basically identical to the initial method except we're going up
        // Also, since we're trying to be precise we scan more and
        // have to worry about where the projection of the line goes
        greenPixels = 0;
        run = 0;
        scanY = 0;
        int maxRun = 0;
        for (l = max(0, firstpix - 5), firstpix = -1; l < width && scanY <
                 height && scanY > -1 && run < MIN_GREEN_SIZE &&
                 (greenPixels < MIN_PIXELS_PRECISE || maxRun < 5); l+=3) {
            if (debugHorizon) {
                drawPoint(l, scanY, BLACK);
            }
            //unsigned char newPixel = getColor(l, scanY);
			getColor(l, scanY);
            //int newPixel = thresh->thresholded[scanY][l];
            if (isGreen()) {
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
			// NEWVISION
            //scanY = thresh->yellow->yProject(0, k, l);
			scanY = k;
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
                if (scanY > height) {
                    scanY = height;
                }
                if (debugHorizon) {
                    drawPoint(j, scanY, BLACK);
                }
                //pixel = getColor(j, scanY);
				getColor(j, scanY);
                if (isGreen()) {
                    run++;
                    greenPixels++;
                    firstpix = j;
                }
				// NEWVISION
                //scanY = thresh->yellow->yProject(0, k, j);
				scanY = k;
            }
            // if we still meet the criteria then we are done
            if (run < MIN_GREEN_SIZE && greenPixels < MIN_PIXELS_PRECISE) {
                if (debugHorizon) {
					cout << "Found horizon " << k << " " << run << " "
						 << greenPixels << endl;
                    drawPoint(100, k + 1, BLACK);
                    drawLine(minpix, minpixrow, firstpix, k + 2, RED);
                }
				if (firstHorizon != -1) {
					if (firstHorizon == k + 6) {
						return firstHorizon;
					} else {
						return k + 2;
					}
				} else {
					firstHorizon = k + 2;
				}
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

int Field::findGreenHorizon(int pH, int rH) {
    // re init shooting info
    for (int i = 0; i < width; i++) {
        topEdge[i] = 0;
    }
    // store field pose
    poseHorizon = min(pH, rH);

	if (drawCameraHorizon) {
		cout << "Drawing camera horizon from " << pH << " to " << rH << endl;
		float diff = (float)(rH - pH) / (float)width;
		float start = static_cast<float>(pH);
		for (int i = 0; i < width; i++) {
			start += diff;
			drawDot(i, (int)start, ORANGE);
		}
	}

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

/* The blocked horizon at the given x value. This is a more stringest
 * horizon that gives lower values in the presence of blackages - usually
 * robots or goal posts.
 * @param x        column to find the blocked horizon in
 * @return        projected value
 */

int Field::blockHorizonAt(int x) {
    if (topCamera) {
        if (x < 0 || x >= width) {
            if (debugHorizon) {
                cout << "Problem in blocked horizon " << x << endl;
            }
            if (x < 0) {
                return topBlock[0];
            } else {
                return topBlock[width - 1];
            }
        }
        return topBlock[x];
    }
    else
        return 0;
}


/* The horizon at the given x value.  Eventually we'll be changing this to
 * return a value based upon the field edges.
 * @param x        column to find the horizon in
 * @return        projected value
 */

int Field::horizonAt(int x) {
    if (topCamera) {
        if (x < 0 || x >= width) {
            if (debugHorizon) {
                cout << "Problem in horizon " << x << endl;
            }
            if (x < 0) {
                return topEdge[0];
            } else {
                return topEdge[width - 1];
            }
        }
        return topEdge[x];
    }
    else
        return 0;
}

/* Returns the distance to the edge of the field in the center column of
 * the image.
 */
float Field::fieldEdgeDistanceCenter() {
	int x = width / 2;
	return getPixDistance(horizonAt(x));
}

/* The horizon at the given x value.  Eventually we'll be changing this to
 * return a value based upon the field edges.
 * @param x        column to find the horizon in
 * @return        projected value
 */

int Field::occludingHorizonAt(int x) {
    if (topCamera) {
        if (x < 0 || x >= width) {
            if (debugHorizon) {
                cout << "Problem in occluding horizon " << x << endl;
            }
            if (x < 0) {
                return topBlock[0];
            } else {
                return topBlock[width - 1];
            }
        }
        return topBlock[x];
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
    drawLine(x, y, x + lineBuff, y - lineBuff, c);
    drawLine(x, y, x + lineBuff, y + lineBuff, c);
    drawLine(x + 1, y, x + lineBuff + 1, y - lineBuff, c);
    drawLine(x + 1, y, x + lineBuff + 1, y + lineBuff, c);
#endif
}

void Field::drawMore(int x, int y, int c)
{
    const int lineBuff = 10;

#ifdef OFFLINE
    drawLine(x, y, x - lineBuff, y - lineBuff, c);
    drawLine(x, y, x - lineBuff, y + lineBuff, c);
    drawLine(x - 1, y, x - lineBuff - 1, y - lineBuff, c);
    drawLine(x - 1, y, x - lineBuff - 1, y + lineBuff, c);
#endif
}

}
}
