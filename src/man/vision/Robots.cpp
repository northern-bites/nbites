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
 * Robots.cpp is where we do our robot recognition work in vision.
 */

#include <iostream>
#include "Robots.h"
#include "debug.h"
#include <vector>

using namespace std;

#ifdef OFFLINE
static const bool ROBOTSDEBUG = false;
#else
static const bool ROBOTSDEBUG = false;
#endif

Robots::Robots(Vision* vis, Threshold* thr, Field* fie, Context* con,
               unsigned char col)
                   : vision(vis), thresh(thr), field(fie), context(con), color(col)
{
    const int MAX_ROBOT_RUNS = 400;
    blobs = new Blobs(MAX_ROBOT_RUNS);
    allocateColorRuns();
}


/* Initialize the data structure.
 * @param s     the slope corresponding to the robot's head tilt
 */
void Robots::init()
{
    blobs->init();
    numberOfRuns = 0;
}

/* Set the primary color.  Depending on the color, we have different space needs
 * @param c        the color
 */
void Robots::setColor(unsigned char c)
{
    const int RUN_VALUES = 3;           // x, y, and h
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
void Robots::allocateColorRuns()
{
    const int RUN_VALUES = 3;         // for x, y and h
    const int RUNS_PER_SCANLINE = 15;
    const int RUNS_PER_LINE = 5;

    int run_num = RUN_VALUES;
    // depending on the color we have more or fewer runs available
    run_num = IMAGE_WIDTH * RUNS_PER_SCANLINE;
    runsize = IMAGE_WIDTH * RUNS_PER_LINE;
    runs = (run*)malloc(sizeof(run) * run_num);
}


/* Robot recognition methods
 */

/* Try and recognize robots.  Basically we're doing blobbing here, but with lots
   of extra twists.  Mainly we're being extremely loose with the conditions
   under which we consider blobs to be "connected."  We're trying to take
   advantage of the properties of the robots - namely that they stand vertically
   normally.
   @param cross     The Field cross data structure - contains white blob info
 */

//findRobots is a wrapper that calls findUniforms which in turn calls blobRobots
//to determine an estimate for the entire robot
void Robots::findRobots(Cross* cross){
    findUniforms(cross);
}

void Robots::findUniforms(Cross* cross){
    if (numberOfRuns < 1) return;
    const int lastRunXInit = -30;
    const int resConst = 10;
    const int robotBlobMin = 10;

    int lastRunX = lastRunXInit, lastRunY = 0, lastRunHeight = 0;

    // loop through all of the runs of this color
    for (int i = 0; i < numberOfRuns; i++) {
        //if two runs are closer than resConst together, add space between them to blob
        if (runs[i].x < lastRunX + resConst) {
            for (int k = lastRunX; k < runs[i].x; k++) {
                blobs->blobIt(k, lastRunY, lastRunHeight, true);
            }
        }
        // now we can add the run normally
        blobs->blobIt(runs[i].x, runs[i].y, runs[i].h, true);
        // set the current as the last
        lastRunX = runs[i].x; lastRunY = runs[i].y; lastRunHeight = runs[i].h;
    }
    mergeBigBlobs();

    //sort to examine largest blobs first since they are most likely to be robots
    blobs->sort();

    // Perform sanity checks on each blob to see if each might be part of a robot
    int viable = 0;
    for (int i = 0; i < blobs->number(); i++) {
        if (!sanityChecks(i, cross)) {
            if (blobs->get(i).getRight() > 0) {
                if (debugRobots) {
                    vision->drawRect(blobs->get(i).getLeft(),
                                     blobs->get(i).getTop(),
                                     blobs->get(i).width(),
                                     blobs->get(i).height(),
                                     WHITE);
                }
            }
            blobs->init(i);
        } else {
            if (debugRobots) {
                vision->drawRect(blobs->get(i).getLeft(), blobs->get(i).getTop(),
                                 blobs->get(i).width(), blobs->get(i).height(),
                                 MAROON);
            }
            viable++;
            blobRobots(cross, i);
            updateRobots(viable, i);
        }
    }
}

/* This method is called only on uniform blobs deemed viable by sanity checks.
   Uses the white blob info from cross to determine an area of interest in which
   more rigorous checks will be run to estimate the entire robot's shape. Because
   of the way we do blobbing, robot shape is limited to a bounding box. Essentially
   this method refines the area of interest to cut out green (field) and organge (ball)
   around the edges in the hopes that whats left is robot
   @param  cross    the cross data structure with white blob info
   @param  i        the index of the uniform blob to expand in our blobs data structure
 */

void Robots::blobRobots(Cross* cross, int i) {

    Blob robotBlob = cross->addRobotBlobs(blobs->get(i));
    int left = max(0, robotBlob.getLeft());
    int right = min(robotBlob.getRight(), IMAGE_WIDTH);
    int top = max(0, robotBlob.getTop());
    int bottom = min(robotBlob.getBottom(), IMAGE_HEIGHT);
    int width = right - left;
    int height = bottom - top;

    int nonRobot = 0;

    //uncomment for debugging of corrections. Drawn rect is blob before correction
    //vision->drawRect(left,top, width, height, WHITE);

    //correct left side
    for (int x = left; x < left + (width/2); x++) {
        for (int y = top; y < bottom; y++) {
            if (Utility::isGreen(thresh->getThresholded(y,x)) ||
                Utility::isOrange(thresh->getThresholded(y,x))) {
                nonRobot++;
            }
            if (nonRobot > .4*height) {
                robotBlob.setLeft(x+1);
                nonRobot = -1;
                break;
            }
        }
        if (nonRobot != -1) break;
        nonRobot = 0;
    }

    //correct right side
    for (int x = right; x > left + (width/2); x--) {
        for (int y = top; y < bottom; y++) {
            if (Utility::isGreen(thresh->getThresholded(y,x)) ||
                Utility::isOrange(thresh->getThresholded(y,x))) {
                nonRobot++;
            }
            if (nonRobot > .4*height) {
                robotBlob.setRight(x-1);
                nonRobot = -1;
                break;
            }
        }
        if (nonRobot != -1) break;
        nonRobot = 0;
    }

    //correct top
    for (int y = top; y < top + (height/2); y++) {
        for (int x = left; x < right; x++) {
            if (Utility::isGreen(thresh->getThresholded(y,x)) ||
                Utility::isOrange(thresh->getThresholded(y,x))) {
                nonRobot++;
            };
            if (nonRobot > .4*width) {
                robotBlob.setTop(y+1);
                nonRobot = -1;
                break;
            }
        }
        if (nonRobot != -1) break;
        nonRobot = 0;
    }

    //correct bottom
    for (int y = bottom; y > top + (height/2); y--) {
        for (int x = left; x < right; x++) {
            if (Utility::isGreen(thresh->getThresholded(y,x)) ||
                Utility::isOrange(thresh->getThresholded(y,x))) {
                nonRobot++;
            }
            if (nonRobot > .4*width) {
                robotBlob.setBottom(y-1);
                nonRobot = -1;
                break;
            }
        }
        if (nonRobot != -1) break;
        nonRobot = 0;
    }

    blobs->set(i, robotBlob);
}

/* Robot sanity checks.  Takes a candidate blob and puts it through a
   bunch of tests to make sure it is ok.
   @param  index        index of the blob to check in our blobs data structure
   @param  cross        the cross data structure containing white blob info
   @return              whether we judge it to be reasonable
 */

bool Robots::sanityChecks(int index, Cross* cross) {
    Blob candidate = blobs->get(index);
    //thresholds for smallest allowable blob
    const int blobHeightMin = 3;
    const int blobAreaMin = 15;

    int height = candidate.height();
    int bottom = candidate.getBottom();
    if (candidate.getRight() > 0) {
        //blobs must be large enough
        if (candidate.height()*candidate.width() < blobAreaMin) {
            if (debugRobots){
                cout << "Blob area was too small" << endl;
            }
            return false;
        }

        // uniforms should be wider than they are tall
        if (candidate.height() > candidate.width()) {
            if (debugRobots){
                cout << "Blob was taller than it was wide" << endl;
            }
            return false;
        }

        /*
        //robV added
        //the blob should have the uniform color as its primary color
        if (!Utility::colorsEqual(color, predominantColor(candidate))) {
            if (debugRobots){
                cout << "Blob was not mostly uniform color" << endl;
            }
            return false;
        }
        */

        //a robot cannot be inside another robot
        for (int i = 0; i < blobs->number(); i++){
            if (i == index) continue;
            if (blobs->get(i).contains(candidate)){
                if (debugRobots){
                    cout << "Blob was inside other robot" << endl;
                }
                return false;
            }
        }

        // there ought to be some white below the uniform
        if (bottom < IMAGE_HEIGHT - 10 &&
            !cross->checkForRobotBlobs(candidate)) {
            if (debugRobots) {
                cout << "Bad robot from cross check" << endl;
            }
            return false;
        }
        // the last check was pretty general, let's improve
        if (candidate.getBottom() < IMAGE_HEIGHT - candidate.height() * 2
            && !whiteBelow(candidate)) {
            if (debugRobots) {
                cout << "Got rid for lack of white below" << endl;
            }
            return false;
        }
        if (candidate.getTop() > candidate.height() * 2
            && !whiteAbove(candidate)) {
            if (debugRobots) {
                cout << "Got rid for lack of white above" << endl;
            }
            return false;
        }
        // for some blobs we check even harder for white
        if (height < 2 * blobHeightMin && noWhite(candidate)) {
            if (debugRobots) {
                cout << "Got rid of small one for white" << endl;
            }
            return false;
        }

        if (color == NAVY_BIT && vision->pose->getHorizonY(0) < 0 &&
            notGreen(candidate)) {
            if (debugRobots){
                cout << "Got rid for pose check" << endl;
            }
            return false;
        }
        return true;
    }
    return false;
}


//robV added this
/* Count the number of each color of pixel that might be part of a
   potential uniform blob. We want to make sure the blob is predominantly
   the color of the uniform*/
char Robots::predominantColor(Blob candidate){
    int bottom = candidate.getBottom();
    int top = candidate.getTop();
    int left = candidate.getLeft();
    int right = candidate.getRight();
    int area = candidate.width() * candidate.height() / 5;
    int green = 0;
    int white = 0;
    int red = 0;
    int navy = 0;

    //horizontally scan pixels
    for (int y = top; y < bottom; y++){
        for (int x = left; x < right; x++){
            if (Utility::isNavy(thresh->getThresholded(y, x))) {
                navy++;
            } else if (Utility::isRed(thresh->getThresholded(y, x))) {
                red++;
            } else if (Utility::isGreen(thresh->getThresholded(y, x))) {
                green++;
            } else if (Utility::isWhite(thresh->getThresholded(y, x))) {
                white++;
            }
        }
    }

    int predominantPixelCount = max(max(navy,red),max(green,white));

    if (predominantPixelCount == navy) {
        return NAVY_BIT;
    } else if (predominantPixelCount == red) {
        return RED_BIT;
    } else if (predominantPixelCount == green) {
        return GREEN_BIT;
    } else {
        return WHITE_BIT;
    }

}


/* When we are looking down, the shadowed carpet often has lots of Navy.
   Make sure we aren't just looking at carpet
 */
bool Robots::notGreen(Blob candidate) {
    int bottom = candidate.getBottom();
    int top = candidate.getTop();
    int left = candidate.getLeft();
    int right = candidate.getRight();
    int area = candidate.width() * candidate.height() / 5;
    int greens = 0;
    for (int i = left; i < right; i++) {
        for (int j = top; j < bottom; j++) {
            if (Utility::isGreen(thresh->getThresholded(j, i))) {
                greens++;
                if (greens > area) {
                    return true;
                }
            }
        }
    }
    return false;
}

/* Since the white blob check catches a lot of extra stuff like lines,
   we need to check a bit more carefully.
 */
bool Robots::whiteBelow(Blob candidate) {
    int bottom = candidate.getBottom();
    int height = candidate.height();
    int scanline = bottom + height;
    for (int y = scanline; y < IMAGE_HEIGHT && y < scanline + height; y += 3) {
        int white = 0;
        int green = 0;
        for (int x = candidate.getLeft(); x < candidate.getRight(); x++) {
            if (Utility::isWhite(thresh->getThresholded(y, x))) {
                white++;
            } else if (Utility::isGreen(thresh->getThresholded(y, x))) {
                green++;
            }
        }
        if (green > candidate.width() / 2 && white == 0) {
            return false;
        }
        if (white > candidate.width() / 4) {
            return true;
        }
    }
    return false;
}

/* Since the white blob check catches a lot of extra stuff like lines,
   we need to check a bit more carefully.
 */
bool Robots::whiteAbove(Blob candidate) {
    int top = candidate.getTop();
    int height = candidate.height();
    int scanline = top - height;
    for (int y = scanline; y > 0 && y > scanline - height; y -= 3) {
        int white = 0;
        int green = 0;
        for (int x = candidate.getLeft(); x < candidate.getRight(); x++) {
            if (Utility::isWhite(thresh->getThresholded(y, x))) {
                white++;
            } else if (Utility::isGreen(thresh->getThresholded(y, x))) {
                green++;
            }
        }
        if (green > candidate.width() / 2 && white == 0) {
            return false;
        }
        if (white > candidate.width() / 4) {
            return true;
        }
    }
    return false;
}

/* Since our uniforms often are wrinkled and whatnot, sometimes the blobs
   are not continuous.  See if we can merge some.
 */
void Robots::mergeBigBlobs() {
    for (int i = 0; i < blobs->number() - 1; i++) {
        Blob merge = blobs->get(i);
        if (merge.height() > 10) {
            for (int j = i + 1; j < blobs->number(); j++) {
                // check if the blobs are relatively near
                if (blobs->get(j).height() > 10 && closeEnough(i, j)) {
                    checkMerge(i, j);
                } else {
                }
            }
        }
    }
}

/* Are two blobs close enough that they might actually be the same?
   Note: if the area between is bigger than one of the blobs then can it?
 */
bool Robots::closeEnough(int i, int j) {
    Blob a = blobs->get(i);
    Blob b = blobs->get(j);
    int width1 = a.width();
    int width2 = b.width();
    int height1 = a.height();
    int height2 = b.height();
    int bigWidth = max(width1, width2);
    int bigHeight = max(height1, height2);
    int dist1 = distance(a.getLeft(), a.getRight(), b.getLeft(), b.getRight());
    int dist2 = distance(a.getTop(), a.getBottom(), b.getTop(), b.getBottom());
    if (a.getRight() <= 0 || b.getRight() <= 0) {
        return false;
    }
    if (dist1 > 5 && dist2 > 5) {
        return false;
    }
    if (dist1 < bigHeight && dist2 < bigWidth) {
        return true;
    }
    return false;
}

/* We have two blobs that are reasonably close.  See if they should be
   merged.
*/
void Robots::checkMerge(int i, int j) {
    Blob a = blobs->get(i);
    Blob b = blobs->get(j);
    if (debugRobots) {
        cout << endl << endl << "Checking merge" << endl;
        printBlob(a);
        printBlob(b);
    }
    int dist1 = distance(a.getLeft(), a.getRight(), b.getLeft(), b.getRight());
    int dist2 = distance(a.getTop(), a.getBottom(), b.getTop(), b.getBottom());
    int green = 0;
    int col = 0;
    int miss = 0;
    // top to bottom defines the y region between the two
    int top = max(a.getTop(), b.getTop());
    int bottom = min(a.getBottom(), b.getBottom());
    int midx = (a.getLeft() + a.getRight()) / 2;
    // left to right defines the x region between the two
    int left = a.getRight();
    int right = b.getLeft();
    int midy = (a.getTop() + a.getBottom()) / 2;
    int ii;
    // check if they overlap in either dimension
    if (left > right) {
        // they overlap in x so we'll swap left and right
        int temp = left;
        left = right;
        right = temp;
    }
    if (top > bottom) {
        int temp = top;
        top = bottom;
        bottom = temp;
    }
    int width = right - left + 1;
    int height = bottom - top + 1;
    int area = max(10, width * height / 27);
    int stripe = max(width, height);
    for (int x = left; x < right; x+=3) {
        for (int y = top; y < bottom; y+=3) {
            if (debugRobots) {
                vision->drawPoint(x, y, MAROON);
            }
            if (Utility::colorsEqual(thresh->getThresholded(y, x), color)) {
                col++;
                if (col > area || col > stripe) {
                    blobs->mergeBlobs(i, j);
                    if (debugRobots) {
                        cout << "Merge" << endl;
                    }
                    return;
                }
            } else if (Utility::isGreen(thresh->getThresholded(y, x))) {
                green++;
                if (green > 5) {
                    return;
                }
            } else if (Utility::isWhite(thresh->getThresholded(y, x))) {
                miss++;
                if (miss > area / 9 || miss > stripe) {
                    return;
                }
            }
        }
    }
    if (col > min(width, height) && green < 3) {
        blobs->mergeBlobs(i, j);
        if (debugRobots) {
            cout << "Merge" << endl;
        }
    }
}

/* Calculate the horizontal distance between two objects
 * (the end of one to the start of the other).
 * @param x1	left x of one object
 * @param x2	right x of the object
 * @param x3	left x of the other
 * @param x4	right x of the other
 * @return		the distance between the objects in the x dimension
 */
int Robots::distance(int x1, int x2, int x3, int x4) {
    if (x2 < x3) {
        return x3 - x2;
    }
    if (x1 > x4) {
        return x1 - x4;
    }
    return 0;
}

/* We have a swatch of color.  Let's make sure that there is some white
   around somewhere as befits our robots.
 */

bool Robots::noWhite(Blob b) {
    const int MINWHITE = 5;

    int left = b.getLeft(), right = b.getRight();
    int top = b.getTop(), bottom = b.getBottom();
    int width = b.width();
    int tops, bottoms;
    for (int i = 1; i < b.height(); i++) {
        tops = 0; bottoms = 0;
        for (int x = left; x <= right; x++) {
            if (top - i >= 0 && Utility::isWhite(thresh->getThresholded(top - i,x)))
                tops++;
            if (bottom + i < IMAGE_HEIGHT &&
                Utility::isWhite(thresh->getThresholded(bottom+i,x)))
                bottoms++;
            if (tops > width / 2 || tops == width) return false;
            if (bottoms > width / 2 || tops == width) return false;
        }
    }
    return true;
}


/*
  We have detected something big enough to be called a robot.  Set the appropriate
  field object.
  @param which    whether it is the biggest or the second biggest object
  @param index    the index of the blob in question
 */

void Robots::updateRobots(int which, int index)
{
    if (debugRobots) {
        cout << "Updating robot " << which << " " << color << endl;
    }
    //printBlob(blobs[index]);
    if (color == RED_BIT) {
        if (which == 1) {
            vision->red1->updateRobot(blobs->get(index));
        } else if (which == 2) {
            vision->red2->updateRobot(blobs->get(index));
        } else {
            vision->red3->updateRobot(blobs->get(index));
        }
    } else {
        if (which == 1) {
            vision->navy1->updateRobot(blobs->get(index));
        } else if (which == 2) {
            vision->navy2->updateRobot(blobs->get(index));
        } else {
            vision->navy3->updateRobot(blobs->get(index));
        }
    }

    blobs->zeroTheBlob(index);
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
void Robots::newRun(int x, int y, int h)
{
    const int RUN_VALUES = 3;    // x, y, and h of course
    const int SKIPS = 8;

    if (numberOfRuns < runsize) {
        int last = numberOfRuns - 1;
        // skip over noise --- jumps over two pixel noise currently.
        //HW--added CONSTANT for noise jumps.
        if (last > 0 && runs[last].x == x &&
            (runs[last].y - (y + h) <= SKIPS)) {
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

/* Print debugging information for a blob.
 * @param b    the blob
 */
void Robots::printBlob(Blob b) {
#if defined OFFLINE
    cout << "Outputting blob" << endl;
    cout << b.getLeftTopX() << " " << b.getLeftTopY() << " " << b.getRightTopX() << " "
            << b.getRightTopY() << endl;
    cout << b.getLeftBottomX() << " " << b.getLeftBottomY() << " " << b.getRightBottomX()
            << " " << b.getRightBottomY() << endl;
#endif
}

