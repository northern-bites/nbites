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
 * Object Fragment class - a ChownDawg production
 * There is one of these for each color.  It holds "runs" of pixels that are of
 * that color and vertically connected.
 * The idea is to use the runs to recognize objects.  We use the mechanics of
 * the field to help in this process in various ways.  In essence this is
 * "run length encoding."  We connect up runs into "blobs" and then filter
 * the blobs to try and pick out the most likely objects.
*/

#include <iostream>

#include "ObjectFragments.h"
#include "debug.h"

// redefine abs for ints or doubles
#undef abs
#define abs(x) ((x) < 0 ? -(x) : (x))


using namespace std;

ObjectFragments::ObjectFragments(Vision* vis, Threshold* thr)
    : vision(vis), thresh(thr)
{

    init(0.0);
#ifdef OFFLINE
    BALLDISTDEBUG = false;
    PRINTOBJS = false;
    POSTDEBUG = false;
    POSTLOGIC = false;
    TOPFIND = false;
    BALLDEBUG = false;
    CORNERDEBUG = false;
    BACKDEBUG = false;
    SANITY = false;
    DEBUGBALLPOINTS = false;
    CORRECT = false;
    OPENFIELD = false;
#endif
}


/* Initialize the data structure.
 * @param s     the slope corresponding to the dog's head tilt
 */
void ObjectFragments::init(float s)
{
    slope = s;
    biggestRun = 0;
    maxHeight = IMAGE_HEIGHT;
    maxOfBiggestRun = 0;
    numberOfRuns = 0;
    indexOfBiggestRun = 0;
    numBlobs = 0;
    zeroBlob.rightTop.y = 0;
    zeroBlob.rightTop.x = 0;
    zeroBlob.leftTop.x = BADVALUE;
    zeroBlob.leftTop.y = BADVALUE;
    zeroBlob.leftBottom.x = 0;
    zeroBlob.leftBottom.y = 0;
    zeroBlob.rightBottom.x = 0;
    zeroBlob.rightBottom.y = 0;
    secondBlob.rightTop.y = 0;
    secondBlob.rightTop.x = 0;
    secondBlob.leftTop.x = BADVALUE;
    secondBlob.leftTop.y = BADVALUE;
    secondBlob.leftBottom.x = 0;
    secondBlob.leftBottom.y = 0;
    secondBlob.rightBottom.x = 0;
    secondBlob.rightBottom.y = 0;
    crossBlob.rightTop.y = 0;
    crossBlob.rightTop.x = 0;
    crossBlob.leftTop.x = BADVALUE;
    crossBlob.leftTop.y = BADVALUE;
    crossBlob.leftBottom.x = 0;
    crossBlob.leftBottom.y = 0;
    crossBlob.rightBottom.x = 0;
    crossBlob.rightBottom.y = 0;
    checker.rightTop.y = 0;
    checker.rightTop.x = 0;
    checker.leftTop.x = BADVALUE;
    checker.leftTop.y = BADVALUE;
    checker.leftBottom.x = 0;
    checker.leftBottom.y = 0;
    checker.rightBottom.x = 0;
    checker.rightBottom.y = 0;
    obj.rightTop.y = 0;
    obj.rightTop.x = 0;
    obj.leftTop.x = BADVALUE;
    obj.leftTop.y = BADVALUE;
    obj.leftBottom.x = 0;
    obj.leftBottom.y = 0;
    obj.rightBottom.x = 0;
    obj.rightBottom.y = 0;
    topSpot = 0;
    numPoints = 0;
    for (int i = 0; i < IMAGE_WIDTH; i++)
        shoot[i] = true;
}

/*  Set the primary color.  Depending on the color, we have different space needs.
 * @param c        the color
 */

void ObjectFragments::setColor(int c)
{
    runsize = 1;
    int run_num = 3;
    color = c;
    // depending on the color we have more or fewer runs available
    switch (color) {
    case YELLOW:
    case BLUE:
        run_num = IMAGE_WIDTH * 15;
        runsize = IMAGE_WIDTH * 5;
        break;
    case RED:
    case NAVY:
        run_num = IMAGE_WIDTH * 15;
        runsize = IMAGE_WIDTH * 5;
        break;
    case ORANGE:
        runsize = BALL_RUNS_MALLOC_SIZE; //max number of runs
        run_num = runsize * 3;
        break;
    }
    runs = (run*)malloc(sizeof(run) * run_num);
}

/* The first group of methods have to do with blob creation.
 */


/*
 * Pseudo-blobbing used for goal recognition.  Since the goals are rectangles we
 * should be able to just paste the new runs in to a main blob directly.
 * This uses the huge hack that our objects (except the ball) are square - so we
 * can just keep a bounding box.
 *
 * Basically we are collecting runs.  Everytime we get one we either add it to
 * an existing run or create a new run. In theory we can fragment runs this way.
 * In fact, we should probably check on that.
 *
 * @param x        x value of run
 * @param y        y value of run
 * @param h        height of run
*/
void ObjectFragments::blobIt(int x, int y, int h)
{
    // start out deciding to make a new blob
    // the loop will decide not to run it
    bool newBlob = true;
    int contig = 4;

    //cout << x << " " << y << " " << h << endl;
    // sanity check: too many blobs on screen
    if (numBlobs >= MAX_BLOBS) {
        //cout << "Ran out of blob space " << color << endl;
        // We're seeing too many blobs -it is unlikely we can do anything
        // useful with this color
        numBlobs = 0;
        numberOfRuns = 0;
        return;
    } else if (numBlobs > 20) {
        contig = 4;
    }

    // is this run contiguous with any previous blob?
    for (int i = 0; i < numBlobs; i++) {

        // first check: if currentBlob x is greater than blob left and less than
        // a little bit more than the blob right.
        // AND
        // second check: currentBlob y is within fits within current blob
        // OR
        // currentBlob's bottom is within blob and height makes it higher
        if ((x > blobs[i].leftTop.x  && x < blobs[i].rightTop.x + contig) &&
            ((y >= blobs[i].leftTop.y - contig &&
              y < blobs[i].leftBottom.y + contig) ||
             (y < blobs[i].leftTop.y && y+h+contig > blobs[i].leftTop.y))) {

            /* BOUNDING BOX CHECKS
             * if current x or y increases the size of the box, do so and keep
             * track of the corresponding x or y value
             */
            //assign the right, if it is better
            if (x > blobs[i].rightTop.x) {
                blobs[i].rightTop.x = x;
                blobs[i].rightBottom.x = x;
            }

            //assign the top, if it is better
            if (blobs[i].leftTop.y > y) {
                blobs[i].leftTop.y = y;
                blobs[i].rightTop.y = y;
            }

            // assign the top, if it is better
            if (y+h > blobs[i].leftBottom.y) {
                blobs[i].leftBottom.y = y+h;
                blobs[i].rightBottom.y = y + h;
            }

            // since we go left->right, we don't have to worry about the left
            // inferred center ball calculation.
            //take the lowest Y value for realLeftY in the first column of the blob
            /* if(blobs[i].leftTop.x == x && y < blobs[i].leftTop.y){
               blobs[i].leftTop.y = y;
               blobs[i].rightTop.y = y;
               }*/

            //add the run length to the number of real pixels in the blob
            //calculate the area of this blob under consideration
            int s = (blobs[i].rightTop.x - blobs[i].leftTop.x + 1) *
                (blobs[i].leftBottom.y - blobs[i].leftTop.y + 1);
            blobs[i].area = s; //store the area for later.
            blobs[i].pixels += h;

            // don't create a blob
            newBlob = false;
            break;
        }
        // no else
    } // END blob for loop

    // create newBlob
    if (newBlob) {
        // bounding box
        blobs[numBlobs].leftTop.x = x;
        blobs[numBlobs].leftTop.y = y;
        blobs[numBlobs].rightTop.x = x;
        blobs[numBlobs].rightTop.y = y;
        blobs[numBlobs].leftBottom.x = x;
        blobs[numBlobs].leftBottom.y = y + h;
        blobs[numBlobs].rightBottom.x = x;
        blobs[numBlobs].rightBottom.y = y + h;
        blobs[numBlobs].pixels = h;
        blobs[numBlobs].area = h;
        numBlobs++;
    }
}

/*
 * Find the biggest blob.  Ideally this will end up also merging blobs when they
 * are occluded (e.g. by a dog).
 * It may not be necessary though.  After this call the data structure "topBlob"
 * will have the top blob.
 * Additionally, the blob "secondBlob" will have the second biggest blob.
 * @param maxY     max value (ignored)
*/

void ObjectFragments::getTopAndMerge(int maxY)
{
    topBlob = zeroBlob;
    int size = 0;
    topSpot = 0;
    //cout << "Blobs " << numBlobs << " " << color << endl;
    //check each blob in the array
    for (int i = 0; i < numBlobs; i++) {
        if (blobs[i].area > size) {
            size = blobs[i].area;
            topBlob = blobs[i];
            topSpot = i; //store the one with the largest size.
        }
        //drawBlob(blobs[i], BLACK);
    }
    size = 0;
    for (int i = 0; i < numBlobs; i++) {
        if (blobs[i].area > size && i != topSpot) {
            size = blobs[i].area;
            secondBlob = blobs[i];
        }
        //drawBlob(blobs[i], BLACK);
    }
}

void ObjectFragments::getWidest()
{
    topBlob = zeroBlob;
    int size = 0;
    topSpot = 0;
    int width = 0;
    //check each blob in the array
    for (int i = 0; i < numBlobs; i++) {
        width = blobWidth(blobs[i]);
        if (width > size) {
            size = width;
            topBlob = blobs[i];
            topSpot = i; //store the one with the largest size.
        }
        //drawBlob(blobs[i], BLACK);
    }
}

/* Turn a blob back to zeros because of merging.
   @param which
*/

void ObjectFragments::zeroTheBlob(int which)
{
    blobs[which].leftTop.x = BADVALUE;
    blobs[which].leftTop.y = 0;
    blobs[which].rightTop.x = 0;
    blobs[which].rightTop.y = 0;
    blobs[which].leftBottom.x = 0;
    blobs[which].leftBottom.y = 0;
    blobs[which].rightBottom.x = 0;
    blobs[which].rightBottom.y = 0;
    blobs[which].pixels = 0;
    blobs[which].area = 0;
}

/* Merge blobs.
   @param first         one of the blobs
   @param second        the other
*/

void ObjectFragments::mergeBlobs(int first, int second)
{
#if ROBOT(NAO_SIM)
    robot_console_printf("merging\n");
#endif
    int value = min(blobs[first].leftTop.x, blobs[second].leftTop.x);
    blobs[first].leftTop.x = value;
    blobs[first].leftBottom.x = value;
    value = max(blobs[first].rightTop.x, blobs[second].rightTop.x);
    blobs[first].rightTop.x = value;
    blobs[first].rightBottom.x = value;
    value = min(blobs[first].leftTop.y, blobs[second].leftTop.y);
    blobs[first].leftTop.y = value;
    blobs[first].rightTop.y = value;
    value = max(blobs[first].leftBottom.y, blobs[second].leftBottom.y);
    blobs[first].leftBottom.y = value;
    blobs[first].rightBottom.y = value;
    zeroTheBlob(second);
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
void ObjectFragments::newRun(int x, int y, int h)
{
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

        if (h > biggestRun) { // tracking largest run
            biggestRun = h;
            maxOfBiggestRun = y;
            indexOfBiggestRun = numberOfRuns * 3;
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


/* Robot recognition methods
 */

/* Try and recognize robots
 */
void ObjectFragments::robot(int bigGreen)
{
    int lastrunx = -30, lastruny = 0, lastrunh = 0;
    for (int i = 0; i < numberOfRuns; i++) {
        //drawPoint(runs[i].x, runs[i].y, BLACK);
        if (runs[i].x < lastrunx + 20) {
            // TODO: change from +2
            for (int k = lastrunx; k < runs[i].x; k+= 2) {
                //cout << "merging " << k << " " << runs[i].x << endl;
                blobIt(k, lastruny, lastrunh);
            }
        }
        blobIt(runs[i].x, runs[i].y, runs[i].h);
        lastrunx = runs[i].x; lastruny = runs[i].y; lastrunh = runs[i].h;
    }
    getRobots(bigGreen);
    for (int i = 0; i < numBlobs; i++) {
        if (blobWidth(blobs[i]) > 5) {
            transferBlob(blobs[i], topBlob);
            expandRobotBlob();
            transferBlob(topBlob, blobs[i]);
        }
    }
    mergeBigBlobs();
    for (int i = 0; i < numBlobs; i++) {
        if (blobWidth(blobs[i]) > 5) {
            transferBlob(blobs[i], topBlob);
            expandRobotBlob();
            transferBlob(topBlob, blobs[i]);
        }
    }
    int biggest = -1, index1 = -1, second = -1, index2 = -1;
    for (int i = 0; i < numBlobs; i++) {
        // TODO: for now we'll use closest y - eventually we should use
        // pixestimated distance
        int area = blobArea(blobs[i]);
        if (viableRobot(blobs[i]) && blobArea(blobs[i]) >= biggest) {
            second = biggest;
            index2 = index1;
            index1 = i;
            biggest = blobArea(blobs[i]);
        } else if (viableRobot(blobs[i]) && blobArea(blobs[i]) > 10) {
            second = area;
            index2 = i;
        }
    }
    if (index1 != -1) {
        updateRobots(1, index1);
        if (index2 != -1)
            updateRobots(2, index2);
    }
}

void ObjectFragments::expandRobotBlob()
{
    //int spany = blobHeight(topBlob);
    //int spanx = blobWidth(topBlob);
    // expand right side
    int x, y;
    int bestr = topBlob.rightTop.x;
    bool good = true;
    for (x = bestr; good && x < IMAGE_WIDTH - 1; x++) {
        good = false; // TODO: change from +2
        for (y = topBlob.rightTop.y; y < topBlob.rightBottom.y && !good;
             y = y + 2) {
            if (thresh->thresholded[y][x] == color)
                good = true;
        }
    }
    topBlob.rightTop.x = x - 1;
    topBlob.rightBottom.x = x - 1;
    good = true;
    for (x = topBlob.leftTop.x; good && x >  -1; x--) {
        good = false; // TODO: change from +2
        for (y = topBlob.rightTop.y; y < topBlob.rightBottom.y && !good;
             y = y + 2) {
            if (thresh->thresholded[y][x] == color)
                good = true;
        }
    }
    topBlob.leftTop.x = x + 1;
    topBlob.leftBottom.x = x + 1;
    int whites = IMAGE_WIDTH, pix, width = blobWidth(topBlob) / 4;
    int goods = 0, lastSaw = 0;
    for (y = topBlob.leftBottom.y; whites >= width && y < IMAGE_HEIGHT - 1;y++){
        whites = 0;
        goods = 0;
        for (x = topBlob.leftBottom.x; x < topBlob.rightTop.x && whites < width;
             x++) {
            pix = thresh->thresholded[y][x];
            if (pix == color) {
                whites++;
                if (goods > 5) {
                    whites = width;
                }
            } else if (pix == WHITE) {
                whites++;
            } else if ((color ==  NAVY && pix == RED) ||
                       (color == RED && pix == NAVY)) {
                whites -= 5;
            }
        }
        if (goods < 5) {
            lastSaw++;
        } else {
            lastSaw = 0;
        }
    }
    int gain = y - 1 - topBlob.leftBottom.y;
    topBlob.leftBottom.y = y - 1;
    topBlob.rightBottom.y = y - 1;
    if (gain > 5) {
        good = true;
        for (x = topBlob.rightTop.x; good && x < IMAGE_WIDTH - 1; x++) {
            good = false;
            for (y = topBlob.rightBottom.y - gain; y < topBlob.rightBottom.y
                     && !good; y++) {
                if (thresh->thresholded[y][x] == color)
                    good = true;
            }
        }
        topBlob.rightTop.x = x - 1;
        topBlob.rightBottom.x = x - 1;
        good = true;
        for (x = topBlob.leftTop.x; good && x >  -1; x--) {
            good = false;
            for (y = topBlob.rightBottom.y - gain; y < topBlob.rightBottom.y
                     && !good; y++) {
                if (thresh->thresholded[y][x] == color)
                    good = true;
            }
        }
        topBlob.leftTop.x = x + 1;
        topBlob.leftBottom.x = x + 1;

    }
}

void ObjectFragments::updateRobots(int which, int index)
{
    //cout << "Updating robot " << which << " " << color << endl;
    //printBlob(blobs[index]);
    if (color == RED) {
        if (which == 1) {
            vision->red1->updateRobot(&blobs[index]);
        } else {
            vision->red2->updateRobot(&blobs[index]);
        }
    } else {
        if (which == 1) {
            vision->navy1->updateRobot(&blobs[index]);
        } else {
            vision->navy2->updateRobot(&blobs[index]);
        }
    }
}

void ObjectFragments::mergeBigBlobs()
{
    for (int i = 0; i < numBlobs - 1; i++) {
        for (int j = i+1; j < numBlobs; j++) {
            if (closeEnough(blobs[i], blobs[j]) &&
                bigEnough(blobs[i], blobs[j])) {
                //cout << "Merging blobs " << endl;
                //printBlob(blobs[i]);
                //printBlob(blobs[j]);
                mergeBlobs(j, i);
                //cout << "Merged are now" << endl;
                //printBlob(blobs[i]);
                //printBlob(blobs[j]);
            }
        }
    }
}

bool ObjectFragments::closeEnough(blob a, blob b)
{
    int xd = distance(a.leftTop.x, a.rightTop.x, b.leftTop.x, b.rightTop.x);
    int yd = distance(a.leftTop.y, a.leftBottom.y, b.leftTop.y, b.rightBottom.y);
// TODO: change constant to lower res stuff
    if (xd < 40) {
        if (yd < 40)
            return true;
    }
    // if (xd < max(blobWidth(a), blobWidth(b)) &&
    //     yd < max(blobHeight(a), /blobHeight(b))) return true;
    return false;
}

bool ObjectFragments::bigEnough(blob a, blob b)
{
// TODO: change constant to lower res stuff
    if (blobArea(a) > 200 && blobArea(b) > 200)
        return true;
    if (a.leftBottom.y > horizonAt(a.leftBottom.x) + 100)
        return true;
    return false;
}

bool ObjectFragments::viableRobot(blob a)
{
    // get rid of obviously false ones
// TODO: change constant to lower res stuff
    if (!(a.rightBottom.y > horizonAt(a.rightBottom.x) && blobWidth(a) > 10)) {
        return false;
    }
    int whites = 0;
    int col = 0;
    for (int i = 0; i < blobWidth(a); i+=2) {
        for (int j = 0; j < blobHeight(a); j+=2) {
            int newpix = thresh->thresholded[j+a.leftTop.y][i+a.leftTop.x];
            if (newpix == WHITE) {
                whites++;
            } else if (newpix == color) {
                col++;
            }
        }
    }
    if ((float)(whites + col) / (float)blobArea(a) > 0.10f)
        return true;
    return false;
}

/* Find robot blobs.
   @param maxY     max value - will be used to pick heads out
*/

void ObjectFragments::getRobots(int maxY)
{
    topBlob = zeroBlob;
    int size = 0;
    topSpot = 0;
    //check each blob in the array
    for (int i = 0; i < numBlobs; i++) {
        int spread = blobs[i].rightTop.x - blobs[i].leftTop.x;
        if (blobs[i].leftTop.x == BADVALUE) {
// TODO: experiment with this constant; probably 3
        } else if (blobs[i].leftTop.y > maxY || spread < 4) {
            //drawBlob(blobs[i], BLACK);
        } else {
            // could be a robot or robots
            //splitBlob(i);
        }
        if (blobs[i].area > size) {
            size = blobs[i].area;
            topBlob = blobs[i];
            topSpot = i; //store the one with the largest size.
        }
    }
    size = 0;
    for (int i = 0; i < numBlobs; i++) {
        int spread = blobWidth(blobs[i]);
// TODO: experiment with this constant; probably 3
        if (blobs[i].leftTop.x != BADVALUE && blobs[i].leftTop.y < maxY &&
            spread > 4) {
            //drawBlob(blobs[i], ORANGE);
        }
        if (blobs[i].area > size && i != topSpot) {
            size = blobs[i].area;
            secondBlob = blobs[i];
        }
        //drawBlob(blobs[i], BLACK);
    }
}

/* Is a robot head centered on its blob?
   @param  head the location of the head
   @param  left the leftpoint of the blob
   @param  right the rightpoint of the blob
*/

int ObjectFragments::isRobotCentered(int head, int left, int right)
{
    // these to be REAL constants
    int LEFTRANGE = 35;
    int RIGHTRANGE = 100 - LEFTRANGE;
    if ((head - left) < (right - left) * LEFTRANGE / 100)
        return LEFT;
    if ((head - left) > (right - left) * RIGHTRANGE / 100)
        return RIGHT;
    return 0;
}

/* The next group of methods has to do with scanning along axis parallel
 * dimensions in order to create objects without blobbing.
 */

/* Project a line given a start coord and a new y value - note that this is
 * dangerous depending on how you do the projection.
 *
 * @param startx   the x point to start at
 * @param starty   the y point to start at
 * @param newy     the y point to end at
 * @return         the corresponding x point
 */
int ObjectFragments::xProject(int startx, int starty, int newy)
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
int ObjectFragments::xProject(point <int> point, int newy) {
    //slope is a float representing the slope of the horizon.
    return point.x - ROUND2(slope * (float)(newy - point.y));
}

/* Project a line given a start coord and a new x value
 * @param startx   the x point to start at
 * @param starty   the y point to start at
 * @param newx     the x point to end at
 * @return         the corresponding y point
 */
int ObjectFragments::yProject(int startx, int starty, int newx)
{
    return starty + ROUND2(slope * (float)(newx - startx));
}

/* Project a line given a start coord and a new x value
 * @param point    the point to start at
 * @param newx     the x point to end at
 * @return         the corresponding y point
 */
int ObjectFragments::yProject(point <int> point, int newx)
{
    return point.y + ROUND2(slope * (float)(newx - point.x));
}

/* Scan from the point along the line until you have hit "stopper" points that
 * aren't color "c" return the last good point found and how many good and bad
 * points seen.  Though this is a void function it actually "returns"
 * information in the scan variable. scan.x and scan.y represent the finish
 * points of the line (last point of appropriate color) and bad and good
 * represent how many bad and good pixels (pixels that are of the right color
 * or not) along the way.
 * @param x        the x point to start at
 * @param y        the y point to start at
 * @param dir      the direction of the scan (positive or negative)
 * @param stopper  how many incorrectly colored pixels we can live with
 * @param c        color we are most interested in
 * @param c2       soft color that could also work
 */
void ObjectFragments::vertScan(int x, int y, int dir, int stopper, int c,
                               int c2)
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
    // go until we hit enough bad pixels
    for ( ; x > -1 && y > -1 && x < width && y < height && bad < stopper; ) {
        //cout << "Vert scan " << x << " " << y << endl;
        // if it is the color we're looking for - good
        if (thresh->thresholded[y][x] == c || thresh->thresholded[y][x] == c2) {
            good++;
            run++;
            if (run > 1) {
                scan.x = x;
                scan.y = y;
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

/* Scan from the point along the line until you have hit "stopper" points that aren't color "c"
   return the last good point found and how many good and bad points seen.  Though this is a void
   function it actually "returns" information in the scan variable. scan.x and scan.y represent
   the finish points of the line (last point of appropriate color) and bad and good represent
   how many bad and good pixels (pixels that are of the right color or not) along the way.
   * @param x          the x point to start at
   * @param y          the y point to start at
   * @param dir        the direction of the scan (positive or negative)
   * @param stopper    how many incorrectly colored pixels we can live with
   * @param c          color we are most interested in
   * @param c2         soft color that could also work
   * @param leftBound  furthest left we can go
   * @param rightBound further right we can go
   */
void ObjectFragments::horizontalScan(int x, int y, int dir, int stopper, int c, int c2, int leftBound, int rightBound) {
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
    // go until we hit enough bad pixels
    for ( ; x > leftBound && y > -1 && x < rightBound && x < IMAGE_WIDTH && y < height && bad < stopper; ) {
        if (thresh->thresholded[y][x] == c || thresh->thresholded[y][x] == c2) {
            // if it is either of the colors we're looking for - good
            good++;
            run++;
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
    //cout << "return with " << temp.x << endl;
}

/* Given two points determine if they constitute an "edge".  For now our definition of an edge is a
 * difference in Y values of 30 (sort of a standard approach in our league).  This is a place for
 * potential improvements in the future.
 * @param x        the x value of the first point
 * @param y        the y value of the first point
 * @param x2       the x value of the second point
 * @param y2       the y value of the second point
 * @return         was an edge detected?
 */

bool ObjectFragments::checkEdge(int x, int y, int x2, int y2) {
    int ydiff = abs(thresh->getY(x, y) - thresh->getY(x2, y2));
    if (ydiff > 30) {
        return true;
    }
    return false;
}

/*  Given two points that should define an edge of a structure, try and improve them to find the true edge.
 * @param top         the topmost estimated point
 * @param bottom      the bottommost estimated point
 * @param c           the primary color of the structure
 * @param c2          the secondary color of the structure
 * @param left        whether the open space is to the left or the right of the structure
 */

int ObjectFragments::findTrueLineVertical(point <int> top, point <int> bottom, int c, int c2, bool left) {
    int spanY = bottom.y - top.y;
    int count = 0;
    int good = spanY;
    int j = 0;
    int dir = 1;
    if (left)
        dir = -1;
    for (j = 1; count < spanY / 3 && top.x + dir * j >= 0 && top.x + dir * j < IMAGE_WIDTH && good > spanY / 2 ; j++) {
        count = 0;
        good = 0;
        for (int i = top.y; count < spanY / 3 && i <= bottom.y; i++) {
            int _spot = top.x + dir * j;
            /*if (checkEdge(spot, i, _spot - dir, i)) {
              count++;
              }*/
            int curcol = thresh->thresholded[i][_spot];
            if (curcol == c || curcol == c2) {
                good++;
            }
        }
    }
    return top.x + dir * j - dir;
}

/*  Given two points that should define an edge of a structure, try and improve them to find the true edge.
 * Exactly like the previous function except that it allows for a slope to the camera angle.
 * @param top         the topmost estimated point
 * @param bottom      the bottommost estimated point
 * @param c           the primary color of the structure
 * @param c2          the secondary color of the structure
 * @param left        whether the open space is to the left or the right of the structure
 */

void ObjectFragments::findTrueLineVerticalSloped(point <int>& top, point <int>& bottom, int c, int c2, bool left) {
    // TODO: change increment to 1
    int increment = 2;
    int spanY = bottom.y - top.y + 1;
    int count = 0;
    int good = spanY;
    int goodRun = 0;
    int j = 0;
    int dir = 1;
    int theSpot = 0;
    //int xShift = 0;
    int run = 0;
    int initRun = 0;
    if (left)
        dir = -1;
    int minCount = (spanY / 3) / increment;
    int minRun = min(spanY, max(5, spanY / 2));
    int minGood = max(1, (spanY / 2) / increment);
    int badLines = 0;
    int i = 0;
    int fake = 0;
    bool atTop = false;
    //drawPoint(top.x, top.y, RED);
    //drawPoint(bottom.x, bottom.y, RED);
    for (j = 1; badLines < 2 && top.x + dir * j >= 0 && top.x + dir * j < IMAGE_WIDTH; j+=increment) {
        //count = 0;
        good = 0;
        fake = 0;
        run = 0;
        goodRun = 0;
        initRun = 0;
        int actualY = yProject(top.x, top.y, top.x + dir * j);
        //cout << "Actual y is " << actualY << endl;
        if (actualY < 1) atTop = true;
        //cout << "Actual y is " << actualY << endl;
        for (i = actualY; count < minCount  &&
                 i <= actualY + spanY && (run < minRun || goodRun > spanY / 2) && (top.y > 1 || initRun < minRun);
             i+= increment) {
            theSpot = xProject(top.x + dir * j, actualY, i);
            if (theSpot < 0 || theSpot > IMAGE_WIDTH - 1 || theSpot - dir < 0 ||
                theSpot - dir > IMAGE_WIDTH - 1 || i < 0 || i > IMAGE_HEIGHT - 1) {
                fake++;
            } else {
                //if (checkEdge(theSpot, i, theSpot - dir, i)) {
                //count++;
                //}
                int curcol = thresh->thresholded[i][theSpot];
                if (curcol == c || curcol == c2) {
                    good++;
                    goodRun++;
                    initRun = -1;
                    run = 0;
                } else {
                    goodRun = 0;
                    if (initRun > -1) {
                        initRun++;
                        if (atTop && initRun > 5) break;
                        //cout << "Init run " << initRun << " " << top.y << " " << minRun << endl;
                    }
                    if (curcol != ORANGE && curcol != WHITE)
                        run++;
                    //if (run == 5)
                    //drawPoint(theSpot, i, BLACK);
                }
            }
        }
        if (good + fake < minGood || good < 1) {
            badLines++;
        }
        //cout << good << " " << minGood << " " << count << " " << initRun << " " << i << endl;
        //if (!left)
        //drawPoint(theSpot, i, BLACK);
    }
    int temp = top.x;
    //drawPoint(top.x, top.y, BLACK);
    top.x = top.x + dir * (j - badLines) - dir;
    top.y = yProject(temp, top.y, top.x);
    //drawPoint(top.x, top.y, RED);
    bottom.y = top.y + spanY;
    bottom.x = xProject(top.x, top.y, top.y + spanY);
    //cout << "Checking " << top.x << " " << top.y << endl;
    if (top.x < 2 || top.x > IMAGE_WIDTH - 3) {
        //cout << "In upward scan" << endl;
        for (j = 1; count < minCount && bottom.x + dir * j >= 0 && bottom.x + dir * j < IMAGE_WIDTH
                 && good > minGood && run < minRun; j+=increment) {
            //count = 0;
            good = 0;
            run = 0;
            int actualY = yProject(bottom.x, bottom.y, bottom.x + dir * j);
            for (i = actualY; count < minCount && i >= actualY - spanY &&
                     run < minRun; i-= increment) {
                theSpot = xProject(bottom.x + dir * j, actualY, i);

                if (theSpot < 0 || theSpot > IMAGE_WIDTH - 1 || theSpot - dir < 0 ||
                    theSpot - dir > IMAGE_WIDTH - 1 || i < 0 || i > IMAGE_HEIGHT - 1)
                    good++;

                else {
                    if (checkEdge(theSpot, i, theSpot - dir, i)) {
                        //count++;
                    }
                    int curcol = thresh->thresholded[i][theSpot];
                    if (curcol == c || curcol == c2) {
                        good++;
                        run = 0;
                    } else {
                        run++;
                        //if (run == 5)
                        //drawPoint(theSpot, i, BLACK);
                    }
                }
            }
        }
        temp = bottom.x;
        //drawPoint(top.x, top.y, BLACK);
        bottom.x = bottom.x + dir * (j - increment) - dir;
        bottom.y = yProject(temp, bottom.y, bottom.x);
        //drawPoint(top.x, top.y, RED);
        top.y = bottom.y - spanY;
        top.x = xProject(bottom.x, bottom.y, bottom.y - spanY);
    }
}

/*  Given two points that should define an edge of a structure, try and improve them to find the true edge.
 * @param left        the leftmost estimated point
 * @param right       the rightmost estimated point
 * @param c           the primary color of the structure
 * @param c2          the secondary color of the structure
 * @param up          whether the open space is up or down
 */

void ObjectFragments::findTrueLineHorizontalSloped(point <int>& left, point <int>& right, int c, int c2, bool up) {
    int spanX = right.x - left.x + 1;
    int spanY = right.y - left.y + 1;
    int count = 0;
    int good = spanX;
    int j = 0;
    int dir = 1;
    int theSpot = 0;
    int run = 0;
    int badLines = 0;
    int maxgreen = 3;
#if ROBOT(NAO) // TODO: change to lower res
    maxgreen = 200;
#endif
    if (up)
        dir = -1;
    int minRun = min(spanX, max(5, spanX / 5));
    int minCount = min(spanX, max(2,spanX / 3));
    int minGood = max(1,spanX / 3);
    int greens = 0;
    int fakegood = 0;

    for (j = 1; count < minCount && left.y + dir * j >= 0 && left.y + dir * j < IMAGE_HEIGHT && badLines < 2 && greens < max(minRun, maxgreen); j++) {
        //count = 0;
        good = 0;
        run = 0;
        greens = 0;
        fakegood = 0;
        int actualX = xProject(left.x, left.y, left.y + dir * j);
        for (int i = actualX; count < minCount && i <= actualX + spanX && greens < maxgreen; i++) {
            theSpot = yProject(actualX, left.y + dir * j, i);
            //drawPoint(i, theSpot, RED);
            if (theSpot < 0 || theSpot > IMAGE_HEIGHT - 1 || theSpot - dir < 0 ||
                theSpot - dir > IMAGE_HEIGHT - 1 || i < 0 || i > IMAGE_WIDTH - 1) {
                // assume the best?
                fakegood++;
            } else {
                if (checkEdge(i, theSpot, i, theSpot - dir)) {
                    //count++;
                }
                int curcol = thresh->thresholded[theSpot][i];
                if (curcol == c || curcol == c2) {
                    good++;
                    run = 0;
                } else {
                    if (curcol != ORANGE && curcol != WHITE)
                        run++;
                    if (curcol == GREEN) {
                        greens++;
                        //if (greens > 1)
                        //cout << "Greens " << greens << " " << theSpot << endl;
                    }
                }
            }
        }
        good += fakegood / 2;
        if (good < minGood)
            badLines++;
    }
    //cout << good << " " << minGood << " " << run << " " << minRun << endl;
    // if we had to stop because we hit the left edge, then see if we can go farther by using the bottom
    int temp = left.y;
    left.y =  left.y + dir * (j - badLines) - dir;
    left.x = xProject(left.x, temp, left.y);
    right.x = left.x + spanX;
    right.y = left.y + spanY;
#if ROBOT(NAO)
    if (!up && thresh->getVisionHorizon() > left.y) {
        // for the heck of it let's scan down
        // TODO: change to increment by 1
        int found = left.y;
        for (int d = left.y; d < thresh->getVisionHorizon(); d+=2) {
            good = 0;
            for (int a = left.x; a < right.x; a++) {
                if (thresh->thresholded[d][a] == c) {
                    good++;
                }
            }
            if (good > spanX * 0.5) {
                found = d;
            }
        }
        if (found != left.y) {
            left.y = found;
            left.x = xProject(left.x, temp, left.y);
            right.x = left.x + spanX;
            right.y = left.y + spanY;
            drawPoint(right.x, right.y, ORANGE);
            findTrueLineHorizontalSloped(left, right, c, c2, up);
        }
    }
#endif
}

/*  Given two points that should define an edge of a structure, try and improve them to find the true edge.
 *  This is just like the previous function except that it doesn't assume a perfectly level plane.  WHich
 *  of course makes it a lot harder.  C'est la vie.
 * @param left        the leftmost estimated point
 * @param right       the rightmost estimated point
 * @param c           the primary color of the structure
 * @param c2          the secondary color of the structure
 * @param up          whether the open space is up or down
 */

int ObjectFragments::findTrueLineHorizontal(point <int> left, point <int> right, int c, int c2, bool up) {
    int spanX = right.x - left.x + 1;
    int count = 0;
    int good = spanX;
    int j = 0;
    int dir = 1;
    if (up)
        dir = -1;
    for (j = 1; count < spanX / 3 && left.y + dir * j >= 0 && left.y + dir * j < IMAGE_HEIGHT && good > spanX / 2; j++) {
        count = 0;
        good = 0;
        for (int i = left.x; count < spanX / 3 && i <= right.x; i++) {
            int theSpot = left.y + dir * j;
            if (checkEdge(i, theSpot, i, theSpot - dir)) {
                count++;
            }
            int curcol = thresh->thresholded[theSpot][i];
            if (curcol == c || curcol == c2) {
                good++;
            }
        }
    }
    return left.y + dir * j - dir;
}

// is our object on a slant?
void ObjectFragments::correct(blob & post, int c, int c2) {
    if (c2 != 10000) return;
    // scan along the bottom
    //int bad1 = -1, bad2 = -1, temp = 0, good = 0;
    int x = max(0, post.leftTop.x), y = max(0, post.leftTop.y);
    int startX = x, startY = y;
    // start with the upper left corner
    vertScan(x, y, 1, 4, c, c2);
    if (post.leftBottom.y - post.leftTop.y < 75) return;
    if (scan.good == 0) {
        // make sure
        vertScan(post.rightBottom.x, post.rightBottom.y, -1, 4, c, c2);
        if (scan.good == 0) {
            // scan until we actually have a point
            for ( ; x > -1 && y > -1; ) { // go until we hit enough bad pixels
                //cout << "Vert scan " << x << " " << y << endl;
                if (thresh->thresholded[y][x] == c || thresh->thresholded[y][x] == c2) { // if it is the color we're looking for - good
                    break;
                }
                y = y + 1;
                x = xProject(startX, startY, y);
            }
            int newx = x, newy = y, count = 0;
            if (CORRECT) {
                drawPoint(x, y, PINK);
            }
            // now scan horizontally
            for (x = startX, y = startY; x < IMAGE_WIDTH; ) {
                if (thresh->thresholded[y][x] == c || thresh->thresholded[y][x] == c2) { // if it is the color we're looking for - good
                    break;
                }
                x++; // we just do on a pure horizontal so we don't need to worry about going offscreen
                count++;
            }
            if (CORRECT) {
                drawPoint(x, y, ORANGE);
            }
            int counter = count;
            // we do the inverse of the slope formula since we are calculating the perpindicular line
            if (y != newy) {
                float newslope = (float)(newx - x) / (float)(y - newy) ;
                if (abs(newslope - slope) < 0.5 && abs(newslope - slope) > 0.05) {
                    if (CORRECT) {
                        drawBlob(post, PINK);
                    }
                    post.leftTop.x = startX + count;
                    post.rightTop.x = post.rightTop.x + count;
                    post.leftBottom.x = post.leftBottom.x - count;
                    post.rightBottom.x = post.rightBottom.x - count;
                    // Now sometimes our right side estimate won't be so good
                    y = max(0, post.rightTop.y);
                    x = post.rightTop.x;
                    count = 0;
                    for ( ; x > post.leftTop.x; ) {
                        int pix = thresh->thresholded[y][x];
                        if (pix == c || pix == c2) {
                            break;
                        }
                        x--; // we just do on a pure horizontal so we don't need to worry about going offscreen
                        count++;
                    }
                    int count2 = 0;
                    y = post.rightBottom.y -1;
                    x = post.rightBottom.x;
                    for ( ; x > post.leftBottom.x; ) {
                        int pix = thresh->thresholded[y][x];
                        if (pix == c || pix == c2) {
                            break;
                        }
                        x--; // we just do on a pure horizontal so we don't need to worry about going offscreen
                        count2++;
                    }
                    count = min(min(count, count2), counter);
                    post.rightBottom.x = post.rightBottom.x - count;
                    post.rightTop.x = post.rightTop.x - count;
                }
            }
        }
    } else {
        x = min(IMAGE_WIDTH - 1,post.rightTop.x); y = max(0,post.rightTop.y);
        startX = x; startY = y;
        vertScan(x, y, 1, 4, c, c2);
        if (scan.good == 0) {
            vertScan(max(0, post.leftBottom.x), min(post.leftBottom.y, IMAGE_HEIGHT - 1), -1, 4, c, c2);
            if (scan.good == 0) {
                // scan until we actually have a point
                for ( ; x > -1 && y > -1; ) { // go until we hit enough bad pixels
                    //cout << "Vert scan " << x << " " << y << endl;
                    if (thresh->thresholded[y][x] == c || thresh->thresholded[y][x] == c2) { // if it is the color we're looking for - good
                        break;
                    }
                    y = y + 1;
                    x = xProject(startX, startY, y);
                }
                int newx = x, newy = y, count = 0;
                if (CORRECT) {
                    drawPoint(x, y, PINK);
                }
                // now scan horizontally
                for (x = startX, y = startY; x < IMAGE_WIDTH; ) {
                    if (thresh->thresholded[y][x] == c || thresh->thresholded[y][x] == c2) { // if it is the color we're looking for - good
                        break;
                    }
                    x--; // we just do on a pure horizontal so we don't need to worry about going offscreen
                    count++;
                }
                if (CORRECT) {
                    drawPoint(x, y, ORANGE);
                    cout << "XY " << x << " " << y << endl;
                }
                int counter = count;
                // we do the inverse of the slope formula since we are calculating the perpindicular line
                if (y != newy) {
                    float newslope = (float)(newx - x) / (float)(y - newy) ;
                    if (abs(newslope - slope) < 0.5 && abs(newslope - slope) > 0.05) {
                        if (CORRECT) {
                            drawBlob(post, PINK);
                        }
                        post.leftTop.x = post.leftTop.x - count;
                        post.rightTop.x = post.rightTop.x - count;
                        post.leftBottom.x = post.leftBottom.x + count;
                        post.rightBottom.x = post.rightBottom.x + count;
                        // Now sometimes our left side estimate won't be so good
                        y = max(0, post.leftTop.y);
                        x = post.leftTop.x;
                        count = 0;
                        for ( ; x < post.rightTop.x; ) {
                            int pix = thresh->thresholded[y][x];
                            if (pix == c || pix == c2) {
                                break;
                            }
                            x++; // we just do on a pure horizontal so we don't need to worry about going offscreen
                            count++;
                        }
                        int count2 = 0;
                        y = post.leftBottom.y - 1;
                        x = post.leftBottom.x;
                        for ( ; x < post.rightBottom.x; ) {
                            int pix = thresh->thresholded[y][x];
                            if (pix == c || pix == c2) {
                                break;
                            }
                            x++; // we just do on a pure horizontal so we don't need to worry about going offscreen
                            count2++;
                        }
                        count = min(min(count, count2), counter);
                        post.leftBottom.x = post.leftBottom.x + count;
                        post.leftTop.x = post.leftTop.x - count;
                        if (CORRECT) {
                            drawBlob(post, GREEN);
                        }
                    }
                }
            }
        }
    }

    // count up how many pixels from the left boundary until we hit one that has the right color
    /*do {
      temp = thresh->thresholded[post.leftBottom.x + bad1][post.leftBottom.y];
      bad1++;
      } while (temp != c && temp != c2);
      do {
      temp = thresh->thresholded[post.rightBottom.x - bad1][post.rightBottom.y];
      bad2++;
      } while (temp != c && temp != c2);
      if (bad1 > bad2) {
      // possible slant to the left as we look at picture
      } else if (bad2 > bad1) {
      // possible slant to the right
      // in theory there should be a like amount of "extra" pixels on top - let's check it out
      horizontalScan(post.topRight.x, post.topRight.y, 1, 3, c, c2, 0, IMAGE_WIDTH - 1);
      if (scan.good > 1) {
      }
      }*/
}



// /*  This method finds squares of a certain color under size constraints.  It is primarily
//  * used to find backstops within goals.
//  * @param x         x value of our starter point
//  * @param y         y value of our starter point
//  * @param c         the primary color
//  * @param c2        the secondary color
//  * @param left      the leftmost X boundary possible
//  * @param right     the rightmost X boundary possible
//  */

// void ObjectFragments::constrainedSquare(int x, int y, int c, int c2, int left, int right) {
//   obj.leftTop.x = BADVALUE; obj.leftTop.y = BADVALUE;       // so we can check for failure
//   int nextx = x, nexty = y;
//   int top = 0, topx = 0, bottom = 0, bottomx = 0;
//   int h = 6, previousH = 0;
//   obj.leftBottom.y = 0;
//   // first we try going up
//   vertScan(nextx, nexty, -1,  3, c, c2);
//   h = scan.good;
//   // at this point we have a very rough idea of how tall the square is
//   vertScan(nextx, nexty, 1,  3, c, c2);
//   h += scan.good;
//   int initialH = h;
//   // going left
//   for (int i = 0; i < 4 && nextx > left; i++) {
//     do {
//       previousH = h;
//       // first we try going up
//       vertScan(nextx, nexty, -1,  4, c, c2);
//       h = scan.good;
//       // at this point we have a very rough idea of how tall the square is
//       top = scan.y;
//       topx = scan.x;
//       vertScan(nextx, nexty, 1,  4, c, c2);
//       bottom = scan.y;
//       bottomx = scan.x;
//       h += scan.good;
//       nextx--;
//       nexty = top + (bottom - top) / 2;
//       if (h > 5) {
// 	obj.leftTop.x = topx;
// 	if (i == 0)
// 	  obj.leftTop.y = top;
// 	obj.leftBottom.x = bottomx;
// 	if (bottom > obj.leftBottom.y && i == 0) {
// 	  obj.leftBottom.y = bottom;
// 	  //drawPoint(bottomx, bottom, BLACK);
// 	}
//       }
//     } while (nextx > left && h > previousH / 2 && h > 5 ); // && h < initialH +6);
//     h = initialH;
//   }
//   // going right
//   nextx = x;
//   nexty = y;
//   h = 6;
//   obj.rightBottom.y = 0;
//   for (int i = 0; i < 4 && nextx < right; i++) {
//     do {
//       // first we try going up
//       previousH = h;
//       vertScan(nextx, nexty, -1,  4, c, c2);
//       h = scan.good;
//       // at this point we have a very rough idea of how tall the square is
//       top = scan.y;
//       topx = scan.x;
//       vertScan(nextx, nexty, 1,  4, c, c2);
//       bottom = scan.y;
//       bottomx = scan.x;
//       //drawPoint(bottomx, bottom, RED);
//       h += scan.good;
//       nextx++;
//       nexty = top + (bottom - top) / 2;
//       if (h > 5) {
// 	obj.rightTop.x = topx;
// 	if (i == 0)
// 	  obj.rightTop.y = top;
// 	obj.rightBottom.x = bottomx;
// 	if (bottom > obj.rightBottom.y && i == 0) {
// 	  obj.rightBottom.y = bottom;
// 	  //drawPoint(bottomx, bottom, RED);
// 	}
//       }
//     } while (nextx < right && h > previousH / 2 && h > 5 ); //&& h < initialH + 6);
//   }
// }

/*  Routine to find a general square goal.  The Aibo version is more general than the Nao version
 * and should end up being the final version soon.  We start with a point.  We scan up from the
 * point and down from the point looking for a strip of the right color.  That serves as our
 * starting point.  Then we try expanding the sides outward.  The we try expanding the top and
 * bottom in a similar fashion.
 * @param x         x value of our starter point
 * @param y         y value of our starter point
 * @param c         the primary color
 * @param c2        the secondary color
 */

void ObjectFragments::squareGoal(int x, int y, int c, int c2) {
    obj.leftTop.x = BADVALUE; obj.leftTop.y = BADVALUE;       // so we can check for failure
    // first we try going up
    vertScan(x, y, -1,  3, c, c2);
    int h = scan.good;
    // at this point we have a very rough idea of how tall the square is
    int top = scan.y;
    int topx = scan.x;
    // now go down
    vertScan(x, y, 1,  3, c, c2);
    h += scan.good;
    if (h < 2) return;
    int bottom = scan.y;
    int bottomx = scan.x;
    //drawPoint(topx, top, RED);
    //drawPoint(bottomx, bottom, RED);
    obj.leftTop.x = topx; obj.leftTop.y = top; obj.rightTop.x = topx; obj.rightTop.y = top;
    obj.leftBottom.x = bottomx; obj.leftBottom.y = bottom; obj.rightBottom.x = bottomx; obj.rightBottom.y = bottom;
    //int spanY = obj.leftBottom.y - obj.leftTop.y;
    findTrueLineVerticalSloped(obj.leftTop, obj.leftBottom, c, c2, true);
    //drawPoint(obj.leftTop.x, obj.leftTop.y, RED);
    //drawPoint(obj.leftBottom.x, obj.leftBottom.y, RED);
    findTrueLineVerticalSloped(obj.rightTop, obj.rightBottom, c, c2, false);
    findTrueLineHorizontalSloped(obj.leftTop, obj.rightTop, c, c2, true);
    findTrueLineHorizontalSloped(obj.leftBottom, obj.rightBottom, c, c2, false);
    correct(obj, c, c2);
    //drawBlob(obj, ORANGE);
}


/* The next group of functions has to do with the creation of square objects.
 */

/*  Our "main" methods.  Entry points into just about everything else.
 */


/* This is the entry  point from object recognition in Threshold.cc  For now it is only called on yellow, blue and orange.
 * In each case we search for objects that have the corresponding colors.
 * @param  c            color we are processing
 * @return              always 0
 */

void ObjectFragments::createObject(int c) {
    // these are in the relative order that they should be called
    switch (color) {
    case GREEN:
        break;
    case BLUE:
        // either we should see a marker or a goal
        blue(c);
        break;
    case RED:
#if ROBOT(NAO)
        // George: I am disabling robot recognition for now because it
        // causes crashes. The blobs formed for robots have negative
        // and/or incorrect dimensions. Those dimensions are later used
        // to access the thresholded array.
        //robot(c);
#endif
        break;
    case NAVY:
#if ROBOT(NAO)
        robot(c);
#endif
        break;
    case YELLOW:
        // either we should see a marker or a goal
        yellow(c);
        break;
    case ORANGE:
        balls(c, vision->ball);
        // the ball
        break;
#ifdef USE_PINK_BALL
    case PINK:
        balls(c, vision->pinkBall);
#endif
    case BLACK:
        break;
    }
}


int ObjectFragments::horizonAt(int x) {
    return yProject(0, thresh->getVisionHorizon(), x);
}

int ObjectFragments::blobArea(blob a) {
    return blobWidth(a) * blobHeight(a);
}

int ObjectFragments::blobWidth(blob a) {
    return a.rightTop.x - a.leftTop.x + 1;
}

int ObjectFragments::blobHeight(blob a) {
    return a.leftBottom.y - a.leftTop.y + 1;
}


/* Process yellow:
 * First we scan for beacons, based on the firstBlueYellow in the RLE as a
 * starting point looking for posts.
 * @param bigGreen      green horizon value at x == 0
 */
void ObjectFragments::yellow(int bigGreen) {
    goalScan(vision->yglp, vision->ygrp, vision->ygCrossbar, YELLOW, ORANGEYELLOW,
             false, bigGreen);
    //naoScan(vision->yglp, vision->ygrp, vision->ygCrossbar, YELLOW, ORANGEYELLOW, bigGreen);
}

/* Process blue:
 * Same as for yellow except we theoretically don't need to worry about finding
 * beacons.
 * @param bigGreen       field horizon at x == 0
 */

void ObjectFragments::blue(int bigGreen) {

    goalScan(vision->bglp, vision->bgrp, vision->bgCrossbar, BLUE, BLUEGREEN,
             false, bigGreen);
    // Now that we're done, print out information on all of the objects that we've found.
    printObjs();

}

/* A collection of miscelaneous methods used in processing goals.
 */


/* Find the index of the biggest run left.  The "run" is our basic currency in
 * vision.  Our idea in looking for the goal is to find the biggest run we can
 * and assume that it is part of a goal beacon.  We look from there for a big
 * square of the right color.  This method is very simple, it just scans all of
 * the runs between "left" and "right" and picks out the bigest one.  Once done
 * it returns the corresponding index.
 *
 * @param left    the left boundary of legal runs to consider
 * @param right   the right boundary of legal runs to consider
 * @param hor     a horizon boundary that we do not currently use
 * @return index  the index of the largest run that meets the criteria
 */
int ObjectFragments::getBigRun(int left, int right, int hor) {
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
        if (nextH > maxRun && (nextX < left || nextX > right)) { // hack for pictures with tape measure && nextY < hor) {
            maxRun = nextH;
            index = i;
        }
    }
    return index;
}


/*  As we saw with beacons, we tend to work with blobs for convenience.  So at
 *  some point need to transfer their contents over to the field object that
 * we have identified.
 * In this case we have a goal.  Before we commit we make sure it has enough of
 * the right color.  We also collect up certainty information and pass that
 * along.
 * @param one             the field object we'd like to update
 * @param two             the blob that contains the information we need
 * @param certainty       how certain are we of its ID?
 * @param distCertainty   how certain are we of how big we think it is?
 */
bool ObjectFragments::updateObject(VisualFieldObject* one, blob two,
                                   certainty _certainty,
                                   distanceCertainty _distCertainty) {
    //cout << "Got an object" << endl;
    // before we do this let's make sure that the object is really our color
    if (rightBlobColor(two, NORMALPOST)) {
        one->updateObject(&two, _certainty, _distCertainty);
        return true;
    } else {
        //cout << "Screening object for low percentage of real color" << endl;
        return false;
    }
}

/* Here we are trying to figure out how confident we are about our values with
 * regard to how they might be used for distance calculations.  Basically if
 * an object is too near one of the screen edges, or if we have some evidence that
 * it is occluded then we note uncertainty in that dimension.
 * @param left     the leftmost point of the object
 * @param right    the rightmost point of the object
 * @param top      the topmost point of the object
 * @param bottom   the bottommost point of the object
 * @return         a constant indicating where the uncertainties (if any) lie
 */
distanceCertainty ObjectFragments::checkDist(int left, int right, int top,
                                             int bottom) {
    distanceCertainty dc = BOTH_SURE;
    int nextX, nextY;
    if (left < DIST_POINT_FUDGE || right > IMAGE_WIDTH - DIST_POINT_FUDGE) {
        if (top < DIST_POINT_FUDGE || bottom > IMAGE_HEIGHT - DIST_POINT_FUDGE) {
            return BOTH_UNSURE;
        } else {
            dc =  WIDTH_UNSURE;
        }
    } else if (top < DIST_POINT_FUDGE || bottom > IMAGE_HEIGHT - DIST_POINT_FUDGE) {
        return HEIGHT_UNSURE;
    }
    // we need to do one more check - make sure that the bottom of the object is not obscured
    nextX = pole.leftBottom.x;
    nextY = pole.leftBottom.y;
    do {
        vertScan(nextX, nextY, 1,  6, GREEN, GREEN);
        nextX = nextX + 2;
        nextY = yProject(pole.leftBottom, nextX);
    } while (nextX <= pole.rightBottom.x && scan.good < 2);
    if (scan.good > 1)
        return dc;
    else if (dc == WIDTH_UNSURE)
        return BOTH_UNSURE;
    return HEIGHT_UNSURE;
}

/* Post recognition for NAOs
 */

/*  Look for a crossbar once we think we've found a goalpost.  The idea is pretty simple, if we
 *  have done a good job with the square post, then the crossbar should either shoot out of the
 *  left or right top corner.  So try scanning from those positions.  The tricky part, or course
 *  involves being able to definitively say that what we see is in fact a bar.  I'd say that
 *  right now this probably needs more work.
 *  @param b     the square post
 *  @return      a constant, either RIGHT or LEFT if a crossbar found, or NOPOST if not
 */
int ObjectFragments::crossCheck(blob b) {
    // try and find the cross bar - start at the upper left corner
    int biggest = 0, biggest2 = 0;
    int x = b.leftTop.x;
    int y = b.leftTop.y;
    int h = blobHeight(b);
    int w = blobWidth(b);
    int need = min(w / 2, 20);
    // TODO: change +2 to +1
    for (int i = 0; i < h / 5 && biggest < need; i+=2) {
        int tx = xProject(x, y, y + i);
        horizontalScan(tx, y + i, -1, 6, color, color, max(0, x - 2 * w), IMAGE_WIDTH - 1);
        if (scan.good > biggest) {
            biggest = scan.good;
        }
    }
    x = b.rightTop.x;
    y = b.rightTop.y;
    // TODO: change +2 to +1
    for (int i = 0; i < h / 5 && biggest2 < need; i+=2) {
        int tx = xProject(x, y, y + i);
        horizontalScan(tx, y + i, 1, 6, color, color, x - 1, IMAGE_WIDTH - 1);
        if (scan.good > biggest2) {
            biggest2 = scan.good;
        }
    }
    if (POSTLOGIC) {
        cout << "Cross check " << biggest << " " << biggest2 << endl;
    }
    if (biggest >= need && (biggest2 < 3 || biggest > 2 * biggest2)) {
        // make sure we're not just off badly
        y = b.leftTop.y + h / 4;
        x = xProject(b.leftTop, y);
        //drawPoint(x, y, WHITE);
        //drawRect(x, y, 20, 20, ORANGE);
        biggest = 0;
        // TODO: change +2 to +1
        for (int i = 0; i < h / 5 && biggest < 5; i+=2) {
            int tx = xProject(x, y, y + i);
            horizontalScan(tx, y + i, -1, 3, color, color, x - 12, IMAGE_WIDTH - 1);
            if (scan.good > biggest) {
                biggest = scan.good;
            }
        }
        if (biggest < 10)
            return RIGHT;
    }
    if (biggest2 > need && (biggest < 3 || biggest2 > 2 * biggest)) {
        y = b.rightTop.y + h / 4;
        x = xProject(b.rightTop, y);
        //drawPoint(x, y, WHITE);
        //drawRect(x, y, 20, 20, WHITE);
        biggest2 = 0;
        // TODO: change +2 to +1
        for (int i = 0; i < h / 5 && biggest2 < 5; i+=2) {
            int tx = xProject(x, y, y + i);
            horizontalScan(tx, y + i, 1, 3, color, color, x - 1, x + 13);
            if (scan.good > biggest2) {
                biggest2 = scan.good;
            }
        }
        if (biggest2 < 10)
            return LEFT;
    }
    return NOPOST;
}

int ObjectFragments::crossCheck2(blob b) {
    // try and find the cross bar - start at the upper left corner
    //int biggest = 0, biggest2 = 0;
    int x = b.leftTop.x;
    int y = b.leftTop.y;
    int h = b.leftBottom.y - b.leftTop.y;
    //int w = b.rightTop.x - b.leftTop.x;
    int lefties = 0, righties = 0;
    // TODO: change +2 to +1
    for (int i = x - 10; i > max(0, x - h); i-=2) {
        int yp = yProject(x, y, i);
        for (int j = max(0,yp); j < yp+10; j++) {
            if (thresh->thresholded[j][i] == color) {
                lefties++;
            }
        }
    }
    // TODO: change +2 to +1
    for (int i = b.rightTop.x + 10; i < min(IMAGE_WIDTH - 1, b.rightTop.x + h); i+=2) {
        int yp = yProject(b.rightTop.x, b.rightTop.y, i);
        for (int j = max(0,yp); j < yp+10; j++) {
            if (thresh->thresholded[j][i] == color) {
                righties++;
            }
        }
    }
    // TODO: look at constants 50 and 10
    if (righties > 50 && righties > 10 * lefties)
        return LEFT;
    if (lefties > 50 && lefties > 10 * righties)
        return RIGHT;
    return NOPOST;
}

int ObjectFragments::checkIntersection(blob post) {
    if (post.rightBottom.y - post.rightTop.y < 30) return NOPOST;
    const list <VisualCorner>* corners = vision->fieldLines->getCorners();
    int spanx = blobWidth(post);
    int spany = blobHeight(post);
    for (list <VisualCorner>::const_iterator k = corners->begin();
         k != corners->end(); k++) {
        if (k->getShape() == T) {
            if (POSTLOGIC) {
                cout << "Got a T" << endl;
            }
            //int mid = midPoint(post.leftBottom.x, post.rightBottom.x);
            int x = k->getX();
            int y = k->getY();
            bool swap = false;
            if (y < post.leftBottom.y) swap = true;
            int close = max(spanx, spany / 4);
            if (x <= post.leftBottom.x && x > post.leftBottom.x - 2 * close) {
                //if (swap) return RIGHT;
                return LEFT;
            }
            if (x >= post.rightBottom.x && x < post.rightBottom.x + 2 * close) {
                //if (swap) return LEFT;
                return RIGHT;
            }
            if (x < post.leftBottom.x || x > post.rightBottom.x) return NOPOST;
            // if we're here we're probably at the side of the goal or looking at a beacon
            //cout << "here" << endl;
            int testX1 = midPoint(post.rightBottom.x, IMAGE_WIDTH - 1);
            int testX2 = midPoint(0, post.leftBottom.x);
            //int cross1 = 0, cross2 = 0;
            // get the tstem and see if it intersect a plumb-line right or left
            VisualLine tstem = k->getTStem();
            point <int> plumbLineTop, plumbLineBottom;
            plumbLineTop.x = testX1; plumbLineTop.y = post.rightBottom.y;
            plumbLineBottom.x = testX1; plumbLineBottom.y = IMAGE_HEIGHT - 1;
            pair<int, int> foo = Utility::
                plumbIntersection(plumbLineTop, plumbLineBottom, tstem.start, tstem.end);
            if (foo.first != NO_INTERSECTION && foo.second != NO_INTERSECTION) {
                if (swap) return RIGHT;
                return LEFT;
            }
            plumbLineTop.x = testX2; plumbLineTop.y = post.leftBottom.y;
            plumbLineBottom.x = testX2; plumbLineBottom.y = IMAGE_HEIGHT - 1;
            foo = Utility::plumbIntersection(plumbLineTop, plumbLineBottom, tstem.start, tstem.end);
            if (foo.first != NO_INTERSECTION && foo.second != NO_INTERSECTION) {
                if (swap) return LEFT;
                return RIGHT;
            }
            //cout << "here 2" << endl;
            // now test in "in the goal" cases
            plumbLineTop.x = testX1; plumbLineTop.y = 0;
            plumbLineBottom.x = testX1; plumbLineBottom.y = post.leftBottom.y;
            foo = Utility::plumbIntersection(plumbLineTop, plumbLineBottom, tstem.start, tstem.end);
            if (foo.first != NO_INTERSECTION && foo.second != NO_INTERSECTION) {
                if (swap) return RIGHT;
                return LEFT;
            }
            plumbLineTop.x = testX2; plumbLineTop.y = 0;
            plumbLineBottom.x = testX2; plumbLineBottom.y = post.rightBottom.y;
            foo = Utility::plumbIntersection(plumbLineTop, plumbLineBottom, tstem.start, tstem.end);
            if (foo.first != NO_INTERSECTION && foo.second != NO_INTERSECTION) {
                if (swap) return LEFT;
                return RIGHT;
            }
        } else {
            //cout << "Got a corner " << (k->getX()) << " " << (k->getY()) << " " << post.leftBottom.x << " " << post.rightBottom.x << " " << post.leftBottom.y << endl;
            if (k->getX() > post.leftBottom.x - spanx && k->getX() < post.rightBottom.x + spanx &&
                post.leftBottom.y < k->getY()) {
                if (k->getX() > post.leftBottom.x - 5 && k->getX() < post.rightBottom.x + 5) {
                    //cout << "Corner underneath" << endl;
                    int whites = 0;
                    for (int i = k->getY(); i > post.leftBottom.y; i--) {
                        if (thresh->thresholded[i][k->getX()] == WHITE) {
                            whites++;
                        }
                    }
                    if (whites > (k->getY() - post.leftBottom.y) / 2) {
                        if (POSTLOGIC) {
                            cout << "Viable corner underneath post" << endl;
                        }
                        point<int> midTop(midPoint(post.leftBottom.x, 0), post.leftBottom.y);
                        point<int> midBottom(midPoint(post.leftBottom.x, 0),
                                             midPoint(midPoint(post.leftBottom.y, IMAGE_HEIGHT - 1), post.leftBottom.y));
                        //drawLine(midTop.x, midTop.y, midBottom.x, midBottom.y, RED);
                        bool intersects = vision->fieldLines->intersectsFieldLines(midTop,midBottom);
                        if (intersects) {
                            return RIGHT;
                        }
                        midTop.x = midPoint(post.rightBottom.x, IMAGE_WIDTH - 1);
                        midBottom.x = midTop.x;
                        //drawLine(midTop.x, midTop.y, midBottom.x, midBottom.y, RED);
                        intersects = vision->fieldLines->intersectsFieldLines(midTop,midBottom);
                        if (intersects) {
                            return LEFT;
                        }
                        midTop.x = midPoint(midTop.x, post.rightBottom.x);
                        midBottom.x = midTop.x;
                        //drawLine(midTop.x, midTop.y, midBottom.x, midBottom.y, RED);
                        intersects = vision->fieldLines->intersectsFieldLines(midTop,midBottom);
                        if (intersects) {
                            return LEFT;
                        }
                        midTop.x = midPoint(midPoint(post.leftBottom.x, 0), post.leftBottom.x);
                        intersects = vision->fieldLines->intersectsFieldLines(midTop,midBottom);
                        if (intersects) {
                            return RIGHT;
                        }
                    }
                } else {
                    //cout << "Offset corner " << endl;
                    // if the corner is near enough we can use it to id the post

                    point<int> midTop(post.leftBottom.x - 10, k->getY() + 10);
                    point<int> midBottom(post.rightBottom.x + 10, k->getY() + 10);
                    bool intersects = vision->fieldLines->intersectsFieldLines(midTop,midBottom);
                    if (intersects) {
                        //cout << "Passed test" << endl;
                        //drawLine(midTop.x, midTop.y, midBottom.x, midBottom.y, RED);
                    } else {
                        midTop.y = k->getY() - 10;
                        midBottom.y = k->getY() - 10;
                        intersects = vision->fieldLines->intersectsFieldLines(midTop,midBottom);
                        if (intersects) {
                            //drawLine(midTop.x, midTop.y, midBottom.x, midBottom.y, RED);
                            //cout << "Passed test 2" << endl;
                        }
                    }
                }
            } else if (k->getX() > post.leftBottom.x - spanx && k->getX() < post.rightBottom.x + spanx &&
                       post.leftBottom.y > k->getY() && k->getY() > post.leftTop.y + (post.leftBottom.y - post.leftTop.y) / 2) {
                if (POSTLOGIC) {
                    cout << "Found a corner near and above the bottom of the post" << endl;
                }
                if (k->getX() < post.leftBottom.x + 5) {
                    return LEFT;
                }
                if (k->getX() > post.rightBottom.x - 5) {
                    return RIGHT;
                }
            }
        }
    }
    return NOPOST;
}

int ObjectFragments::checkCorners(blob post) {
    if (post.rightBottom.y - post.rightTop.y < 30) return NOPOST;
    const list <VisualCorner>* corners = vision->fieldLines->getCorners();
    int spanx = post.rightBottom.x - post.leftBottom.x;
    for (list <VisualCorner>::const_iterator k = corners->begin();
         k != corners->end(); k++) {
        // we've already checked the Ts so ignore them
        if (k->getShape() != T) {
            if (k->getX() > post.leftBottom.x - spanx && k->getX() < post.rightBottom.x + spanx &&
                post.leftBottom.y < k->getY()) {
                if (k->getX() > post.leftBottom.x - 5 && k->getX() < post.rightBottom.x + 5) {
                    // we checked these when we checked for Ts so ignore them
                } else {
                    // if the corner is near enough we can use it to id the post
                    if (k->getY() < post.rightBottom.y + spanx) {
                        if (k->getX() > post.rightBottom.x) return RIGHT;
                        return LEFT;
                    }
                }
            }
        }
    }
    return NOPOST;
}

/*  Sometimes we process posts differently depending on how big they are.
 * This just characterizes a post's size such that we can make that
 * determination.
 *
 * @param b   the post in question
 * @return    a constant indicating size - SMALL, MEDIUM, or LARGE
 */
// TODO: change these constants
int ObjectFragments::characterizeSize(blob b) {
    int w = b.rightTop.x - b.leftTop.x + 1;
    int h = b.leftBottom.y - b.leftTop.y + 1;
    if (h > 60) return LARGE;
    if (h < 20 || w < 10) return SMALL;
    if (h < 40 || w < 20) return MEDIUM;
    return LARGE;
}

/* Sets a standard of proof for a post.  In this case that the blob comprising the post
 * is at least 60% correct color.  Note:  this is actually not the greatest idea when
 * the angle of the head is significantly off horizontal.
 * @param b   the post in question
 * @return    a constant indicating size - SMALL, MEDIUM, or LARGE
 */

bool ObjectFragments::qualityPost(blob b, int c) {
    int good = 0;
    //bool soFar;
    for (int i = b.leftTop.x; i < b.rightTop.x; i++)
        for (int j = b.leftTop.y; j < b.leftBottom.y; j++)
            if (thresh->thresholded[j][i] == c)
                good++;
    if (good < blobArea(b) * 3 / 5) return false;
    return true;
}

/* Provides a kind of sanity check on the size of the post.  Essentially we are looking for
 * cases where we don't have a post, but are looking at a backstop.  Also just let's us know
 * how good the size estimate is.  This needs lots of beefing up.
 * @param b   the post in question
 * @return    a constant indicating size - SMALL, MEDIUM, or LARGE
 */

bool ObjectFragments::checkSize(blob b, int c) {
    int midY = b.leftTop.y + (b.leftBottom.y - b.leftTop.y) / 2;
    horizontalScan(b.leftTop.x, midY, -1,  6, c, c, 0, b.leftTop.x + 1);
    drawPoint(scan.x, scan.y, RED);
    int leftMid = scan.good;
    horizontalScan(b.rightTop.x, midY, 1, 6, c, c, b.rightTop.x - 1, b.rightTop.x + 10);
    drawPoint(scan.x, scan.y, RED);
    if (leftMid > 5 && scan.good > 5) return false;
    return true;
}

/* Shooting stuff */

/* Determines shooting information. Basically scans down from backstop and looks for occlusions.
 * Sets the information found within the backstop data structure.
 * @param one the backstop
 */

void ObjectFragments::setShot(VisualCrossbar* one) {
    int pix, bad, white, grey, run, greyrun;
    int ySpan = IMAGE_HEIGHT - one->getLeftBottomY();
    bool colorSeen = false;
    int lx = one->getLeftTopX(), ly = one->getLeftTopY(), rx = one->getRightTopX(), ry = one->getRightTopY();
    int bx = one->getLeftBottomX(), brx = one->getRightBottomX();
    int intersections[3];
    int crossings = 0;
    bool lineFound = false;
    // now let's see if our backstop is "shootable" and where
    for (int i = max(min(lx, bx), 0); i < min(max(rx, brx), IMAGE_WIDTH - 1); i++) {
        bad = 0; white = 0; grey = 0; run = 0; greyrun = 0; colorSeen = false;
        // first - determine if any lines intersection this plumbline and where
        point <int> plumbLineTop, plumbLineBottom, line1start, line1end;
        plumbLineTop.x = i; plumbLineTop.y = ly;
        plumbLineBottom.x = i; plumbLineBottom.y = IMAGE_HEIGHT;
        const vector <VisualLine>* lines = vision->fieldLines->getLines();
        crossings = 0;
        for (vector <VisualLine>::const_iterator k = lines->begin();
             k != lines->end(); k++) {
            pair<int, int> foo = Utility::
                plumbIntersection(plumbLineTop, plumbLineBottom,
                                  k->start, k->end);
            if (foo.first != NO_INTERSECTION && foo.second != NO_INTERSECTION) {
                intersections[crossings] = foo.second;
                crossings++;
                if (crossings == 3) {
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
                if (strip > 5)
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
                        if (intersections[k] - j < 10 && intersections[k] - j > 0) {
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
            } else if (colorSeen && pix == GREY || pix == BLACK) {
                grey++;
                greyrun++;
            } else if (pix == GREEN || pix == BLUEGREEN) {
                run = 0;
                greyrun = 0;
                lineFound = false;
            }
            if (greyrun > 15) {
                //shoot[i] = false;
                if (BACKDEBUG) {
                    //drawPoint(i, j, RED);
                }
            }
            if (run > 10 && (pix == NAVY || pix == RED)) {
                shoot[i] = false;
                if (BACKDEBUG)
                    drawPoint(i, j, RED);
            }
            if (run > 10) {
                shoot[i] = false;
                if (BACKDEBUG) {
                    drawPoint(i, j, RED);
                }
            }
        }
        if (bad > ySpan / 5) {
            shoot[i] = false;
        }
    }
    // now find the range of shooting
    int r1 = IMAGE_WIDTH / 2;
    int r2 = IMAGE_WIDTH / 2;
    for ( ;r1 < brx && r1 >= bx && shoot[r1]; r1--) {}
    for ( ;r2 > bx && r2 <= rx && shoot[r2]; r2++) {}
    if (r2 - r1 < MINSHOTWIDTH || abs(r1 - IMAGE_WIDTH / 2) < MINSHOTWIDTH / 2 || abs(r2 - IMAGE_WIDTH / 2) < MINSHOTWIDTH) {
        one->setShoot(false);
        one->setBackLeft(-1);
        one->setBackRight(-1);
    } else {
        one->setShoot(true);
        if (BACKDEBUG) {
            drawLine(r1, ly, r1, IMAGE_HEIGHT - 1, RED);
            drawLine(r2, ly, r2, IMAGE_HEIGHT - 1, RED);
        }
    }
    one->setBackLeft(r1);
    one->setBackRight(r2);

    if (BACKDEBUG) {
        drawPoint(r1, ly, RED);
        drawPoint(r2, ly, BLACK);
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
    if (BACKDEBUG) {
        cout << "Crossbar info: Left Col: " << r1 << " Right Col: " << r2 << " Dir: " << one->getBackDir();
        if (one->shotAvailable())
            cout << " Take the shot!" << endl;
        else
            cout << " Don't shoot!" << endl;
    }
}
void ObjectFragments::bestShot(VisualFieldObject* left,
                               VisualFieldObject* right,
                               VisualCrossbar* middle) {
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
    if (big > 10) {
        int bot = max(horizonAt(index), bottom);
        middle->setLeftTopX(index);
        middle->setLeftTopY(bot - 20);
        middle->setLeftBottomX(index);
        middle->setLeftBottomY(bot);
        middle->setRightTopX(indexr);
        middle->setRightTopY(bot - 20);
        middle->setRightBottomX(indexr);
        middle->setRightBottomY(bot);
        middle->setX(index);
        middle->setY(bot);
        middle->setWidth(big);
        middle->setHeight(20);
        middle->setCenterX(middle->getLeftTopX() + ROUND2(middle->getWidth() / 2));
        middle->setCenterY(middle->getRightTopY() + ROUND2(middle->getHeight() / 2));
        middle->setDistance(1);

    }
}

/* Determines what is the most open part of the field.  Basically scans up and looks for occlusions.
 */
void ObjectFragments::openDirection(int horizon, NaoPose *pose) {
    const int divider = 10;

    int pix, bad, white, grey, run, greyrun;
    int intersections[5];
    int crossings = 0;
    bool lineFound = false;
    int y;
    int open[IMAGE_WIDTH / divider];
    int open2[IMAGE_WIDTH / divider];
    //cout << "In open direction " << endl;
    open[0] = horizon;
    open2[0] = horizon;
    int lastd = 0;
    int sixty = IMAGE_HEIGHT - 1;
    for (int i = IMAGE_HEIGHT - 1; i > horizon; i--) {
        estimate d = pose->pixEstimate(IMAGE_WIDTH / 2, i, 0.0);
        //cout << "Distances " << i << " " << d.dist << endl;
        if (d.dist > 60 && lastd < 60) {
            if (OPENFIELD) {
                drawPoint(IMAGE_WIDTH / 2, i, MAROON);
            }
            sixty = i;
        }
        lastd = (int)d.dist;
    }
    const vector <VisualLine>* lines = vision->fieldLines->getLines();
    for (int x = divider; x < IMAGE_WIDTH - 1; x += divider) {
        bad = 0; white = 0; grey = 0; run = 0; greyrun = 0;
        open[(int)(x / divider)] = horizon;
        open2[(int)(x / divider)] = horizon;
        // first - determine if any lines intersection this plumbline and where
        point <int> plumbLineTop, plumbLineBottom, line1start, line1end;
        plumbLineTop.x = x; plumbLineTop.y = 0;
        plumbLineBottom.x = x; plumbLineBottom.y = IMAGE_HEIGHT;
        crossings = 0;
        for (vector <VisualLine>::const_iterator k = lines->begin();
             k != lines->end(); k++) {
            pair<int, int> foo = Utility::
                plumbIntersection(plumbLineTop, plumbLineBottom,
                                  k->start, k->end);
            if (foo.first != NO_INTERSECTION && foo.second != NO_INTERSECTION) {
                intersections[crossings] = foo.second;
                crossings++;
                if (crossings == 5) {
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
                        if (intersections[k] - y < 10 && intersections[k] - y > 0) {
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
            } else if (pix == GREEN || pix == BLUEGREEN || pix == BLUE || pix == ORANGE) {
                run = 0;
                greyrun = 0;
                lineFound = false;
            }
            if (greyrun == 15) {
                //shoot[i] = false;
                if (open[(int)x / divider] == horizon) {
                    open[(int)x / divider] = y + 15;
                }
                open2[(int)x / divider] = y + 15;
                //drawPoint(x, y, RED);
                //drawPoint(x - 1, y, RED);
                //drawPoint(x + 1, y, RED);
                y = 0;
            }
            if (run == 10) {
                if (open[(int)x / divider] == horizon) {
                    open[(int)x / divider] = y + 10;
                }
                if (bad == 10) {
                    open2[(int)x / divider] = y + 10;
                    y = 0;
                }
                //drawPoint(x, y, RED);
                //drawPoint(x - 1, y, RED);
                //drawPoint(x + 1, y, RED);
            }
            if (run > 10 && (bad > 3 || y < sixty)) {
                open2[(int)x / divider] = y + run;
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
    for (i = 0; i < IMAGE_WIDTH / divider; i++) {
        if (i - jumpdown < 11 && !vert && open[i] > IMAGE_HEIGHT - 10 && jumpdown != -1) {
            vert = true;
        }
        if (open[i] > horizon + 20 &&  lastone < horizon + 20 && i != 0) {
            jumpdown = i;
            vert = false;
        }
        if (vert && lastone > horizon + 20  && open[i] < horizon + 20) {
            //cout << "Testing for vertical " << jumpdown << " " << i << endl;
            if (i - jumpdown < 10 && jumpdown != -1) {
                point<int> midTop(jumpdown * divider,IMAGE_HEIGHT - horizon / 2);
                point<int> midBottom(i * divider,IMAGE_HEIGHT- horizon/2);
                bool intersects = vision->fieldLines->intersectsFieldLines(midTop,midBottom);
                if (intersects) {
                    if (OPENFIELD) {
                        cout << "VERTICAL LINE DETECTED BY OPEN FIELD*********" << endl;
                    }
                    for (int k = jumpdown; k < i; k++) {
                        open[k] = horizon;
                        open2[k] = horizon;
                    }
                }
            }
        }
        lastone = open[i];
        if (open[i] - 10 <= horizon) {
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
    for (i = 1; i < IMAGE_WIDTH / 15; i++) {
        if (open[i] > open[index1]) {
            index1 = i;
        }
        if (open2[i] > open2[index12]) {
            index12 = i;
        }
    }
    index2 = i; index22 = i;
    for (i++ ; i < 2 * IMAGE_WIDTH / 15; i++) {
        if (open[i] > open[index2]) {
            index2 = i;
        }
        if (open2[i] > open2[index22]) {
            index22 = i;
        }
    }
    index3 = i; index32 = i;
    for (i++ ; i < IMAGE_WIDTH / divider; i++) {
        if (open[i] > open[index3]) {
            index3 = i;
        }
        if (open2[i] > open2[index32]) {
            index32 = i;
        }
    }
    // All distance estimates are to the HARD values
    estimate e;
    e = pose->pixEstimate(IMAGE_WIDTH/6, open2[index12], 0.0);
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

    e = pose->pixEstimate(divider*IMAGE_WIDTH/6, open2[index32],0.0);
    vision->fieldOpenings[2].soft = open[index3];
    vision->fieldOpenings[2].hard = open2[index32];
    vision->fieldOpenings[2].horizonDiffSoft = open[index3] - horizon;
    vision->fieldOpenings[2].horizonDiffHard = open2[index32] - horizon;
    vision->fieldOpenings[2].dist = e.dist;
    vision->fieldOpenings[2].bearing = e.bearing;
    vision->fieldOpenings[2].elevation = e.elevation;


    if (OPENFIELD) {
        cout << "Obstacle 1 Dist:" << vision->fieldOpenings[0].dist << endl
             << "Obstacle 2 Dist:" << vision->fieldOpenings[1].dist << endl
             << "Obstacle 3 Dist:" << vision->fieldOpenings[2].dist << endl;
        //drawLine(0, open[index1], IMAGE_WIDTH / 3, open[index1], PINK);
        //drawLine(IMAGE_WIDTH / 3, open[index2], 2 * IMAGE_WIDTH / 3, open[index2], PINK);
        //drawLine(2 * IMAGE_WIDTH / 3, open[index3], IMAGE_WIDTH  - 1, open[index3], PINK);
        //drawLine(0, open[index1] + 1, IMAGE_WIDTH / 3, open[index1] + 1, MAROON);
        //drawLine(IMAGE_WIDTH / 3, open[index2] + 1, 2 * IMAGE_WIDTH / 3, open[index2] + 1, PINK);
        //drawLine(2 * IMAGE_WIDTH / 3, open[index3] + 1, IMAGE_WIDTH  - 1, open[index3] + 1, PINK);
        drawLine(0, open2[index12], IMAGE_WIDTH / 3, open2[index12], MAROON);
        drawLine(IMAGE_WIDTH / 3, open2[index22], 2 * IMAGE_WIDTH / 3, open2[index22], MAROON);
        drawLine(2 * IMAGE_WIDTH / 3, open2[index32], IMAGE_WIDTH  - 1, open2[index32], MAROON);
        drawLine(0, open2[index12] - 1, IMAGE_WIDTH / 3, open2[index12] - 1, MAROON);
        drawLine(IMAGE_WIDTH / 3, open2[index22] - 1, 2 * IMAGE_WIDTH / 3, open2[index22] - 1, MAROON);
        drawLine(2 * IMAGE_WIDTH / 3, open2[index32] - 1, IMAGE_WIDTH  - 1, open2[index32] - 1, MAROON);
        if (open2[index12] != open2[index22]) {
            drawLine(IMAGE_WIDTH / 3, open2[index12], IMAGE_WIDTH / 3, open2[index22], MAROON);
        }
        if (open2[index32] != open2[index22]) {
            drawLine(2 * IMAGE_WIDTH / 3, open2[index32], 2 * IMAGE_WIDTH / 3, open2[index22], MAROON);
        }
        if (open2[index12] <  open2[index22] && open2[index12] < open2[index32]) {
            for (i = IMAGE_WIDTH / 3; open[i / divider] <= open2[index12]; i++) {}
            drawMore(i, open2[index12], PINK);
        }
        else if (open2[index22] <  open2[index12] && open2[index22] < open2[index32]) {
            for (i = IMAGE_WIDTH / 3; open[i / divider] <= open2[index22]; i--) {}
            drawLess(i, open2[index22], PINK);
            for (i = 2 * IMAGE_WIDTH / 3; open[i / divider] <= open2[index22]; i++) {}
            drawMore(i, open2[index22], PINK);
        }
        else if (open2[index32] <  open2[index22] && open2[index32] < open2[index12]) {
            for (i = 2 * IMAGE_WIDTH / 3; open[i / divider] <= open2[index32]; i--) {}
            drawLess(i, open2[index32], PINK);
        }
        else if (open2[index22] ==  open2[index12] && open2[index22] < open2[index32]) {
            for (i = 2 * IMAGE_WIDTH / 3; open[i / divider] <= open2[index22]; i++) {}
            drawMore(i, open2[index22], PINK);
        }
        else if (open2[index22] < open2[index12] && open2[index22] == open2[index32]) {
            for (i = IMAGE_WIDTH / 3; open[i / divider] <= open2[index22]; i--) {}
            drawLess(i, open2[index22], PINK);
        } else if (open2[index12] < open2[index22] && open2[index12] == open2[index32]) {
            // vertical line?
            cout << "Vertical line?" << endl;
        }
        //drawMore(longIndex * divider + longsize * divider + divider, horizon, PINK);
        cout << "Estimate soft is " << open[index1] << " " << open[index2] << " " << open[index3] << endl;
        cout << "Estimate hard is " << open2[index12] << " " << open2[index22] << " " << open2[index32] << endl;
    }
}

/* Try and find the biggest post left on the screen.  We start by looking for our longest
 * "run" of the current color.  We then call squareGoal to expand that into a post.  Later
 * we will check if it actually meets the criteria for a good post.
 * @param c       current color
 * @param c2      secondary color
 * @param horizon green horizon
 * @param left    leftmost limit to look
 * @param right   rightmost limit to look
 * @param         indication of whether we found a decent candidate
 */

int ObjectFragments::grabPost(int c, int c2, int horizon, int left, int right) {
    int maxRun = 0, maxY = 0, maxX = 0, index = 0;
    // find the biggest Run
    index = getBigRun(left, right, horizon);
    if (index == BADVALUE) return NOPOST;
    maxRun = runs[index].h;  maxY = runs[index].y;  maxX = runs[index].x;
    // Try and figure out the true axis-parallel post dimensions - we're going to try and start right in the middle
    int startX = maxX;
    int startY = maxY + maxRun / 2;
    squareGoal(startX, startY, c, c2); //starts a scan in the middle of the tallest run.
    // make sure we're looking at something big enough to be a post
    if (!postBigEnough(obj)) {
        return NOPOST;
    }
    transferToPole();
    return LEFT; // Just return something other than NOPOST
}


/* Another post classification method.  In this one we look left and right of the post trying
 * to find a really long run of the same color.  If we find one this is good evidence since it
 * may correspond to the matching post of this color.
 * @param left       leftmost value (x dim) of the post
 * @param right      rightmost value
 * @param height     the height of the post
 * @return           potential classification
 */

int ObjectFragments::checkOther(int left, int right, int height, int horizon) {
    int largel = 0;
    int larger = 0;
    int mind = max(MIN_POST_SEPARATION, height);
    for (int i = 0; i < numberOfRuns; i++) {
        int nextX = runs[i].x;
        int nextY = runs[i].y;
        int nextH = runs[i].h;
        //int nextB = nextY + nextH;
        if (nextH > 0) { // meanwhile collect some information on which post we're looking at
            if (nextX < left - mind && nextH > MIN_GOAL_HEIGHT && nextY < horizonAt(nextX) &&
                nextY + nextH > horizonAt(nextX) - 10) {
                if (nextH > largel)
                    largel = nextH;
                //drawPoint(nextX, nextY, ORANGE);
                //cout << largel << endl;
            } else if (nextX > right + mind && nextH > MIN_GOAL_HEIGHT && nextY < horizonAt(nextX) &&
                       nextY + nextH > horizonAt(nextX) - 10) {
                if (nextH > larger) {
                    larger = nextH;
                }
            }
}
    }
    if ((larger > height / 2 || larger > 20) && larger > largel) {
        if (POSTLOGIC)
            cout << "Larger" << endl;
        return LEFT;
    } else if (largel > 20 || largel > height / 2) {
        if (POSTLOGIC) cout << "Largel" << endl;
        return RIGHT;
    }
    return NOPOST;
}


/* Main routine for classifying posts (Aibos).  We have a variety of methods to classify posts
 * in our tool box.  The idea is to start with the best ones and keep trying until one produces
 * an answer.
 * @param horizon        the green horizon (y value)
 * @param c              color of the post
 * @param c2             secondary color
 * @param beaconFound      did we find a beacon in this image?
 * @param left           field object to send out if we find a left post
 * @param right          ditto for right post
 * @param mid            ditto for backstop
 * @return               classification
 */

int ObjectFragments::classifyFirstPost(int horizon, int c,int c2,
                                       bool beaconFound,
                                       VisualFieldObject* left,
                                       VisualFieldObject* right,
                                       VisualCrossbar* mid) {
    // ok now we're going to try and figure out which post it is and where the other one might be
    int trueLeft = min(pole.leftTop.x, pole.leftBottom.x);          // leftmost value in the blob
    int trueRight = max(pole.rightTop.x, pole.rightBottom.x);    // rightmost value in the blob
    int trueTop = min(pole.leftTop.y, pole.rightTop.y);             // topmost value in the blob
    int trueBottom = max(pole.leftBottom.y, pole.rightBottom.y); // bottommost value in teh blob
    //int lx = pole.leftTop.x;                                       // save these values in case we need
    int ly = pole.leftTop.y;                                       // to look for the crossbar
    //int rx = pole.rightTop.x;
    // These variables are used to figure out whether we are looking at a right or left post
    // before proclaiming this a post, let's make sure its boundaries are in reasonable places
    int horizonLeft = horizonAt(trueLeft);
    int fakeBottom = max(trueBottom, horizonLeft);
    //int spanX = rx - lx + 1;
    int spanY = pole.leftBottom.y - ly;
    // do some sanity checking - this one makes sure the blob is ok
    if (!locationOk(pole, horizon)) {
        // once again check if the blob is better
        if (topBlob.leftTop.x != BADVALUE && topBlob.rightTop.x - topBlob.leftTop.x
            > 10 && locationOk(topBlob, horizon) && greenCheck(topBlob)) {
            return BACKSTOP;
        }
    }
    if (spanY + 1 == 0) return BADVALUE;
    // first characterize the size of the possible pole
    int howbig = characterizeSize(pole);
    int post = RIGHT;
    int stopp = spanY / 2;
    if (howbig == LARGE)
        stopp = spanY;

    // start the long process of figuring out which post we've got - fortunately
    // with the Naos it is easier
    post = checkOther(trueLeft, trueRight, fakeBottom - trueTop, horizon);
    if (post != NOPOST) {
        if (POSTLOGIC)
            cout << "Found from checkOther" << endl;
        return post;
    }

    post = crossCheck(pole);        // look for the crossbar
    if (post != NOPOST) {
        if (POSTLOGIC) {
            cout << "Found crossbar " << post << endl;
        }
        return post;
    }

    post = checkIntersection(pole);
    if (post != NOPOST) {
        if (POSTLOGIC)
            cout << "Found from Intersection" << endl;
        return post;
    }

    post = crossCheck2(pole);        // look for the crossbar
    if (post != NOPOST) {
        if (POSTLOGIC) {
            cout << "Found crossbar2 " << post << endl;
        }
        return post;
    }

    // //post = triangle(pole);        // look for the triangle
    // if (post != NOPOST) {
    //   if (POSTLOGIC) {
    //     cout << "Found triangle " << post << endl;
    //   }
    //   return post;
    // }
    return post;
}

/* Look for goal posts.  This is the biggest method we've got and probably the hardest and most complicated.
 * However, the basic idea is pretty simple.  We start by looking for the biggest run of color we can find.
 * We then build a rectangular blob out of that color.  From there we need to determine if the rectangular blob
 * is a post, and if so, then which post it is.  If that all goes well we look to see if there is a second post,
 * and potentially a backstop.
 * @param left        the left goal post
 * @param right       the right post
 * @param mid         the backstop
 * @param c           the color we're processing
 * @param c2          the soft color closest to it (e.g. bluegreen for blue)
 * @param beaconFound   did we find a beacon?
 * @param horizon     the green field horizon
 */
// Look for posts and goals given the runs we've collected
void ObjectFragments::goalScan(VisualFieldObject* left,
                               VisualFieldObject* right,
                               VisualCrossbar* mid, int c, int c2,
                               bool beaconFound, int horizon) {
    //cout << horizon << " " << slope << endl;
    // if we don't have any runs there is nothing to do
    if (numberOfRuns <= 1) return;

    int nextX = 0;
    //int nextY = 0;
    int nextH = 0;
    distanceCertainty dc = BOTH_UNSURE;

    int isItAPost = grabPost(c, c2, horizon, IMAGE_WIDTH, -1);
    //drawBlob(obj, ORANGE);
    // make sure we're looking at something big enough to be a post
    if (isItAPost == NOPOST) {
        if (POSTDEBUG) {
            cout << "Not a post" << endl;
        }
        return;
    }
    //drawBlob(obj, PINK);
    //printBlob(obj);
    // ok now we're going to try and figure out which post it is and where the other one might be
    int trueLeft = min(pole.leftTop.x, pole.leftBottom.x);          // leftmost value in the blob
    int trueRight = max(pole.rightTop.x, pole.rightBottom.x);    // rightmost value in the blob
    int trueTop = min(pole.leftTop.y, pole.rightTop.y);             // topmost value in the blob
    int trueBottom = max(pole.leftBottom.y, pole.rightBottom.y); // bottommost value in teh blob
    int lx = pole.leftTop.x;                                       // save these values in case we need
    int ly = pole.leftTop.y;                                       // to look for the crossbar
    int rx = pole.rightTop.x;
    // before proclaiming this a post, let's make sure its boundaries are in reasonable places
    int horizonLeft = horizonAt(trueLeft);
    //if (slope < 0)
    //horizonLeft = yProject(IMAGE_WIDTH - 1, horizon, trueLeft);
    int fakeBottom = max(trueBottom, horizonLeft);
    int spanX = rx - lx + 1;
    int spanY = pole.leftBottom.y - ly;
    int pspanY = fakeBottom - trueTop;
    //cout << "Spans " << spanY << " " << pspanY << endl;
    //int ySpan = pole.leftBottom.y - pole.leftTop.y + 1;
    // do some sanity checking - this one makes sure the blob is ok
    if (!locationOk(pole, horizon)) {

        if (POSTLOGIC)
            cout << "Bad location on post" << endl;
        return;
    }
    if (spanY + 1 == 0) return;
    float rat = (float)(spanX) / (float)(spanY);
    if (!postRatiosOk(rat) && spanY < IMAGE_HEIGHT / 2) {
        return;
    }

    dc = checkDist(trueLeft, trueRight, trueTop, trueBottom);
    // first characterize the size of the possible pole
    int howbig = characterizeSize(pole);
    int post = classifyFirstPost(horizon, c, c2, beaconFound, left, right, mid);
    if (post == LEFT) {
        updateObject(right, pole, _SURE, dc);
    } else if (post == RIGHT) {
        updateObject(left, pole, _SURE, dc);
    } else if (post == BACKSTOP) {
    } else if (pole.rightTop.x - pole.leftTop.x > IMAGE_WIDTH - 3) {
    } else {
        if (howbig == LARGE)
            updateObject(right, pole, NOT_SURE, dc);
        if (POSTLOGIC)
            cout << "Post not classified" << endl;
        return;
    }
    bool questions = howbig == SMALL && !rightBlobColor(pole, QUESTIONABLEPOST);
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
        if ((nextX < trueLeft && post == LEFT) || (nextX > trueRight && post == RIGHT)) {
            runs[i].h = 0;
        }
        if (nextX > trueLeft - 10 && post == RIGHT || nextX < trueRight + 10 && post == LEFT) {
            runs[i].h = 0;
        }
    }
    // find the other post if possible - the process is basically identical to the first post
    point <int> leftP = pole.leftTop;
    point <int> rightP = pole.rightTop;
    int trueLeft2 = 0;
    int trueRight2 = 0;
    int trueBottom2 = 0;
    int trueTop2 = 0;
    int second = 0;
    int spanX2 = 0, spanY2 = 0;
    isItAPost = grabPost(c, c2, horizon, trueLeft - 5, trueRight + 5);
    if (isItAPost == NOPOST) {
        // before returning make sure we don't need to screen the previous post
        if (questions) {
            if (post == LEFT) {
                if (right->getIDCertainty() != _SURE)
                    right->init();
            } else {
                if (left->getIDCertainty() != _SURE)
                    left->init();
            }
        }
        return;
    } else {
        trueLeft2 = min(pole.leftTop.x, pole.leftBottom.x);
        trueRight2 = max(pole.rightTop.x, pole.rightBottom.x);
        trueTop2 = min(pole.leftTop.y, pole.rightTop.y);
        trueBottom2 = max(pole.leftBottom.y, pole.rightBottom.y);
        spanX2 = pole.rightTop.x - pole.leftTop.x + 1;
        spanY2 = pole.leftBottom.y - pole.leftTop.y + 1;
        dc = checkDist(trueLeft2, trueRight2, trueTop2, trueBottom2);
        rat = (float)spanX2 / (float)spanY2;
        bool ratOk = postRatiosOk(rat) || (!greenCheck(pole) && rat < SQUATRAT);
        bool goodSecondPost = checkSize(pole, c);
        if (SANITY) {
            if (ratOk && goodSecondPost) {
                cout << "Ratio and second are ok" << endl;
            } else {
                cout << "Problem with ratio or second" << endl;
            }
        }
        if (POSTDEBUG) {
            if (ratOk && goodSecondPost) {
                cout << "First two ok on 2d" << endl;
                if (secondPostFarEnough(leftP, rightP, pole.leftTop, pole.rightTop, post)) {
                    cout << "separation is fine" << endl;
                } else {
                    cout << "Not far enough apart" << endl;
                }
            } else {
                cout << "First two not ok on 2d" << endl;
            }
        }
        //drawBlob(pole, GREEN);
        // if things look ok, then we have ourselves a second post
        int fudge = 0;
        if (trueLeft < 1 || trueRight > IMAGE_WIDTH - 2) {
            fudge = spanX / 2;
        }
        if (locationOk(pole, horizon) && ratOk && goodSecondPost &&
            secondPostFarEnough(leftP, rightP, pole.leftTop, pole.rightTop, post) &&
            relativeSizesOk(spanX, pspanY, spanX2, spanY2, trueTop, trueTop2, fudge)) {
            if (post == LEFT) {
                second = 1;
                updateObject(left, pole, _SURE, dc);
                // make sure the certainty was set on the other post
                right->setIDCertainty(_SURE);
            } else {
                second = 1;
                updateObject(right, pole, _SURE, dc);
                left->setIDCertainty(_SURE);
            }
        } else {
            if (SANITY) {
                drawBlob(pole, ORANGE);
            }
            if (locationOk(pole, horizon) && ratOk && goodSecondPost && pole.leftTop.x
                > trueRight) {
                // maybe it really is a side-goal situation
                if (abs(trueTop - trueTop2) < 10 && qualityPost(pole, c)) {
                    if (post == LEFT) {
                        second = 1;
                        updateObject(left, pole, _SURE, dc);
                        // make sure the certainty was set on the other post
                        right->setIDCertainty(_SURE);
                    } else {
                        second = 1;
                        updateObject(right, pole, _SURE, dc);
                        left->setIDCertainty(_SURE);
                    }
                }
            } else if (!locationOk(pole, horizon) && secondPostFarEnough(leftP,
                                                                         rightP,
                                                                         pole.leftTop,
                                                                         pole.rightTop,
                                                                         post)) {
            }
        }
    }
}

/* We misidentified the first post.  Now that we've figured that out we need to
 *  switch it to the correct post.  Just transfer the information and reinit the
 *  previously IDd post.
 * @param p1    the correct post
 * @param p2    the wrong one
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

void ObjectFragments::transferBlob(blob from, blob & to) {
    to.leftTop.x = from.leftTop.x;
    to.leftTop.y = from.leftTop.y;
    to.rightTop.x = from.rightTop.x;
    to.rightTop.y = from.rightTop.y;
    to.rightBottom.x = from.rightBottom.x;
    to.rightBottom.y = from.rightBottom.y;
    to.leftBottom.x = from.leftBottom.x;
    to.leftBottom.y = from.leftBottom.y;
}

void ObjectFragments::transferToChecker(blob b) {
    checker.leftTop.x = b.leftTop.x;
    checker.leftTop.y = b.leftTop.y;
    checker.rightTop.x = b.rightTop.x;
    checker.rightTop.y = b.rightTop.y;
    checker.rightBottom.x = b.rightBottom.x;
    checker.rightBottom.y = b.rightBottom.y;
    checker.leftBottom.x = b.leftBottom.x;
    checker.leftBottom.y = b.leftBottom.y;
}

void ObjectFragments::transferToPole() {
    pole.leftTop.x = obj.leftTop.x;
    pole.leftTop.y = obj.leftTop.y;
    pole.rightTop.x = obj.rightTop.x;
    pole.rightTop.y = obj.rightTop.y;
    pole.rightBottom.x = obj.rightBottom.x;
    pole.rightBottom.y = obj.rightBottom.y;
    pole.leftBottom.x = obj.leftBottom.x;
    pole.leftBottom.y = obj.leftBottom.y;
}

/*  As a convenience we often operate on blobs.  Eventually we need to transfer that
 * information to our real object data structures.  This does that.
 * @param one         the place to go to
 * @param cert        how sure we are we IDd it correctly
 * @param distCert    how sure we are about distance certainty information
 */
void ObjectFragments::transferTopBlob(VisualFieldObject * one, certainty cert,
                                      distanceCertainty distCert) {
    one->setLeftTopX(topBlob.leftTop.x);
    one->setLeftTopY(topBlob.leftTop.y);
    one->setLeftBottomX(topBlob.leftBottom.x);
    one->setLeftBottomY(topBlob.leftBottom.y);
    one->setRightTopX(topBlob.rightTop.x);
    one->setRightTopY(topBlob.rightTop.y);
    one->setRightBottomX(topBlob.rightBottom.x);
    one->setRightBottomY(topBlob.rightBottom.y);
    one->setX(topBlob.leftTop.x);
    one->setY(topBlob.leftTop.y);
    one->setWidth(dist(topBlob.leftTop.x, topBlob.leftTop.y, topBlob.rightTop.x,
                       topBlob.rightTop.y));
    one->setHeight(dist(topBlob.leftTop.x, topBlob.leftTop.y, topBlob.leftBottom.x
                        , topBlob.rightBottom.y));
    one->setCenterX(one->getLeftTopX() + ROUND2(one->getWidth() / 2));
    one->setCenterY(one->getRightTopY() + ROUND2(one->getHeight() / 2));
    one->setIDCertainty(cert);
    one->setDistanceCertainty(distCert);
    one->setDistance(1);
}


/*  The next batch of routines have to do with processing the ball.
 */

/*  Normally we want our balls to be orange and can just check the number of pixels within the blob
 * that are orange.  However, sometimes the balls are occluded.  If we have a nice big orange blob,
 * but it doesn't seem orange enough it might be occluded.  So we look at different halves of the blob
 * to see if one of them is properly orange.
 * @param tempobj      the current ball candidate
 * @return             the best percentage we found
 */
// only called on really big orange blobs
float ObjectFragments::rightHalfColor(blob tempobj) {
    int x = tempobj.leftTop.x;
    int y = tempobj.leftTop.y;
    int spanY = tempobj.leftBottom.y - y;
    int spanX = tempobj.rightTop.x - x;
    int good = 0, good1 = 0, good2 = 0;
    int pix;
    if (rightColor(tempobj, ORANGE) < 0.15) return 0.10;
    for (int i = spanY / 2; i < spanY; i++) {
        for (int j = 0; j < spanX; j++) {
            pix = thresh->thresholded[y + i][x + j];
            if (y + i > -1 && x + j > -1 && (y + i) < IMAGE_HEIGHT &&
                x + j < IMAGE_WIDTH &&  (pix == ORANGE || pix == ORANGERED || pix == ORANGEYELLOW)) {
                good++;
            }
        }
    }
    for (int i = 0; i < spanY; i++) {
        for (int j = 0; j < spanX / 2; j++) {
            pix = thresh->thresholded[y + i][x + j];
            if (y + i > -1 && x + j > -1 && (y + i) < IMAGE_HEIGHT &&
                x + j < IMAGE_WIDTH && (pix == ORANGE || pix == ORANGERED || pix == ORANGEYELLOW)) {
                good1++;
            }
        }
    }
    for (int i = 0; i < spanY; i++) {
        for (int j = spanX / 2; j < spanX; j++) {
            pix = thresh->thresholded[y + i][x + j];
            if (y + i > -1 && x + j > -1 && (y + i) < IMAGE_HEIGHT &&
                x + j < IMAGE_WIDTH && (pix == ORANGE || pix == ORANGERED || pix == ORANGEYELLOW)) {
                good2++;
            }
        }
    }
    if (BALLDEBUG) {
        cout << "Checking half color " << good << " " << good1 << " " << good2 << " " << (spanX * spanY / 2) << endl;
    }
    float percent = (float)max(max(good, good1), good2) / (float) (spanX * spanY / 2);
    //cout << "Tossed because of low percentage " << percent << " " << color << endl;
    return percent;
}

/* Checks out how much of the current blob is orange.  Also looks for too much red.
 * @param tempobj     the candidate ball blob
 * @param col         ???
 * @return            the percentage (unless a special situation occurred)
 */

float ObjectFragments::rightColor(blob tempobj, int col) {
    int x = tempobj.leftTop.x;
    int y = tempobj.leftTop.y;
    int spanY = blobHeight(tempobj);
    int spanX = blobWidth(tempobj);
    if (spanX < 2 || spanY < 2) return false;
    int good = 0;
    int ogood = 0;
    int orgood = 0;
    int oygood = 0;
    int red = 0;
    for (int i = 0; i < spanY; i++) {
        for (int j = 0; j < spanX; j++) {
            int pix = thresh->thresholded[y + i][x + j];
            if (y + i > -1 && x + j > -1 && (y + i) < IMAGE_HEIGHT && x + j < IMAGE_WIDTH && (pix == ORANGE || pix == ORANGERED || pix == ORANGEYELLOW)) {
                good++;
                if (pix == ORANGE)
                    ogood++;
                else if (pix == ORANGEYELLOW)
                    oygood++;
                else
                    orgood++;
            } else if (pix == RED)
                red++;
        }
    }
    // here's a big hack - if we have a ton of orange, let's say it is enough unless the percentage is really low
    if (BALLDEBUG) {
        cout << "ORange " << ogood << " " << orgood << " " << red << " " << blobArea(tempobj) << endl;
    }
    if (blobArea(tempobj) > 1000) return (float) good / (float) blobArea(tempobj);
    //if (ogood < 2 * orgood) return 0.1; // at least two thirds of the "orange" pixels should be orange
    if (red > spanX * spanY * 0.10) return 0.1;
    if (ogood < spanX * spanY * 0.20) return 0.1;
    if (tempobj.area > 1000 && ogood+oygood > (spanX * spanY * 0.4)  && good < (spanX * spanY * 0.65)) return 0.65;
    float percent = (float)good / (float) (spanX * spanY);
    if (col == GREEN)
        return (float)good;
    //cout << "Tossed because of low percentage " << percent << " " << color << endl;
    return percent;
}

/*  When we're looking for balls it is helpful if they are surrounded by green.  The best place
 * to look is underneath.  So let's do that.
 * @param b    the potential ball
 * @return     did we find some green?
 */
bool ObjectFragments::greenCheck(blob b) {
    if (b.rightBottom.y >= IMAGE_HEIGHT - 1 || b.leftBottom.y >= IMAGE_HEIGHT - 1) return true;
    if (b.rightTop.x - b.leftTop.x > IMAGE_WIDTH / 2) return true;
    int w = b.rightBottom.x - b.leftBottom.x + 1;
    int y = 0;
    int x = b.leftBottom.x;
    for (int i = 0; i < w; i+= 2) {
        y = yProject(x, b.leftBottom.y, x + i);
        vertScan(x + i, y, 1, 5, GREEN, GREEN);
        if (scan.good > 1)
            return true;
    }
    // try one more in case its a white line
    int bad = 0;
    for (int i = 0; i < 10 && bad < 4; i++) {
        x = max(0, xProject(x, b.leftBottom.y, b.leftBottom.y + i));
        int pix = thresh->thresholded[min(IMAGE_HEIGHT - 1, b.leftBottom.y + i)][x];
        if (pix == GREEN) return true;
        if (pix != WHITE) bad++;
    }
    return false;
}

/*  When we're looking for balls it is helpful if they are surrounded by green.  The best place
 * to look is underneath, but that doesn't always work.  So let's try the other sides.
 * @param b    the potential ball
 * @return     did we find some green?
 */
bool ObjectFragments::greenSide(blob b) {
    int x = b.rightBottom.x;
    int y = b.rightBottom.y;
    for (int i = y; i > (b.rightBottom.y - b.rightTop.y) / 2; i = i - 2) {
        horizontalScan(x, i, 1, 5, GREEN, GREEN, x - 1, x + 8);
        if (scan.good > 0)
            return true;
    }
    x = b.leftBottom.x;
    y = b.leftBottom.y;
    for (int i = y; i > (b.leftBottom.y - b.leftTop.y) / 2; i = i - 2) {
        horizontalScan(x, i, -1, 5, GREEN, GREEN, x - 8, x + 1);
        if  (scan.good == 0)
            return true;
    }
    return false;
}

/* Scan from a point in the ball, in a certain direction to find a point.
 * @param start_x     the x location of the point
 * @param start_y     the y location of the point
 * @param slopel      the slope of the scanline
 * @param dir         positive or negative direction
 * @return            whether the point is good (0) or bad (1) - Joho, a) why not boolean, b) why not the other way?
 */
int ObjectFragments::scanOut(int start_x, int start_y, float slopel, int dir){
    if(DEBUGBALLPOINTS) {
        printf("Passed start_x %d, start_y %d, slopel %g, dir %d \n",
               start_x,start_y,slopel,dir);
    }
    //thresh->drawPoint(start_x,start_y,PINK);

    bool yOrX = true; //which axis do we scan on?

    int SNOISE_SKIP = 4;
    int EDGE_DEPTH = 3;
    int x = start_x;
    int y = start_y;
    int good = 0;
    int bad = 0;
    int goodEdge = 0;
    int lastGoodX = x;
    int lastGoodY = y;

    if(slopel> 1 || slopel<-1){ //when the slope is big, we must switch axis.
        yOrX = false;
        slopel = 1/slopel;
        //dir = -dir;
    }

    //scan for orange, or other ball colors
    while(x < IMAGE_WIDTH && x >= 0
          &&y < IMAGE_HEIGHT && y >= 0
          && bad <= SNOISE_SKIP && goodEdge <= EDGE_DEPTH){
        int thisPix = thresh->thresholded[y][x];
        //printf("new pix:%d good:%d bad:%d\n",thisPix,good,bad);
        if(thisPix == ORANGE || thisPix == ORANGERED || thisPix == ORANGEYELLOW) {
            good++;
            if(good > SNOISE_SKIP){
                bad = 0;
                goodEdge = 0;
            }
            lastGoodX = x;
            lastGoodY = y;
#ifdef OFFLINE
            if(DEBUGBALLPOINTS)
                thresh->debugImage[y][x] = NAVY;
#endif
        }else if(thisPix == GREEN || thisPix == BLACK){
            //if(DEBUGBALLPOINTS)printf("found a green or blac pix:%d\n",thisPix);
            good  = 0;
            bad++;
            goodEdge++;
#ifdef OFFLINE
            if(DEBUGBALLPOINTS)
                thresh->debugImage[y][x] = WHITE;
#endif
        }else{
            good  = 0;
            bad++;
#ifdef OFFLINE
            if(DEBUGBALLPOINTS)
                thresh->debugImage[y][x] = BLACK;
#endif
        }

        //update position for both y, x
        if(yOrX){
            x = x + dir;
            y = start_y  +ROUND2(slopel*(float)(x - (start_x)));
        }else{
            y = y + dir;
            x = start_x  +ROUND2(slopel*(float)(y - (start_y)));
        }
    }
    if(goodEdge < EDGE_DEPTH){ // bad point
        //printf("EDGE point == no good!\n");
        if(DEBUGBALLPOINTS)
            drawPoint(lastGoodX,lastGoodY,RED);
        return 1;
    }else{ //good point
        //printf("Got a good point at (%d,%d) bad:%d, goodEdge %d \n",lastGoodX,lastGoodY,bad,goodEdge);
        if(DEBUGBALLPOINTS)
            drawPoint(lastGoodX,lastGoodY,BLUE);
        addPoint(lastGoodX,lastGoodY);
        return 0;
    }

}

/*  It probably goes without saying that the ideal ball is round.  So let's see how round our
 * current candidate is.  Among other things we check its heigh/width ratio (should be about 1)
 * and where the orange is (shouldn't be in the corners, should be in the middle)
 * @param b      the candidate ball
 * @return       a constant result - BADVALUE, or 0 for round
 */

int  ObjectFragments::roundness(blob b) {
    int w = blobWidth(b);
    int h = blobHeight(b);
    int x = b.leftTop.x;
    int y = b.leftTop.y;
    float ratio = (float)w / (float)h;
    int r = 10;
    if ((h < SMALLBALLDIM && w < SMALLBALLDIM && ratio > BALLTOOTHIN && ratio < BALLTOOFAT)) {
    } else if (ratio > THINBALL && ratio < FATBALL) {
    } else if (y + h > IMAGE_HEIGHT - 3 || x == 0 || (x + w) > IMAGE_WIDTH - 2 || y == 0) {
        if (BALLDEBUG)
            cout << "Checking ratio on occluded ball:  " << ratio << endl;
        // we're on an edge so allow for streching - first check for top of bottom
        if (h > 4 && w > 4 && (y + h > IMAGE_HEIGHT - 2 || y == 0) && ratio < MIDFAT && ratio > 1) {
            // then sides
        } else if (h > 4 && w > 4 && (x == 0 || x + w > IMAGE_WIDTH - 2) && ratio > MIDTHIN && ratio < 1) {
        } else if ((h > 20 || w > 20) && (ratio > OCCLUDEDTHIN && ratio < OCCLUDEDFAT) ) {
            // when we have big slivers then allow for extra
        } else if (b.leftBottom.y > IMAGE_HEIGHT - 3 && w > 15) {
            // the bottom is a really special case
        } else {
            if (BALLDEBUG)
                //cout << "Screening for ratios" << endl;
                return BADVALUE;
        }
    } else {
        if (BALLDEBUG) {
            //drawBlob(b, BLACK);
            //printBlob(b);
            //cout << "Screening for ratios " << ratio << endl;
        }
        return BADVALUE;
    }
    if (ratio < 1.0) {
        int offRat = ROUND2((1.0 - ratio) * 10.0);
        r -= offRat;
    } else {
        int offRat = ROUND2((1.0 - 1.0/ratio) * 10.0);
        r -= offRat;
    }
    if (w * h > SMALLBALL) {
        // now make some scans through the blob - horizontal, vertical, and each diagonal
        int pix;
        int goodPix = 0, badPix = 0;
        if (y + h > IMAGE_HEIGHT - 3 || x == 0 || (x + w) > IMAGE_WIDTH - 2 || y == 0) {
        } else {
            // we're in the screen
            int d = ROUND2((float)max(w, h) / 6.0);
            int d3 = min(w, h);
            for (int i = 0; i < d3; i++) {
                pix = thresh->thresholded[y+i][x+i];
                if (i < d || (i > d3 - d)) {
                    if (pix == ORANGE)
                        badPix++;
                    else
                        goodPix++;
                } else if (pix == ORANGE || pix == ORANGERED || pix == ORANGEYELLOW)
                    goodPix++;
                else if (pix != GREY)
                    badPix++;
                pix = thresh->thresholded[y+i][x+w-i];
                if (i < d || (i > d3 - d)) {
                    if (pix == ORANGE)
                        badPix++;
                    else
                        goodPix++;
                } else if (pix == ORANGE || pix == ORANGERED || pix == ORANGEYELLOW)
                    goodPix++;
                else if (pix != GREY)
                    badPix++;
            }
            for (int i = 0; i < h; i++) {
                pix = thresh->thresholded[y+i][x + w/2];
                if (pix == ORANGE || pix == ORANGERED || pix == ORANGEYELLOW) {
                    goodPix++;
                } else if (pix != GREY)
                    badPix++;
            }
        }
        for (int i = 0; i < w; i++) {
            pix = thresh->thresholded[y+h/2][x + i];
            if (pix == ORANGE || pix == ORANGERED || pix == ORANGEYELLOW) {
                goodPix++;
            } else if (pix != GREY)
                badPix++;
        }
        if (BALLDEBUG)
            cout << "Good " << goodPix << " " << badPix << endl;
        if ((float)goodPix / (float)badPix < 2) {
            if (BALLDEBUG)
                cout << "Screening for bad roundness" << endl;
            return BADVALUE;
        }
    }
    return 0;
}

/* Checks all around the ball for green.  Returns all of the sides that is on.  Does this by multiplying
 * a base value by various prime numbers representing different cases.
 * @param b   the candidate ball
 * @return    where the green is
 */

int ObjectFragments::ballNearGreen(blob b) {
    // first check the bottom
    int w = b.rightTop.x - b.leftTop.x + 1;
    int h = b.leftBottom.y - b.leftTop.y + 1;
    int where = NOGREEN;
    if (greenCheck(b))
        where = where * GREENBELOW;
    // now try the sides - happily the ball is round so we don't have to worry about scan angles
    int x = b.leftTop.x;
    int y = b.leftTop.y;
    for (int i = 0; i < h && y + i < IMAGE_HEIGHT && where % GREENLEFT != 0; i= i+2) {
        for (int j =-1; j < 6 && x + j > -1 && where % GREENLEFT != 0; j++) {
            if (thresh->thresholded[i+y][x - j] == GREEN) {
                where = where * GREENLEFT;
            }
        }
    }
    for (int i = 0; i < w && x + i < IMAGE_WIDTH && where % GREENABOVE != 0; i= i+2) {
        for (int j = 0; j < 6 && y - j > 0 && where % GREENABOVE != 0; j++) {
            if (thresh->thresholded[i+y][j+x] == GREEN) {
                where = where * GREENABOVE;
            }
        }
    }

    x = b.rightTop.x;
    y = b.rightTop.y;
    for (int i = 0; i < h && y + i < IMAGE_HEIGHT && where % GREENRIGHT != 0; i= i+2) {
        for (int j = 0; j < 6 && x + j < IMAGE_WIDTH && where % GREENRIGHT != 0; j++) {
            if (thresh->thresholded[i+y][j+x] == GREEN) {
                where = where * GREENRIGHT;
            }
        }
    }
    // put in the case where we don't have any, but want to check the corners
    return where;
}

/*  Check the information surrounding the ball and look to see if it might be a
 * false ball.  Since our main candidate for false balls is the red uniform, the
 * main thing we worry about is a preponderance of red.
 *
 * @param b    our ball candidate
 * @return     true if the surround looks bad, false if its ok
 */

bool ObjectFragments::badSurround(blob b) {
    // basically check around the blob and see if it is ok - ideally we'd have
    // some green, worrisome would be lots of RED
    static const int surround = 12;

    int x = b.leftTop.x;
    int y = b.leftTop.y;
    int w = b.rightTop.x - b.leftTop.x + 1;
    int h = b.rightBottom.y - b.leftTop.y + 1;
    int greens = 0, orange = 0, red = 0, borange = 0, pix;
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            pix = thresh->thresholded[y + j][x + i];
            if (pix == ORANGE)
                borange++;
        }
    }
    x = max(0, x - surround);
    y = max(0, y - surround);
    w = w + surround * 2;
    h = h + surround * 2;
    for (int i = 0; i < w && x + i < IMAGE_WIDTH; i++) {
        for (int j = 0; j < h && y + j < IMAGE_HEIGHT; j++) {
            pix = thresh->thresholded[y + j][x + i];
            if (pix == ORANGE || pix == ORANGEYELLOW)
                orange++;
            else if (pix == RED || pix == ORANGERED)
                red++;
            else if (pix == GREEN)
                greens++;
        }
    }
    if (BALLDEBUG) {
        cout << "Surround information " << red << " " << orange << " " << borange << " " << greens << endl;
    }
    if (red > orange) return true;
    if (red > greens) return true;
    if (red > borange) return true;
    return false;
}

/*  Is the ball at the boundary of the screen?
 * @param b    the ball
 * @return     whether or not it borders a boundary
 */
bool ObjectFragments::atBoundary(blob b) {
    return b.leftTop.x == 0 || b.rightTop.x >= IMAGE_WIDTH -1 || b.leftTop.y == 0
        || b.leftBottom.y >= IMAGE_HEIGHT - 1;
}

/* See if there is a ball onscreen.  Basically we get all of the orange blobs
 * and test them for viability.  Once we've screened all of the obviously bad
 * ones we then pick the biggest one and check it some more.
 *
 * @param  horizon   the horizon intercept
 * @param  thisBall  the ball object
 * @return           we always return 0 - the return is an artifact of other methods
 */
int ObjectFragments::balls(int horizon, VisualBall *thisBall) {
    int confidence = 10;
    occlusion = NOOCCLUSION;
    if (numberOfRuns > 1) {
        for (int i = 0; i < numberOfRuns; i++) {
            // search for contiguous blocks
            int nextX = runs[i].x;
            int nextY = runs[i].y;
            int nextH = runs[i].h;
            blobIt(nextX, nextY, nextH);
        }
    }
    // pre-screen blobs that don't meet our criteria
    //cout << "horizon " << horizon << " " << slope << endl;
    for (int i = 0; i < numBlobs; i++) {
        int ar = blobArea(blobs[i]);
        float perc = rightColor(blobs[i], ORANGE);
        estimate es;
        es = vision->pose->pixEstimate(blobs[i].leftTop.x + blobWidth(blobs[i]) / 2, blobs[i].leftTop.y + 2 * blobHeight(blobs[i]) / 3, 0.0);
        int diam = max(blobWidth(blobs[i]), blobHeight(blobs[i]));
        /*int dist = (int)es.dist;
          if (diam < 15) {
          if (dist < 300) {
          blobs[i].area = 0;
          }
          } else if (diam < 20) {
          if (dist < 250 || dist > 500) {
          blobs[i].area = 0;
          }
          } else if (diam < 25) {
          if (dist < 200 || dist >  400) {
          blobs[i].area = 0;
          }
          } else if (diam < 35) {
          if (dist < 1400 || dist > 300) {
          blobs[i].area = 0;
          }
          } else {
          if (dist > 200) {
          blobs[i].area = 0;
          }
          }
          if (blobs[i].area == 0) {
          cout << "Diam was " << diam << " " << dist << endl;
          }*/
        if (blobs[i].leftBottom.y + diam < horizonAt(blobs[i].leftTop.x)) {
            blobs[i].area = 0;
        } else if (ar > 35 && perc > MINORANGEPERCENT) {
            // don't do anything
        } else if (ar > 1000 && rightHalfColor(blobs[i]) > MINORANGEPERCENT) {
        } else {
            //drawBlob(blobs[i], BLACK);
            blobs[i].area = 0;
        }
    }
    // now find the best remaining blob
    getTopAndMerge(horizon);
    if (!blobOk(topBlob)) {
        if (BALLDEBUG)
            cout << "No viable blobs" << endl;
        return 0;
    }

    // try to screen out "false balls"
    int w = blobWidth(topBlob);
    int h = blobHeight(topBlob);
    estimate e;
    e = vision->pose->pixEstimate(topBlob.leftTop.x + blobWidth(topBlob) / 2, topBlob.leftTop.y + 2 * blobHeight(topBlob) / 3, 0.0);
    //cout << "Estimated distance is " << e.dist << endl;
    if (BALLDEBUG) {
        if (topBlob.leftTop.x > 0) {
            cout << "Vision found ball " << endl;
            printBlob(topBlob);
            cout << topBlob.leftTop.x << " " << topBlob.leftTop.y << " " << w << " " << h << endl;
        }
    }
    // check for obvious occlusions
    if (topBlob.leftBottom.y > IMAGE_HEIGHT - 3) {
        occlusion = BOTTOMOCCLUSION;
    }
    if (topBlob.leftTop.y < 1) {
        occlusion *= TOPOCCLUSION;
    }
    if (topBlob.leftTop.x < 1) {
        occlusion *= LEFTOCCLUSION;
    }
    if (topBlob.rightTop.x > IMAGE_WIDTH - 2) {
        occlusion *= RIGHTOCCLUSION;
    }

    // for smallish blobs, make sure we're near some green
    int whereIsGreen = ballNearGreen(topBlob);
    int horb = horizonAt(topBlob.leftBottom.x);

    //look for edge points!
    int NUM_EDGE_POINTS = 20;
    int cenX = midPoint(topBlob.leftTop.x, topBlob.rightBottom.x);
    int cenY = midPoint(topBlob.leftTop.y, topBlob.leftBottom.y);

    for(float angle = 0; angle < PI; angle +=PI/NUM_EDGE_POINTS){
        scanOut(cenX,cenY,tan(angle), 1);
        scanOut(cenX,cenY,tan(angle), -1);
    }

    if (w < SMALLBALLDIM || h < SMALLBALLDIM) {
        if (badSurround(topBlob)) {
            if (BALLDEBUG) {
                drawBlob(topBlob, BLACK);
                cout << "Screening for lack of green and bad surround" << endl;
            }
            return 0;
        }
    }

    if (w < SMALLBALLDIM || h < SMALLBALLDIM) {
        // small balls should be near the horizon - this check makes extra sure
        if (topBlob.leftBottom.y > horb + 30) {
            if (BALLDEBUG)
                cout << "Screening small ball for horizon" << endl;
            return 0;
        }
        if (BALLDEBUG)
            cout << "Small ball passed green and horizon tests" << endl;
    }
    float colPer = rightColor(topBlob, ORANGE);

    confidence -= ROUND2((0.85 - colPer) * 10);
    //cout << (ROUND2((0.85 - colPer) * 10)) << " " << confidence << endl;
    if (topBlob.area < 75) {
        confidence -= 3;
    } else if (topBlob.area < 150) {
        confidence -= 1;
    } else if (topBlob.area > 500) {
        confidence += 2;
    } else if (topBlob.area > 250) {
        confidence += 1;
    }
    if (BALLDEBUG) {
        printBall(topBlob, confidence, colPer, occlusion, whereIsGreen);
    }

    // SORT OUT BALL INFORMATION
    // start out by figuring out whether we're using blobs or inferred information
    //float rat = (float) w / (float) h;
    // x, y, width, and height. Not up for debate.
    thisBall->setX(topBlob.leftTop.x);
    thisBall->setY(topBlob.leftTop.y);

    thisBall->setWidth(w);
    thisBall->setHeight(h);
    thisBall->setRadius((float)max((float)w/2.0, (float)h/2.0));
    int amount = h / 2;
    if (w > h)
        amount = w / 2;
    if (occlusion == LEFTOCCLUSION) {
        thisBall->setCenterX(topBlob.rightTop.x - amount);
        thisBall->setX(topBlob.rightTop.x - amount * 2);
    } else {
        thisBall->setCenterX(topBlob.leftTop.x + amount);
    }
    if (occlusion == BOTTOMOCCLUSION) {
        thisBall->setCenterY(topBlob.leftTop.y + amount);
    } else {
        thisBall->setCenterY(topBlob.leftBottom.y - amount);
    }
    thisBall->setConfidence(SURE);
    thisBall->findAngles();
    thisBall->setFocalDistanceFromRadius();
    thisBall->setDistanceEst(vision->pose->
                             bodyEstimate(thisBall->getCenterX(),
                                          thisBall->getCenterY(),
                                          static_cast<float>(thisBall->
                                                             getFocDist())));
    if (atBoundary(topBlob)) {
        // INFERRED MEASUREMENTS
        //estimate es;
        //es = vision->pose->pixEstimate(blobs[i].leftTop.x + blobWidth(blobs[i]) / 2, blobs[i].leftTop.y + 2 * blobHeight(blobs[i]) / 3, 0.0);
        //int dist = (int)es.dist;
        //thisBall->setConfidence(MILDLYSURE);
    }
    if (BALLDISTDEBUG) {
        estimate es;
        es = vision->pose->pixEstimate(topBlob.leftTop.x + blobWidth(topBlob) / 2, topBlob.leftTop.y + 2 * blobHeight(topBlob) / 3, 0.0);
        cout << "Distance is " << thisBall->getDistance() << " " << thisBall->getFocDist() << " " << es.dist << endl;
        cout<< "Radius"<<thisBall->getRadius()<<endl;
    }
    return 0;
}

/* Sanity check routines for beacons and posts
 */


/* Checks out how much of the blob is of the right color.  If it is enough returns true, if not false.
 * @param tempobj     the blob we're checking (usually a post)
 * @param minpercent  how good it needs to be
 * @return            was it good enough?
 */
bool ObjectFragments::rightBlobColor(blob tempobj, float minpercent) {
    int x = tempobj.leftTop.x;
    int y = tempobj.leftTop.y;
    int spanX = tempobj.rightTop.x - tempobj.leftTop.x; //ROUND2(dist(x, y, tempobj.rightTop.x, tempobj.rightTop.y));
    int spanY = tempobj.leftBottom.y - tempobj.leftTop.y; //ROUND2(dist(x, y, tempobj.leftBottom.x, tempobj.leftBottom.y));
    if (spanX < 1 || spanY < 1) return false;
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
                if (thresh->thresholded[ny][nx] == color) {
                    good++;
                }
            }
        }
    }
    float percent = (float)good / (float) (total);
    if (percent > minpercent) {
        return true;
    }
    //cout << "Tossed because of low percentage " << percent << " " << color << endl;
    return false;
}

/*  We don't want to identify crossbars as backstops because we might shoot at them.  Basically
 * we just look at all the blobs and throw away any blob that is over another blob.  Note that this
 * isn't a guarantee that we're throwing crossbars away, but it will catch many crossbars.
 */

void ObjectFragments::screenCrossbar() {
    //check each blob in the array
    for (int i = 0; i < numBlobs; i++) {
        for (int j = 0; j < numBlobs; j++) {
            if (i != j && distance(blobs[i].leftTop.x, blobs[i].rightTop.x, blobs[j].leftTop.x, blobs[j].rightTop.x) < 1) {
                if (blobs[i].leftTop.y < blobs[j].leftTop.y && blobs[j].area > 100) {
                    blobs[i].area = 0;
                    if (POSTDEBUG) {
                        cout << "Screening blob " << i  << " because of blob " << j << " " << blobs[j].area << endl;
                        drawBlob(blobs[i], GREEN);
                    }
                }
            }
        }
    }
}



/* Checks if a potential post meets our size requirements.
 * @param b     the post
 * @return      true if its big enough, false otherwise
 */
bool ObjectFragments::postBigEnough(blob b) {
    if (b.leftTop.x == BADVALUE || (b.rightTop.x - b.leftTop.x + 1 < MIN_GOAL_WIDTH) ||
        b.leftBottom.y - b.leftTop.y + 1 < MIN_GOAL_HEIGHT) {
        return false;
    }
    return true;
}

/* Combines several sanity checks into one.  Checks that the bottom of the object is ok and the top too.
 * Also, just makes sure that the object is in fact an object.
 * @param b        the potential post
 * @param hor      the green horizon
 * @return         true if it is reasonably located, false otherwise
 */

bool ObjectFragments::locationOk(blob b, int hor) {
    if (!blobOk(b)) return false;
    if (hor < -50) return false;
    int trueLeft = min(b.leftTop.x, b.leftBottom.x);       // leftmost value in the blob
    int trueRight = max(b.rightTop.x, b.rightBottom.x);    // rightmost value in the blob
    int trueTop = min(b.leftTop.y, b.rightTop.y);          // topmost value in the blob
    int trueBottom = max(b.leftBottom.y, b.rightBottom.y); // bottommost value in teh blob
    int horizonLeft = yProject(0, hor, trueLeft);          // the horizon at the leftmost point
    int horizonRight = yProject(0, hor, trueRight);        // the horizon at the rightmost point
    //if (slope < 0) {
    //  horizonLeft = yProject(IMAGE_WIDTH - 1, hor, trueLeft);
    //  horizonRight = yProject(IMAGE_WIDTH - 1, hor, trueRight);
    //}
    //drawPoint(trueLeft, horizonLeft, YELLOW);
    //drawPoint(trueRight, horizonRight, YELLOW);
    int spanX = b.rightTop.x - b.leftTop.x + 1;
    int spanY = b.leftBottom.y - b.leftTop.y;
    int mh = min(horizonLeft, horizonRight);
    if (!horizonBottomOk(spanX, spanY, mh, trueLeft, trueRight, trueBottom, trueTop)) {
        if (!greenCheck(b) || mh - trueBottom > spanY || spanX < 5 || mh - trueBottom > 25) {
#if ROBOT(NAO)
            if (spanY > 50) {
                return true;
            }
#endif
            if (SANITY) {
                cout << "Screening blob for bottom reasons" << endl;
                printBlob(b);
            }
            return false;
        } else {
        }
    }
    //if (trueRight - trueLeft > IMAGE_WIDTH - 10) return true;
    return horizonTopOk(trueTop, max(horizonAt(trueLeft), horizonAt(trueRight)));
}

/* Objects need to be at or below the horizon.  We get the basic shape of the object and either the horizon
 * or the projected horizon.
 * @param spanX      how wide the post is
 * @param spanY      how tall it is
 * @param minHeight  the normal minimum value for where the bottom could be
 * @param left       the leftmost x value of the object
 * @param right      the rightmost x value of the object
 * @param bottom     the biggest y value of the object
 * @param top        the smallest y value of the object
 * @return           true if it seems reasonable, false otherwise
 */

bool ObjectFragments::horizonBottomOk(int spanX, int spanY, int minHeight, int left, int right, int bottom, int top) {
    // add a width fudge factor in case the object is occluded - bigger objects will also be taller
    //cout << (bottom + 5) << " " << minHeight << " " << top << " " << spanY << " " << spanX << " " << left << " " << right << endl;
    //int fudge = 20;
    if (spanY > 100) return true;
    if (bottom + 5 + min(spanX, 20) < minHeight) {
        if (SANITY) {
            cout << "Bad height" << endl;
        }
        return false;
    }
    // when we're at the edges of the image make the rules a bit more stringent
    if (bottom + 5 < minHeight && (left < 10 || right > IMAGE_WIDTH - 10 || top < 5) && (spanY < 15)) {
        if (SANITY)
            cout << "Bad Edge Information" << endl;
        return false;
    }
    // if we're fudging then make sure there is green somewhere - in other words watch out for occluded beacons
    if (top == 0 && bottom + 5 < minHeight) {
        //int x = left + spanX / 2;
        //int y = bottom + 5 + spanX;
        //vertScan(x, y, 1, 3, WHITE, ORANGEYELLOW);
        //drawPoint(x, y, RED);
        if (scan.good > 5) {
            if (SANITY)
                cout << "Occluded beacon?" << endl;
            return false;
        }
    }
    return true;
}

/* The top of objects need to be above the horizon.  Make sure they are.  Note:  we had to futz
 * with this in Atlanta because of the wonky field conditions.
 * @param top      the top of the post
 * @param hor      the green field horizon
 * @return         true when the horizon is below the top of the object
 */
bool ObjectFragments::horizonTopOk(int top, int hor) {
    if (hor <= 0) return false;
    if (top < 1) return true;
    if (top + MIN_GOAL_HEIGHT / 2 > hor) {
        if (SANITY) {
            drawPoint(100, top, RED);
            drawPoint(100, hor, BLACK);
            drawBlob(pole, ORANGE);
            cout << "Top is " << top << " " << hor << endl;
            cout << "Problems at top" << endl;
        }
        return false;
    }
    if (SANITY)
        cout << "Horizon top is ok " << top << " " << hor << endl;

    return true;
}

/*  Posts shouldn't show up too close to each other (yes, I realize they can be when you're
 * looking from the side).  Make sure there is some separation.
 * @param l1      left x of one post
 * @param r1      right x of the same post
 * @param l2      left x of the other post
 * @param r2      right x of the other post
 * @return        true when there is enough separation.
 */
bool ObjectFragments::secondPostFarEnough(point <int> left1, point <int> right1, point <int> left2, point <int> right2, int post) {
    if (SANITY) {
        cout << "Separations " << (dist(left1.x, left1.y, right2.x, right2.y)) << " " << (dist(left2.x, left2.y, right1.x, right1.y)) << endl;
    }
    //cout << left1.x << " " << left2.x << " " << right1.x << " " << right2.x << endl;
    if ((post == RIGHT && right2.x > left1.x) || (post == LEFT && left2.x < right1.x)) {
        if (SANITY) {
            cout << "Second post is on the wrong side!" << endl;
        }
        return false;
    }
    if (dist(left1.x, left1.y, right2.x, right2.y) > MIN_POST_SEPARATION && dist(left2.x, left2.y, right1.x, right1.y) > MIN_POST_SEPARATION) {
        if (dist(left1.x, left1.y, left2.x, left2.y) > MIN_POST_SEPARATION && dist(right2.x, right2.y, right1.x, right1.y) > MIN_POST_SEPARATION) {
            return true;
        }
    }
    return false;
}


/* When we process blobs we start them with BADVALUE such that we can easily tell if
 * whatever processing we did worked out.  Here we make that check.
 * @param b    the blob we worked on.
 * @return     true when the processing worked, false otherwise
 */
bool ObjectFragments::blobOk(blob b) {
    if (b.leftTop.x > BADVALUE && b.leftBottom.x > BADVALUE)
        return true;
    return false;
}

/*  When we have two candidate posts we don't want one to be huge and the other tiny.  So we need to make
 * sure that the size ratios are within reason.
 * @param spanX    the width of one post
 * @param spanY    its height
 * @param spanX2   the width of the other post
 * @param spanY2   its height
 * @return         are the ratios reasonable?
 */

bool ObjectFragments::relativeSizesOk(int spanX, int spanY, int spanX2, int spanY2, int t1, int t2, int fudge) {
    if (spanY2 > 100) return true;
    if (spanY2 > 3 * spanY / 4) return true;
    // we need to get the "real" offset
    int f = max(yProject(0, t1, spanY), yProject(IMAGE_WIDTH - 1, t1, IMAGE_WIDTH - spanY));
    if (abs(t1 - t2) > 3 * min(spanY, spanY2) / 4 + f)  {
        if (SANITY) {
            cout << "Bad top offsets" << endl;
        }
        return false;
    }
    if (spanY2 > 70) return true;
    if (spanX2 > 2 && (spanY2 > spanY / 2 || spanY2 > BIGPOST ||
                       (spanY2 > spanY / 3 && spanX2 > 10) &&
                       (spanX2 <= spanX / 2 || fudge != 0)) && (spanX2 > spanX / 4))  {
        return true;
    }
    if (t1 < 1 && t2 < 1) return true;
    if (SANITY) {
        cout << "Bad relative sizes" << endl;
        cout << spanX << " "  << spanY << " " << spanX2 << " " << spanY2 << endl;
    }
    return false;
}

/*  Try and add a new circle fitting point.
 * @param x     x value
 * @param y     y value
 */
void ObjectFragments::addPoint(float x, float y){
    if(numPoints < MAX_POINTS){
        points[numPoints*2] = x;
        points[numPoints*2+1] = y;
        numPoints++;
    }else{
    }
}


/*  Is the ratio of width to height ok for the second post?  We use a different criteria here than for
 * the first post because we have lots of other ways to verify if this is a good post.
 * @param ratio     the height/width ratio
 * @return          is it a legal value?
 */
bool ObjectFragments::postRatiosOk(float ratio) {
    return ratio < GOODRAT;
}


/* Misc. routines
 */

/*  How many pixels of the right color does a blob have?
 * @param index    which blob
 * @return         the value
 */

int ObjectFragments::getPixels(int index){
    return blobs[index].pixels;
}


/* Calculate the horizontal distance between two objects (the end of one to the start of the other).
 * @param x1    left x of one object
 * @param x2    right x of the object
 * @param x3    left x of the other
 * @param x4    right x of the other
 * @return      the distance between the objects in the x dimension
 */
int ObjectFragments::distance(int x1, int x2, int x3, int x4) {
    if (x2 < x3)
        return x3 - x2;
    if (x1 > x4)
        return x1 - x4;
    return 0;
}

/*
 * The next group of functions are for debugging only.  They are set up so that
 * debugging information will only appear when processing is done off-line.
 */

/*  Print debugging information for a field object.
 * @param objs     the object in question
 */

void ObjectFragments::printObject(VisualFieldObject * objs) {
#if defined OFFLINE
    cout << objs->getLeftTopX() << " " << objs->getLeftTopY() << " "
         << objs->getRightTopX() << " " << objs->getRightTopY() << endl;
    cout << objs->getLeftBottomX() << " " << objs->getLeftBottomY() << " "
         << objs->getRightBottomX() << " " << objs->getRightBottomY() << endl;
    cout << "Height is " << objs->getHeight() << " Width is "
         << objs->getWidth() << endl;
    if (objs->getIDCertainty() == _SURE)
        cout << "Very sure" << endl;
    else
        cout << "Not sure" << endl;
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
        if (vision->bglp->getWidth() >  0) {
            cout << "Vision found left blue post " << endl;
            printObject(vision->bglp);
        }
        if (vision->bgrp->getWidth() >  0) {
            cout << "Vision found right blue post " << endl;
            printObject(vision->bgrp);
        }
        if (vision->yglp->getWidth() >  0) {
            cout << "Vision found left yellow post " << endl;
            printObject(vision->yglp);
        }
        if (vision->ygrp->getWidth() >  0) {
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
 * @param b    the blob
 */
void ObjectFragments::printBlob(blob b) {
#if defined OFFLINE
    cout << "Outputting blob" << endl;
    cout << b.leftTop.x << " " << b.leftTop.y << " " << b.rightTop.x << " " << b.rightTop.y << endl;
    cout << b.leftBottom.x << " " << b.leftBottom.y << " " << b.rightBottom.x << " " << b.rightBottom.y << endl;
#endif
}

/* Prints a bunch of ball information about the best ball candidate (or any one).
 * @param b    the candidate ball
 * @param c    how confident we are its a ball
 * @param p    how many occlusions
 * @param o    what the occlusions are if any
 * @param bg   where around the ball there is green
 */

void ObjectFragments::printBall(blob b, int c, float p, int o, int bg) {
#ifdef OFFLINE
    if (BALLDEBUG) {
        cout << "Ball info: " << b.leftTop.x << " " << b.leftTop.y << " " << (b.rightTop.x - b.leftTop.x);
        cout << " " << (b.leftBottom.y - b.leftTop.y) << endl;
        cout << "Confidence: " << c << " Orange Percent: " << p << " Occlusions: ";
        if (o == NOOCCLUSION) cout <<  "none";
        if (o % LEFTOCCLUSION == 0) cout << "left ";
        if (o % RIGHTOCCLUSION == 0) cout << "right ";
        if (o % TOPOCCLUSION == 0) cout << "top ";
        if (o % BOTTOMOCCLUSION == 0) cout << "bottom ";
        if (bg == NOGREEN) {
            cout << "No green anywhere";
        } else {
            cout << "Green can be found to the ";
        }
        if (bg % GREENBELOW == 0)
            cout << "bottom ";
        if (bg % GREENABOVE == 0)
            cout << "top ";
        if (bg % GREENLEFT == 0)
            cout << "left ";
        if (bg % GREENRIGHT == 0)
            cout << "right";
        cout << endl;
    }
#endif
}

/* Debugging method used to show where things were processed on the image.  Paints a verticle
 * stripe corresponding to a "run" of color.
 * @param x     x coord
 * @param y     y coord
 * @param h     height
 * @param c     the color to paint
 */

void ObjectFragments::paintRun(int x, int y, int h, int c){
    vision->drawLine(x,y+1,x,y+h+1,c);
}

/*  More or less the same as the previous method, but with different parameters.
 * @param run     a run of color
 * @param c       the color to paint
 */

void ObjectFragments::drawRun(const run& run, int c) {
    vision->drawLine(run.x,run.y+1,run.x,run.y+run.h+1,c);
}

/*  Draws a "+" on the screen at the specified location with the specified color.
 * @param x     x coord
 * @param y     y coord
 * @param c     the color to paint
 */

void ObjectFragments::drawPoint(int x, int y, int c) {
#ifdef OFFLINE
    thresh->drawPoint(x, y, c);
#endif
}

/*  Draws the outline of a rectangle in the specified color.
 * @param b    the rectangle
 * @param c    the color to paint
 */
void ObjectFragments::drawRect(int x, int y, int w, int h, int c) {
#ifdef OFFLINE
    thresh->drawRect(x, y, w, h, c);
#endif
}

/*  Draws the outline of a blob in the specified color.
 * @param b    the blob
 * @param c    the color to paint
 */
void ObjectFragments::drawBlob(blob b, int c) {
#ifdef OFFLINE
    thresh->drawLine(b.leftTop.x, b.leftTop.y, b.rightTop.x, b.rightTop.y, c);
    thresh->drawLine(b.leftTop.x, b.leftTop.y, b.leftBottom.x, b.leftBottom.y, c);
    thresh->drawLine(b.leftBottom.x, b.leftBottom.y, b.rightBottom.x, b.rightBottom.y, c);
    thresh->drawLine(b.rightTop.x, b.rightTop.y, b.rightBottom.x, b.rightBottom.y, c);
#endif
}

/* Draws a line on the screen of the specified color.
 * @param x    x value of point 1
 * @param y    y value of point 1
 * @param x1   x value of point 2
 * @param y1   y value of point 2
 * @param c    the color to paint the line.
 */

void ObjectFragments::drawLine(int x, int y, int x1, int y1, int c) {
#ifdef OFFLINE
    thresh->drawLine(x, y, x1, y1, c);
#endif
}

void ObjectFragments::drawLess(int x, int y, int c) {
#ifdef OFFLINE
    thresh->drawLine(x, y, x + 10, y - 10, c);
    thresh->drawLine(x, y, x + 10, y + 10, c);
    thresh->drawLine(x + 1, y, x + 11, y - 10, c);
    thresh->drawLine(x + 1, y, x + 11, y + 10, c);
#endif
}

void ObjectFragments::drawMore(int x, int y, int c) {
#ifdef OFFLINE
    thresh->drawLine(x, y, x - 10, y - 10, c);
    thresh->drawLine(x, y, x - 10, y + 10, c);
    thresh->drawLine(x - 1, y, x - 11, y - 10, c);
    thresh->drawLine(x - 1, y, x - 11, y + 10, c);
#endif
}



