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
 * This is where we do all field cross processing.
 *
 */

#include <iostream>
#include "Cross.h"
#include "debug.h"

using namespace std;

Cross::Cross(Vision* vis, Threshold* thr, Field* fie, Context* con)
: vision(vis), thresh(thr), field(fie), context(con)
{
    const int MAX_CROSS_RUNS = 400;
    blobs = new Blobs(MAX_CROSS_RUNS);
    allocateColorRuns();

#ifdef OFFLINE
    CROSSDEBUG = false;
#endif
}


/* Initialize the data structure.
 * @param s		the slope corresponding to the robot's head tilt
 */
void Cross::init()
{
    blobs->init();
    numberOfRuns = 0;
}

/* This is the entry  point for cross detection called in Threshold.cpp.
   Now only builds blobs for use here and in Robots.cpp.  The actual
   checking occurs in the next method.
 */

void Cross::createObject() {
    // do basic run-length encoding
    if (numberOfRuns > 1) {
        for (int i = 0; i < numberOfRuns; i++) {
            // search for contiguous blocks
            int nextX = runs[i].x;
            int nextY = runs[i].y;
            int nextH = runs[i].h;
            blobs->blobIt(nextX, nextY, nextH, false);
        }
    }

    if (CROSSDEBUG){
        cout << blobs->number() << " white blobs" << endl;
    }
}

/* See if any of our candidate blobs are actually worthy crosses.
 */

void Cross::checkForCrosses() {
    // TODO:  These were thrown together in an hour.  They should
    // be more rigorously determined
    const int maxWidth = 75;
    const int maxHeight = 75;
    const int minWidth = 5;
    const int minHeight = 5;
    const int maxRatio = 5;

    // loop through all the blobs and test the ones that are the right
    // basic size
    for (int i = 0; i < blobs->number(); i++) {
        Blob candidate = blobs->get(i);
        if (CROSSDEBUG) {
            cout << "Blob " << candidate.width() << " " << candidate.height() <<
				endl;
            cout << "Coords " << candidate.getLeft() << " " <<
				candidate.getTop() << endl;
            vision->drawRect(candidate.getLeft(), candidate.getTop(),
                             candidate.width(), candidate.height(), BLACK);
        }
        if (candidate.width() < maxWidth && candidate.height() < maxHeight &&
                candidate.width() > minWidth && candidate.height() > minHeight &&
                candidate.width() < maxRatio * candidate.height()  &&
                candidate.height() < maxRatio * candidate.width()) {
            checkForX(candidate);
        }
    }
}

/* Returns whether true when the size of the cross is reasonable when tested
   against the pixEstimated distance.
   @param b		the candidate cross
   @return		if the size is reasonable
 */
bool Cross::checkSizeAgainstPixEstimate(Blob b) {
    int x = b.getLeftTopX();
    int y = b.getLeftTopY();
    int w = b.width();
    int h = b.height();
    // before we spend a lot of time processing, let's see if it is a reasonable size
    if (w > 3 * h || h > 2 * w) {
        return false;
    }
    estimate e = vision->pose->pixEstimate(x, y, 0.0);
	if (CROSSDEBUG) {
		cout << "Distance check: " << e.dist << " " << w << endl;
	}
    if (e.dist < 100.0f && w < 20) {
        return false;
    } else if (e.dist < 150.0f && w < 12) {
        return false;
    } else if (e.dist < 200.0f && w < 8) {
        return false;
    }
    if (e.dist > 200.0f && w > 20) {
        return false;
    }
    return true;
}

/* Scans around the outside of the blob looking for green.	Ideally the
   cross will have only green around it.  White is a big problem, so is
   having the cross near the edge.	This is because our version of the
   cross is so simple - we don't look at its shape at all!	Instead we
   just look for white blobs with the right general properties.
   @param b	  the candidate cross
   @return	  the amount of green in the perimeter (white gives a huge
        negative penalty)
 */
bool Cross::scanAroundPerimeter(Blob b) {
    const float greenThreshold = 0.75f;
    int x = b.getLeftTopX();
    int y = b.getLeftTopY();
    int w = b.width();
    int h = b.height();
    int counter = 0, count = 0;
    // first scan the sides
    for (int i = max(0, y - 2); i < min(IMAGE_HEIGHT - 1, y + h + 2); i++) {
        if (x > 3) {
            if (Utility::isGreen(thresh->getThresholded(i,x - 4))) {
                count++;
            } else if (Utility::isWhite(thresh->getThresholded(i,x - 4))) {
                count-=3;
            }
            counter++;
        } else return false;

        if (x + w + 4 < IMAGE_WIDTH) {
            if (Utility::isGreen(thresh->getThresholded(i,x + w+ 4))) {
                count++;
            } else if (Utility::isWhite(thresh->getThresholded(i,x + w+ 4))) {
                count-=3;
            }
            counter++;
        } else return false;
    }

    // now scan above and below
    for (int i = max(0, x - 2); i < min(IMAGE_WIDTH - 1, x + w + 2); i++) {
        if (y > 1) {
            if (Utility::isGreen(thresh->getThresholded(y - 2,i))) {
                count++;
            } else if (Utility::isUndefined(thresh->getThresholded(y - 2,i))) {
                count--;
            } else if (Utility::isWhite(thresh->getThresholded(y - 2,i))) {
                count-=3;
            }
            counter++;
        } else return false;

        if (y + h + 2 < IMAGE_HEIGHT) {
            if (Utility::isGreen(thresh->getThresholded(y+h+2,i))) {
                count++;
            } else if (Utility::isWhite(thresh->getThresholded(y+h+2,i))) {
                count-=3;
            }
            counter++;
        } else return false;
    }
    if (count > (float)counter * greenThreshold) {
        if (CROSSDEBUG) {
            cout << "White stats: " << count << " " << counter << endl;
        }
        return true;
    }
    return false;
}

/* Determines if a line intersects the candidate cross, if so it
   is thrown out.
   @param b	  the candidate cross
   @return	  whether a line intersects
 */
bool Cross::checkForLineIntersection(Blob b) {
    int x = b.getLeftTopX();
    int y = b.getLeftTopY();
    int w = b.width();
    int h = b.height();
    point <int> plumbLineTop, plumbLineBottom, line1start, line1end;
    plumbLineTop.x = x + w / 2; plumbLineTop.y = y;
    plumbLineBottom.x = x; plumbLineBottom.y = y + h;
    const vector <boost::shared_ptr<VisualLine> >* lines =
            vision->fieldLines->getLines();

    for (vector <boost::shared_ptr<VisualLine> >::const_iterator k =
            lines->begin();
            k != lines->end(); k++) {
        pair<int, int> foo = Utility::plumbIntersection(plumbLineTop,
                                                        plumbLineBottom,
                                                        (*k)->getStartpoint(),
                                                        (*k)->getEndpoint());

        if (foo.first != Utility::NO_INTERSECTION && foo.second !=
			Utility::NO_INTERSECTION) {
            if (CROSSDEBUG)
                cout << "Throwing out blob that is part of a line" << endl;
            return true;
        }
    }
    return false;
}

/* This method takes in a candidate field cross and decides if it is good enough.
   We run it through a series of sanity checks.	 If it passes them all then
   it is called a cross.
   @param b		  the candidate cross
 */

void Cross::checkForX(Blob b) {

    int x = b.getLeftTopX();
    int y = b.getLeftTopY();
    int w = b.width();
    int h = b.height();
    int count = 0, counter = 0;
    // before we spend a lot of time processing, let's see if it is a reasonable size
    if (!checkSizeAgainstPixEstimate(b)) {
        return;
    }

    if (CROSSDEBUG) {
        cout << "Have a candidate white blob " << x << " " << y << endl;
    }

    // First we scan the outside of the blob.  It should basically be all
    // green.  What we don't want are line fragments or robot fragments
    // so finding white is very bad.
    if (!scanAroundPerimeter(b)) {
        return;
    }

    const int HORIZONCHECK = 15;
    // make sure we aren't too close to the horizon
    if (y - HORIZONCHECK < field->horizonAt(x) && field->horizonAt(x) > 0) return;
    if (y - HORIZONCHECK < field->horizonAt(x+w) && field->horizonAt(x+w) > 0) return;

    if (CROSSDEBUG) {
        cout << "Passed Horizon checks " << endl;
    }

    // first make sure this isn't really a line
    if (checkForLineIntersection(b)) {
        return;
    }

    // Is the cross white enough?  At least half the pixels must be white.
    if (!rightBlobColor(b, 0.5f)) {
        if (CROSSDEBUG) {
            cout << "Tossing a blob for not being white enough " << endl;
        }
        return;
    }

    // passed all of our current sanity checks
    if (CROSSDEBUG) {
        cout << "Found a cross " << endl;
        b.printBlob();
    }

    // Make sure we don't have more than one candidate cross.  Note:  we
    // actually can see two crosses at some places on the field, but for
    // now we just will ID one of them.
    // TODO:  allow seeing two crosses - but must test whether they are
    // correctly aligned with each other, etc.
    if (vision->cross->getWidth() > 0) {
        if (w * h > vision->cross->getWidth() * vision->cross->getHeight()) {
            vision->cross->updateCross(&b);
            if (CROSSDEBUG) {
                cout << "Larger than previous cross." << endl;
            }
        } else {
            if (CROSSDEBUG) {
                cout << "Threw out extra cross - smaller " << endl;
            }
        }
    } else {
        vision->cross->updateCross(&b);
    }
}

//returns the blobs data structure containing white blob info
Blobs* Cross::getBlobs(){
    return blobs;
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
void Cross::newRun(int x, int y, int h)
{
    const int RUN_VALUES = 3;	 // x, y, and h of course

    if (numberOfRuns < runsize) {
        int last = numberOfRuns - 1;
        // skip over noise --- jumps over two pixel noise currently.
        //HW--added CONSTANT for noise jumps.
        if (last > 0 && runs[last].x == x &&
                (runs[last].y - (y + h) <= NOISE)) {
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

/* Allocate the required amount of memory dependent on the primary color
 */
void Cross::allocateColorRuns()
{
    const int RUN_VALUES = 3;		  // x, y and h
    const int RUNS_PER_SCANLINE = 15;
    const int RUNS_PER_LINE = 5;

    int run_num = RUN_VALUES;
    run_num = IMAGE_WIDTH * RUNS_PER_SCANLINE;
    runsize = IMAGE_WIDTH * RUNS_PER_LINE;
    runs = (run*)malloc(sizeof(run) * run_num);
}

/* Checks out how much of the blob is of the right color.
 * If it is enough returns true, if not false.
 * @param tempobj	  the cross we're checking
 * @param minpercent  how good it needs to be
 * @return			  was it good enough?
 */
bool Cross::rightBlobColor(Blob tempobj, float minpercent) {
    int x = tempobj.getLeft();
    int y = tempobj.getTop();
    int spanX = tempobj.width();
    int spanY = tempobj.height();
    if (spanX < 1 || spanY < 1) return false;
    int ny, nx, starty, startx;
    int good = 0, total = 0;

    for (int i = 0; i < spanY; i++) {
        starty = y + i;
        startx = x;

        for (int j = 0; j < spanX; j++) {
            nx = startx + j;
            ny = starty;

            if (ny > -1 && nx > -1 && ny < IMAGE_HEIGHT && nx < IMAGE_WIDTH) {
                total++;
                if (Utility::isWhite(thresh->getThresholded(ny,nx))) {
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



