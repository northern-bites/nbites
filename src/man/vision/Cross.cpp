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
 * This is where we do all field cross processing.
 *
 */

#include <iostream>
#include "Cross.h"
#include "debug.h"

//using namespace std;

#ifdef OFFLINE
static bool CROSSDEBUG = false;
#else
static bool CROSSDEBUG = false;
#endif

Cross::Cross(Vision* vis, Threshold* thr, Field* fie)
    : vision(vis), thresh(thr), field(fie)
{
    const int MAX_CROSS_RUNS = 400;
    blobs = new Blobs(MAX_CROSS_RUNS);
    allocateColorRuns();
}


/* Initialize the data structure.
 * @param s     the slope corresponding to the robot's head tilt
 */
void Cross::init()
{
    blobs->init();
    numberOfRuns = 0;
}

/* This is the entry  point for cross detection called in Threshold.cc
 */

void Cross::createObject() {
    // TODO:  These were thrown together in an hour.  They should
    // be more rigorously determined
    const int maxWidth = 75;
    const int maxHeight = 75;
    const int minWidth = 5;
    const int minHeight = 5;
    const int maxRatio = 5;

    // do basic run-length encoding
    if (numberOfRuns > 1) {
        for (int i = 0; i < numberOfRuns; i++) {
            // search for contiguous blocks
            int nextX = runs[i].x;
            int nextY = runs[i].y;
            int nextH = runs[i].h;
            blobs->blobIt(nextX, nextY, nextH);
        }
    }

    if (CROSSDEBUG){
        cout << blobs->number() << " white blobs" << endl;
    }

    // loop through all the blobs and test the ones that are the right
    // basic size
    for (int i = 0; i < blobs->number(); i++) {
        Blob candidate = blobs->get(i);
        if (CROSSDEBUG) {
            cout << "Blob " << candidate.width() << " " << candidate.height() << endl;
            cout << "Coords " << candidate.getLeft() << " " << candidate.getTop() << endl;
        }
        if (candidate.width() < maxWidth && candidate.height() < maxHeight &&
            candidate.width() > minWidth && candidate.height() > minHeight &&
            candidate.width() < maxRatio * candidate.height()  &&
            candidate.height() < maxRatio * candidate.width()) {
            checkForX(candidate);
        }
    }
}

/* This method takes in a candidate field cross and decides if it is good enough.
   We run it through a series of sanity checks.  If it passes them all then
   it is called a cross.
*/


void Cross::checkForX(Blob b) {

    const float greenThreshold = 0.75f;
    int x = b.getLeftTopX();
    int y = b.getLeftTopY();
    int w = b.width();
    int h = b.height();
    int count = 0, counter = 0;
    // First we scan the outside of the blob.  It should basically be all
    // green.  What we don't want are line fragments or robot fragments
    // so finding white is very bad.

    // first scan the sides
    for (int i = max(0, y - 2); i < min(IMAGE_HEIGHT - 1, y + h + 2); i++) {
        if (x > 3) {
            if (thresh->thresholded[i][x - 4] == GREEN)
                count++;
            else if (thresh->thresholded[i][x - 4] == WHITE)
                count-=3;
            counter++;
        } else return;

        if (x + w + 4 < IMAGE_WIDTH) {
            if (thresh->thresholded[i][x + w+ 4] == GREEN)
                count++;
            else if (thresh->thresholded[i][x + w+ 4] == WHITE)
                count-=3;
            counter++;
        } else return;
    }

    // now scan above and below
    for (int i = max(0, x - 2); i < min(IMAGE_WIDTH - 1, x + w + 2); i++) {
        if (y > 1) {
            if (thresh->thresholded[y - 2][i] == GREEN)
                count++;
	    else if (thresh->thresholded[y - 2][i] == GREY)
	      count--;
            else if (thresh->thresholded[y - 2][i] == WHITE)
                count-=3;
            counter++;
        } else return;

        if (y + h + 2 < IMAGE_HEIGHT) {
            if (thresh->thresholded[y+h+2][i] == GREEN)
                count++;
            else if (thresh->thresholded[y+h+2][i] == WHITE)
                count-=3;
            counter++;
        } else return;
    }

    if (CROSSDEBUG) {
        cout << "Have a candidate white blob " << x << " " << y << endl;
    }

    const int HORIZONCHECK = 15;
    // make sure we aren't too close to the horizon
    if (y - HORIZONCHECK < field->horizonAt(x) && field->horizonAt(x) > 0) return;
    if (y - HORIZONCHECK < field->horizonAt(x+w) && field->horizonAt(x+w) > 0) return;

    if (CROSSDEBUG) {
        cout << "Passed Horizon checks " << endl;
        cout << "White green information " << count << " " << counter << endl;
    }

    // if we pass the basic threshold then make sure it isn't a line
    if (count > (float)counter * greenThreshold) {
        // first make sure this isn't really a line
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

            if (foo.first != Utility::NO_INTERSECTION && foo.second != Utility::NO_INTERSECTION) {
                if (CROSSDEBUG)
                    cout << "Throwing out blob that is part of a line" << endl;
                return;
            }
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
void Cross::newRun(int x, int y, int h)
{
    const int RUN_VALUES = 3;    // x, y, and h of course

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
    const int RUN_VALUES = 3;         // x, y and h
    const int RUNS_PER_SCANLINE = 15;
    const int RUNS_PER_LINE = 5;

    int run_num = RUN_VALUES;
    run_num = IMAGE_WIDTH * RUNS_PER_SCANLINE;
    runsize = IMAGE_WIDTH * RUNS_PER_LINE;
    runs = (run*)malloc(sizeof(run) * run_num);
}

/* Checks out how much of the blob is of the right color.
 * If it is enough returns true, if not false.
 * @param tempobj     the cross we're checking
 * @param minpercent  how good it needs to be
 * @return            was it good enough?
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
                if (thresh->thresholded[ny][nx] == WHITE) {
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
