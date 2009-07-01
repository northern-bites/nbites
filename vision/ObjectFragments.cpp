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
#include <vector>

using namespace std;


//here are defined the lower bounds on the sizes of goals, posts, and balls
//IMPORTANT: they are only guesses right now.

#define MIN_GOAL_HEIGHT	25
#define MIN_GOAL_WIDTH  5

// ID information on goal post constant
static const int NOPOST = 0;   // don't know which
static const int RIGHT = 1;
static const int LEFT = 2;
static const int USEBIG = 3;
static const int BACKSTOP = 4;
static const int MAX_NUM_BALL_RUNS = 500; //never use this!
static const int BALL_RUNS_MALLOC_SIZE = 10000;

// Comparison of spatial relationships of two blobs
static const int OUTSIDE = 0;      // one is outside the other
static const int TIGHT = 1;        // they are basically the same blob
// the small one is way on the left side of the big one
static const int CLOSELEFT = 2;
// the small one is way on the right side of the big one
static const int CLOSERIGHT = 3;
// the small one is sort of in the middle of the big one
static const int MURKY = 4;

static const int BIGGAP = 80;
static const int SIMILARSIZE = 5;

// Relative size of posts
static const int LARGE = 2;
static const int MEDIUM = 1;
static const int SMALL = 0;

// Am I looking at a post or a beacon?
//static const int BEACON = 0;
static const int POST = 1;

// Universal bad value used to id whether or not we successfully did something
static const int BADVALUE = -100;

// actually just skips 2 pixel noise in runs
static const int NOISE_SKIP = 3;
// minimum distance between goal and post - changed from 50 to 40 JS
static const int MIN_SPLIT = 40;
// Amount of post that has to match (so backstop isn't counted in posts)
static const float PERCENTMATCH = 0.65f;
// threshold for expanding sides back out
static const float HALFISH = 0.49f;
// highest ratio of width over height for posts
static const float GOODRAT = 0.75f;
// indicator that post may be salvagable
static const float SQUATRAT = 1.2f;
// EXAMINED: lowered
// goal posts of the same color have to be this far apart
static const int MIN_POST_SEPARATION = 7;
// how big a post is to be declared a big post
// EXAMINED: change this
static const int BIGPOST = 25;
static const float NORMALPOST = 0.6f;
static const float QUESTIONABLEPOST = 0.85f;

// Ball constants
// EXAMINED: look at this switch - SMALLBALLDIM
static const int SMALLBALLDIM = 4; // below this size balls are considered small
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
static const float MINORANGEPERCENT = 0.59f;
static const float MINGOODBALL = 0.5f;
static const float MAXGOODBALL = 3.0f;
static const int BIGAREA = 400;
static const int BIGGERAREA = 600;
static const float FATBALL = 2.0f;
static const float THINBALL = 0.5f;

static const int DIST_POINT_FUDGE = 5;

#ifdef OFFLINE
static const bool BALLDISTDEBUG = false;
static const bool PRINTOBJS = false;
static const bool POSTDEBUG = false;
static const bool POSTLOGIC = false;
static const bool TOPFIND = false;
static const bool BALLDEBUG = false;
static const bool CORNERDEBUG = false;
static const bool CROSSDEBUG = false;
static const bool BACKDEBUG = false;
static const bool SANITY = false;
static const bool DEBUGCIRCLEFIT = false;
static const bool DEBUGBALLPOINTS = false;
static const bool CORRECT = false;
static const bool OPENFIELD = false;
#else
static const bool BALLDISTDEBUG = false;
static const bool PRINTOBJS = false;
static const bool POSTDEBUG = false;
static const bool POSTLOGIC = false;
static const bool TOPFIND = false;
static const bool BALLDEBUG = false;
static const bool CORNERDEBUG = false;
static const bool CROSSDEBUG = false;
static const bool BACKDEBUG = false;
static const bool SANITY = false;
static const bool DEBUGCIRCLEFIT = false;
static const bool DEBUGBALLPOINTS = false;
static const bool CORRECT = false;
static const bool OPENFIELD = false;
#endif

//previous constants inserted from .h class


ObjectFragments::ObjectFragments(Vision* vis, Threshold* thr, int _color)
    : vision(vis), thresh(thr), color(_color), runsize(1)
{
    init(0.0);
    allocateColorRuns();
#ifdef OFFLINE
    BALLDISTDEBUG = true;
    PRINTOBJS = true;
    POSTDEBUG = true;
    POSTLOGIC = false;
    TOPFIND = false;
    BALLDEBUG = false;
    CORNERDEBUG = false;
    CROSSDEBUG = false;
    BACKDEBUG = false;
    SANITY = true;
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
    maxOfBiggestRun = 0L;
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


/* Set the primary color.  Depending on the color, we have different space needs
 * @param c        the color
 */
void ObjectFragments::setColor(int c)
{
    const int runNumInit = 3;
    const int numWidthMult = 15;
    const int sizeWidthMult = 5;

    runsize = 1;
    int run_num = runNumInit;
    color = c;
    // depending on the color we have more or fewer runs available
    switch (color) {
    case YELLOW:
    case BLUE:
	case WHITE:
        run_num = IMAGE_WIDTH * numWidthMult;
        runsize = IMAGE_WIDTH * sizeWidthMult;
        break;
    case RED:
    case NAVY:
        run_num = IMAGE_WIDTH * numWidthMult;
        runsize = IMAGE_WIDTH * sizeWidthMult;
        break;
    case ORANGE:
        runsize = BALL_RUNS_MALLOC_SIZE; //max number of runs
        run_num = runsize * runNumInit;
        break;
    }
    runs = (run*)malloc(sizeof(run) * run_num);
}


/* Allocate the required amount of memory dependent on the primary color
*/
void ObjectFragments::allocateColorRuns()
{
	const int runNumInit = 3;
	const int numWidthMult = 15;
    const int sizeWidthMult = 5;

    int run_num = runNumInit;
    // depending on the color we have more or fewer runs available
    switch (color) {
    case YELLOW:
    case BLUE:
	case WHITE:
        run_num = IMAGE_WIDTH * numWidthMult;
        runsize = IMAGE_WIDTH * sizeWidthMult;
        break;
    case RED:
    case NAVY:
        run_num = IMAGE_WIDTH * numWidthMult;
        runsize = IMAGE_WIDTH * sizeWidthMult;
        break;
    case ORANGE:
        runsize = BALL_RUNS_MALLOC_SIZE; //max number of runs
        run_num = runsize * runNumInit;
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
    const int contigVal = 4;
    const int blobMin = 20;

    // start out deciding to make a new blob
    // the loop will decide not to run it
    bool newBlob = true;
    int contig = contigVal;

    //cout << x << " " << y << " " << h << endl;
    // sanity check: too many blobs on screen
    if (numBlobs >= MAX_BLOBS) {
        //cout << "Ran out of blob space " << color << endl;
        // We're seeing too many blobs -it is unlikely we can do anything
        // useful with this color
        numBlobs = 0;
        numberOfRuns = 0;
        return;
    } else if (numBlobs > blobMin) {
        contig = contigVal;
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

/*
  Checks all of the blobs of this color.  Can be used to draw the widest blob.
*/
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
   @param which     The index of the blob to be zeroed
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

/* Merge blobs.  If two blobs are discovered to be connected, then they
   should be merged into one.  This is done here.
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
    const int newRunMult = 3;

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
            indexOfBiggestRun = numberOfRuns * newRunMult;
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

/* Try and recognize robots.  Basically we're doing blobbing here, but with lots of extra
   twists.  Mainly we're being extremely loose with the conditions under which we consider
   blobs to be "connected."  We're trying to take advantage of the properties of the robots -
   namely that they stand vertically normally.
 */
 
 
void ObjectFragments::robot(int bigGreen)
{
    const int lastRunXInit = -30;
    const int resConst = 20;
    const int blobWidthMin = 5;
    const int robotBlobMin = 10;

    int lastrunx = lastRunXInit, lastruny = 0, lastrunh = 0;

    // loop through all of the runs of this color
    // NOTE: "20" is a guess at something that should be a constant
    // and with the change in resolution, this ought to be changed
    for (int i = 0; i < numberOfRuns; i++) {
        //drawPoint(runs[i].x, runs[i].y, BLACK);
        if (runs[i].x < lastrunx + resConst) {
            for (int k = lastrunx; k < runs[i].x; k+= 1) {
                //cout << "merging " << k << " " << runs[i].x << endl;
                blobIt(k, lastruny, lastrunh);
            }
        }
	// now we can add the run normally
        blobIt(runs[i].x, runs[i].y, runs[i].h);
	// set the current as the last
        lastrunx = runs[i].x; lastruny = runs[i].y; lastrunh = runs[i].h;
    }
    // make first pass at attempting to identify robots
    getRobots(bigGreen);
    // check each of the candidate blobs to see if it might reasonably be
    // called a piece of a robot
    for (int i = 0; i < numBlobs; i++) {
      // NOTE: "5" is another constant that needs to be checked and changed
      if (blobWidth(blobs[i]) > blobWidthMin) {
	// temporarily put the blob into topBlob
	transferBlob(blobs[i], topBlob);
	// see if we can expand it to other parts
	expandRobotBlob();
	// put it back
	transferBlob(topBlob, blobs[i]);
      }
    }
    // now that we've done some expansion, see if we can merge any of the big blobs
    mergeBigBlobs();
    // try expanding again after the merging
    for (int i = 0; i < numBlobs; i++) {
        if (blobWidth(blobs[i]) > blobWidthMin) {
            transferBlob(blobs[i], topBlob);
            expandRobotBlob();
            transferBlob(topBlob, blobs[i]);
        }
    }
    int biggest = -1, index1 = -1, second = -1, index2 = -1;
    // collect up the two biggest blobs - those are the two we'll put into field objects
    for (int i = 0; i < numBlobs; i++) {
      // for now we'll use closest y - eventually we should use pixestimated distance
        // TODO: for now we'll use closest y - eventually we should use
        // pixestimated distance
        int area = blobArea(blobs[i]);
        if (viableRobot(blobs[i]) && blobArea(blobs[i]) >= biggest) {
            second = biggest;
            index2 = index1;
            index1 = i;
            biggest = blobArea(blobs[i]);
        } else if (viableRobot(blobs[i]) && blobArea(blobs[i]) > robotBlobMin) {
            second = area;
            index2 = i;
        }
    }
    // if we found some viable blobs, then add them as field objects
    if (index1 != -1) {
        updateRobots(1, index1);
        if (index2 != -1)
            updateRobots(2, index2);
    }
}

/*  We have a "blob" that might be part of a robot.  We built our blobs a little strangely and they
    may not be complete.  Let's see if we can expand them a bit.  Essentially we look around the
    sides of the blobs and see if we can expand the area of the blob.
 */

void ObjectFragments::expandRobotBlob()
{
	const int blobDiv = 4;
    const int goodsMin = 5;

    //int spany = blobHeight(topBlob);
    //int spanx = blobWidth(topBlob);
    // expand right side
    int x, y;
    int bestr = topBlob.rightTop.x;
    bool good = true;
    // start on the right side and keep going until we're sure we're done
    for (x = bestr; good && x < IMAGE_WIDTH - 1; x++) {
        good = false;
	// if we see anything of the right color that's good enough to expand our blob
        for (y = topBlob.rightTop.y; y < topBlob.rightBottom.y && !good;
             y += 1) {
            if (thresh->thresholded[y][x] == color)
                good = true;
        }
    }
    // now reset the right side information to reflect the new state of things
    topBlob.rightTop.x = x - 1;
    topBlob.rightBottom.x = x - 1;
    // repeat the process on the left side
    good = true;
    for (x = topBlob.leftTop.x; good && x >  -1; x--) {
        good = false;
        for (y = topBlob.rightTop.y; y < topBlob.rightBottom.y && !good;
             y += 2) {
            if (thresh->thresholded[y][x] == color)
                good = true;
        }
    }
    topBlob.leftTop.x = x + 1;
    topBlob.leftBottom.x = x + 1;
	

    // now try the bottom.  We're going to do this differently.
    // robots are mainly white, so if we run into a big swatch of white we'll
    // assume that its the same robot.
    int whites = IMAGE_WIDTH, pix, width = blobWidth(topBlob) / blobDiv;
    int goods = 0, lastSaw = 0;
    // loop down from the bottom until we can't expand anymore
    for (y = topBlob.leftBottom.y; whites >= width && y < IMAGE_HEIGHT - 1;y++){
        whites = 0;
        goods = 0;
	// check this row of pixels for white or same color (good), or for opposite color (bad)
        for (x = topBlob.leftBottom.x; x < topBlob.rightTop.x && whites < width;
             x++) {
            if(topBlob.leftBottom.x < 0) {
                cout << "Top blob left bottom is negative" << endl;
            }
            pix = thresh->thresholded[y][x];
            if (pix == color) {
                whites++;
		// theoretically a color match of enough pixels would be plenty of evidence that we're
		// able to expand here.  Currently we don't seem to increment goods.  Something to
		// experiment with.
                if (goods > goodsMin) {
                    whites = width;
                }
            } else if (pix == WHITE) {
                whites++;
            } else if ((color ==  NAVY && pix == RED) ||
                       (color == RED && pix == NAVY)) {
	      // Uh oh, we may be seeing another robot of the opposite color.  Could be trouble.
                whites -= goodsMin;
            }
        }
        if (goods < goodsMin) {
            lastSaw++;
        } else {
            lastSaw = 0;
        }
    }
    int gain = y - 1 - topBlob.leftBottom.y;
    topBlob.leftBottom.y = y - 1;
    topBlob.rightBottom.y = y - 1;
    // if we expanded enough, it is probably worth looking at the sides again.
    if (gain > goodsMin) {
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

/*
  We have detected something big enough to be called a robot.  Set the appropriate
  field object.
  @param which    whether it is the biggest or the second biggest object
  @param index    the index of the blob in question
 */

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

/* Like regular merging of blobs except that with robots we used a relaxed criteria.
 */
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

/*  Are two robot blobs close enough to merge?
    Needless to say this needs lots of experimentation.  "40" was based on some, but
    at high resolution.  Obviously it should be a constant.
 */

bool ObjectFragments::closeEnough(blob a, blob b)
{
    // EXAMINED: change constant to lower res stuff
    const int closeDistMax = 40;
	
    int xd = distance(a.leftTop.x,a.rightTop.x,
                      b.leftTop.x,b.rightTop.x);
    int yd = distance(a.leftTop.y,a.leftBottom.y,
                      b.leftTop.y,b.rightBottom.y);
    if (xd < closeDistMax) {
        if (yd < closeDistMax)
            return true;
    }
    // if (xd < max(blobWidth(a), blobWidth(b)) &&
    //     yd < max(blobHeight(a), /blobHeight(b))) return true;
    return false;
}

/*  Are the two blobs big enough to merge.  Again the constants are merely guesses
    at this stage.  And guesses at high rez to boot.
 */

bool ObjectFragments::bigEnough(blob a, blob b)
{
    // EXAMINED: change constant to lower res stuff // at half right now
    const int minBlobArea = 100;
    const int horizonOffset = 50;
	
    if (blobArea(a) > minBlobArea && blobArea(b) > minBlobArea)
        return true;
    if (a.leftBottom.y > horizonAt(a.leftBottom.x) + horizonOffset)
        return true;
    return false;
}


/*  Is this blob potentially a robot?  Return true if so.  Basically we look at
    the blob and see how many pixels seem to be "robot" pixels.  If there are
    enough, then we call it good enough.  This is probably a dumb way to do this
    because it is slow as heck.
    See the comments for the other things - the constants should become real constants!
    @param a   the index of the blob we're checking
    @return    whether it meets our criteria
 */

bool ObjectFragments::viableRobot(blob a)
{
    const int blobPix = 10;
    const float blobAreaMin = 0.10f;

    // get rid of obviously false ones
    // TODO: change constant to lower res stuff
    if (!(a.rightBottom.y > horizonAt(a.rightBottom.x) && blobWidth(a) > blobPix)) {
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
    if ((float)(whites + col) / (float)blobArea(a) > blobAreaMin)
        return true;
    return false;
}


/* Find robot blobs.
   @param maxY     max value - will be used to pick heads out
*/

void ObjectFragments::getRobots(int maxY)
{
    const int spreadInit = 3;

    topBlob = zeroBlob;
    int size = 0;
    topSpot = 0;
    // TODO: experiment with this constant; probably 3; maybe 2
    int spreadSeparation = spreadInit;
    //check each blob in the array
    for (int i = 0; i < numBlobs; i++) {
        int spread = blobs[i].rightTop.x - blobs[i].leftTop.x;
        if (blobs[i].leftTop.x == BADVALUE) {
        } else if (blobs[i].leftTop.y > maxY || spread < spreadSeparation) {
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
        if (blobs[i].leftTop.x != BADVALUE && blobs[i].leftTop.y < maxY
            && spread > spreadSeparation) {
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
    const int leftInit = 35;
	const int rightInit = 100;
    const int rangeDiv = 100;

    // these to be REAL constants
    int LEFTRANGE = leftInit;
    int RIGHTRANGE = rightInit - LEFTRANGE;
	
    if ((head - left) < (right - left) * LEFTRANGE / rangeDiv)
        return LEFT;
    if ((head - left) > (right - left) * RIGHTRANGE / rangeDiv)
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

/* Scan from the point along the line until you have hit "stopper" points that
 * aren't color "c"
 * return the last good point found and how many good and bad points seen.
 * Though this is a void function it actually "returns" information in the scan
 * variable. scan.x and scan.y represent the finish points of the line (last
 * point of appropriate color) and bad and good represent how many bad and good
 * pixels (pixels that are of the right color or not) along the way.
 *
 * @param x          the x point to start at
 * @param y          the y point to start at
 * @param dir        the direction of the scan (positive or negative)
 * @param stopper    how many incorrectly colored pixels we can live with
 * @param c          color we are most interested in
 * @param c2         soft color that could also work
 * @param leftBound  furthest left we can go
 * @param rightBound further right we can go
 */
void ObjectFragments::horizontalScan(int x, int y, int dir, int stopper, int c,
                                     int c2, int leftBound, int rightBound) {
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
    for ( ; x > leftBound && y > -1 && x < rightBound && x < IMAGE_WIDTH
              && y < height && bad < stopper; ) {
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

/*
 * TODO: Check this edge value
 * Given two points determine if they constitute an "edge".  For now our
 * definition of an edge is a difference in Y values of 30 (sort of a standard
 * approach in our league).  This is a place for potential improvements in the
 * future.
 *
 * @param x        the x value of the first point
 * @param y        the y value of the first point
 * @param x2       the x value of the second point
 * @param y2       the y value of the second point
 * @return         was an edge detected?
 */

bool ObjectFragments::checkEdge(int x, int y, int x2, int y2)
{
    const int diffMin = 30;

    int ydiff = abs(thresh->getY(x, y) - thresh->getY(x2, y2));
    if (ydiff > diffMin) {
        return true;
    }
    return false;
}

/*
 * Given two points that should define an edge of a structure, try and improve
 * them to find the true edge.
 * @param top         the topmost estimated point
 * @param bottom      the bottommost estimated point
 * @param c           the primary color of the structure
 * @param c2          the secondary color of the structure
 * @param left        whether the open space is to the left or the right
 */

int ObjectFragments::findTrueLineVertical(point <int> top, point <int> bottom,
                                          int c, int c2, bool left)
{
    const int countSpanDiv = 3;

    int spanY = bottom.y - top.y;
    int count = 0;
    int good = spanY;
    int j = 0;
    int dir = 1;
    if (left)
        dir = -1;
    // go until you hit enough bad pixels or some strong reason to stop
    for (j = 1; count < spanY / countSpanDiv && top.x + dir * j >= 0
             && top.x + dir * j < IMAGE_WIDTH && good > spanY / 2 ; j++) {
        count = 0;
        good = 0;
        for (int i = top.y; count < spanY / countSpanDiv && i <= bottom.y; i++) {
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

/*
 * Given two points that should define an edge of a structure, try and improve
 * them to find the true edge. Exactly like the previous function except that it
 * allows for a slope to the camera angle.
 *
 * @param top         the topmost estimated point
 * @param bottom      the bottommost estimated point
 * @param c           the primary color of the structure
 * @param c2          the secondary color of the structure
 * @param left        whether the open space is to the left or the right of the structure
 */

void ObjectFragments::findTrueLineVerticalSloped(point <int>& top,
                                                 point <int>& bottom,
                                                 int c, int c2, bool left)
{
    const int countSpanDiv = 3;
	const int runX = 5;
	const int initMin = 5;
	const int imageDiff = 3;

    int increment = 1;
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
    int minCount = (spanY / countSpanDiv) / increment;
    int minRun = min(spanY, max(runX, spanY / 2));
    int minGood = max(1, (spanY / 2) / increment);
    int badLines = 0;
    int i = 0;
    int fake = 0;
    bool atTop = false;
    //drawPoint(top.x, top.y, RED);
    //drawPoint(bottom.x, bottom.y, RED);
    /* loop until we now longer have viable expansion
          too many bad lines in a row
          top is off the edge of the screen
       basically we scan the next line out and count how many good and bad points we
       get along that scanline.  If there are enough good ones, we expand and keep
       moving.  If not, then we may stop
    */
    for (j = 1; badLines < 2 && top.x + dir * j >= 0
             && top.x + dir * j < IMAGE_WIDTH; j+=increment) {
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
	// here's where we do the scanning.  Stop early if we have enough information good or bad.
        for (i = actualY; count < minCount  &&
                 i <= actualY + spanY && (run < minRun || goodRun > spanY / 2)
                 && (top.y > 1 || initRun < minRun);
             i+= increment) {
            theSpot = xProject(top.x + dir * j, actualY, i);
            if (theSpot < 0 || theSpot > IMAGE_WIDTH - 1 || theSpot - dir < 0 ||
                theSpot - dir > IMAGE_WIDTH - 1 || i < 0 ||
                i > IMAGE_HEIGHT - 1) {
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
                        if (atTop && initRun > initMin) break;
                        // cout << "Init run " << initRun << " " << top.y << " "
                        //      << minRun << endl;
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
        // cout << good << " " << minGood << " " << count << " " << initRun << " "
        //      << i << endl;
        //if (!left)
        //drawPoint(theSpot, i, BLACK);
    }

    // ok so we did the basic thing.  The problem is that sometimes we ended early because we hit
    // a screen edge and weren't really done.  Try and check for those situations.
    int temp = top.x;
    //drawPoint(top.x, top.y, BLACK);
    top.x = top.x + dir * (j - badLines) - dir;
    top.y = yProject(temp, top.y, top.x);
    //drawPoint(top.x, top.y, RED);
    bottom.y = top.y + spanY;
    bottom.x = xProject(top.x, top.y, top.y + spanY);
    //cout << "Checking " << top.x << " " << top.y << endl;
    if (top.x < 2 || top.x > IMAGE_WIDTH - imageDiff) {
        //cout << "In upward scan" << endl;
        for (j = 1; count < minCount && bottom.x + dir * j >= 0
                 && bottom.x + dir * j < IMAGE_WIDTH
                 && good > minGood && run < minRun; j+=increment) {
            //count = 0;
            good = 0;
            run = 0;
            int actualY = yProject(bottom.x, bottom.y, bottom.x + dir * j);
            for (i = actualY; count < minCount && i >= actualY - spanY &&
                     run < minRun; i-= increment) {
                theSpot = xProject(bottom.x + dir * j, actualY, i);

                if (theSpot < 0 || theSpot > IMAGE_WIDTH - 1
                    || theSpot - dir < 0 || theSpot - dir > IMAGE_WIDTH - 1
                    || i < 0 || i > IMAGE_HEIGHT - 1)
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

/*
 * Given two points that should define an edge of a structure, try and improve
 * them to find the true edge.
 * @param left        the leftmost estimated point
 * @param right       the rightmost estimated point
 * @param c           the primary color of the structure
 * @param c2          the secondary color of the structure
 * @param up          whether the open space is up or down
 */

void ObjectFragments::findTrueLineHorizontalSloped(point <int>& left,
                                                   point <int>& right, int c,
                                                   int c2, bool up)
{
    const int greenInit = 3;
    const int runX = 5;
	const int runDiv = 5;
	const int countDiv = 3;
	const int goodDiv = 3;
	const float spanMult = 0.5;

    int spanX = right.x - left.x + 1;
    int spanY = right.y - left.y + 1;
    int count = 0;
    int good = spanX;
    int j = 0;
    int dir = 1;
    int theSpot = 0;
    int run = 0;
    int badLines = 0;
    int maxgreen = greenInit;
// EXAMINED: change to lower res
    maxgreen = 100;
    if (up)
        dir = -1;
    int minRun = min(spanX, max(runX, spanX / runDiv));
    int minCount = min(spanX, max(2, spanX / countDiv));
    int minGood = max(1, spanX / goodDiv);
    int greens = 0;
    int fakegood = 0;

    // loop until we can't expand anymore
    for (j = 1; count < minCount && left.y + dir * j >= 0
             && left.y + dir * j < IMAGE_HEIGHT && badLines < 2
             && greens < max(minRun, maxgreen); j++) {
        //count = 0;
        good = 0;
        run = 0;
        greens = 0;
        fakegood = 0;
        int actualX = xProject(left.x, left.y, left.y + dir * j);
	// count up the good and bad pixels in this scanline
        for (int i = actualX; count < minCount && i <= actualX + spanX
                 && greens < maxgreen; i++) {
            theSpot = yProject(actualX, left.y + dir * j, i);
            //drawPoint(i, theSpot, RED);
            if (theSpot < 0 || theSpot > IMAGE_HEIGHT - 1 || theSpot - dir < 0
                || theSpot - dir > IMAGE_HEIGHT - 1 || i < 0
                || i > IMAGE_WIDTH - 1) {
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
    // cout << good << " " << minGood << " " << run << " " << minRun << endl;
    // if we had to stop because we hit the left edge, then see if we can go
    // farther by using the bottom
    int temp = left.y;
    left.y =  left.y + dir * (j - badLines) - dir;
    left.x = xProject(left.x, temp, left.y);
    right.x = left.x + spanX;
    right.y = left.y + spanY;

    if( right.x > IMAGE_WIDTH) {
        //cout << "right.x > IMAGE_WIDTH, value is: " << right.x << endl;
        right.x = IMAGE_WIDTH;
    }

    if( left.x < 0) {
        //cout << "left.x < 0, value is: " << left.x << endl;
        left.x = 0;
    }


    if (!up && thresh->getVisionHorizon() > left.y) {
        // for the heck of it let's scan down
        int found = left.y;
        for (int d = left.y; d < thresh->getVisionHorizon(); d+=1) {
            good = 0;
            for (int a = left.x; a < right.x; a++) {
                if (thresh->thresholded[d][a] == c) {
                    good++;
                }
            }
            if (good > spanX * spanMult) {
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
}

/*
 * Given two points that should define an edge of a structure, try and improve
 * them to find the true edge. This is just like the previous function except
 * that it doesn't assume a perfectly level plane.  Which of course makes it a
 * lot harder.  C'est la vie.
 *
 * @param left        the leftmost estimated point
 * @param right       the rightmost estimated point
 * @param c           the primary color of the structure
 * @param c2          the secondary color of the structure
 * @param up          whether the open space is up or down
 */

int ObjectFragments::findTrueLineHorizontal(point <int> left, point <int> right,
                                            int c, int c2, bool up)
{
    const int spanDiv = 3;

    int spanX = right.x - left.x + 1;
    int count = 0;
    int good = spanX;
    int j = 0;
    int dir = 1;
    if (up)
        dir = -1;
    for (j = 1; count < spanX / spanDiv && left.y + dir * j >= 0
             && left.y + dir * j < IMAGE_HEIGHT && good > spanX / 2; j++) {
        count = 0;
        good = 0;
        for (int i = left.x; count < spanX / spanDiv && i <= right.x; i++) {
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

/*  In theory our pose information tells us the slant of an object.  In practice it doesn't always get it
    for a vareity of reasons.  This is an attempt to correct for the errors in the process.  At this point
    it is basically a rough draft of a good methodology.
    @param post    the blob we're examining
    @param c       primary color
    @param c2      secondary color
 */

float ObjectFragments::correct(blob b, int color, int c2) {

	const int maxParam = 10;
	const int hDiv = 5;
	const int scanParam = 6;
	const int bigMin = 5;
	const int skewrMin = 5;
	const int bigMin2 = 5;
	const int skewlMin = 5;

    // try and find the cross bar - start at the upper left corner
    int biggest = 0, biggest2 = 0;
	int skewr = 0, skewl = 0;
    int x = b.leftTop.x;
    int y = b.leftTop.y;
    int h = blobHeight(b);
    int w = blobWidth(b);
    //int need = min(w / 2, 20);
    int need = max(w, maxParam);
	float newSlope = 0.0f;
	if (CORRECT) {
		//drawBlob(b, ORANGE);
	}
	// scan the left side to see how far out we can go seeing post
    for (int i = 0; i < h / hDiv && biggest < need; i+=1) {
        int tx = xProject(x, y, y + i);
        horizontalScan(tx, y + i, -1, scanParam, color, color, max(0, x - 2 * w),
                       IMAGE_WIDTH - 1);
        if (scan.good > biggest) {
            biggest = scan.good;
        }
    }
	// now the right side
    x = b.rightTop.x;
    y = b.rightTop.y;
    for (int i = 0; i < h / hDiv && biggest2 < need; i+=1) {
        int tx = xProject(x, y, y + i);
        horizontalScan(tx, y + i, 1, scanParam, color, color, x - 1, IMAGE_WIDTH - 1);
        if (scan.good > biggest2) {
            biggest2 = scan.good;
        }
    }
	// What we're going to do is scan opposite corners at the bottom and subtract
	// those results.
	x = b.rightBottom.x;
	y = b.rightBottom.y;
	int bestr = 0, bestl = 0;
    for (int i = 0; i < h / hDiv && skewr < biggest; i+=1) {
        int tx = xProject(x, y, y - i);
        horizontalScan(tx, y - i, 1, scanParam, color, color, x - 1, IMAGE_WIDTH - 1);
        if (scan.good > skewr) {
            skewr = scan.good;
			bestr = i;
        }
    }
	x = b.leftBottom.x;
	y = b.leftBottom.y;
    for (int i = 0; i < h / hDiv && skewl < biggest2; i+=1) {
        int tx = xProject(x, y, y - i);
        horizontalScan(tx, y - i, -1, scanParam, color, color, max(0, x - 2 * w),
                       IMAGE_WIDTH - 1);
        if (scan.good > skewl) {
			bestl = i;
            skewl = scan.good;
        }
    }

	int stops = 10;
	if (biggest > bigMin && skewr > skewrMin) {
		if (CORRECT)
			cout << "Left lean detected " << biggest << " " << skewr << " " << bestr << endl;
		int topmove = min(skewr, biggest);
		//drawLine(b.leftTop.x - skewr, b.leftTop.y, b.leftBottom.x + skewr, b.leftBottom.y, ORANGE);
		b.leftTop.x = max(0, b.leftTop.x - topmove);
		b.rightBottom.x = b.rightBottom.x + skewr;
		// make sure we don't over-correct
		x = b.leftBottom.x + skewr;
		y = b.leftBottom.y;
		int backr = 0;
		int stopper = max(stops, bestr);
		for (int i = 0; i < stopper; i+=1) {
			int tx = xProject(x, y, y - i);
			horizontalScan(tx, y - i, -1, scanParam, color, color, max(0, x - 2 * w),
						   IMAGE_WIDTH - 1);
			if (scan.good > backr) {
				backr = scan.good;
			}
		}
		b.leftBottom.x = b.leftBottom.x + skewr - backr;
		b.rightTop.x = min(IMAGE_WIDTH - 1, b.rightTop.x - topmove + backr);
		newSlope = -(float)(b.leftBottom.x - b.leftTop.x) / (float)(b.leftBottom.y - b.leftTop.y);
		return newSlope;
	}
	if (biggest2 > bigMin2 && skewl > skewlMin) {
		if (CORRECT)
			cout << "Right lean detected " << biggest2 << " " << skewl << " " << bestl << endl;
		int topmove = min(skewl, biggest2);
		b.rightTop.x = b.rightTop.x + topmove;
		b.leftBottom.x = max(0, b.leftBottom.x - skewl);
		x = max(0, b.rightBottom.x - skewl);
		y = b.rightBottom.y;
		int backl = 0;
		int stopper = max(stops, bestl);
		for (int i = 0; i < stopper; i+=1) {
			int tx = xProject(x, y, y - i);
			horizontalScan(tx, y - i, 1, scanParam, color, color, 0, IMAGE_WIDTH - 1);
			if (scan.good > backl) {
				backl = scan.good;
			}
		}
		b.rightBottom.x = min(IMAGE_WIDTH - 1, b.rightBottom.x - skewl + backl);
		b.leftTop.x = b.leftTop.x + topmove - backl;
		newSlope = -(float)(b.rightBottom.x - b.rightTop.x) / (float)(b.rightBottom.y - b.rightTop.y);
	}
	return newSlope;
	}

/*  Routine to find a general square goal.
 * We start with a point.  We scan up from the point and down from the point
 * looking for a strip of the right color.  That serves as our starting point.
 * Then we try expanding the sides outward.  The we try expanding the top and
 * bottom in a similar fashion.
 *
 * @param x         x value of our starter point
 * @param y         y value of our starter point
 * @param c         the primary color
 * @param c2        the secondary color
 */
void ObjectFragments::squareGoal(int x, int y, int c, int c2)
{
    const int scanVal = 3;

    // so we can check for failure
	int count = 0;
	bool looping = false;
    obj.leftTop.x = BADVALUE; obj.leftTop.y = BADVALUE;
	do {
		// first we try going up
		vertScan(x, y, -1,  scanVal, c, c2);
		int h = scan.good;
		// at this point we have a very rough idea of how tall the square is
		int top = scan.y;
		int topx = scan.x;
		// now go down
		vertScan(x, y, 1,  scanVal, c, c2);
		h += scan.good;
		if (h < 2) return;
		int bottom = scan.y;
		int bottomx = scan.x;
		//drawPoint(topx, top, RED);
		//drawPoint(bottomx, bottom, RED);
		obj.leftTop.x = topx;
		obj.leftTop.y = top;
		obj.rightTop.x = topx;
		obj.rightTop.y = top;
		obj.leftBottom.x = bottomx;
		obj.leftBottom.y = bottom;
		obj.rightBottom.x = bottomx;
		obj.rightBottom.y = bottom;
		//int spanY = obj.leftBottom.y - obj.leftTop.y;
		findTrueLineVerticalSloped(obj.leftTop, obj.leftBottom, c, c2, true);
		//drawPoint(obj.leftTop.x, obj.leftTop.y, RED);
		//drawPoint(obj.leftBottom.x, obj.leftBottom.y, RED);
		findTrueLineVerticalSloped(obj.rightTop, obj.rightBottom, c, c2, false);
		findTrueLineHorizontalSloped(obj.leftTop, obj.rightTop, c, c2, true);
		findTrueLineHorizontalSloped(obj.leftBottom, obj.rightBottom, c, c2, false);
		float newSlope = correct(obj, c, c2);
		if (newSlope != 0.0) {
			if (CORRECT)
				cout << "Old slope was " << slope << " " << newSlope << endl;
			slope = newSlope;
			looping = true;
		}
		count++;
	} while (count < 2 && looping);
    //drawBlob(obj, ORANGE);
}

/*  Our "main" methods.  Entry points into just about everything else.
 */

/* This method takes in a candidate field cross and decides if it is good enough.
   We run it through a series of sanity checks.  If it passes them all then
   it is called a cross.
 */


void ObjectFragments::checkForX(blob b) {
	int x = b.leftTop.x;
	int y = b.leftTop.y;
	int w = blobWidth(b);
	int h = blobHeight(b);
	int count = 0, counter = 0;
	// First we scan the outside of the blob.  It should basically be all
	// green.  What we don't want are line fragments or robot fragments
	// so finding white is very bad.
	for (int i = max(0, y - 2); i < min(IMAGE_HEIGHT - 1, y + h + 2); i++) {
		if (x > 1) {
			if (thresh->thresholded[i][x - 2] == GREEN)
				count++;
			else if (thresh->thresholded[i][x - 2] == WHITE)
				count-=3;
			counter++;
		} else return;
		if (x + w + 2 < IMAGE_WIDTH) {
			if (thresh->thresholded[i][x + w+ 2] == GREEN)
				count++;
			else if (thresh->thresholded[i][x + w+ 2] == WHITE)
				count-=3;
			counter++;
		} else return;
	}
	for (int i = max(0, x - 2); i < min(IMAGE_WIDTH - 1, x + w + 2); i++) {
		if (y > 1) {
			if (thresh->thresholded[y - 2][i] == GREEN)
				count++;
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
	// Next we check and make sure that this isn't part of any lines
	if (count > (float)counter * 0.8f) {
		// first make sure this isn't really a line
        point <int> plumbLineTop, plumbLineBottom, line1start, line1end;
        plumbLineTop.x = x + w / 2; plumbLineTop.y = y;
        plumbLineBottom.x = x; plumbLineBottom.y = y + h;
        const vector <VisualLine>* lines = vision->fieldLines->getLines();
        for (vector <VisualLine>::const_iterator k = lines->begin();
             k != lines->end(); k++) {
            pair<int, int> foo = Utility::
                plumbIntersection(plumbLineTop, plumbLineBottom,
                                  k->start, k->end);
            if (foo.first != NO_INTERSECTION && foo.second != NO_INTERSECTION) {
				if (CROSSDEBUG)
					cout << "Throwing out blob that is part of a line" << endl;
				return;
            }
        }
		if (CROSSDEBUG) {
			cout << "Found a cross " << endl;
			printBlob(b);
			drawBlob(b, RED);
		}
		// Is the cross white enough?  At least half the pixels must be white.
		if (!rightBlobColor(b, 0.5f)) {
			if (CROSSDEBUG) {
				cout << "Tossing a blob for not being white enough " << endl;
			}
			return;
		}
		// Make sure we don't have more than one candidate cross.  Note:  we
		// actually can see two crosses at some places on the field, but for
		// now we just will ID one of them.
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


/* This is the entry  point from object recognition in Threshold.cc  For now it
 * is only called on yellow, blue and orange.
 * In each case we search for objects that have the corresponding colors.
 * @param  c            color we are processing
 * @return              always 0
 */

void ObjectFragments::createObject(int c) {
    // these are in the relative order that they should be called
    switch (color) {
    case GREEN:
        break;
	case WHITE:
		if (numberOfRuns > 1) {
			for (int i = 0; i < numberOfRuns; i++) {
				// search for contiguous blocks
				int nextX = runs[i].x;
				int nextY = runs[i].y;
				int nextH = runs[i].h;
				blobIt(nextX, nextY, nextH);
			}
		}
		if (CROSSDEBUG)
			cout << numBlobs << " white blobs" << endl;
		for (int i = 0; i < numBlobs; i++) {
			if (blobWidth(blobs[i]) < 55 && blobHeight(blobs[i]) < 55 &&
				blobWidth(blobs[i]) > 5 && blobHeight(blobs[i]) > 5 &&
				blobWidth(blobs[i]) < 5 * blobHeight(blobs[i])  &&
				blobHeight(blobs[i]) < 5 * blobWidth(blobs[i])) {
				checkForX(blobs[i]);
			}
			//else printBlob(blobs[i]);
		}
		break;
    case BLUE:
        // either we should see a marker or a goal
        blue(c);
        break;
    case RED:
    case NAVY:
        // George: I am disabling robot recognition for now because it
        // causes crashes. The blobs formed for robots have negative
        // and/or incorrect dimensions. Those dimensions are later used
        // to access the thresholded array.
        //robot(c);
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
    goalScan(vision->yglp, vision->ygrp, vision->ygCrossbar, YELLOW,
             ORANGEYELLOW, false, bigGreen);
}

/* Process blue:
 * Same as for yellow except we theoretically don't need to worry about finding
 * beacons.
 * @param bigGreen       field horizon at x == 0
 */

void ObjectFragments::blue(int bigGreen) {

    goalScan(vision->bglp, vision->bgrp, vision->bgCrossbar, BLUE, BLUEGREEN,
             false, bigGreen);
    // Now that we're done, print out information on all of the objects that
    // we've found.
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
        if (nextH > maxRun && (nextX < left || nextX > right)) {
            // hack for pictures with tape measure
            // && nextY < hor) {
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
  const float BLUEPOST = 0.75f;
  float perc = NORMALPOST;
  if (_certainty != _SURE && blobHeight(two) < 40 && color == BLUE) {
    //cout << "uppint the anty on blue" << endl;
    perc = BLUEPOST;
  }
    if (rightBlobColor(two, perc)) {
        one->updateObject(&two, _certainty, _distCertainty);
        return true;
    } else {
      //cout << "Screening object for low percentage of real color" << endl;
        return false;
    }
}

/* Here we are trying to figure out how confident we are about our values with
 * regard to how they might be used for distance calculations.  Basically if
 * an object is too near one of the screen edges, or if we have some evidence
 * that it is occluded then we note uncertainty in that dimension.
 *
 * @param left     the leftmost point of the object
 * @param right    the rightmost point of the object
 * @param top      the topmost point of the object
 * @param bottom   the bottommost point of the object
 * @return         a constant indicating where the uncertainties (if any) lie
 */
distanceCertainty ObjectFragments::checkDist(int left, int right, int top,
                                             int bottom)
{
    const int scanParam = 6;

    distanceCertainty dc = BOTH_SURE;
    int nextX, nextY;
    if (left < DIST_POINT_FUDGE || right > IMAGE_WIDTH - DIST_POINT_FUDGE) {
        if (top < DIST_POINT_FUDGE || bottom > IMAGE_HEIGHT - DIST_POINT_FUDGE){
            return BOTH_UNSURE;
        } else {
            dc =  WIDTH_UNSURE;
        }
    } else if (top < DIST_POINT_FUDGE || bottom > IMAGE_HEIGHT -
               DIST_POINT_FUDGE) {
        return HEIGHT_UNSURE;
    }
    // we need to do one more check - make sure that the bottom of the object is
    // not obscured
    nextX = pole.leftBottom.x;
    nextY = pole.leftBottom.y;
    do {
        vertScan(nextX, nextY, 1,  scanParam, GREEN, GREEN);
        nextX = nextX + 2;
        nextY = yProject(pole.leftBottom, nextX);
    } while (nextX <= pole.rightBottom.x && scan.good < 2);
    if (scan.good > 1)
        return dc;
    else if (dc == WIDTH_UNSURE)
        return BOTH_UNSURE;
    return HEIGHT_UNSURE;
}

/*
 * Post recognition for NAOs
 */

/*  Look for a crossbar once we think we've found a goalpost.  The idea is
 * pretty simple, if we  have done a good job with the square post,
 * then the crossbar should either shoot out of the left or right top corner.
 * So try scanning from those positions.  The tricky part, or course involves
 * being able to definitively say that what we see is in fact a bar.  I'd say
 * that right now this probably needs more work.
 *
 *  @param b   the square post
 *  @return   either RIGHT or LEFT if a crossbar found, or NOPOST if not
 */
int ObjectFragments::crossCheck(blob b)
{
    const int needParam = 10;
	const int hDiv = 5;
	const int horzScanParam = 6;
	const int drawParam = 20;

    // try and find the cross bar - start at the upper left corner
    int biggest = 0, biggest2 = 0;
	int skewr = 0, skewl = 0;
    int x = b.leftTop.x;
    int y = b.leftTop.y;
    int h = blobHeight(b);
    int w = blobWidth(b);
    //int need = min(w / 2, 20);
    int need = max(w, needParam);

	// scan the left side to see how far out we can go seeing post
    for (int i = 0; i < h / hDiv && biggest < need; i+=1) {
        int tx = xProject(x, y, y + i);
        horizontalScan(tx, y + i, -1, horzScanParam, color, color, max(0, x - 2 * w),
                       IMAGE_WIDTH - 1);
        if (scan.good > biggest) {
            biggest = scan.good;
        }
    }
	// now the right side
    x = b.rightTop.x;
    y = b.rightTop.y;

    for (int i = 0; i < h / hDiv && biggest2 < need; i+=1) {
        int tx = xProject(x, y, y + i);
        horizontalScan(tx, y + i, 1, horzScanParam, color, color, x - 1, IMAGE_WIDTH - 1);
        if (scan.good > biggest2) {
            biggest2 = scan.good;
        }
    }

    if (POSTLOGIC) {
        cout << "Cross check " << biggest << " " << biggest2 << endl;
		if (biggest > need)
			drawRect(b.leftTop.x - biggest, b.leftTop.y, biggest, drawParam, ORANGE);
		if (biggest2 > need)
			drawRect(x, y, biggest2, drawParam, ORANGE);
    }

	if (biggest > need && biggest > 2 * biggest2) return RIGHT;
	if (biggest2 > need && biggest2 > 2 * biggest) return LEFT;

    return NOPOST;
}


int ObjectFragments::crossCheck2(blob b) {
    // try and find the cross bar - start at the upper left corner
    //int biggest = 0, biggest2 = 0;
    int x = b.leftTop.x;
    int y = b.leftTop.y;
    int h = b.leftBottom.y - b.leftTop.y;
    //int w = b.rightTop.x - b.leftTop.x;
    // EXAMINED: look at constants 50 and 10
    const int numLiesMin = 25;
    const int numLiesFactorMin = 5;
	
	const int buff = 10;
	const int ypAdd = 10;
	
    int lefties = 0, righties = 0;
    for (int i = x - buff; i > max(0, x - h); i-=1) {
        int yp = yProject(x, y, i);
        for (int j = max(0, yp); j < yp + ypAdd; j++) {
            if (thresh->thresholded[j][i] == color) {
                lefties++;
            }
        }
    }
    for (int i = b.rightTop.x + buff; i < min(IMAGE_WIDTH - 1, b.rightTop.x + h);
         i+=1) {
        int yp = yProject(b.rightTop.x, b.rightTop.y, i);
        for (int j = max(0, yp); j < yp + ypAdd; j++) {
            if (thresh->thresholded[j][i] == color) {
                righties++;
            }
        }
    }
    if (righties > numLiesMin && righties > numLiesFactorMin * lefties)
        return LEFT;
    if (lefties > numLiesMin && lefties > numLiesFactorMin * righties)
        return RIGHT;
    return NOPOST;
}

/* Try to use field line information to decide which post we're looking at.
   This is actually considerably easier with the large goal boxes as the only
   times you'll see corners reasonably near a post is when they are on the
   same side as the post.
 */

int ObjectFragments::checkIntersection(blob post) {

	const int postBuff = 30;

    // TODO: check if this should be the same standard minHeight for a post
    if (post.rightBottom.y - post.rightTop.y < postBuff) return NOPOST;
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
            bool closeEnough = false;
            if (y < post.leftBottom.y + spany) closeEnough = true;
			// if the T is higher in the visual field than the bottom of the post
			// then we have an easy job - we can decide based on which side its on
			if (closeEnough) {
				if (x <= post.leftBottom.x)
					return LEFT;
				return RIGHT;
			}
        } else {
            int x = k->getX();
            int y = k->getY();
            bool closeEnough = false;
			// we have a somwhat more stringent standard of closeness for corners for now
            if (y < post.leftBottom.y) closeEnough = true;
			// if the corner is higher in the visual field than the bottom of the post
			// then we have an easy job - we can decide based on which side its on
			// This is based on the layout of the field and the limitations of the
			// camera (and the fact that the goal box is absurdly large)
			if (closeEnough) {
				if (x <= post.leftBottom.x)
					return LEFT;
				return RIGHT;
			}
        }
    }
    return NOPOST;
}

/*  We have a post and wonder which one it is.  This method looks for the nearby intersection
    of the goal line and the goal box.  If it can be found it is the best way to ID the goal.
    @param post    the post we have id'd
    @return        the id of the post (or lack of id)
 */

int ObjectFragments::checkCorners(blob post)
{
    const int postDiffMin = 30;
	const int postBuff = 5;

    if (post.rightBottom.y - post.rightTop.y < postDiffMin) return NOPOST;
    const list <VisualCorner>* corners = vision->fieldLines->getCorners();
    int spanx = post.rightBottom.x - post.leftBottom.x;
    for (list <VisualCorner>::const_iterator k = corners->begin();
         k != corners->end(); k++) {
        // we've already checked the Ts so ignore them
        if (k->getShape() != T) {
            if (k->getX() > post.leftBottom.x - spanx &&
                k->getX() < post.rightBottom.x + spanx &&
                post.leftBottom.y < k->getY()) {
                if (k->getX() > post.leftBottom.x - postBuff &&
                    k->getX() < post.rightBottom.x + postBuff) {
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
// EXAMINED: change these constants
int ObjectFragments::characterizeSize(blob b) {
    int w = b.rightTop.x - b.leftTop.x + 1;
    int h = b.leftBottom.y - b.leftTop.y + 1;
    const int largePostHeight = 30;
    const int smallPostHeight = 15;
    const int smallPostWidth = 10;
    const int midPostHeight = 30;
    const int midPostWidth = 15;
    if (h > largePostHeight) return LARGE;
    if (h < smallPostHeight || w < smallPostWidth) return SMALL;
    if (h < midPostHeight || w < midPostWidth) return MEDIUM;
    return LARGE;
}

/* Sets a standard of proof for a post.  In this case that the blob comprising
 * the post is at least 60% correct color.
 * Note:  this is actually not the greatest idea when the angle of the head is
 * significantly off horizontal.
 *
 * @param b   the post in question
 * @return    a constant indicating size - SMALL, MEDIUM, or LARGE
 */

bool ObjectFragments::qualityPost(blob b, int c)
{
    const int num = 3;
	const int denom = 5;

    int good = 0;
    //bool soFar;
    for (int i = b.leftTop.x; i < b.rightTop.x; i++)
        for (int j = b.leftTop.y; j < b.leftBottom.y; j++)
            if (thresh->thresholded[j][i] == c)
                good++;
    if (good < blobArea(b) * num / denom) return false;
    return true;
}

/* Provides a kind of sanity check on the size of the post.  Essentially we are
 * looking for cases where we don't have a post, but are looking at a backstop.
 * Also just let's us know how good the size estimate is.
 * This needs lots of beefing up.
 *
 * @param b   the post in question
 * @return    a constant indicating size - SMALL, MEDIUM, or LARGE
 */

bool ObjectFragments::checkSize(blob b, int c)
{
    const int horzScanParam = 6;
	const int topBuff = 10;
	const int truthMax = 5;

    int midY = b.leftTop.y + (b.leftBottom.y - b.leftTop.y) / 2;
    horizontalScan(b.leftTop.x, midY, -1,  horzScanParam, c, c, 0, b.leftTop.x + 1);
    //drawPoint(scan.x, scan.y, RED);
    int leftMid = scan.good;
    horizontalScan(b.rightTop.x, midY, 1, horzScanParam, c, c, b.rightTop.x - 1,
                   b.rightTop.x + topBuff);
    //drawPoint(scan.x, scan.y, RED);
    if (leftMid > truthMax && scan.good > truthMax) return false;
    return true;
}

/* Shooting stuff */

/* Determines shooting information. Basically scans down from backstop and looks
 * for occlusions.
 * Sets the information found within the backstop data structure.
 * @param one the backstop
 */

void ObjectFragments::setShot(VisualCrossbar* one)
{
    const int intersectNum = 3;
    const int crossNum = 3;
    const int stripMin = 5;
    const int intersectMax = 10;
    const int greyMax = 15;
    const int runMax = 10;
    const int spanDiv = 5;

    int pix, bad, white, grey, run, greyrun;
    int ySpan = IMAGE_HEIGHT - one->getLeftBottomY();
    bool colorSeen = false;
    int lx = one->getLeftTopX(), ly = one->getLeftTopY(),
        rx = one->getRightTopX(), ry = one->getRightTopY();
    int bx = one->getLeftBottomX(), brx = one->getRightBottomX();
    int intersections[intersectNum];
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
                if (crossings == crossNum) {
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
                if (strip > stripMin)
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
                        if (intersections[k] - j < intersectMax &&
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
            if (greyrun > greyMax) {
                //shoot[i] = false;
                if (BACKDEBUG) {
                    //drawPoint(i, j, RED);
                }
            }
            if (run > runMax && (pix == NAVY || pix == RED)) {
                shoot[i] = false;
                if (BACKDEBUG)
                    drawPoint(i, j, RED);
            }
            if (run > runMax) {
                shoot[i] = false;
                if (BACKDEBUG) {
                    drawPoint(i, j, RED);
                }
            }
        }
        if (bad > ySpan / spanDiv) {
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
        cout << "Crossbar info: Left Col: " << r1 << " Right Col: " << r2
             << " Dir: " << one->getBackDir();
        if (one->shotAvailable())
            cout << " Take the shot!" << endl;
        else
            cout << " Don't shoot!" << endl;
    }
}


void ObjectFragments::bestShot(VisualFieldObject* left,
                               VisualFieldObject* right,
                               VisualCrossbar* middle)
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
	setShot(middle);
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
void ObjectFragments::openDirection(int horizon, NaoPose *pose)
{
    const int intersectNum = 5;
    const int divider = 10;
	const int bound = 60;
	const int crossMax = 5;
	const int interUpper = 10;
	const int greyMax = 15;
	const int yBuff = 15;
	const int runBuff = 10;
	const int runMax = 10;
	const int badMax = 3;
	const int jumpMax = 11;
	const int imgBuff= 10;
	const int horzBuff = 20;
	const int downMax = 10;
	const int openBuff = 10;
	const int imgDiv = 15;
	const int widthDiv = 6;
	const int lineDiv = 3;

    int pix, bad, white, grey, run, greyrun;
    int intersections[intersectNum];
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
        if (d.dist > bound && lastd < bound) {
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
                if (crossings == crossMax) {
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
                        if (intersections[k] - y < interUpper &&
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
            if (greyrun == greyMax) {
                //shoot[i] = false;
                if (open[(int)x / divider] == horizon) {
                    open[(int)x / divider] = y + yBuff;
                }
                open2[(int)x / divider] = y + yBuff;
                //drawPoint(x, y, RED);
                //drawPoint(x - 1, y, RED);
                //drawPoint(x + 1, y, RED);
                y = 0;
            }
            if (run == runBuff) {
                if (open[(int)x / divider] == horizon) {
                    open[(int)x / divider] = y + runBuff;
                }
                if (bad == runBuff) {
                    open2[(int)x / divider] = y + runBuff;
                    y = 0;
                }
                //drawPoint(x, y, RED);
                //drawPoint(x - 1, y, RED);
                //drawPoint(x + 1, y, RED);
            }
            if (run > runMax && (bad > badMax || y < sixty)) {
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
        if (i - jumpdown < jumpMax && !vert && open[i] > IMAGE_HEIGHT - imgBuff
            && jumpdown != -1) {
            vert = true;
        }
        if (open[i] > horizon + horzBuff &&  lastone < horizon + horzBuff && i != 0) {
            jumpdown = i;
            vert = false;
        }
        if (vert && lastone > horizon + horzBuff  && open[i] < horizon + horzBuff) {
            //cout << "Testing for vertical " << jumpdown << " " << i << endl;
            if (i - jumpdown < downMax && jumpdown != -1) {
                point<int> midTop(jumpdown * divider,IMAGE_HEIGHT - horizon /2);
                point<int> midBottom(i * divider,IMAGE_HEIGHT- horizon/2);
                bool intersects = vision->fieldLines->
                    intersectsFieldLines(midTop,midBottom);
                if (intersects) {
                    if (OPENFIELD) {
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
        if (open[i] - openBuff <= horizon) {
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
    for (i = 1; i < IMAGE_WIDTH / imgDiv; i++) {
        if (open[i] > open[index1]) {
            index1 = i;
        }
        if (open2[i] > open2[index12]) {
            index12 = i;
        }
    }
    index2 = i; index22 = i;
    for (i++ ; i < 2 * IMAGE_WIDTH / imgDiv; i++) {
        if (open[i] > open[index2]) {
            index2 = i;
        }
        if (open2[i] > open2[index22]) {
            index22 = i;
        }
    }
    index3 = i; index32 = i;
    for (i++ ; i < (IMAGE_WIDTH / divider) ; i++) {
        if (open[i] > open[index3]) {
            index3 = i;
        }
        if (open2[i] > open2[index32]) {
            index32 = i;
        }
    }
    // All distance estimates are to the HARD values
    estimate e;
    e = pose->pixEstimate(IMAGE_WIDTH/widthDiv, open2[index12], 0.0);
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

    e = pose->pixEstimate(divider*IMAGE_WIDTH/widthDiv, open2[index32],0.0);
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
        drawLine(0, open2[index12], IMAGE_WIDTH / lineDiv, open2[index12], MAROON);
        drawLine(IMAGE_WIDTH / lineDiv, open2[index22], 2 * IMAGE_WIDTH / lineDiv,
                 open2[index22], MAROON);
        drawLine(2 * IMAGE_WIDTH / lineDiv, open2[index32], IMAGE_WIDTH  - 1,
                 open2[index32], MAROON);
        drawLine(0, open2[index12] - 1, IMAGE_WIDTH / lineDiv, open2[index12] - 1,
                 MAROON);
        drawLine(IMAGE_WIDTH / lineDiv, open2[index22] - 1, 2 * IMAGE_WIDTH / lineDiv,
                 open2[index22] - 1, MAROON);
        drawLine(2 * IMAGE_WIDTH / lineDiv, open2[index32] - 1, IMAGE_WIDTH  - 1,
                 open2[index32] - 1, MAROON);
        if (open2[index12] != open2[index22]) {
            drawLine(IMAGE_WIDTH / lineDiv, open2[index12], IMAGE_WIDTH / lineDiv,
                     open2[index22], MAROON);
        }
        if (open2[index32] != open2[index22]) {
            drawLine(2 * IMAGE_WIDTH / lineDiv, open2[index32], 2 * IMAGE_WIDTH / lineDiv,
                     open2[index22], MAROON);
        }
        if (open2[index12] <  open2[index22] &&
            open2[index12] < open2[index32]) {
            for (i = IMAGE_WIDTH / lineDiv; open[i / divider] <= open2[index12]; i++){
            }
            drawMore(i, open2[index12], PINK);
        }
        else if (open2[index22] <  open2[index12] &&
                 open2[index22] < open2[index32]) {
            for (i = IMAGE_WIDTH / lineDiv; open[i / divider] <= open2[index22]; i--){
            }
            drawLess(i, open2[index22], PINK);
            for (i = 2 * IMAGE_WIDTH / lineDiv; open[i / divider] <= open2[index22];
                 i++) {}
            drawMore(i, open2[index22], PINK);
        }
        else if (open2[index32] <  open2[index22] &&
                 open2[index32] < open2[index12]) {
            for (i = 2 * IMAGE_WIDTH / lineDiv; open[i / divider] <= open2[index32];
                 i--) {}
            drawLess(i, open2[index32], PINK);
        }
        else if (open2[index22] ==  open2[index12] &&
                 open2[index22] < open2[index32]) {
            for (i = 2 * IMAGE_WIDTH / lineDiv; open[i / divider] <= open2[index22];
                 i++) {}
            drawMore(i, open2[index22], PINK);
        }
        else if (open2[index22] < open2[index12] &&
                 open2[index22] == open2[index32]) {
            for (i = IMAGE_WIDTH / lineDiv; open[i / divider] <= open2[index22]; i--){
            }
            drawLess(i, open2[index22], PINK);
        } else if (open2[index12] < open2[index22] &&
                   open2[index12] == open2[index32]) {
            // vertical line?
            cout << "Vertical line?" << endl;
        }
        // drawMore(longIndex * divider + longsize * divider + divider, horizon,
        //          PINK);
        cout << "Estimate soft is " << open[index1] << " " << open[index2]
             << " " << open[index3] << endl;
        cout << "Estimate hard is " << open2[index12] << " " << open2[index22]
             << " " << open2[index32] << endl;
    }
}

/* Try and find the biggest post left on the screen.  We start by looking for
 * our longest "run" of the current color.
 * We then call squareGoal to expand that into a post.  Later
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
    // Try and figure out the true axis-parallel post dimensions - we're going
    // to try and start right in the middle
    int startX = maxX;
    int startY = maxY + maxRun / 2;
    // starts a scan in the middle of the tallest run.
    squareGoal(startX, startY, c, c2);
    // make sure we're looking at something big enough to be a post
    if (!postBigEnough(obj)) {
        return NOPOST;
    }
    transferToPole();
    return LEFT; // Just return something other than NOPOST
}


/* Another post classification method.  In this one we look left and right of
 * the post trying to find a really long run of the same color.  If we find one
 * this is good evidence since it
 *
 * may correspond to the matching post of this color.
 * @param left       leftmost value (x dim) of the post
 * @param right      rightmost value
 * @param height     the height of the post
 * @return           potential classification
 */

int ObjectFragments::checkOther(int left, int right, int height, int horizon)
{
    const int nextDiff = 10;
	const int largeMax = 20;

    int largel = 0;
    int larger = 0;
    //int mind = max(MIN_POST_SEPARATION, height / 2);
	int mind = MIN_POST_SEPARATION;
    for (int i = 0; i < numberOfRuns; i++) {
        int nextX = runs[i].x;
        int nextY = runs[i].y;
        int nextH = runs[i].h;
        //int nextB = nextY + nextH;
        // meanwhile collect some information on which post we're looking at
        if (nextH > 0) {
            if (nextX < left - mind && nextH > MIN_GOAL_HEIGHT &&
                nextY < horizonAt(nextX) &&
                nextY + nextH > horizonAt(nextX) - nextDiff) {
                if (nextH > largel)
                    largel = nextH;
                //drawPoint(nextX, nextY, ORANGE);
                //cout << largel << endl;
            } else if (nextX > right + mind && nextH > MIN_GOAL_HEIGHT &&
                       nextY < horizonAt(nextX) &&
                       nextY + nextH > horizonAt(nextX) - nextDiff) {
                if (nextH > larger) {
                    larger = nextH;
                }
            }
		}
    }
    if ((larger > height / 2 || larger > largeMax) && larger > largel) {
        if (POSTLOGIC)
            cout << "Larger" << endl;
        return LEFT;
    } else if (largel > largeMax || largel > height / 2) {
        if (POSTLOGIC) cout << "Largel" << endl;
        return RIGHT;
    }
	if (POSTLOGIC)
		cout << "Large R " << larger << " " << largel << " " << endl;
    return NOPOST;
}


/* Main routine for classifying posts (Aibos).  We have a variety of methods to
 * classify posts in our tool box.  The idea is to start with the best ones and
 * keep trying until one produces an answer.
 *
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
                                       VisualCrossbar* mid)
{
    const int blobMin = 10;

    // ok now we're going to try and figure out which post it is and where the other one might be
    int trueLeft = min(pole.leftTop.x, pole.leftBottom.x);          // leftmost value in the blob
    int trueRight = max(pole.rightTop.x, pole.rightBottom.x);    // rightmost value in the blob
    int trueTop = min(pole.leftTop.y, pole.rightTop.y);             // topmost value in the blob
    int trueBottom = max(pole.leftBottom.y, pole.rightBottom.y); // bottommost value in the blob
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
            > blobMin && locationOk(topBlob, horizon) && greenCheck(topBlob)) {
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

    // post = crossCheck2(pole);        // look for the crossbar
    // if (post != NOPOST) {
    //     if (POSTLOGIC) {
    //         cout << "Found crossbar2 " << post << endl;
    //     }
    //     return post;
    // }

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
                               bool beaconFound, int horizon)
{
    const int imgDiff = 3;
	const int trueBuff = 10;
	const int postBuff = 5;
	const int topMax = 10;

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
    } else if (pole.rightTop.x - pole.leftTop.x > IMAGE_WIDTH - imgDiff) {
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
        if ((nextX < trueLeft && post == LEFT) ||
            (nextX > trueRight && post == RIGHT)) {
            runs[i].h = 0;
        }
        if ( (nextX > trueLeft - trueBuff && post == RIGHT) ||
			 (nextX < trueRight + trueBuff && post == LEFT) ) {
            runs[i].h = 0;
        }
    }
    // find the other post if possible - the process is basically identical to
    // the first post
    point <int> leftP = pole.leftTop;
    point <int> rightP = pole.rightTop;
    int trueLeft2 = 0;
    int trueRight2 = 0;
    int trueBottom2 = 0;
    int trueTop2 = 0;
    int second = 0;
    int spanX2 = 0, spanY2 = 0;
    isItAPost = grabPost(c, c2, horizon, trueLeft - postBuff, trueRight + postBuff);
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
                if (secondPostFarEnough(leftP, rightP,
                                        pole.leftTop, pole.rightTop, post)) {
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
                if (abs(trueTop - trueTop2) < topMax && qualityPost(pole, c)) {
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
float ObjectFragments::rightHalfColor(blob tempobj)
{
    const float maxColor = 0.15f;
	const float colorNum = 0.10f;

    int x = tempobj.leftTop.x;
    int y = tempobj.leftTop.y;
    int spanY = tempobj.leftBottom.y - y;
    int spanX = tempobj.rightTop.x - x;
    int good = 0, good1 = 0, good2 = 0;
    int pix;
    if (rightColor(tempobj, ORANGE) < maxColor) return colorNum;
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

float ObjectFragments::rightColor(blob tempobj, int col)
{
    const int blobMin = 1000;
	const float redMult = 0.10f;
	const float ogoodMult = 0.20f;
	const float oygoodMult = 0.40f;
	const float goodMult = 0.65f;

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
            if (y + i > -1 && x + j > -1 && (y + i) < IMAGE_HEIGHT &&
                x + j < IMAGE_WIDTH && (pix == ORANGE || pix == ORANGERED ||
                                        pix == ORANGEYELLOW)) {
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
        cout << "Orange " << ogood << " " << orgood << " " << red << " "
             << blobArea(tempobj) << endl;
    }
    if (blobArea(tempobj) > blobMin) return (float) good /
                                      (float) blobArea(tempobj);
    //if (ogood < 2 * orgood) return 0.1; // at least two thirds of the "orange" pixels should be orange
    if (red > static_cast<float>(spanX * spanY) * redMult) {
		if (BALLDEBUG)
			cout << "Too much red" << endl;
		// before giving up let's try and salvage this one
		if (ogood < static_cast<float>(spanX * spanY) * ogoodMult)
			return redMult;
		if (greenCheck(tempobj) && greenSide(tempobj) && roundness(tempobj) != BADVALUE) {
			return goodMult;
		}
		return redMult;
	}
    /*if (ogood < static_cast<float>(spanX * spanY) * ogoodMult) {
		if (BALLDEBUG)
			cout << "Not enough pure orange" << endl;
		return redMult;
		}*/
    if (tempobj.area > blobMin &&
		ogood + oygood > (static_cast<float>(spanX * spanY) * oygoodMult)  &&
		good < ( static_cast<float>(spanX * spanY) * goodMult)) return goodMult;
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
bool ObjectFragments::greenCheck(blob b)
{
    const int scanParam = 5;
	const int iMax = 10;
	const int badMax = 4;

    if (b.rightBottom.y >= IMAGE_HEIGHT - 1 || b.leftBottom.y >= IMAGE_HEIGHT - 1) return true;
    if (b.rightTop.x - b.leftTop.x > IMAGE_WIDTH / 2) return true;
    int w = b.rightBottom.x - b.leftBottom.x + 1;
    int y = 0;
    int x = b.leftBottom.x;
    for (int i = 0; i < w; i+= 2) {
        y = yProject(x, b.leftBottom.y, x + i);
        vertScan(x + i, y, 1, scanParam, GREEN, GREEN);
        if (scan.good > 1)
            return true;
    }
    // try one more in case its a white line
    int bad = 0;
    for (int i = 0; i < iMax && bad < badMax; i++) {
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
bool ObjectFragments::greenSide(blob b)
{
    const int scanParam = 5;
	const int xDiff = 8;

    int x = b.rightBottom.x;
    int y = b.rightBottom.y;
    for (int i = y; i > (b.rightBottom.y - b.rightTop.y) / 2; i = i - 2) {
        horizontalScan(x, i, 1, scanParam, GREEN, GREEN, x - 1, x + xDiff);
        if (scan.good > 0)
            return true;
    }
    x = b.leftBottom.x;
    y = b.leftBottom.y;
    for (int i = y; i > (b.leftBottom.y - b.leftTop.y) / 2; i = i - 2) {
        horizontalScan(x, i, -1, scanParam, GREEN, GREEN, x - xDiff, x + 1);
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
        addPoint(static_cast<float>(lastGoodX),
				 static_cast<float>(lastGoodY) );
        return 0;
    }

}

/*  It probably goes without saying that the ideal ball is round.  So let's see how round our
 * current candidate is.  Among other things we check its heigh/width ratio (should be about 1)
 * and where the orange is (shouldn't be in the corners, should be in the middle)
 * @param b      the candidate ball
 * @return       a constant result - BADVALUE, or 0 for round
 */

int  ObjectFragments::roundness(blob b)
{
    const int imgDiff = 3;
	const int sizeMin = 4;
	const int sizeMax = 20;
	const int widthNum = 15;
	const float ratioMult = 10.0f;
	const float maxDiv = 6.0f;

    int w = blobWidth(b);
    int h = blobHeight(b);
    int x = b.leftTop.x;
    int y = b.leftTop.y;
    float ratio = static_cast<float>(w) / static_cast<float>(h);
    int r = 10;
	//cout << "Height is " << h << " width is " << w << endl;
	
    if ((h < SMALLBALLDIM && w < SMALLBALLDIM && ratio > BALLTOOTHIN && ratio < BALLTOOFAT)) {
    } else if (ratio > THINBALL && ratio < FATBALL) {
    } else if (y + h > IMAGE_HEIGHT - imgDiff || x == 0 || (x + w) > IMAGE_WIDTH - 2 || y == 0) {
        if (BALLDEBUG)
            cout << "Checking ratio on occluded ball:  " << ratio << endl;
        // we're on an edge so allow for streching - first check for top of bottom
        if (h > sizeMin && w > sizeMin && (y + h > IMAGE_HEIGHT - 2 || y == 0) &&
			ratio < MIDFAT && ratio > 1) {
            // then sides
        } else if (h > sizeMin && w > sizeMin
				   && (x == 0 || x + w > IMAGE_WIDTH - 2)
				   && ratio > MIDTHIN && ratio < 1) {
        } else if ((h > sizeMax || w > sizeMax)
				   && (ratio > OCCLUDEDTHIN && ratio < OCCLUDEDFAT) ) {
            // when we have big slivers then allow for extra
        } else if (b.leftBottom.y > IMAGE_HEIGHT - imgDiff && w > widthNum) {
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
        int offRat = ROUND2((1.0f - ratio) * ratioMult);
        r -= offRat;
    } else {
        int offRat = ROUND2((1.0f - 1.0f/ratio) * ratioMult);
        r -= offRat;
    }
    if (w * h > SMALLBALL) {
        // now make some scans through the blob - horizontal, vertical, and each diagonal
        int pix;
        int goodPix = 0, badPix = 0;
        if (y + h > IMAGE_HEIGHT - imgDiff || x == 0 || (x + w) > IMAGE_WIDTH - 2 || y == 0) {
        } else {
            // we're in the screen
            int d = ROUND2(static_cast<float>(std::max(w, h)) / maxDiv);
            int d3 = min(w, h);
            for (int i = 0; i < d3; i++) {
                pix = thresh->thresholded[y+i][x+i];
                if (i < d || (i > d3 - d)) {
                    if (pix == ORANGE || pix == ORANGERED) {
						//drawPoint(x+i, y+i, BLACK);
                        badPix++;
					}
                    else
                        goodPix++;
                } else {
					if (pix == ORANGE || pix == ORANGERED || pix == ORANGEYELLOW)
						goodPix++;
					else if (pix != GREY) {
						badPix++;
						//drawPoint(x+i, y+i, PINK);
					}
				}
                pix = thresh->thresholded[y+i][x+w-i];
                if (i < d || (i > d3 - d)) {
                    if (pix == ORANGE || pix == ORANGERED) {
						//drawPoint(x+w-i, y+i, BLACK);
                        badPix++;
					}
                    else
                        goodPix++;
                } else if (pix == ORANGE || pix == ORANGERED || pix == ORANGEYELLOW)
                    goodPix++;
                else if (pix != GREY) {
                    badPix++;
					//drawPoint(x+w-i, y+i, BLACK);
				}
            }
			//cout << "here" << endl;
            for (int i = 0; i < h; i++) {
                pix = thresh->thresholded[y+i][x + w/2];
				//drawPoint(x + w/2, y+i, BLACK);
                if (pix == ORANGE || pix == ORANGERED || pix == ORANGEYELLOW) {
                    goodPix++;
                } else if (pix != GREY)
                    badPix++;
            }
        }
        for (int i = 0; i < w; i++) {
            pix = thresh->thresholded[y+h/2][x + i];
			//drawPoint(x+i, y+h/2, BLACK);
            if (pix == ORANGE || pix == ORANGERED || pix == ORANGEYELLOW) {
                goodPix++;
            } else if (pix != GREY)
                badPix++;
        }
        if (BALLDEBUG)
            cout << "Roundness: Good " << goodPix << " " << badPix << endl;
		// if more than 20% or so of our pixels tested are bad, then we toss the ball out
        if (goodPix < badPix * 5) {
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

int ObjectFragments::ballNearGreen(blob b)
{
    const int jMax = 6;

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
        for (int j =-1; j < jMax && x + j > -1 && where % GREENLEFT != 0; j++) {
            if (thresh->thresholded[i+y][x - j] == GREEN) {
                where = where * GREENLEFT;
            }
        }
    }
    for (int i = 0; i < w && x + i < IMAGE_WIDTH && where % GREENABOVE != 0; i= i+2) {
        for (int j = 0; j < jMax && y - j > 0 && where % GREENABOVE != 0; j++) {
            if (thresh->thresholded[i+y][j+x] == GREEN) {
                where = where * GREENABOVE;
            }
        }
    }

    x = b.rightTop.x;
    y = b.rightTop.y;
    for (int i = 0; i < h && y + i < IMAGE_HEIGHT && where % GREENRIGHT != 0; i= i+2) {
        for (int j = 0; j < jMax && x + j < IMAGE_WIDTH && where % GREENRIGHT != 0; j++) {
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
    static const int SURROUND = 12;
	
	const int greenDiv = 10;

    int x = b.leftTop.x;
    int y = b.leftTop.y;
    int w = b.rightTop.x - b.leftTop.x + 1;
    int h = b.rightBottom.y - b.leftTop.y + 1;
	int surround = min(SURROUND, w/2);
    int greens = 0, orange = 0, red = 0, borange = 0, pix, realred = 0;
    for (int i = -1; i < w+1; i++) {
        for (int j = -1; j < h+1; j++) {
			if (x + i > -1 && x + i < IMAGE_WIDTH && y + j > -1 &&
				y + j < IMAGE_HEIGHT) {
				pix = thresh->thresholded[y + j][x + i];
				if (pix == ORANGE)
					borange++;
			}
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
            else if (pix == RED)
                realred++;
			else if (pix == ORANGERED)
				red++;
            else if (pix == GREEN)
                greens++;
        }
    }
    if (BALLDEBUG) {
        cout << "Surround information " << red << " " << realred << " " 
			 << orange << " " << borange << " " << greens << endl;
    }
	if (orange - borange > borange * 0.3 && orange - borange > 10) {
		if (BALLDEBUG) {
			cout << "Too much orange outside of the ball" << endl;
		}
		return true;
	}
    if (realred > borange) {
		if (BALLDEBUG) {
			cout << "Too much real red" << endl;
		}
		return true;
	}
    if (realred > greens) {
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
	if (red > orange && greens < (w * h) / greenDiv) {
		if (BALLDEBUG) {
			cout << "Too much real orangered without enough green" << endl;
		}
		return true;
	}
	if (red > orange)  {
		if (BALLDEBUG) {
			cout << "Too much real red - doing more checking" << endl;
		}
		x = b.leftTop.x;
		y = b.leftBottom.y;
		if ((x < 1 || x + w > IMAGE_WIDTH - 2) && y  > IMAGE_HEIGHT - 2) {
			if (BALLDEBUG) {
				cout << "Dangerous corner location detected " << x << " " << y <<  endl;
			}
			return true;
		}
		return roundness(b) == BADVALUE;
	}
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
int ObjectFragments::balls(int horizon, VisualBall *thisBall)
{
    const int blobMin = 3;
	const int arMin = 35;
	const int arMax = 1000;
	const int heightDiv = 3;
	const int imgBuff = 3;
	const int numMin = 5;
	const int horbBuff = 30;
	const float roundParam = 0.85f;
	const float roundMult = 10.0f;
	const int confAdjust = 3;
	const int blobLow = 75;
	const int blobMid = 150;
	const int blobHigh = 250;
	const int blobMax = 500;
	const float radDiv = 2.0f;
	const float PIXACC = 300;

    int confidence = 10;
    occlusion = NOOCCLUSION;
	static const int MAXDIAM = 100;
    if (numberOfRuns > 1) {
        for (int i = 0; i < numberOfRuns; i++) {
            // search for contiguous blocks
            int nextX = runs[i].x;
            int nextY = runs[i].y;
            int nextH = runs[i].h;
            blobIt(nextX, nextY, nextH);
        }
    }
	// when we have red uniforms, sometimes we get tons of orange blobs
	// and sometimes they are inside of each other
	if (numBlobs > blobMin) {
		int big = 0, bigArea = blobArea(blobs[0]);
		for (int i = 1; i < numBlobs; i++) {
			if (blobArea(blobs[i]) > bigArea) {
				big = i;
				bigArea = blobArea(blobs[i]);
			}
		}
		for (int i = 0; i < numBlobs; i++) {
			if (i != big && blobs[i].rightTop.x > blobs[big].leftTop.x &&
				blobs[i].leftTop.x < blobs[big].rightTop.x &&
				blobs[i].leftBottom.y > blobs[big].leftTop.y &&
				blobs[i].leftTop.y < blobs[big].leftBottom.y) {
				if (BALLDEBUG) {
					cout << "Screening an inner ball" << endl;
					drawBlob(blobs[i], WHITE);
				}
				blobs[i].area = 0;
			}
		}
	}
    // pre-screen blobs that don't meet our criteria
    //cout << "horizon " << horizon << " " << slope << endl;
    for (int i = 0; i < numBlobs; i++) {
        int ar = blobArea(blobs[i]);
        float perc = rightColor(blobs[i], ORANGE);
        int diam = max(blobWidth(blobs[i]), blobHeight(blobs[i]));
		if (blobs[i].area > 0) {
			if (blobs[i].leftBottom.y + diam < horizonAt(blobs[i].leftTop.x)) {
				blobs[i].area = 0;
				if (BALLDEBUG) {
					cout << "Screened one for horizon problems " << endl;
					drawBlob(blobs[i], WHITE);
				}
			} else if (diam > MAXDIAM) {
				if (blobWidth(blobs[i]) > MAXDIAM) {
					if (diam < MAXDIAM + 20) {
						// Try trimming the ball
						int lefty = diam / 2, righty = diam / 2, pix;
						// scan in from the sides and see where we see orange faster - trim other side
						for (int j = blobs[i].leftTop.x; j < blobs[i].leftTop.x + diam / 2; j++) {
							pix = thresh->thresholded[blobs[i].leftTop.y + 20][j];
							if (pix == ORANGE || pix == ORANGEYELLOW || pix == ORANGERED) {
								lefty = j - blobs[i].leftTop.x;
								j = IMAGE_WIDTH;
							}
						}
						for (int j = blobs[i].rightTop.x; j > blobs[i].rightTop.x - diam / 2; j--) {
							pix = thresh->thresholded[blobs[i].leftTop.y + 20][j];
							if (pix == ORANGE || pix == ORANGEYELLOW || pix == ORANGERED) {
								righty = blobs[i].leftTop.x - j;
								j = 0;
							}
						}
						if (lefty < righty) {
							// the right side is too wide
							blobs[i].rightTop.x = blobs[i].leftTop.x + MAXDIAM;
							blobs[i].rightBottom.x = blobs[i].rightTop.x;
						} else {
							blobs[i].leftTop.x = blobs[i].rightTop.x - MAXDIAM;
							blobs[i].leftBottom.x = blobs[i].leftTop.x;
						}
						if (blobHeight(blobs[i]) > MAXDIAM) {
							blobs[i].leftBottom.y = blobs[i].leftTop.y + MAXDIAM;
							blobs[i].rightBottom.y = blobs[i].leftBottom.y;
						}
					} else {
						blobs[i].area = 0;
						if (BALLDEBUG) {
							cout << "Screened one that was too big " << diam << endl;
							drawBlob(blobs[i], NAVY);
						}
					}
				} else {
					// Hacktacular:  trim the height to equal the width
					int newHeight = blobWidth(blobs[i]);
					blobs[i].leftBottom.y = blobs[i].leftTop.y + newHeight;
					blobs[i].rightBottom.y = blobs[i].leftBottom.y;
				}
			} else if (ar > arMin && perc > MINORANGEPERCENT) {
				// don't do anything
			} else if (ar > arMax && rightHalfColor(blobs[i]) > MINORANGEPERCENT) {
				//} else if (perc > 0.25f && redBallCheck()) {
			} else {
				if (BALLDEBUG) {
					drawBlob(blobs[i], BLACK);
					cout << "Screened one for not being orange enough" << endl;
				}
				blobs[i].area = 0;
			}
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
    const float BALL_REAL_HEIGHT = 8.6f;
    e = vision->pose->pixEstimate(topBlob.leftTop.x + blobWidth(topBlob) / 2, 
								  topBlob.leftTop.y + 2 * blobHeight(topBlob) / heightDiv, BALL_REAL_HEIGHT);

    //cout << "Estimated distance is " << e.dist << endl;
    if (BALLDEBUG) {
        if (topBlob.leftTop.x > 0) {
            cout << "Vision found ball " << endl;
            printBlob(topBlob);
            cout << topBlob.leftTop.x << " " << topBlob.leftTop.y << " " << w << " " << h << endl;
        }
	
    }
    // check for obvious occlusions
    if (topBlob.leftBottom.y > IMAGE_HEIGHT - imgBuff) {
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
    //int whereIsGreen = ballNearGreen(topBlob);
    int horb = horizonAt(topBlob.leftBottom.x);

    //look for edge points!
    int NUM_EDGE_POINTS = 20;
    int cenX = midPoint(topBlob.leftTop.x, topBlob.rightBottom.x);
    int cenY = midPoint(topBlob.leftTop.y, topBlob.leftBottom.y);

    /*for(float angle = 0; angle < M_PI_FLOAT;
		angle += M_PI_FLOAT / static_cast<float>(NUM_EDGE_POINTS) ){

        scanOut(cenX,cenY,tan(angle), 1);
        scanOut(cenX,cenY,tan(angle), -1);
		}*/

    //if (w < SMALLBALLDIM || h < SMALLBALLDIM || numBlobs > numMin) {
        if (badSurround(topBlob)) {
            if (BALLDEBUG) {
                drawBlob(topBlob, BLACK);
                cout << "Screening for lack of green and bad surround" << endl;
            }
            return 0;
        }
		//}

    if (w < SMALLBALLDIM || h < SMALLBALLDIM) {
        // small balls should be near the horizon - this check makes extra sure
        if (topBlob.leftBottom.y > horb + horbBuff) {
            if (BALLDEBUG)
                cout << "Screening small ball for horizon" << endl;
            return 0;
        }
        if (BALLDEBUG)
            cout << "Small ball passed green and horizon tests" << endl;
    }
    float colPer = rightColor(topBlob, ORANGE);
	

    confidence -= ROUND2((roundParam - colPer) * roundMult);
    //cout << (ROUND2((0.85 - colPer) * 10)) << " " << confidence << endl;
    if (topBlob.area < blobLow) {
        confidence -= confAdjust;
    } else if (topBlob.area < blobMid) {
        confidence -= 1;
    } else if (topBlob.area > blobMax) {
        confidence += 2;
    } else if (topBlob.area > blobHigh) {
        confidence += 1;
    }
    if (BALLDEBUG) {
        printBall(topBlob, confidence, colPer, occlusion);
    }

    // SORT OUT BALL INFORMATION
    // start out by figuring out whether we're using blobs or inferred information
    //float rat = (float) w / (float) h;
    // x, y, width, and height. Not up for debate.
    thisBall->setX(topBlob.leftTop.x);
    thisBall->setY(topBlob.leftTop.y);
	
    thisBall->setWidth( static_cast<float>(w) );
    thisBall->setHeight( static_cast<float>(h) );
    thisBall->setRadius( std::max(static_cast<float>(w)/radDiv,
								  static_cast<float>(h)/radDiv ) );
    int amount = h / 2;
    if (w > h)
        amount = w / 2;

    if (occlusion == LEFTOCCLUSION) {
        thisBall->setCenterX(topBlob.rightTop.x - amount);
        thisBall->setX(topBlob.rightTop.x - amount * 2);
    } else {
        thisBall->setCenterX(topBlob.leftTop.x + amount);
    }
    if (occlusion != TOPOCCLUSION) {
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
    if ((e.dist * 2 < thisBall->getDistance() || thisBall->getDistance() * 2 < e.dist) &&
	e.dist < PIXACC && e.dist > 0) {
      if (BALLDEBUG) {
	cout << "Screening due to distance mismatch " << e.dist << " " << thisBall->getDistance() << endl;
      }
      thisBall->init();
      return 0;
    }
    // sometimes when we're close to the ball we catch reflections off the tape or posts
    if (thisBall->getDistance() < 75.0f && abs(h - w) > 3) { // && thisBall->getDistance() < e.dist - 5.0f) {
      // we probably have misidentified the distance see if we can fix it.
      if (BALLDISTDEBUG) {
	cout << "Detected bad ball distance - trying to fix " << w << " " << h << endl;
      }
      if (h > w) {
	// scan the sides to find the real sides
	int count = -2;
	if (topBlob.rightTop.x - h > 0) {
	  for (int i = topBlob.rightTop.x - h; i < IMAGE_WIDTH - 1; i++) {
	    for (int j = topBlob.leftTop.y; j < topBlob.leftBottom.y; j++) {
	      if (thresh->thresholded[j][i] == ORANGE) {
		topBlob.rightTop.x = i;
		j = IMAGE_HEIGHT;
		i = IMAGE_WIDTH;
	      }
	    }
	    count++;
	  }
	}
	if (topBlob.leftTop.x + h < IMAGE_WIDTH) {
	  for (int i = topBlob.leftTop.x + h; i > -1; i--) {
	    for (int j = topBlob.leftTop.y; j < topBlob.leftBottom.y; j++) {
	      if (thresh->thresholded[j][i] == ORANGE) {
		topBlob.rightTop.x = i;
		j = IMAGE_HEIGHT;
		i = -1;
	      }
	    }
	    count++;
	  }
	}
	cout << "Count is " << count << endl;
	if (count > 1) {
	}
      } else {
      }
      thisBall->setDistanceEst(e);
    }
    if (atBoundary(topBlob)) {
        // INFERRED MEASUREMENTS
        //estimate es;
        //es = vision->pose->pixEstimate(blobs[i].leftTop.x + blobWidth(blobs[i]) / 2, blobs[i].leftTop.y + 2
		//	   * blobHeight(blobs[i]) / 3, 0.0);
        //int dist = (int)es.dist;
        //thisBall->setConfidence(MILDLYSURE);
    }
    if (BALLDISTDEBUG) {
        estimate es;
        es = vision->pose->pixEstimate(topBlob.leftTop.x + blobWidth(topBlob) / 2, topBlob.leftTop.y + 2
			 * blobHeight(topBlob) / heightDiv, 8.0);
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

void ObjectFragments::screenCrossbar()
{
    const int blobMax = 100;

    //check each blob in the array
    for (int i = 0; i < numBlobs; i++) {
        for (int j = 0; j < numBlobs; j++) {
            if (i != j && distance(blobs[i].leftTop.x, blobs[i].rightTop.x, blobs[j].leftTop.x, blobs[j].rightTop.x) < 1) {
                if (blobs[i].leftTop.y < blobs[j].leftTop.y && blobs[j].area > blobMax) {
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

bool ObjectFragments::locationOk(blob b, int hor)
{
    const int horzVal = -50;
	const int minPostSpanY = 55;
	const int spanMax = 5;
	const int bottomMin = 25;
	
    if (!blobOk(b)) {
        if (POSTLOGIC) {
            cout << "Blob not okay on location check" << endl;
        }
        return false;
    }
    if (hor < horzVal) {
        if (POSTLOGIC) {
            cout << "Horizon too high" << endl;
        }
        return false;
    }
    int trueLeft = min(b.leftTop.x, b.leftBottom.x);       // leftmost value in the blob
    int trueRight = max(b.rightTop.x, b.rightBottom.x);    // rightmost value in the blob
    int trueTop = min(b.leftTop.y, b.rightTop.y);          // topmost value in the blob
    int trueBottom = max(b.leftBottom.y, b.rightBottom.y); // bottommost value in teh blob
    int horizonLeft = yProject(0, hor, trueLeft);          // the horizon at the leftmost point
    int horizonRight = yProject(0, hor, trueRight);        // the horizon at the rightmost point
    cout << "Horizon stuff " << horizonLeft << " " << horizonRight << " " << hor << endl;
    //drawPoint(trueLeft, horizonLeft, RED);
    //if (slope < 0) {
    //  horizonLeft = yProject(IMAGE_WIDTH - 1, hor, trueLeft);
    //  horizonRight = yProject(IMAGE_WIDTH - 1, hor, trueRight);
    //}
    //drawPoint(trueLeft, horizonLeft, YELLOW);
    //drawPoint(trueRight, horizonRight, YELLOW);
    int spanX = b.rightTop.x - b.leftTop.x + 1;
    int spanY = b.leftBottom.y - b.leftTop.y;
    int mh = min(horizonLeft, horizonRight);
    if (!horizonBottomOk(spanX, spanY, mh, trueLeft, trueRight, trueBottom,
                         trueTop)) {
        if (!greenCheck(b) || mh - trueBottom > spanY || spanX < spanMax ||
            mh - trueBottom > bottomMin) {
            if (spanY > minPostSpanY) {
	      //return true;
            } else {
	      if (SANITY) {
                cout << "Screening blob for bottom reasons" << endl;
                printBlob(b);
	      }
	      return false;
	    }
        } else {
        }
    }
    //if (trueRight - trueLeft > IMAGE_WIDTH - 10) return true;
    return horizonTopOk(trueTop, max(horizonAt(trueLeft),
                                     horizonAt(trueRight)));
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

bool ObjectFragments::horizonBottomOk(int spanX, int spanY, int minHeight, int left, int right, int bottom, int top)
{
    const int spanYMin = 100;
	const int bottBuff = 5;
	const int param = 20;
	const int leftMax = 10;
	const int imgDiff = 10;
	const int topMax = 5;
	const int spanMax = 15;
	const int scanMin = 5;

    // add a width fudge factor in case the object is occluded - bigger objects will also be taller
    //cout << (bottom + 5) << " " << minHeight << " " << top << " " << spanY << " " << spanX << " " << left << " " << right << endl;
    //int fudge = 20;
    if (spanY > spanYMin) return true;
    if (color == BLUE) {
      //cout << "Bottom info: " << (bottom + bottBuff) << " " << minHeight << " " << endl;
      if (bottom + bottBuff < minHeight) {
	if (SANITY)
	  cout << "Removed risky blue post" << endl;
	return false;
      }
    }
    if (bottom + bottBuff + min(spanX, param) < minHeight) {
        if (SANITY) {
            cout << "Bad height" << endl;
        }
        return false;
    }
    // when we're at the edges of the image make the rules a bit more stringent
    if (bottom + bottBuff < minHeight && (left < leftMax || right > IMAGE_WIDTH - imgDiff || top < topMax) && (spanY < spanMax)) {
        if (SANITY)
            cout << "Bad Edge Information" << endl;
        return false;
    }
    // if we're fudging then make sure there is green somewhere - in other words watch out for occluded beacons
    if (top == 0 && bottom + bottBuff < minHeight) {
        //int x = left + spanX / 2;
        //int y = bottom + 5 + spanX;
        //vertScan(x, y, 1, 3, WHITE, ORANGEYELLOW);
        //drawPoint(x, y, RED);
        if (scan.good > scanMin) {
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
bool ObjectFragments::horizonTopOk(int top, int hor)
{
    const int drawX = 100;

    if (hor <= 0) return false;
    if (top < 1) return true;
    if (top + MIN_GOAL_HEIGHT / 2 > hor) {
        if (SANITY) {
            drawPoint(drawX, top, RED);
            drawPoint(drawX, hor, BLACK);
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

/*  Posts shouldn't show up too close to each other (yes, I realize they can be
 * when you're looking from the side).  Make sure there is some separation.
 *
 * @param l1      left x of one post
 * @param r1      right x of the same post
 * @param l2      left x of the other post
 * @param r2      right x of the other post
 * @return        true when there is enough separation.
 */
bool ObjectFragments::secondPostFarEnough(point <int> left1, point <int> right1,
                                          point <int> left2, point <int> right2,
                                          int post) {
    if (SANITY) {
        cout << "Separations " << (dist(left1.x, left1.y, right2.x, right2.y))
             << " " << (dist(left2.x, left2.y, right1.x, right1.y)) << endl;
    }
    // cout << left1.x << " " << left2.x << " " << right1.x << " "
    //      << right2.x << endl;
    if ((post == RIGHT && right2.x > left1.x) ||
        (post == LEFT && left2.x < right1.x)) {
        if (SANITY) {
            cout << "Second post is on the wrong side!" << endl;
        }
        return false;
    }
    if (dist(left1.x, left1.y, right2.x, right2.y) > MIN_POST_SEPARATION &&
        dist(left2.x, left2.y, right1.x, right1.y) > MIN_POST_SEPARATION) {
        if (dist(left1.x, left1.y, left2.x, left2.y) > MIN_POST_SEPARATION &&
            dist(right2.x, right2.y, right1.x, right1.y) > MIN_POST_SEPARATION){
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

bool ObjectFragments::relativeSizesOk(int spanX, int spanY, int spanX2, int spanY2, int t1, int t2, int fudge)
{
    const int spanMin = 100;
	const int spanMult = 3;
	const int spanDiv = 4;
	const int spanMin2 = 70;
	const int spanDiv2 = 3;
	const int spanDiff = 10;

    if (spanY2 > spanMin) return true;
    if (spanY2 > spanMult * spanY / spanDiv) return true;
    // we need to get the "real" offset
    int f = max(yProject(0, t1, spanY), yProject(IMAGE_WIDTH - 1, t1, IMAGE_WIDTH - spanY));
    if (abs(t1 - t2) > spanMult * min(spanY, spanY2) / spanDiv + f)  {
        if (SANITY) {
            cout << "Bad top offsets" << endl;
        }
        return false;
    }
    if (spanY2 > spanMin2) return true;
    if (spanX2 > 2
		&& (spanY2 > spanY / 2 || spanY2 > BIGPOST ||
			( (spanY2 > spanY / spanDiv2 && spanX2 > spanDiff) &&
			  (spanX2 <= spanX / 2 || fudge != 0)) ) &&
		(spanX2 > spanX / spanDiv))  {
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
    }
	else{}
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


/* Calculate the horizontal distance between two objects
 * (the end of one to the start of the other).
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
    cout << b.leftTop.x << " " << b.leftTop.y << " " << b.rightTop.x << " "
         << b.rightTop.y << endl;
    cout << b.leftBottom.x << " " << b.leftBottom.y << " " << b.rightBottom.x
         << " " << b.rightBottom.y << endl;
#endif
}

/* Prints a bunch of ball information about the best ball candidate (or any one).
 * @param b    the candidate ball
 * @param c    how confident we are its a ball
 * @param p    how many occlusions
 * @param o    what the occlusions are if any
 * @param bg   where around the ball there is green
 */
void ObjectFragments::printBall(blob b, int c, float p, int o) {
#ifdef OFFLINE
    if (BALLDEBUG) {
        cout << "Ball info: " << b.leftTop.x << " " << b.leftTop.y << " "
             << (b.rightTop.x - b.leftTop.x);
        cout << " " << (b.leftBottom.y - b.leftTop.y) << endl;
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
    thresh->drawLine(b.leftTop.x, b.leftTop.y,
                     b.rightTop.x, b.rightTop.y,
                     c);
    thresh->drawLine(b.leftTop.x, b.leftTop.y,
                     b.leftBottom.x, b.leftBottom.y,
                     c);
    thresh->drawLine(b.leftBottom.x, b.leftBottom.y,
                     b.rightBottom.x, b.rightBottom.y,
                     c);
    thresh->drawLine(b.rightTop.x, b.rightTop.y,
                     b.rightBottom.x, b.rightBottom.y,
                     c);
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

void ObjectFragments::drawLess(int x, int y, int c)
{
    const int lineBuff = 10;

#ifdef OFFLINE
    thresh->drawLine(x, y, x + lineBuff, y - lineBuff, c);
    thresh->drawLine(x, y, x + lineBuff, y + lineBuff, c);
    thresh->drawLine(x + 1, y, x + lineBuff + 1, y - lineBuff, c);
    thresh->drawLine(x + 1, y, x + lineBuff + 1, y + lineBuff, c);
#endif
}

void ObjectFragments::drawMore(int x, int y, int c)
{
    const int lineBuff = 10;

#ifdef OFFLINE
    thresh->drawLine(x, y, x - lineBuff, y - lineBuff, c);
    thresh->drawLine(x, y, x - lineBuff, y + lineBuff, c);
    thresh->drawLine(x - 1, y, x - lineBuff - 1, y - lineBuff, c);
    thresh->drawLine(x - 1, y, x - lineBuff - 1, y + lineBuff, c);
#endif
}
