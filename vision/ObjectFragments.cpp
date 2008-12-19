/* Object Fragment class - a ChownDawg production
   There is one of these for each color.  It holds "runs" of pixels that are of that color and vertically connected.
   The idea is to use the runs to recognize objects.  We use the mechanics of the field to help in this process
   in various ways.  In essence this is "run length encoding."  We connect up runs into "blobs" and then filter
   the blobs to try and pick out the most likely objects.
*/

#include <iostream>

#include "ObjectFragments.h"
#include "debug.h"

// redefine abs for ints or doubles
#undef abs
#define abs(x) ((x) < 0 ? -(x) : (x))


using namespace std;

ObjectFragments::ObjectFragments(Vision *vis, Threshold *thr) {
  vision = vis;
  thresh = thr;

  init(0.0);
#ifdef OFFLINE
  BALLDISTDEBUG = false;
  PRINTOBJS = false;
  POSTDEBUG = false;
  POSTLOGIC = false;
  TOPFIND = false;
  BEACONDEBUG = false;
  BALLDEBUG = false;
  CORNERDEBUG = false;
  BACKDEBUG = false;
  SANITY = false;
  DEBUGCIRCLEFIT = false;
  DEBUGBALLPOINTS = false;
  ARCDEBUG = false;
  CORRECT = false;
  OPENFIELD = false;
#endif
}


/* Initialize the data structure.
 * @param s     the slope corresponding to the dog's head tilt
 */
void ObjectFragments::init(double s){
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

void ObjectFragments::setColor(int c) {
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
#ifdef USE_PINK_BALL
  case PINK:
#endif
    runsize = BALL_RUNS_MALLOC_SIZE; //max number of runs
    run_num = runsize * 3;
    break;
  }
  runs = (run*)malloc(sizeof(run) * run_num);
}

/* The first group of methods have to do with blob creation.
 */


/*
  Pseudo-blobbing used for goal recognition.  Since the goals are rectangles we should be able to
  just paste the new runs in to a main blob directly.

  This uses the huge hack that our objects (except the ball) are square - so we can just keep a bounding box

  Basically we are collecting runs.  Everytime we get one we either add it to an existing run or create a new run.
  In theory we can fragment runs this way.  In fact, we should probably check on that.

  @param x        x value of run
  @param y        y value of run
  @param h        height of run

*/
void ObjectFragments::blobIt(int x, int y, int h) {
  // start out deciding to make a new blob
  // the loop will decide not to run it
  bool newBlob = true;
#if ROBOT(NAO)
  int contig = 4;
#elif ROBOT(AIBO)
  int contig = 4;
#endif

  //cout << x << " " << y << " " << h << endl;
  // sanity check: too many blobs on screen
//#if ROBOT(AIBO)
//  if (color == ORANGE) contig--;
//#endif
  if (numBlobs >= MAX_BLOBS) {
    //cout << "Ran out of blob space " << color << endl;
    // We're seeing too many blobs -it is unlikely we can do anything useful with this color
    numBlobs = 0;
    numberOfRuns = 0;
    return;
  } else if (numBlobs > 20) {
    contig = 4;
  }

  // is this run contiguous with any previous blob?
  for (int i = 0; i < numBlobs; i++) {

    // first check: if currentBlob x is greater than blob left and less than a little bit more than the blob right.
    // AND
    // second check: currentBlob y is within fits within current blob
    // OR
    // currentBlob's bottom is within blob and height makes it higher
    if ((x > blobs[i].leftTop.x  && x < blobs[i].rightTop.x + contig) &&
	((y >= blobs[i].leftTop.y - contig && y < blobs[i].leftBottom.y + contig) ||
	 (y < blobs[i].leftTop.y && y+h+contig > blobs[i].leftTop.y))) {

      /* BOUNDING BOX CHECKS -- if current x or y increases the size of the box, do so and keep track of the corresponding x or y value */
      //assign the right, if it is better
      if (x > blobs[i].rightTop.x) {
	//cout << "Setting right " << x << " " << blobs[i].rightTop.x << endl;
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
      int s = (blobs[i].rightTop.x - blobs[i].leftTop.x + 1) *
	(blobs[i].leftBottom.y - blobs[i].leftTop.y + 1); //calculate the area of this blob under consideration
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

/* Find the biggest blob.  Ideally this will end up also merging blobs when they are occluded (e.g. by a dog).
   It may not be necessary though.  After this call the data structure "topBlob" will have the top blob.
   Additionally, the blob "secondBlob" will have the second biggest blob.
   @param maxY     max value (ignored)
*/

void ObjectFragments::getTopAndMerge(int maxY) {
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

void ObjectFragments::getWidest() {
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

void ObjectFragments::zeroTheBlob(int which) {
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

void ObjectFragments::mergeBlobs(int first, int second) {
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
void ObjectFragments::newRun(int x, int y, int h) {
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
    if(color == ORANGE) print("WARNING!!!: INSUFFICIENT MEMORY ALLOCATED TO RUNS OF COLOR ORANGE");
    //cout << "Too many runs " << color << endl;
  }
}


/* Robot recognition methods
 */

#if ROBOT(NAO)

/* Try and recognize robots
 */
void ObjectFragments::robot(int bigGreen) {
  int lastrunx = -30, lastruny = 0, lastrunh = 0;
  for (int i = 0; i < numberOfRuns; i++) {
    //drawPoint(runs[i].x, runs[i].y, BLACK);
    if (runs[i].x < lastrunx + 20) {
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
    // for now we'll use closest y - eventually we should use pixestimated distance
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
  if (BEACONDEBUG) {
    for (int i = 0; i < numBlobs; i++) {
      if (viableRobot(blobs[i])) {
	if (color == RED) {
	  drawRect(topBlob.leftTop.x, topBlob.leftTop.y - 5, topBlob.rightTop.x - topBlob.leftTop.x + 1,
		   topBlob.leftBottom.y - topBlob.leftTop.y + 1, BLACK);
	  drawRect(topBlob.leftTop.x - 1, topBlob.leftTop.y - 1, topBlob.rightTop.x - topBlob.leftTop.x + 3,
		   topBlob.leftBottom.y - topBlob.leftTop.y + 3, BLACK);
	}
	else {
	  drawRect(topBlob.leftTop.x, topBlob.leftTop.y - 5, topBlob.rightTop.x - topBlob.leftTop.x + 1,
		   topBlob.leftBottom.y - topBlob.leftTop.y + 1, ORANGE);
	  drawRect(topBlob.leftTop.x - 1, topBlob.leftTop.y - 1, topBlob.rightTop.x - topBlob.leftTop.x + 3,
		   topBlob.leftBottom.y - topBlob.leftTop.y + 3, ORANGE);
	}
      }
    }
  }
}

void ObjectFragments::expandRobotBlob() {
    //int spany = blobHeight(topBlob);
    //int spanx = blobWidth(topBlob);
  // expand right side
  int x, y;
  int bestr = topBlob.rightTop.x;
  bool good = true;
  for (x = bestr; good && x < IMAGE_WIDTH - 1; x++) {
    good = false;
    for (y = topBlob.rightTop.y; y < topBlob.rightBottom.y && !good; y = y + 2) {
      if (thresh->thresholded[y][x] == color)
	good = true;
    }
  }
  topBlob.rightTop.x = x - 1;
  topBlob.rightBottom.x = x - 1;
  good = true;
  for (x = topBlob.leftTop.x; good && x >  -1; x--) {
    good = false;
    for (y = topBlob.rightTop.y; y < topBlob.rightBottom.y && !good; y = y + 2) {
      if (thresh->thresholded[y][x] == color)
	good = true;
    }
  }
  topBlob.leftTop.x = x + 1;
  topBlob.leftBottom.x = x + 1;
  int whites = IMAGE_WIDTH, pix, width = blobWidth(topBlob) / 4;
  int goods = 0, lastSaw = 0;
  for (y = topBlob.leftBottom.y; whites >= width && y < IMAGE_HEIGHT - 1; y++) {
    whites = 0;
    goods = 0;
    for (x = topBlob.leftBottom.x; x < topBlob.rightTop.x && whites < width; x++) {
      pix = thresh->thresholded[y][x];
      if (pix == color) {
	whites++;
	if (goods > 5) {
	  whites = width;
	}
      } else if (pix == WHITE) {
	whites++;
      } else if ((color ==  NAVY && pix == RED) || (color == RED && pix == NAVY)) {
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
      for (y = topBlob.rightBottom.y - gain; y < topBlob.rightBottom.y && !good; y++) {
	if (thresh->thresholded[y][x] == color)
	  good = true;
      }
    }
    topBlob.rightTop.x = x - 1;
    topBlob.rightBottom.x = x - 1;
    good = true;
    for (x = topBlob.leftTop.x; good && x >  -1; x--) {
      good = false;
      for (y = topBlob.rightBottom.y - gain; y < topBlob.rightBottom.y && !good; y++) {
	if (thresh->thresholded[y][x] == color)
	  good = true;
      }
    }
    topBlob.leftTop.x = x + 1;
    topBlob.leftBottom.x = x + 1;

  }
}

void ObjectFragments::updateRobots(int which, int index) {
  //cout << "Updating robot " << which << " " << color << endl;
  //printBlob(blobs[index]);
  if (color == RED) {
    if (which == 1) {
      updateRobot(vision->red1, blobs[index]);
    } else {
      updateRobot(vision->red2, blobs[index]);
    }
  } else {
    if (which == 1) {
      updateRobot(vision->navy1, blobs[index]);
    } else {
      updateRobot(vision->navy2, blobs[index]);
    }
  }
}

void ObjectFragments::mergeBigBlobs() {
  for (int i = 0; i < numBlobs - 1; i++) {
    for (int j = i+1; j < numBlobs; j++) {
      if (closeEnough(blobs[i], blobs[j]) && bigEnough(blobs[i], blobs[j])) {
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

bool ObjectFragments::closeEnough(blob a, blob b) {
  int xd = distance(a.leftTop.x, a.rightTop.x, b.leftTop.x, b.rightTop.x);
  int yd = distance(a.leftTop.y, a.leftBottom.y, b.leftTop.y, b.rightBottom.y);
  if (xd < 40) {
    if (yd < 40)
      return true;
  }
  //if (xd < max(blobWidth(a), blobWidth(b)) && yd < max(blobHeight(a), blobHeight(b))) return true;
  return false;
}

bool ObjectFragments::bigEnough(blob a, blob b) {
  if (blobArea(a) > 200 && blobArea(b) > 200)
    return true;
  if (a.leftBottom.y > horizonAt(a.leftBottom.x) + 100)
    return true;
  return false;
}

bool ObjectFragments::viableRobot(blob a) {
  // get rid of obviously false ones
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
  if ((double)(whites + col) / (double)blobArea(a) > 0.10)
    return true;
  return false;
}

/* Find robot blobs.
   @param maxY     max value - will be used to pick heads out
*/

void ObjectFragments::getRobots(int maxY) {
  topBlob = zeroBlob;
  int size = 0;
  topSpot = 0;
  //check each blob in the array
  for (int i = 0; i < numBlobs; i++) {
    int spread = blobs[i].rightTop.x - blobs[i].leftTop.x;
    if (blobs[i].leftTop.x == BADVALUE) {
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
    if (blobs[i].leftTop.x != BADVALUE && blobs[i].leftTop.y < maxY && spread > 4) {
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

int ObjectFragments::isRobotCentered(int head, int left, int right) {
  // these to be REAL constants
  int LEFTRANGE = 35;
  int RIGHTRANGE = 100 - LEFTRANGE;
  if ((head - left) < (right - left) * LEFTRANGE / 100)
    return LEFT;
  if ((head - left) > (right - left) * RIGHTRANGE / 100)
    return RIGHT;
  return 0;
}

/* We have a potential robot blob.  Let's try and figure out if it is just one, or really more than one.
 @param which         the blob we're to examine
 */
void ObjectFragments::splitBlob(int which) {
  // eventually these must become real constants - when we can check them on the robot
  int MINROBOTWIDTH = 5;

  // the algorithm starts out pretty simply - scan for heads
  int headX = 0, left = blobs[which].leftTop.x, right = blobs[which].rightTop.x, top = blobs[which].leftTop.y;
  // scan until you hit the right color pixel
#if ROBOT(NAO_SIM)
  robot_console_printf("Examining %d %d %d %d\n", left, right, top, blobs[which].leftBottom.y);
#endif
  for (headX = left; headX <= right && headX < IMAGE_WIDTH && thresh->thresholded[top+1][headX] != color; headX++) {}
  drawPoint(headX, top, BLACK);
  for (int i = headX; i < IMAGE_WIDTH && i < right && thresh->thresholded[top+1][i] == color; i++) {
    if (i % 2 == 0)
      headX++;
  }
  drawPoint(headX, top, BLACK);
  // now we have a candidate head - so where is it within the blob?
  int width = right - left, xdist = headX - left;
  if (isRobotCentered(headX, left, right) == LEFT && xdist > 3 && left > 3 && width - xdist > MINROBOTWIDTH) {
    // probably have two robots - our heuristic is that since this robot is tall it will control the width
    int splitpoint = headX + xdist;
    drawBlob(blobs[which], BLUE);
    drawPoint(splitpoint, 10, BLACK);
    // now try and find the head of the second blob
    int newY, newX = 0;
    bool found = false;
    for (newY = top; newY < IMAGE_HEIGHT && newY < blobs[which].leftBottom.y && !found; newY++) {
      for (newX = splitpoint; newX < right && newX < IMAGE_WIDTH && !found; newX++) {
	if (thresh->thresholded[newY][newX] == color) {
	  found = true;
	}
      }
    }
    newY--;
    // do we really want to split?  Split if: a) the drop isn't too large, b) the new "head" is centered
    if ((newY - top) < xdist && isRobotCentered(newX, splitpoint, right) == 0) {
      blobs[which].rightTop.x = splitpoint;
      blobs[which].rightBottom.x = splitpoint;
#if ROBOT(NAO_SIM)
      robot_console_printf("Splitting %d %d %d\n", left, right, splitpoint);
#endif
      blobs[numBlobs].leftTop.x = splitpoint;
      blobs[numBlobs].leftTop.y = newY;
      blobs[numBlobs].rightTop.x = right;
      blobs[numBlobs].rightTop.y = newY;
      blobs[numBlobs].leftBottom.x = splitpoint;
      blobs[numBlobs].leftBottom.y = blobs[which].leftBottom.y;
      blobs[numBlobs].rightBottom.x = right;
      blobs[numBlobs].rightBottom.y = blobs[which].rightBottom.y;
      blobs[numBlobs].pixels = 0;
      blobs[numBlobs].area = 0;
      numBlobs++;
    }
  } else if (isRobotCentered(headX, left, right) == RIGHT && xdist < width - 3 && right < IMAGE_WIDTH - 3 && width - xdist > MINROBOTWIDTH) {
    drawBlob(blobs[which], BLUE);
    // probably have two
    int splitter = right - headX;
    int splitpoint = headX - splitter;
    blobs[which].leftTop.x = splitpoint;
    blobs[which].leftBottom.x = splitpoint;
    // now try and find the head of the second blob
    // and here is where I stopped last
    int newY, newX = 0;
    bool found = false;
    for (newY = top; newY < IMAGE_HEIGHT && newY < blobs[which].leftBottom.y && !found; newY++) {
      for (newX = left; newX < IMAGE_WIDTH && newX < splitpoint && !found; newX++) {
	if (thresh->thresholded[newY][newX] == color) {
	  found = true;
	}
      }
    }
    newY--;
    if ((newY - top) < width - xdist && isRobotCentered(newX, left, splitpoint) == 0) {
      blobs[numBlobs].rightTop.x = splitpoint;
      blobs[numBlobs].rightTop.y = newY;
      blobs[numBlobs].leftTop.x = left;
      blobs[numBlobs].leftTop.y = newY;
      blobs[numBlobs].rightBottom.x = splitpoint;
      blobs[numBlobs].rightBottom.y = blobs[which].leftBottom.y;
      blobs[numBlobs].leftBottom.x = left;
      blobs[numBlobs].leftBottom.y = blobs[which].rightBottom.y;
      blobs[numBlobs].pixels = 0;
      blobs[numBlobs].area = 0;
      numBlobs++;
    }
  } else {
    // punt
    //drawBlob(blobs[which], ORANGE);
  }
  // should we merge this with nearby blobs?
  bool another = false;
  if (which > 0) {
    // check previous blob
    int lastY = blobs[which - 1].leftTop.y;
    int lastRight = blobs[which - 1].rightTop.x;
    if (which > 1 && blobs[which-1].rightTop.x - blobs[which-1].leftTop.x < 20)
      another = true;
    if (lastY - top > 50 && left - lastRight < 10) {
      mergeBlobs(which, which - 1);
    }
    if (another) {
      lastY = blobs[which - 2].leftTop.y;
      lastRight = blobs[which - 2].rightTop.x;
      if (lastY - top > 50 && left - lastRight < 10) {
	mergeBlobs(which, which - 2);
      }
    }
  }
  if (which < numBlobs - 1) {
    another = false;
    int lastY = blobs[which + 1].leftTop.y;
    int lastLeft = blobs[which + 1].leftTop.x;
    if (which < numBlobs - 2 && blobs[which+1].rightTop.x - blobs[which+1].leftTop.x < 20)
      another = true;
    if (lastY - top > 50 && lastLeft - left < 10) {
      mergeBlobs(which, which + 1);
    }
    if (another) {
      lastY = blobs[which + 2].leftTop.y;
      lastLeft = blobs[which + 2].leftTop.x;
      if (lastY - top > 50 && lastLeft - left < 10) {
	mergeBlobs(which, which + 2);
      }
    }
  }
}

void ObjectFragments::detectOrientation(int which) {
}

#endif

/* The next group of methods has to do with scanning along axis parallel dimensions in order to
 * create objects without blobbing.
 */




/* Project a line given a start coord and a new y value - note that this is dangerous depending on how you
 * do the projection.
 * @param startx   the x point to start at
 * @param starty   the y point to start at
 * @param newy     the y point to end at
 * @return         the corresponding x point
 */
int ObjectFragments::xProject(int startx, int starty, int newy) {
  //slope is a double representing the slope of the horizon.
  return startx - ROUND2(slope * (double)(newy - starty));
}

/* Project a line given a start coord and a new y value - note that this is dangerous depending on how you
 * do the projection.
 * @param point    the point to start at
 * @param newy     the y point to end at
 * @return         the corresponding x point
 */
int ObjectFragments::xProject(point <int> point, int newy) {
  //slope is a double representing the slope of the horizon.
  return point.x - ROUND2(slope * (double)(newy - point.y));
}

/* Project a line given a start coord and a new x value
 * @param startx   the x point to start at
 * @param starty   the y point to start at
 * @param newx     the x point to end at
 * @return         the corresponding y point
 */
int ObjectFragments::yProject(int startx, int starty, int newx) {
  return starty + ROUND2(slope * (double)(newx - startx));
}

/* Project a line given a start coord and a new x value
 * @param point    the point to start at
 * @param newx     the x point to end at
 * @return         the corresponding y point
 */
int ObjectFragments::yProject(point <int> point, int newx) {
  return point.y + ROUND2(slope * (double)(newx - point.x));
}

/* Scan from the point along the line until you have hit "stopper" points that aren't color "c"
   return the last good point found and how many good and bad points seen.  Though this is a void
   function it actually "returns" information in the scan variable. scan.x and scan.y represent
   the finish points of the line (last point of appropriate color) and bad and good represent
   how many bad and good pixels (pixels that are of the right color or not) along the way.
 * @param x        the x point to start at
 * @param y        the y point to start at
 * @param dir      the direction of the scan (positive or negative)
 * @param stopper  how many incorrectly colored pixels we can live with
 * @param c        color we are most interested in
 * @param c2       soft color that could also work
*/
void ObjectFragments::vertScan(int x, int y, int dir, int stopper, int c, int c2) {
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
  for ( ; x > -1 && y > -1 && x < width && y < height && bad < stopper; ) { // go until we hit enough bad pixels
    //cout << "Vert scan " << x << " " << y << endl;
    if (thresh->thresholded[y][x] == c || thresh->thresholded[y][x] == c2) { // if it is the color we're looking for - good
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
#if  ROBOT(AIBO)
  int increment = 1;
#elif ROBOT(NAO)
  int increment = 2;
#endif
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
#if ROBOT(NAO)
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
	double newslope = (double)(newx - x) / (double)(y - newy) ;
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
	  double newslope = (double)(newx - x) / (double)(y - newy) ;
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



/*  This method finds squares of a certain color under size constraints.  It is primarily
 * used to find backstops within goals.
 * @param x         x value of our starter point
 * @param y         y value of our starter point
 * @param c         the primary color
 * @param c2        the secondary color
 * @param left      the leftmost X boundary possible
 * @param right     the rightmost X boundary possible
 */

void ObjectFragments::constrainedSquare(int x, int y, int c, int c2, int left, int right) {
  obj.leftTop.x = BADVALUE; obj.leftTop.y = BADVALUE;       // so we can check for failure
  int nextx = x, nexty = y;
  int top = 0, topx = 0, bottom = 0, bottomx = 0;
  int h = 6, previousH = 0;
  obj.leftBottom.y = 0;
  // first we try going up
  vertScan(nextx, nexty, -1,  3, c, c2);
  h = scan.good;
  // at this point we have a very rough idea of how tall the square is
  vertScan(nextx, nexty, 1,  3, c, c2);
  h += scan.good;
  int initialH = h;
  // going left
  for (int i = 0; i < 4 && nextx > left; i++) {
    do {
      previousH = h;
      // first we try going up
      vertScan(nextx, nexty, -1,  4, c, c2);
      h = scan.good;
      // at this point we have a very rough idea of how tall the square is
      top = scan.y;
      topx = scan.x;
      vertScan(nextx, nexty, 1,  4, c, c2);
      bottom = scan.y;
      bottomx = scan.x;
      h += scan.good;
      nextx--;
      nexty = top + (bottom - top) / 2;
      if (h > 5) {
	obj.leftTop.x = topx;
	if (i == 0)
	  obj.leftTop.y = top;
	obj.leftBottom.x = bottomx;
	if (bottom > obj.leftBottom.y && i == 0) {
	  obj.leftBottom.y = bottom;
	  //drawPoint(bottomx, bottom, BLACK);
	}
      }
    } while (nextx > left && h > previousH / 2 && h > 5 ); // && h < initialH +6);
    h = initialH;
  }
  // going right
  nextx = x;
  nexty = y;
  h = 6;
  obj.rightBottom.y = 0;
  for (int i = 0; i < 4 && nextx < right; i++) {
    do {
      // first we try going up
      previousH = h;
      vertScan(nextx, nexty, -1,  4, c, c2);
      h = scan.good;
      // at this point we have a very rough idea of how tall the square is
      top = scan.y;
      topx = scan.x;
      vertScan(nextx, nexty, 1,  4, c, c2);
      bottom = scan.y;
      bottomx = scan.x;
      //drawPoint(bottomx, bottom, RED);
      h += scan.good;
      nextx++;
      nexty = top + (bottom - top) / 2;
      if (h > 5) {
	obj.rightTop.x = topx;
	if (i == 0)
	  obj.rightTop.y = top;
	obj.rightBottom.x = bottomx;
	if (bottom > obj.rightBottom.y && i == 0) {
	  obj.rightBottom.y = bottom;
	  //drawPoint(bottomx, bottom, RED);
	}
      }
    } while (nextx < right && h > previousH / 2 && h > 5 ); //&& h < initialH + 6);
  }
}

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

void ObjectFragments::aiboSquareGoal(int x, int y, int c, int c2) {
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



/* Starts with a point and a color.  Tries to figure out if the point exists within a larger
 * "square" of that color.  If it is big enough it returns the information on the square.
 * Virtually identical to squareIt, except that we're more worried about bands of color sticking
 * out of one or the other side.
 * @param x           starting x point to search for square
 * @param y           starting y point
 * @param c           the color we're looking for
 * @return void       places results in struct called "obj"
 */
void ObjectFragments::squareGoal(int x, int y, int c, int c2, int leftBound, int rightBound, int whatFor) {
  aiboSquareGoal(x, y, c, c2);
}


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
    robot(c);
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


/*  Process yellow:
 *  First we scan for beacons, based on the firstBlueYellow in the RLE as a starting point looking for posts.
 * @param bigGreen      green horizon value at x == 0
 */
void ObjectFragments::yellow(int bigGreen) {
#if ROBOT(AIBO)
  //after this method, we would have all necessary post stuff in the obj struct, if it found one.
  bool beacon = beaconCheck(thresh->firstYellowBlue, thresh->firstYellowBlueY, thresh->firstBlueYellow, thresh->firstBlueYellowY);
  if (beacon) {
    int leftPost = 0;
    int rightPost = 0;
    if (vision->by->getDist() > 0) {
      leftPost = min(vision->by->getLeftTopX(), vision->by->getLeftBottomX());
      rightPost = max(vision->by->getRightTopX(), vision->by->getRightBottomX());
    }
    if (vision->yb->getDist() > 0) {
      leftPost = min(vision->yb->getLeftTopX(), vision->yb->getLeftBottomX());
      rightPost = max(vision->yb->getRightTopX(), vision->yb->getRightBottomX());
    }
    if (rightPost > BADVALUE) {
      int nextX = 0;
      for (int i = 0; i < numberOfRuns; i++) {
	nextX = runs[i].x;
	if (distance(nextX, nextX, leftPost, rightPost) < MIN_SPLIT) {
	  runs[i].h = 0;
	}
      }
    }
  }
  goalScan(vision->yglp, vision->ygrp, vision->ygBackstop, YELLOW, ORANGEYELLOW, beacon, bigGreen);
#elif ROBOT(NAO)
  goalScan(vision->yglp, vision->ygrp, vision->ygBackstop, YELLOW, ORANGEYELLOW, false, bigGreen);
  //naoScan(vision->yglp, vision->ygrp, vision->ygBackstop, YELLOW, ORANGEYELLOW, bigGreen);
#else
  #error "ROBOT not defined as AIBO or NAO"
#endif
}

/* Process blue.  Same as for yellow except we theoretically don't need to worry about finding beacons.
 * @param bigGreen       field horizon at x == 0
 */

void ObjectFragments::blue(int bigGreen) {
#if ROBOT(AIBO)
  // take advantage of the fact that we've processed yellow
  int leftPost = 0;
  int rightPost = BADVALUE;
  if (vision->by->getDist() > 0) {
    leftPost = min(vision->by->getLeftTopX(), vision->by->getLeftBottomX());
    rightPost = max(vision->by->getRightTopX(), vision->by->getRightBottomX());
  }
  if (vision->yb->getDist() > 0) {
    leftPost = min(vision->yb->getLeftTopX(), vision->yb->getLeftBottomX());
    rightPost = max(vision->yb->getRightTopX(), vision->yb->getRightBottomX());
  }
  if (rightPost > BADVALUE) {
    int nextX = 0;
    for (int i = 0; i < numberOfRuns; i++) {
      nextX = runs[i].x;
      if (distance(nextX, nextX, leftPost, rightPost) < MIN_SPLIT) {
	runs[i].h = 0;
      }
    }
  }

  if (rightPost > BADVALUE)
    goalScan(vision->bglp, vision->bgrp, vision->bgBackstop, BLUE, BLUEGREEN, true, bigGreen);
  else
    goalScan(vision->bglp, vision->bgrp, vision->bgBackstop, BLUE, BLUEGREEN, false, bigGreen);
  //cout << "Blue " << numberOfRuns << " " << numBlobs << endl;
#elif ROBOT(NAO)
  goalScan(vision->bglp, vision->bgrp, vision->bgBackstop, BLUE, BLUEGREEN, false, bigGreen);
#else
  #error "ROBOT defined as neither AIBO or NAO"
#endif
  // Now that we're done, print out information on all of the objects that we've found.
  printObjs();

}

#if ROBOT(AIBO)
/* The next group of methods have to do with beacon identification (sometimes called "posts" in the code)
 */


/*  Look for a beacon of the given color.
 * Starts with the info from the RLE system, which consists of x-locations
 *  where there is a transition between blue,yellow or vice versa
 *  Then finds about how wide the beacon is, and uses this information to pick a
 *  point inside the color blob. (the midpoint). This point is then expanded to find the rest of the beacon.
 * @param pFirst         x value of the first place we saw a yellow-blue transition
 * @param pFirstY        y value at that point
 * @param pSecond        x value of the first place we saw a blue-yellow transition
 * @param pSecondY       y value at that point
 * @return               true when we found a beacon, false otherwise
 */

bool ObjectFragments::beaconCheck(int pFirst, int pFirstY, int pSecond, int pSecondY) {
  // we have two sets of beacon variables - one for yellow on top, the other for blue on top
  int pFirst1 = pFirst;     // we'll let the original parameters scan for the width //-yellowBlue
  int pSecond1 = pSecond;   //blueYellow
  int nextX, nextY, nextH;
  if (pFirst ==0 && pSecond == 0) return false;
  if (numberOfRuns > 1) {
    for (int i = 0; i < numberOfRuns; i++) {
      // search for continguous blocks - in order to determine possible widths
      nextX = runs[i].x;
      // categorize the run according to whether it is a potential beacon, or just part of this color color
      if (nextX == pFirst + 1 || nextX == pFirst + 2) {
	pFirst++;
      }
      if (nextX == pSecond + 1 || nextX == pSecond + 2) {
	pSecond++;
      }
    }
  } else
    return false;
  // sometimes we have potential beacons in both dimension
  if (BEACONDEBUG) {
    print("Beacon check bounds %i %i %i %i %i %i", pFirst1, pSecond1, pFirst,
          pSecond, pFirstY, pSecondY);
    //drawPoint(pFirst, pFirstY, BLACK);
  }
  // we have potential bounds for our beacon(s) now rescan and try and find a good fit for where to look
  if (pFirst == 0 &&  pFirst1 == 0 && pSecond == 0 && pSecond1 == 0) return false;
  int startx = BADVALUE, starty = BADVALUE, startx2 = BADVALUE, starty2 = BADVALUE;
  for (int i = 0; i < numberOfRuns; i++) {
    // search for continguous blocks - in order to determine possible widths
    nextX = runs[i].x;
    nextY = runs[i].y;
    nextH = runs[i].h;
    //Look to see if this run is near the center of either of the two navigation beacons
    // ####### needs some fixing up as the german photos show - sometimes we don't have runs in the middle
    if (nextX < midPoint(pFirst, pFirst1) && abs(nextY - pFirstY) < 15) {
      //if (abs(nextX - pFirst1 - (pFirst - pFirst1) / 2) < 2 && abs(nextY - pFirstY) < 15) {
      startx = nextX;
      starty = nextY;
    }
    if (nextX < midPoint(pSecond, pSecond1) && abs(nextY - pSecondY) < 15) {
      //if (abs(nextX - pSecond1 - (pSecond - pSecond1) / 2) < 2 && abs(nextY - pSecondY) < 15) {
      startx2 = nextX;
      starty2 = nextY + nextH;
    }
    /*if ((nextX >= pFirst1 && nextX <= pFirst) || (nextX >= pSecond1 && nextX <= pSecond)) {
      runs[i].h = 0;
      }*/
  }
  // if one of our boundary sets is non-negative then let's look for a beacon there
  if (startx > BADVALUE) { //If we found a starting place for beacon 1, lets elaborate our scan
    // before we go let's be sure that we have white underneath
    //cout << "Beacon " << midPoint(pFirst, pFirst1) << " " << starty << endl;
    int i;
    for (i = pFirst1; i < pFirst; i++) {
      vertScan(i, starty, 1, 5, YELLOW, ORANGEYELLOW);
      if (scan.good != 0) {
	int wx = scan.x;
	int wy = scan.y;
	vertScan(wx, wy, 1, 3, WHITE, WHITE);
	if (BEACONDEBUG) {
	  drawPoint(wx, wy, RED);
	  cout << "Beacon points " << wx << " " << wy << " " << scan.good << endl;
	}
	if (scan.good > 2) {
	  if (processBeacon(pFirst1, pFirst, startx, starty, false, true))
	  return true;
	}
      }
    }
    return false;
  }
  if (startx2 > BADVALUE) {
    //cout << "Beacon " << midPoint(pSecond, pSecond1) << " " << starty2 << endl;
    int i;
    for (i = pSecond1; i < pSecond; i++) {
      vertScan(i, starty2, 1, 5, BLUE, BLUEGREEN);
      if (scan.good != 0) {
	int wx = scan.x;
	int wy = scan.y;
	vertScan(wx, wy, 1, 3, WHITE, WHITE);
	if (BEACONDEBUG) {
	  drawPoint(wx, wy, RED);
	  cout << "Beacon points2 " << wx << " " << wy << " " << scan.good << endl;
	}
	if (scan.good > 2) {
	  if  (processBeacon(pSecond1, pSecond, startx2, starty2, true, true)) return true;
	}
      }
    }
  }
  return false;
}

/*  Now that we've identified a spot where a beacon might be, let's actually look around for it.
 *  The main idea is to try and grab to square color swatches one above the other.  From there
 *  we fiddle around a bit to get the best boundaries we can.
 * @param leftBound      the leftmost we could accept this beacon as being
 * @param rightBound     the rightmost
 * @param startx         x value where we have a stripe of both colors
 * @param starty         y value of that strip (transition between the colors)
 * @param yellowOnTop    should yellow be above blue or vice versa
 * @param normal         is this business as usual, or an odd case (e.g. called from goalScan)?
 * @return               true when the beacon is found, found otherwise
 */

bool ObjectFragments::processBeacon(int leftBound, int rightBound, int startx, int starty,  bool yellowOnTop, bool normal) {
  int spanY = 0;
  // if one of our boundary sets is non-negative then let's look for a beacon there
  // we want to start within the square we're searching to make sure we don't miss it.
  int y2 = starty + 2;
  bool occluded = false;
  if (yellowOnTop)
    y2 = starty - 2;
  int x2 = xProject(startx, starty, y2);
  //drawPoint(startx, starty, RED);
  squareGoal(x2, y2, YELLOW, ORANGEYELLOW, 0, IMAGE_WIDTH - 1, BEACON); //basically finds a square patch
  if (BEACONDEBUG) {
    print("Searching");
    printBlob(obj);
    //drawPoint(x2, y2, RED);
    drawBlob(obj, ORANGE);
  }
  // check for a top blob occluded situation
  if (blobOk(obj) && (obj.leftTop.y <= 0 || obj.rightTop.y <= 0) && normal) {
    //cout << "Got it here" << endl;
    // let's improve our blob
    if (yellowOnTop) {
      occluded = true;
    } else {
      // we're pretty much done  - use the yellow blob to infer the entire beacon
      return inferBeaconFromBlob(obj, vision->by);
    }
  }
  // now store the blob in the checker variable
  transferToChecker(obj);
  spanY = checker.rightBottom.y - checker.rightTop.y;
  // make sure it really is a worthwhle blob
  if (blobOk(checker)) {
    int nx, ny;
    if (!yellowOnTop) {
      startx = midPoint(checker.leftTop.x, checker.rightTop.x);
      starty = midPoint(checker.leftTop.y, checker.rightTop.y);
      nx = startx;
      ny = max(0, starty - spanY / 2);
      // now look for a blue blob
    } else {
      // since it is y on top then it is blue on bottom
      startx = midPoint(checker.leftBottom.x, checker.rightBottom.x);
      starty = yProject(checker.leftBottom, startx);
      nx = xProject(startx, starty, starty + spanY / 4);
      ny = starty + spanY / 4;
    }
    squareGoal(nx, ny, BLUE, BLUEGREEN, 0, IMAGE_WIDTH -1, BEACON);
    if (!blobOk(obj)) return false;
    if (BEACONDEBUG) {
      print("looking for beacon %i %i %i %i %i", obj.leftTop.x, obj.rightTop.x,
            spanY, nx, ny);
      drawPoint(nx, ny, BLACK);
      printBlob(obj);
      drawBlob(obj, ORANGE);
    }
    if (obj.rightTop.x - obj.leftTop.x < 1 || obj.rightBottom.y - obj.rightTop.y < 1) {
      // keep trying
      // right now we don't keep trying when yellow is on bottom
      if (yellowOnTop) {
	int i = 0;;
	for (i = checker.leftBottom.x; i < checker.rightBottom.x; i++) {
	  int startx2 = i;
	  int starty2 = yProject(checker.leftBottom, i);
	  squareGoal(startx2, starty2 + spanY / 4, BLUE, BLUEGREEN, 0, IMAGE_WIDTH - 1, BEACON);
	  if (blobArea(obj) > 1)
	    break;
	}
	if (i == checker.rightBottom.x) {
	  return false;
	}
      } else {
        int i = 0;
        for (i = checker.leftTop.x; i < checker.rightTop.x; i++) {
          int startx2 = i;
          int starty2 = yProject(checker.leftTop, i);
          squareGoal(startx2, starty2 - spanY / 4, BLUE, BLUEGREEN, 0, IMAGE_WIDTH - 1, BEACON);
          if (BEACONDEBUG)
            printBlob(obj);
	  if (blobArea(obj) > 1)
            break;
        }
        if (i == checker.rightTop.x) {
          return false;
        }
      }
    } //else {
      if (occluded) {
	return inferBeaconFromBlob(obj, vision->yb);
      }
      int c1 = YELLOW, c2 = ORANGEYELLOW, c3 = BLUE, c4 = BLUEGREEN;
      int x1 = obj.leftTop.x, y1 = obj.leftTop.y;
      y2 = obj.rightTop.y;
      x2 = obj.rightTop.x;
      if (!yellowOnTop) {
	c1 = c3; c2 = c4; c3 = YELLOW; c4 = ORANGEYELLOW;
	if (blobWidth(checker) > blobWidth(obj)) {
	  x1 = checker.leftTop.x; y1 = checker.leftTop.y, x2 = checker.rightTop.x, y2 = checker.rightTop.y;
	} else {
	  x1 = obj.leftBottom.x; y1 = obj.leftBottom.y, x2 = obj.rightBottom.x, y2 = obj.rightBottom.y;
	}
      } else {
	if (blobWidth(checker) > blobWidth(obj)) {
	  x1 = checker.leftBottom.x; y1 = checker.leftBottom.y; x2 = checker.rightBottom.x; y2 = checker.rightBottom.y;
	}
      }
      if (blobWidth(checker) > blobWidth(obj)) {
	// first check if we have a huge disparity - it might be an occluded beacon
	findSpot(x1, y1, x2, y2, -1, c1, c2, c3, c4, 1);
      } else {
	findSpot(x1, y1, x2, y2, 1, c1, c2, c3, c4, 0);
      }
      VisualFieldObject* beacon = vision->by;
      if (yellowOnTop)
	beacon = vision->yb;
      return setCorners(scan.x, scan.y, scan.good, scan.bad, c1, c2, c3, c4, beacon);
    //}
  }
  return false;
}

/*  As a convenience we use our blob data structure instead of an actual beacon data structure.  So once we've
 * found the beacon we need to convert the blob accordingly.
 * @param b      the blob
 * @param beacon the real beacon object
 * @return       always true
 */

bool ObjectFragments::inferBeaconFromBlob(blob b, VisualFieldObject* beacon) {
  int leftx = b.leftTop.x;
  int lefty = b.leftTop.y;
  int rightx = b.rightTop.x;
  int righty = b.rightTop.y;
  int spanX = rightx - leftx;
  int spanY = righty - lefty;
  int longSpanY =  max(b.rightBottom.y - b.rightTop.y, b.leftBottom.y - b.leftTop.y);
  // let's make absolutely sure! There must be white underneath!
  int i;
  for (i = leftx; i < rightx; i++) {
    vertScan(i, b.leftBottom.y, 1, 3, GREEN, GREEN);
    if (scan.good > 3) return false;
    vertScan(i, b.leftBottom.y, 1, 3, WHITE, WHITE);
    if (scan.good > 3) break;
  }
  if (i == rightx) return false;
  // ok, let's project up - the relationship to the top should be like our relationship
  b.leftTop.y = lefty - longSpanY;
  b.leftTop.x = leftx + b.leftTop.x - b.leftBottom.x;
  b.rightTop.x = b.leftTop.x + spanX;
  b.rightTop.y = b.leftTop.y + spanY;
  beacon->setLeftTopX(b.leftTop.x);
  beacon->setLeftTopY(b.leftTop.y);
  beacon->setRightTopX(b.rightTop.x);
  beacon->setRightTopY(b.rightTop.y);
  beacon->setLeftBottomX(b.leftBottom.x);
  beacon->setLeftBottomY(b.leftBottom.y);
  beacon->setRightBottomX(b.rightBottom.x);
  beacon->setRightBottomY(b.rightBottom.y);
  beacon->setX(beacon->getLeftTopX());
  beacon->setY(beacon->getLeftTopY());
  beacon->setWidth(dist(b.leftTop.x, b.leftTop.y, b.rightTop.x, b.rightTop.y));
  beacon->setHeight(dist(b.leftTop.x, b.leftTop.y, b.leftBottom.x, b.rightBottom.y));
  beacon->setCenterX(beacon->getLeftTopX() + ROUND2(beacon->getWidth()) / 2);
  beacon->setCenterY(beacon->getRightTopY() + ROUND2(beacon->getHeight()) / 2);
  beacon->setCertainty(SURE);
  beacon->setDistCertainty(true);
  beacon->setDist(1);
  return true;

}

/* Takes a beacon and the two points that define the borderline between the beacon's colors.  Extracts the defining
   points of the beacon based upon those two points and sets the beacon accordingly.  The difference between this
   one and the next one is that this one figures out the corner, and then makes sure that they are reasonable.
 * @param leftx   x value of leftmost point
 * @param lefty   y value of that point
 * @param rightx  x value of rightmost point
 * @param righty  y value of that point
 * @param c       primary color
 * @param c2      its soft color
 * @param c3      other color (e.g. if primary is blue, then yellow)
 * @param c4      its soft color
 * @param beacon    the beacon that we're looking at
 */

bool ObjectFragments::setCorners(int leftx, int lefty, int rightx, int righty, int c, int c2, int c3, int c4, VisualFieldObject* beacon) {
  // sometimes we don't have perfect alignment - let's fix that
  int spanX = rightx - leftx;
  int spanY = righty - lefty;
  int leftX = 0, leftY = 0;
  int bottomX = 0, bottomY = 0, good = 0, xCheck = 0, yCheck = 0;
  int i, j;
  if (CORNERDEBUG) {
    print("In setCorners");
    print("%i %i %i %i %i", leftx, lefty, rightx, righty, spanY);
  }
  if (spanX > 1) {
    // we have a beacon!  Let's calculate the apparent left side points
    leftX = leftx + spanY;
    leftY = lefty - spanX;
    bottomX = leftx - spanY;
    bottomY = lefty + spanX;
    int longSpanY;
    longSpanY =  abs(bottomY - leftY);
    if (leftY < 0) {
      // we have an occluded beacon - let's figure out what it is based just on the bottom blob
      //return true;
    }
    if (CORNERDEBUG) {
      print("In with %i %i", leftX, leftY);
    }
    // now scan from the corner across making sure we hit some pixels of the right color
    for (i = 0; i < longSpanY && good < 2 && leftY >= 0 && leftX >= 0; i++) {
      xCheck = xProject(leftX, leftY, leftY + i);
      good = 0;
      for (j = xCheck; j < xCheck + spanX && good < 2 && xCheck < IMAGE_WIDTH; j++) {
	yCheck = yProject(xCheck, leftY + i, j);
	//cout << j << " " << yCheck << endl;
	if (yCheck < 0 || yCheck >= IMAGE_HEIGHT || j < 0 || j >= IMAGE_WIDTH)
	  break;
	if (thresh->thresholded[yCheck][j] == c) { // || thresh->thresholded[yCheck][j] == c2) {
	  good++;
	}
      }
      if (CORNERDEBUG) {
	print("Scan %i %i %i %i %i %i", i, good, j, spanX, xCheck, spanY);
      }
    }
    if (good > 1) {
      leftX = xCheck;
      leftY = leftY + i - 1;
    } else {
    }
    // now go back the other way
    good = spanX;
    int bx = 0;
    int by = 0;
    for (i = 0; i < longSpanY && good >= spanX / 2 && leftY >= 0 && leftX >= 0; i++) {
      xCheck = xProject(leftX, leftY, leftY - i);
      good = 0;
      for (j = xCheck; j < xCheck + spanX && good < spanX / 2 && xCheck < IMAGE_WIDTH; j++) {
	yCheck = yProject(xCheck, leftY - i, j);
	//cout << j << " " << yCheck << endl;
	if (yCheck < 0 || yCheck >= IMAGE_HEIGHT || j < 0 || j >= IMAGE_WIDTH)
	  break;
	if (thresh->thresholded[yCheck][j] == c) { // || thresh->thresholded[yCheck][j] == c2) {
	  good++;
	}
      }
      if (good >= spanX / 2) {
	bx = xCheck;
	by = leftY - i;
      }
      if (CORNERDEBUG) {
	print("Scan %i %i %i %i %i %i", i, good, j, spanX, xCheck, spanY);
      }
    }
    if (bx != 0 || by != 0) {
      leftX = bx;
      leftY = by;
    }

    if (CORNERDEBUG) {
      print("Out with %i %i", leftX, leftY);
      print("In with %i %i", bottomX, bottomY);
    }
    // now do the same thing for the bottom corners
    good = 0;
    for (i = 0; i < longSpanY && good < 2 && bottomY < IMAGE_HEIGHT && bottomX >= 0; i++) {
      xCheck = xProject(bottomX, bottomY, bottomY - i);
      good = 0;
      for (j = xCheck; j < xCheck + spanX && good < 2 && xCheck < IMAGE_WIDTH; j++) {
	yCheck = yProject(xCheck, bottomY - i, j);
	if (yCheck < 0 || yCheck >= IMAGE_HEIGHT || j < 0 || j >= IMAGE_WIDTH)
	  break;
	if (thresh->thresholded[yCheck][j] == c3) { // || thresh->thresholded[yCheck][j] == c4) {
	  good++;
	}
      }
    }
    if (good > 1) {
      bottomX = xCheck;
      bottomY = bottomY - i + 1;
    }
    good = spanX;
    bx = 0;
    by = 0;
    for (i = 0; i < longSpanY && good >= spanX / 2 && bottomY < IMAGE_HEIGHT && bottomX >= 0; i++) {
      xCheck = xProject(bottomX, bottomY, bottomY + i);
      good = 0;
      for (j = xCheck; j < xCheck + spanX && good < spanX / 2 && xCheck < IMAGE_WIDTH; j++) {
	yCheck = yProject(xCheck, bottomY + i, j);
	if (yCheck < 0 || yCheck >= IMAGE_HEIGHT || j < 0 || j >= IMAGE_WIDTH)
	  break;
	if (thresh->thresholded[yCheck][j] == c3) { // || thresh->thresholded[yCheck][j] == c4) {
	  good++;
	}
      }
      if (good >= spanX / 2) {
	bx = xCheck;
	by = bottomY + i;
      }

    }
    if (bx != 0 || by != 0) {
      bottomX = bx;
      bottomY = by;
    }
    // since our height has to be accurate, let's check heights of each blob
    double bigd = 0;
    int toph = 0, both = 0;
    for (i = 0; i < spanX; i++) {
      bx = leftX + i;
      by = yProject(leftX, leftY, leftX + i);
      vertScan(bx, by, 1,  5, c, c2);
      double d1 = dist(bx, by, scan.x, scan.y);
      if (d1 > bigd)
	bigd = d1;
      if (scan.good > toph)
	toph = scan.good;
      bx = bottomX + i;
      by = yProject(bottomX, bottomY, bx);
      vertScan(bx, by, -1, 5, c3, c4);
      double d2 = dist(bx, by, scan.x, scan.y);
      if (d2 > bigd)
	bigd = d2;
      if (scan.good > both)
	both = scan.good;
    }
    if (BEACONDEBUG) {
      cout << "Heights " << toph << " " << both << endl;
    }
    if (CORNERDEBUG) {
      print("Out with %i %i", bottomX, bottomY);
    }
    // now update our beacon
    // let's make absolutely sure! There must be white underneath!
    if (max(bottomY, bottomY + spanY) < IMAGE_WIDTH - 5) {
      for (i = leftX; i < leftX + spanX; i++) {
	vertScan(i, bottomY, 1, 3, GREEN, GREEN);
	if (scan.good > 3) return false;
	vertScan(i, bottomY, 1, 3, WHITE, WHITE);
	if (scan.good > 3) break;
      }
      if (i == leftX + spanX) return false;
    }
    beacon->setLeftTopX(leftX);
    beacon->setLeftTopY(leftY);
    beacon->setRightTopX(leftX + spanX);
    beacon->setRightTopY(leftY + spanY);
    beacon->setLeftBottomX(bottomX);
    beacon->setLeftBottomY(bottomY);
    beacon->setRightBottomX(bottomX + spanX);
    beacon->setRightBottomY(bottomY + spanY);
    beacon->setX(beacon->getLeftTopX());
    beacon->setY(beacon->getLeftTopY());
    beacon->setWidth(dist(leftX, leftY, leftX + spanX, leftY + spanY));
    beacon->setHeight(max(bigd,dist(leftX, leftY, bottomX, bottomY)));
    beacon->setCenterX(beacon->getLeftTopX() + ROUND2(beacon->getWidth()) / 2);
    beacon->setCenterY(beacon->getRightTopY() + ROUND2(beacon->getHeight()) / 2);
    beacon->setCertainty(SURE);
    beacon->setDistCertainty(true);
    beacon->setDist(1);
    return true;
  }
  return false;
}


/* Much like the last function except that we're going to scan in both directions for both colors, instead of just
 * one direction.  Whereas findColorTransition is shrinking our beacon, this function will try and expand it back out.
 * The given direction in this case is either left or right (the direction of expansion).
 * @param x1      x value to start at
 * @param y1      y value to start at
 * @param dir     direction to look (positive or negative)
 * @param c       primary color
 * @param c2      its soft color
 * @param c3      other color (e.g. if primary is blue, then yellow)
 * @param c4      its soft color
 */

void ObjectFragments::expandSpot(int x1, int y1, int dir, int c, int c2,
    int c3, int c4) {

  int startx = max(0, x1);
  int starty = max(0, y1);
  int newX = 0, newY = 0, nx = 0, ny = 0, nx2 = 0, ny2 = 0;
  int scanlines = 0, i = 0, good = 0, good2 = 0;
  int width = IMAGE_WIDTH, height = IMAGE_HEIGHT;
  int firstx = x1, firsty = y1, firstx2 = x1, firsty2 = y1;
  spot.x = x1;
  spot.y = y1;
  do {
    good = 0;
    good2 = 0;
    // newX and newY move along an axis-parallel horizontal line from the start point
    newX = startx + dir * scanlines;
    newY = yProject(startx, starty, newX);
    scanlines++;
    // here's the loop that actually looks for the colors - remember we need to see both of the colors
    for (i = 0; i < 10 && (good < 2 || good2 <  2); i++) {
      // project up (or down)  from newX and newY
      ny = starty + i;
      ny2 = starty - i;
      nx = xProject(newX, newY, ny);
      nx2 = xProject(newX, newY, ny2);
      if (nx < 0 || ny < 0 || nx >= width || ny >= height)
	break;
      if (thresh->thresholded[ny][nx] == c3 || thresh->thresholded[ny][nx] == c4) {
	if (good == 0) {
	  // track when we see the color
	  firstx = nx;
	  firsty = ny;
	}
	good++;
      }
      if (nx2 < 0 || ny2 < 0 || nx2 >= width || ny2 >= height)
	break;
      if (thresh->thresholded[ny2][nx2] == c || thresh->thresholded[ny2][nx2] == c2) {
	if (good2 == 0) {
	  // track when we see the color
	  firstx2 = nx2;
	  firsty2 = ny2;
	}
	good2++;
      }
    }
    if (good > 1 && good2 > 1) {
      spot.x = midPoint(firstx, firstx2);
      spot.y = midPoint(firsty, firsty2);
    }
  } while (good > 1 && good2 > 1);

}

/* The "spot" we're trying to find are the two points at the end of the line segment dividing blue
   and yellow on a beacon.  What we get are two points that define the bigger of the two blobs we
   found.  Basically we scan up and down to try and find the points where we have both blue and
   yellow on the correct respective side.
 * @param x1        starter x point on one end
 * @param y1        y point on that end
 * @param x2        starter x point on the other end
 * @param y2        y point on that end
 * @param dir       which direction to go
 * @param c         primary color
 * @param c2        soft color
 * @param c3        other color (e.g. if blue is primary, then yellow is other)
 * @param c4        its soft color
 * @param cases     not used
 *
 */

void ObjectFragments::findSpot(int x1, int y1, int x2, int y2, int dir, int c, int c2, int c3, int c4, int cases) {
  spot.x = x1;
  spot.y = y1;
  expandSpot(spot.x, spot.y, -1, c, c2, c3, c4);
  scan.x = spot.x;
  scan.y = spot.y;
  spot.x = x2;
  spot.y = y2;
  expandSpot(spot.x, spot.y, 1, c, c2, c3, c4);
  scan.good = spot.x;
  scan.bad = spot.y;
  //drawPoint(scan.good, scan.bad, RED);
  //drawPoint(scan.good, scan.bad, BLACK);
  // ok we have our two points, but they may not be aligned properly.  Let's try and fix that.  Our  goal
  // is to intersect the horizontal projection of one point against the vertical projection of the other.
  //determine the intercept of line 1
  int ax = scan.good;
  int ay = yProject(scan.x, scan.y, scan.good);
  int ax2 = ax;
  int ay2 = scan.bad;
  //if (scan.good >  0) {
  //scan.bad = yProject(scan.x, scan.y, scan.good);
    //vision->drawLine(scan.x, scan.y, scan.good, scan.bad, BLACK);
    //return;
  //}
  // for some reason negative and positive slopes behave differently when projected
  bool foundit = false;
  for (int k = 0; k < 3; k++) {
    for (int i = 0; i < 5; i++) {
      projx[i] = scan.good + i + k * 5;
      projy[i] = yProject(scan.x, scan.y, scan.good + i + k * 5);
      //cout << "Generating " << projx[i] << " " << projy[i] << endl;
    }
    for (int i = 0; i < 5 && !foundit; i++) {
      ay = scan.y + i + k * 5;
      ay2 = scan.y - i + k * 5;
      ax = xProject(scan.good, scan.bad, ay);
      ax2 = xProject(scan.good, scan.bad, ay2);
      //cout << "Testing " << ax << " " << ay << " " << ax2 << " " << ay2 << endl;
      for (int j = 0; j < 5; j++) {
	if (ay == projy[j] && ax == projx[j]) {
	  scan.good = ax;
	  scan.bad = ay;
	  //cout << "found it " << endl;
	return;
	} else if (ay2 == projy[j] && ax2 == projx[j]) {
	  scan.good = ax2;
	  scan.bad = ay2;
	  //cout << "Found it" << endl;
	  return;
	}
      }
    }
  }
  // crap!  Let's just project the original point and be done with it
  scan.bad = yProject(scan.x, scan.y, scan.good);
}


/* Start at one of the edges - and then scan in towards the middle. As soon as a scan line is found which is more than "check" percent of "myHeight", stop, and report how many lines we moved over - since these can then ultimately be discarded - read shaved off.
   Results are stored in scan1.x,scan1.y for the top corner, and scan1.bad,scan1.good for the bottom corner.
 *
 * @param oneX         a pointX - We want to left or right scan this direction from this x value
 * @param oneY         a pointY - Serves to kepp track of where to scan
 * @param dir          the direction to scan in
 * @param myHeight     the current height of our object
 * @param check        a double which represents what percent of the height we need to find a run to match
 * @param x            center of where we are looking
 * @param y            center of where we are looking
 * @param stopper      Noise tolerance on scans?
 * @param c            Object color
 *
 */
void ObjectFragments::findCorner(int oneX, int oneY, int dir, int myHeight, double check, int x, int y, int stopper, int c, int c2) {
  int i = oneX;
  int j = oneY;
  int checkSpan = BADVALUE;
  int check1 = 0, check2 = 0;
  int count = 0;
  scan1.x = oneX;
  scan1.y = oneY;
  scan1.bad = x;
  scan1.good = y;
  //cout << "In findcorner " << myHeight << endl;
  //myHeight times this magical .65 const basically means we are looking for a run that is at least that tall.
  //Also, make sure we are scanning in
  for ( ; checkSpan < ROUND2((double)myHeight * check) && ((i < x && dir > 0) || (i > x && dir < 0)) ;  ) {
    //cout << "Finding " << i << " " << j << " " << checkSpan << endl;
    vertScan(i, j, -1,  stopper, c, c2); //scan up
    check1 = scan.y;
    scan1.x = scan.x;
    scan1.y = scan.y;
    //cout << "Next find " << endl;
    vertScan(i, j, 1,  stopper, c, c2); //scan down
    check2 = scan.y;
    checkSpan = check2 - check1 + 1;//how far does the blob span up and down?
    i = i + dir; //move left, right
    j = y - (int)(slope * (i - x));
    scan1.bad = scan.x;
    scan1.good = scan.y;
    count++;
    //cout << "Found" << checkSpan << endl;
  }
  scan1.span = count -1;
}
#endif


/* A collection of miscelaneous methods used in processing goals.
 */


/* Find the index of the biggest run left.  The "run" is our basic currency in vision.  Our idea
 * in looking for the goal is to find the biggest run we can and assume that it is part of a goal
 * beacon.  We look from there for a big square of the right color.  This method is very simple, it
 * just scans all of the runs between "left" and "right" and picks out the bigest one.  Once done
 * it returns the corresponding index.
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


/*  As we saw with beacons, we tend to work with blobs for convenience.  So at some point
 * we need to transfer their contents over to the field object that we have identified.
 * In this case we have a goal.  Before we commit we make sure it has enough of the right
 * color.  We also collect up certainty information and pass that along.
 * @param one             the field object we'd like to update
 * @param two             the blob that contains the information we need
 * @param certainty       how certain are we of its ID?
 * @param distCertainty   how certain are we of how big we think it is?
 */
bool ObjectFragments::updateObject(VisualFieldObject* one, blob two, int certainty, int distCertainty) {
  //cout << "Got an object" << endl;
  // before we do this let's make sure that the object is really our color
  if (rightBlobColor(two, NORMALPOST)) {
      one->setLeftTopX(two.leftTop.x);
      one->setLeftTopY(two.leftTop.y);
      one->setLeftBottomX(two.leftBottom.x);
      one->setLeftBottomY(two.leftBottom.y);
      one->setRightTopX(two.rightTop.x);
      one->setRightTopY(two.rightTop.y);
      one->setRightBottomX(two.rightBottom.x);
      one->setRightBottomY(two.rightBottom.y);
      one->setX(two.leftTop.x);
      one->setY(two.leftTop.y);
      one->setWidth(dist(two.rightTop.x, two.rightTop.y, two.leftTop.x, two.leftTop.y));
      one->setHeight(dist(two.leftTop.x, two.leftTop.y, two.leftBottom.x, two.leftBottom.y));
      one->setCenterX(one->getLeftTopX() + ROUND2(one->getWidth() / 2));
      one->setCenterY(one->getRightTopY() + ROUND2(one->getHeight() / 2));
      one->setCertainty(certainty);
      one->setDistCertainty(distCertainty);
      one->setDist(1);
      return true;
  } else {
    //cout << "Screening object for low percentage of real color" << endl;
    return false;
  }
}

void ObjectFragments::updateRobot(VisualFieldObject* one, blob two) {
  one->setLeftTopX(two.leftTop.x);
  one->setLeftTopY(two.leftTop.y);
  one->setLeftBottomX(two.leftBottom.x);
  one->setLeftBottomY(two.leftBottom.y);
  one->setRightTopX(two.rightTop.x);
  one->setRightTopY(two.rightTop.y);
  one->setRightBottomX(two.rightBottom.x);
  one->setRightBottomY(two.rightBottom.y);
  one->setX(two.leftTop.x);
  one->setY(two.leftTop.y);
  one->setWidth(dist(two.rightTop.x, two.rightTop.y, two.leftTop.x, two.leftTop.y));
  one->setHeight(dist(two.leftTop.x, two.leftTop.y, two.leftBottom.x, two.leftBottom.y));
  one->setCenterX(one->getLeftTopX() + ROUND2(one->getWidth() / 2));
  one->setCenterY(one->getRightTopY() + ROUND2(one->getHeight() / 2));
  one->setDist(1);
}

/* Transfer a blob into a field arc.
 * @param one             the field object we'd like to update
 * @param two             the blob that contains the information we need
 */
bool ObjectFragments::updateArc(VisualFieldObject* one, blob two, int goalHeight) {
  one->setWidth(dist(two.rightTop.x, two.rightTop.y, two.leftTop.x, two.leftTop.y));
  if (one->getWidth() > 10) {

    int midx = two.leftTop.x + (two.rightTop.x - two.leftTop.x) / 2;
    point <int> leftPoint = thresh->findIntersection(midx, -1, color);
    point <int> rightPoint = thresh->findIntersection(midx, 1, color);
    if (leftPoint.x != BADVALUE) {
      if (ARCDEBUG) {
        print("Found left point %i %i", leftPoint.x, leftPoint.y);
        drawPoint(leftPoint.x, leftPoint.y, RED);
      }
    }
    if (rightPoint.x != BADVALUE) {
      if (ARCDEBUG) {
        print("Found right point %i %i", rightPoint.x, rightPoint.y);
        drawPoint(rightPoint.x, rightPoint.y, RED);
      }
    }

    one->setHeight(dist(two.leftTop.x, two.leftTop.y, two.leftBottom.x, two.leftBottom.y));
    int lt = -1, rt = -1, lb = -1, rb = -1, col = GREEN, ltt = 0, rtt = 0, lbt = 0, rbt = 0;
    for (int i = 0; i < min(one->getWidth(), one->getHeight()); i++) {
      if (lt < 0) {
        col = thresh->thresholded[two.leftTop.y + i][two.leftTop.x + i];
        if (col == color) {
          ltt++;
          if (ltt > 1) {
            lt = i;
            //drawPoint(two.leftTop.x + i, two.leftTop.y + i, RED);
          }
        } else ltt = 0;
      }
      if (lb < 0) {
        col = thresh->thresholded[two.leftBottom.y - i][two.leftTop.x + i];
        if (col == color) {
          lbt++;
          if (lbt > 1) {
            lb = i;
            //drawPoint(two.leftBottom.x + i, two.leftBottom.y - i, RED);
          }
        } else lbt = 0;
      }
      if (rt < 0) {
        col = thresh->thresholded[two.leftTop.y + i][two.rightTop.x - i];
        if (col == color) {
          rtt++;
          if (rtt > 1) {
            rt = i;
            //drawPoint(two.rightTop.x - i, two.rightTop.y + i, RED);
          }
        } else rtt = 0;
      }
      if (rb < 0) {
        col = thresh->thresholded[two.leftBottom.y - i][two.rightBottom.x - i];
        if (col == color) {
          rbt++;
          if (rbt > 1) {
            rb = i;
            //drawPoint(two.rightBottom.x - i, two.leftBottom.y - i, RED);
          }
        } else rbt = 0;
      }
    }
    int half = (two.rightTop.x - two.leftTop.x) / 2;
    int half2 = (two.rightBottom.y - two.rightTop.y) / 2;
    int top = 0, bottom = 0;
    for (int i = 0; i < half; i++) {
      for (int j = 0; j < half2; j++) {
        if (thresh->thresholded[two.leftTop.y + j][two.leftTop.x + half / 2 + i] == GREEN)
          top++;
        if (thresh->thresholded[two.leftTop.y + half2 + j][two.leftTop.x + half / 2 + i] == GREEN)
          bottom++;
      }
    }
    if (ARCDEBUG) {
      print("Distances %i %i %i %i", lt, rt, lb, rb);
      print("Top %i %i", top, bottom);
      drawRect(two.leftTop.x + half / 2, two.leftTop.y, half, ((two.leftBottom.y - two.leftTop.y) / 2), RED);
      drawRect(two.leftTop.x + half / 2, two.leftTop.y + (two.leftBottom.y - two.leftTop.y) / 2, half, ((two.leftBottom.y - two.leftTop.y) / 2), BLACK);
    }
    int check = 0;
    if (lt < 0) check++;
    if (rt < 0) check++;
    if (rb < 0) check++;
    if (lb < 0) check++;
    if (check > 2) {
      one->init();
      return false;
    }
    one->setCertainty(SURE);
    if (goalHeight > 12 || ((min(lt, rt) > max(lb, rb) && lt > 3) && (max(lb, rb) > max(lt, rt) + 2)))
      one->setCertainty(NOTSURE);
    else if (rt < 4 && lb < 4 && rb > lt + 3 && lt > 4)
      one->setCertainty(NOTSURE);
    else if (lt < 4 && rb < 4 && lb > rt + 3 && rt > 4)
      one->setCertainty(NOTSURE);
    /*if (min(lt, rt) > max(lb, rb) && lt > 3)
      one->setCertainty(SURE);
    else if (min(lb, rb) > max(lt, rt))
      one->setCertainty(NOTSURE);
    else {
      lt = max(lt, rt);
      lb = max(lb, rb);
      if (lb > lt + 2) {
        one->setCertainty(SURE);
      } else {
        one->setCertainty(NOTSURE);
      }
      }*/
    // try and figure out concavity
    one->setLeftTopX(two.leftTop.x);
    one->setLeftTopY(two.leftTop.y);
    one->setLeftBottomX(leftPoint.x);
    one->setLeftBottomY(leftPoint.y);
    one->setRightTopX(two.rightTop.x);
    one->setRightTopY(two.rightTop.y);
    one->setRightBottomX(rightPoint.x);
    one->setRightBottomY(rightPoint.y);
    one->setX(two.leftTop.x);
    one->setY(two.leftTop.y);
    one->setCenterX(one->getLeftTopX() + ROUND2(one->getWidth() / 2));
    one->setCenterY(one->getRightTopY() + ROUND2(one->getHeight() / 2));
    one->setDistCertainty(NOTSURE);
    one->setDist(1);
  }
  return true;
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
int ObjectFragments::checkDist(int left, int right, int top, int bottom) {
  int dc = BOTH_SURE;
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

/*  As we saw with beacons, we tend to work with blobs for convenience.  So at some point
 * we need to transfer their contents over to the field object that we have identified.
 * @param one    the field object we'd like to update
 * @param two    the blob that contains the information we need
 */
void ObjectFragments::updateBackstop(VisualFieldObject* one, blob two) {
  one->setLeftTopX(two.leftTop.x);
  one->setLeftTopY(two.leftTop.y);
  one->setLeftBottomX(two.leftBottom.x);
  one->setLeftBottomY(two.leftBottom.y);
  one->setRightTopX(two.rightTop.x);
  one->setRightTopY(two.rightTop.y);
  one->setRightBottomX(two.rightBottom.x);
  one->setRightBottomY(two.rightBottom.y);
  one->setX(two.leftTop.x);
  one->setY(two.leftTop.y);
  one->setWidth(dist(two.rightTop.x, two.rightTop.y, two.leftTop.x, two.leftTop.y));
  one->setHeight(dist(two.leftTop.x, two.leftTop.y, two.leftBottom.x, two.leftBottom.y));
  one->setCenterX(one->getLeftTopX() + ROUND2(one->getWidth() / 2));
  one->setCenterY(one->getRightTopY() + ROUND2(one->getHeight() / 2));
  one->setDist(1);
}

/* Post recognition for NAOs
 */
#if ROBOT(NAO)
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
  for (int i = 0; i < h / 5 && biggest < need; i+=2) {
    int tx = xProject(x, y, y + i);
    horizontalScan(tx, y + i, -1, 6, color, color, max(0, x - 2 * w), IMAGE_WIDTH - 1);
    if (scan.good > biggest) {
      biggest = scan.good;
    }
  }
  x = b.rightTop.x;
  y = b.rightTop.y;
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
  for (int i = x - 10; i > max(0, x - h); i-=2) {
    int yp = yProject(x, y, i);
    for (int j = max(0,yp); j < yp+10; j++) {
      if (thresh->thresholded[j][i] == color) {
	lefties++;
      }
    }
  }
  for (int i = b.rightTop.x + 10; i < min(IMAGE_WIDTH - 1, b.rightTop.x + h); i+=2) {
    int yp = yProject(b.rightTop.x, b.rightTop.y, i);
    for (int j = max(0,yp); j < yp+10; j++) {
      if (thresh->thresholded[j][i] == color) {
	righties++;
      }
    }
  }
  if (righties > 50 && righties > 10 * lefties)
    return LEFT;
  if (lefties > 50 && lefties > 10 & righties)
    return RIGHT;
  return NOPOST;
}

int ObjectFragments::triangle(blob b) {
  int x = b.leftBottom.x - 20;
  int y = b.leftBottom.y - 20;
  int h = blobHeight(b);
  int w = blobWidth(b);
  for (int i = 0; i < h / 5; i+=2) {
    int tx = xProject(x, y, y - i);
    horizontalScan(tx, y - i, -1, 6, WHITE, WHITE, 0, IMAGE_WIDTH - 1);
    if (scan.good >= 3 * w / 4) {
      //drawPoint(scan.x, scan.y, MAROON);
      int x2 = x - w / 4;
      int y2 = min(midPoint(y - i, scan.y), IMAGE_HEIGHT - 1);
      //drawPoint(x2, y2, MAROON);
      vertScan(x2, y2, -1, 6, WHITE, WHITE);
      int tot = scan.good;
      int cx = scan.x, cy = scan.y;
      vertScan(x2, y2, 1, 6, WHITE, WHITE);
      if (scan.good + tot > w / 2) {
	int greens = 0;
	//drawRect(cx, cy, x - cx, cy - scan.y, MAROON);
	for (int j = cx; j < x + 20; j++)
	  for (int k = cy; k < y2; k++)
	    if (thresh->thresholded[k][j] == GREEN) {
	      greens++;
	    }
	if (greens > 1) break;
	return RIGHT;
      }
    }
    //drawPoint(scan.x, scan.y, MAROON);
  }
  x = b.rightBottom.x + 20;
  y = b.rightBottom.y - 20;
  for (int i = 0; i < h / 5; i+=2) {
    int tx = xProject(x, y, y - i);
    horizontalScan(tx, y - i, 1, 6, WHITE, WHITE, x - 1, IMAGE_WIDTH - 1);
    if (scan.good >= 3 * w / 4) {
      int x2 = x + w / 4;
      int y2 = min(midPoint(y - i, scan.y), IMAGE_HEIGHT - 1);
      //drawPoint(x2, y2, MAROON);
      vertScan(x2, y2, -1, 6, WHITE, WHITE);
      int tot = scan.good, cx = scan.good, cy = scan.y, greens = 0;
      vertScan(x2, y2, 1, 6, WHITE, WHITE);
      if (scan.good + tot > w / 2) {
	for (int j = cx; j > x - 20; j--)
	  for (int k = cy; k < y2; k++)
	    if (thresh->thresholded[k][j] == GREEN) {
	      greens++;
	    }
	if (greens > 1) break;
	return LEFT;
      }
    }
    //drawPoint(scan.x, scan.y, MAROON);
  }
  return NOPOST;
}
#endif

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

#if ROBOT(AIBO)
/*  Identifying which post we are looking at is hard as heck, so we have a variety of tricks
 * in our toolbox including this one.  Essentially at this point we have found a blob of color
 * that we think is a goal post.  We then grab the biggest swatch of that color we can find.  It
 * probably will contain our swatch.  If so we can use the location of our swatch within the bigger
 * swatch to determine which post it is.  If it doesn't contain our swatch then that can be useful
 * too - perhaps its the other post!
 * @param big     the biggest blob of the current color
 * @param small   the possible post of that color
 * @return int    a constant indicating what we've found - USEBIG, NOPOST (we don't really know), RIGHT, LEFT
 */

int ObjectFragments::checkPostBlob(blob big, blob small) {

  // first just check if its reasonable
  int leftGap = small.leftTop.x - big.leftTop.x;
  int rightGap = big.rightTop.x - small.rightTop.x;
  int spanY1 = blobHeight(small);
  int spanY2 = blobHeight(big);
  int spanX1 = blobWidth(small);
  int spanX2 = blobWidth(big);

  // when the big one and the small one are relatively similar in size it could indicate the two posts
  if (spanX1 > spanX2 / 4) {
    // if we have widely disparate blobs,  they are basically the same height, and they are fairly squat
    // then we're probably looking at the backstop
    if (small.leftTop.x < big.leftTop.x) {
      if (big.leftTop.x - small.rightTop.x > BIGGAP && abs(spanY1 - spanY2) < SIMILARSIZE && (spanY1 > spanX1 || spanY2 > spanX2)) {
	if (POSTDEBUG) {
	  print("Found backstop %i", (big.leftTop.x - small.rightTop.x));
	}
	return USEBIG;
      }
    } else {
      if (small.leftTop.x - big.rightTop.x > BIGGAP && abs(spanY1 - spanY2) < SIMILARSIZE && (spanY1 > spanX1 || spanY2 > spanX2)) {
	if (POSTDEBUG) {
	  print("Found backstop");
	}
	return USEBIG;
      }
    }
    return NOPOST;
  } else {
    // if the blob matches up with the post then we can use it if the big post is big enough relative to the small one
    if ((leftGap > -2 || small.leftTop.x < 0) && rightGap > -2 &&
	(big.leftTop.y <= small.leftTop.y || small.leftTop.y < 0) && big.leftBottom.y >= small.leftBottom.y) {
      // ok we have it, now let's classify what the relationship is
      if (POSTLOGIC) {
	print("Setting post according to blobs");
      }
      if (leftGap < rightGap) {
	return LEFT;
      } else {
	// we're on the right side of the post
	return RIGHT;
      }
    }
  }
  return NOPOST;
}

/*  Sometimes we have a potential post that doesn't quite seem right - e.g. it is too fat and might just be a
 * backstop.  So we need to check it out.  What we do is grab the big color blob that it is a part of and analyze
 * its relationship to that blob.  In some cases we can call it a post and a backstop.  Otherwise we say that we
 * don't know and the temptation will be to call it a backstop.
 * @param big         the big blob
 * @param small       the potential post
 * @param c           the color we're processing
 * @param c2          its neighbor color (e.g. blue - bluegreen,  yellow - yellowwhite).
 * @param left        the left post of that color
 * @param mid         the backstop of that color
 * @param right       the right post of that color
 * @return            whether or not we concluded we had a viable post
 */
// Check if one blob is inside the other, if so, then we may have a post and a backstop
bool ObjectFragments::contains(blob big, blob small, int c, int c2, VisualFieldObject* left, VisualFieldObject* right, VisualFieldObject* mid) {
  // first just check if its reasonable
  int leftGap = small.leftTop.x - big.leftTop.x;
  int rightGap = big.rightTop.x - small.rightTop.x;
  if ((leftGap > -2 || small.leftTop.x < 0) && rightGap > -2 && (big.leftTop.y <= small.leftTop.y || small.leftTop.y < 0) && big.leftBottom.y >= small.leftBottom.y) {
    // ok we have it, now let's classify what the relationship is
    int bw = blobWidth(big);
    int sw = blobWidth(small);
    int sh = blobHeight(small);
    int bh = blobHeight(big);
    if (bw  >= sw + 10 && sh > bh * 3 / 4) {
      // we probably have a post and backstop situation - make a new blob out of the difference
      if (leftGap > rightGap) {
	// we're on the left side of the post
	squareGoal(min(small.leftTop.x - 1, big.leftTop.x + bw / 4), small.leftTop.y + sh / 3, c, c2, big.leftTop.x, small.leftTop.x, POST);
	if (POSTDEBUG)
	  drawPoint(min(small.leftTop.x - 1, big.leftTop.x + bw / 4), small.leftTop.y + sh / 3, RED);
	if (blobOk(obj) && (obj.leftBottom.y - obj.leftTop.y) < sh * 0.75) {
	  // we seem to have a post-backstop situation - backstop now becomes the post
	  updateObject(left, pole, SURE, HEIGHT_UNSURE);
	  // just "subtract" post from topblob
	  topBlob.rightTop.x = pole.leftTop.x;
	  topBlob.rightBottom.x = pole.leftBottom.x;
	  int px = midPoint(topBlob.leftTop.x, topBlob.rightTop.x);
	  int last = topBlob.leftTop.y;
	  bool topFound = false;
	  for (int i = topBlob.leftTop.y; i < topBlob.leftBottom.y; i++) {
	    int pix = thresh->thresholded[i][px];
	    if (pix == c) {
	      if (!topFound) {
		topFound = true;
		topBlob.leftTop.y = i;
		topBlob.rightTop.y = i;
	      } else {
		last = i;
	      }
	    }
	  }
	  topBlob.leftBottom.y = last;
	  topBlob.rightBottom.y = last;
	  updateBackstop(mid, topBlob);
	  return true;
	}
      } else {
	// we're on the right side of the post
	squareGoal(min(big.rightTop.x, small.rightTop.x + bw / 4), small.leftTop.y + sh / 3, c, c2, small.rightTop.x, big.rightTop.x, POST);
	if (POSTDEBUG)
	  drawPoint(min(big.rightTop.x, small.rightTop.x + bw / 4), small.leftTop.y + (small.leftBottom.y - small.leftTop.y) / 3, RED);
	if (blobOk(obj) && (obj.leftBottom.y - obj.leftTop.y) < sh * 0.75) {
	  updateObject(right, pole, SURE, HEIGHT_UNSURE);
	  topBlob.leftTop.x = pole.rightTop.x;
	  topBlob.leftBottom.x = pole.rightBottom.x;
	  int px = midPoint(topBlob.leftTop.x, topBlob.rightTop.x);
	  int last = topBlob.leftTop.y;
	  bool topFound = false;
	  for (int i = topBlob.leftTop.y; i < topBlob.leftBottom.y; i++) {
	    int pix = thresh->thresholded[i][px];
	    if (pix == c) {
	      if (!topFound) {
		topFound = true;
		topBlob.leftTop.y = i;
		topBlob.rightTop.y = i;
	      } else {
		last = i;
	      }
	    }
	  }
	  topBlob.leftBottom.y = last;
	  topBlob.rightBottom.y = last;
	  updateBackstop(mid, topBlob);
	  return true;
	}
      }
    }
  }
  return false;
}

/*  In our ongoing attempt to determine which post we're looking at we have hatched a crazy scheme in
 * which we project boxes on either side of the post.  We then look at points where we have runs and try
 * to determine whether or not those points are actually in the boxes.  This is actually much harder
 * than it seems like it should be (mostly because the true boxes may be on an angle).  This method isn't
 * actually a solution to the problem, more like an approximation.  It handles the easy cases anyway.
 * @param b      the blob we're checking
 * @param x      the x coord of the top of the run we'd like to be in the box
 * @param y      its y coord
 * @param h      the height of the run
 * @return       true if we think its partially in the box, false otherwise
 */

bool ObjectFragments::inBlob(blob b, int x, int y, int h) {
  if (!((b.leftTop.x < x && x < b.rightTop.x) || (b.leftBottom.x < x && x < b.rightBottom.x)))
    return false;
  if (!(((b.leftTop.y < y || b.leftTop.y < y + h) && y < b.leftBottom.y) || ((b.rightTop.y < y || b.rightTop.y < y+h) && y < b.rightBottom.y)))
    return false;
  if (x > max(b.leftTop.x, b.leftBottom.x) && x < min(b.rightTop.x, b.rightBottom.x)) {
    if (y + h > min(b.leftTop.y, b.rightTop.y) && y < max(b.leftBottom.y, b.rightBottom.y))
      return true;
    return false;
  }
  return false;
}

/*  In our neverending quest to determine which post we're looking at, one method involves
 * projecting boxes on either side of the potential post.  We then see if we have lots of
 * runs of color in either or both of the boxes.  If we have lots in one and not the other
 * it is good evidence that we can ID the post properly.
 * @param spanX       How big the post is in the X direction
 * @param spanY       Same for y
 * @param howbig      relative size of the post
 */

void ObjectFragments::makeBoxes(int spanX, int spanY, int howbig) {
  // let's figure out the bounds of where other parts of the goal might be
  leftBox.leftTop.x = pole.leftTop.x - spanY * 3;
  leftBox.leftTop.y = yProject(pole.leftTop, leftBox.leftTop.x);
  leftBox.rightTop.x = pole.leftTop.x - min(spanX, 10);
  leftBox.rightTop.y = yProject(pole.leftTop, leftBox.rightTop.x);
  leftBox.leftBottom.x = pole.leftBottom.x - spanY * 3;
  leftBox.leftBottom.y = yProject(pole.leftBottom, leftBox.leftBottom.x);
  leftBox.rightBottom.x = pole.leftBottom.x - min(spanX, 10);
  leftBox.rightBottom.y = yProject(pole.leftBottom, leftBox.rightBottom.x);
  rightBox.rightTop.x = pole.rightTop.x + spanY * 3;
  rightBox.rightTop.y = yProject(pole.rightTop, rightBox.rightTop.x);
  rightBox.leftTop.x = pole.rightTop.x + min(10, spanX);
  rightBox.leftTop.y = yProject(pole.rightTop, rightBox.leftTop.x);
  rightBox.rightBottom.x = pole.rightBottom.x + spanY * 3;
  rightBox.rightBottom.y = yProject(pole.rightBottom, rightBox.rightBottom.x);
  rightBox.leftBottom.x = pole.rightBottom.x + min(10, spanX);
  rightBox.leftBottom.y = yProject(pole.rightBottom, rightBox.leftBottom.x);
  if (howbig == SMALL) {
    leftBox.leftTop.x = xProject(leftBox.leftTop, leftBox.leftTop.y + spanY / 2);
    leftBox.leftTop.y += spanY / 2;
    leftBox.rightTop.x = xProject(leftBox.rightTop, leftBox.rightTop.y + spanY / 2);
    leftBox.rightTop.y += spanY / 2;
    rightBox.leftTop.x = xProject(rightBox.leftTop, rightBox.leftTop.y + spanY / 2);
    rightBox.leftTop.y += spanY / 2;
    rightBox.rightTop.x = xProject(rightBox.rightTop, rightBox.rightTop.y + spanY / 2);
    rightBox.rightTop.y += spanY / 2;
  }
  if (POSTLOGIC) {
    drawBlob(rightBox, BLACK);
    drawBlob(leftBox, BLACK);
  }
}

/* First of a bunch of methods where we try and classify a post as being LEFT, RIGHT or NOPOST.
 * In this one we look to the left and right of the post for horizontal scanlines that might
 * correspond to a backstop or a crossbar.  Either would be good evidence of which post we have.
 * @param stopp      stopping point (y dim) of our scans
 * @param spanX      width of the potential post
 * @param c          current color
 * @return           post classification if found
 */

int ObjectFragments::scanOut(int stopp, int spanX, int c) {
  int _checker = NOPOST;
  int lscan = 0, rscan = 0;
  // scan out from the bottoms
  int bestl = 0, bestr = 0;
  stopp = stopp / 2;
  for (int i = 0; i < stopp; i+= 3) {
    int sx = xProject(pole.leftBottom, pole.leftBottom.y - i);
    //drawPoint(sx, pole.leftBottom.y - i, BLACK);
    horizontalScan(sx, pole.leftBottom.y - i, -1, 15, c, c, 0, sx + 1);
    lscan = scan.good;
    if (lscan > bestl)
      bestl = lscan;
    sx = xProject(pole.rightBottom, pole.rightBottom.y - i);
    horizontalScan(sx, pole.rightBottom.y - i, 1, 15, c, c, sx - 1, IMAGE_WIDTH -1);
    //cout << scan.good << endl;
    rscan = scan.good;
    if (rscan > bestr)
      bestr = rscan;
  }
  if (POSTLOGIC)
    cout << "Scans " << bestl << " " << bestr << endl;
  /*if (bestl > bestr + 3 && bestl > max(8, min(spanX, 30)) && (bestr < 20 || bestr < bestl / 4) && spanX > 15) {
    // before declaring this, let's make sure we're not looking at the side of the goal
    int whites = 0;
    for (int i = 0; i < stopp; i+= 3) {
      int sx = xProject(pole.leftBottom, pole.leftBottom.y - i);
      //drawPoint(sx, pole.leftBottom.y - i, BLACK);
      horizontalScan(sx, pole.leftBottom.y - i, -1, 15, WHITE, WHITE, 0, sx + 1);
      lscan = scan.good;
      if (lscan > whites)
	whites = lscan;
    }
    if (bestl > whites + 5) {
      _checker = RIGHT;
    } else {
      _checker = LEFT;
      if (POSTLOGIC) {
	cout << "Found side of goal" << endl;
      }
    }
  } else if (bestr > bestl + 3 && bestr > max(8, min(spanX, 30)) && (bestl < 20 || bestl < bestr / 4) && spanX > 15) {
    int whites = 0;
    for (int i = 0; i < stopp; i+= 3) {
      int sx = 0;
      sx = xProject(pole.rightBottom, pole.rightBottom.y - i);
      horizontalScan(sx, pole.rightBottom.y - i, 1, 15, WHITE, WHITE, sx - 1, IMAGE_WIDTH -1);
      //cout << scan.good << endl;
      rscan = scan.good;
      if (rscan > whites)
	whites = rscan;
    }
    if (bestr > whites + 5) {
      _checker = LEFT;
    } else {
      if (POSTLOGIC) {
	cout << "Found side of goal" << endl;
      }
      _checker = RIGHT;
    }
    }*/
  return _checker;
}

/* Another post classification method.  If we get to this one we're getting fairly desparate.
 * Essentially we project boxes to the left and right of the post and check how much color
 * is in those boxes.  Presumably such color comes from backstops, other posts, etc.
 * @param spanX       width of the post
 * @param spanY       height of the post
 * @param howbig      relative size of the post
 * @param fakebottom  y value of the bottom of the post
 * @param trueRight   largest x value of the post
 * @param trueLeft    smallest x value of the post
 * @param trueTop     y value of the top of the post
 * @return            post classification
 */

int ObjectFragments::projectBoxes(int spanX, int spanY, int howbig, int fakeBottom, int trueRight, int trueLeft, int trueTop) {
  int result = NOPOST;
  // let's figure out the bounds of where other parts of the goal might be
  makeBoxes(spanX, spanY, howbig);
  int largel = 0;
  int larger = 0;
  //int nextB = 0;
  int allRights = 0, allLefts = 0, rSum = 0, lSum = 0, lefts = 0, rights = 0;
  int bigl = 0, bigr = 0;
  for (int i = 0; i < numberOfRuns; i++) {
    int nextX = runs[i].x;
    int nextY = runs[i].y;
    int nextH = runs[i].h;
    //nextB = nextY + nextH;
    if (nextH > 0) { // meanwhile collect some information on which post we're looking at
      if (nextX > trueRight + 10)
	allRights++;
      else if (nextX < trueLeft - 10)
	allLefts++;
      // the run needs to be sort of far away, but not too far, it also can't be too high up
      if (inBlob(rightBox, nextX, nextY, nextH)) {
	if (nextH > larger && nextX - trueRight > spanY)
	  larger = nextH;
	if (nextH > bigr)
	  bigr = nextH;
	rights++;
	rSum += nextH;
      } else if (inBlob(leftBox, nextX, nextY, nextH)) {
	lefts++;
	if (nextH > largel && trueLeft - nextX > spanY)
	  largel = nextH;
	if (nextH > bigl)
	  bigl = nextH;
	lSum += nextH;
      }
    }
  }
  // Now let's see if we have learned enough to classify the post
  // if we have a possible run outside the post that is big relative to this post, then that's gold
  if (larger > (fakeBottom - trueTop) / 2 && larger > largel) {
    if (POSTLOGIC)
      cout << "Larger" << endl;
    return LEFT;
  } else if (largel > (fakeBottom - trueTop) / 2) {
    if (POSTLOGIC) cout << "Largel" << endl;
    return RIGHT;
    // next we check if we have lots of runs on either side
  } else if (lefts > 5 && rights > 5) {
    // we're basically screwed - but try anyway
    if (POSTLOGIC)
      cout << "in here " << bigr << " " << bigl << endl;
    if (bigr > 2 * bigl) {
      if (POSTLOGIC)
	cout << "Found a bigger one on the left" << endl;
      return LEFT;
    } else if (bigl > 2 * bigr) {
      if (POSTLOGIC)
	cout << "FOUND a bigger one on the right" << endl;
      return RIGHT;
    // ideally we have many more on one side than the other
    } else if (lefts > spanX && lefts > rights) {
      return RIGHT;
    } else if (rights > spanX) {
      return LEFT;
    }
    // see if we have a preponderance on one side
  } else if (lefts > spanX && ((lSum > rSum && lefts > rights) || (lefts > 2 * rights) || (lSum > 2 * rSum))) {
    if ((rights == 0 || rights > 15) && trueRight - trueLeft > BIGPOST) {
      int px = pole.leftBottom.x - spanX / 3;
      int py = pole.leftBottom.y - spanY / 4;
      horizontalScan(px, py, -1, 3, WHITE, ORANGEYELLOW, px - 11, px);
      if (scan.good > 3) {
	if (POSTLOGIC) cout << "Left scan successful" << endl;
	return LEFT;
      } else {
	if (POSTLOGIC) cout << "Left scan found " << scan.good << endl;
	return RIGHT;
      }
    } else {
      return RIGHT;
    }
  } else if ((rights > spanX) && ((rSum > lSum && rights > lefts) || (rights > 2 * lefts) || (rSum > 2 * lSum))) {
    if ((lefts == 0 || lefts > 15) && spanX > BIGPOST) {
      int px = pole.rightBottom.x + spanX / 3;
      int py = pole.rightBottom.y - spanY / 4;
      horizontalScan(px, py, 1,  3, WHITE, ORANGEYELLOW, px - 1, px + 10);
      if (POSTLOGIC) drawPoint(px, py, RED);
      if (scan.good > 3) {
	if (POSTLOGIC) cout << "RIght scan successful" << endl;
	return RIGHT;
      } else {
	if (POSTLOGIC) cout << "Right scan found " << scan.good << endl;
	return LEFT;
      }
    } else {
      if (POSTLOGIC) cout << "here " << rSum << " " << rights << " " << lSum << " " << lefts << endl;
      return LEFT;
    }
  }
  return result;
}

/* Yet another method focussed on determining which post we're looking at.  Essentially we scan
 * out in each direction from the bottom of the post.  The idea is that if a backstop is present
 * on one side then we can definitively ID the post.  Of course things are never that simple.
 * For example if you are looking at a post from the side you'll see this situation, but potentially
 * mis-ID the post.
 * @param howbig    the relative size of the post
 * @return          a constant with our guess - LEFT, RIGHT, or NOPOST (i.e. we don't know)
 */
int ObjectFragments::scanLogic(int howbig) {
  int spanX = blobWidth(pole);
  int spanY = blobHeight(pole);
  // scan out from the bottoms
  int lscan = 0, rscan = 0;
  int bestl = 0, bestr = 0;
  int stopp = spanY / 2;
  if (howbig == LARGE)
    stopp = spanY;
  for (int i = 0; i < stopp; i+= 3) {
    int sx = xProject(pole.leftBottom, pole.leftBottom.y - i);
    horizontalScan(sx, pole.leftBottom.y - i, -1, 5, color, color, 0, sx + 1);
    lscan = scan.good;
    if (lscan > bestl)
      bestl = lscan;
    sx = xProject(pole.rightBottom, pole.rightBottom.y - i);
    horizontalScan(sx, pole.rightBottom.y - i, 1, 5, color, color, sx - 1, IMAGE_WIDTH -1);
    //cout << scan.good << endl;
    rscan = scan.good;
    if (rscan > bestr)
      bestr = rscan;
  }
  if (POSTLOGIC)
    cout << "Scans " << bestl << " " << bestr << " " << spanX << endl;
  if (bestl > bestr + 3 && bestl > max(8, min(spanX, 30)) && (bestr < 20 || bestl > 2 * bestr)) {
    return RIGHT;
  } else if (bestr > bestl + 3 && bestr > max(8, min(spanX, 30)) && (bestl < 20 || bestr > 2 * bestl)) {
    return LEFT;
  }
  return NOPOST;
}

/*  Sometimes we use relative blob comparisons to determine information about a post.  Usually its
 * the potential post and the biggest blob of that color.  This method looks at those items and
 * determines their basic spatial relationship.
 * @param post     the potential post
 * @param big      the biggest blob
 * @return         a constant indicating the relationship - OUTSIDE, TIGHT, CLOSELEFT, CLOSERIGHT, or MURKY
 */

int ObjectFragments::determineRelationship(blob post, blob big) {
  if (post.rightTop.x < big.leftTop.x || post.leftTop.x > big.rightTop.x) return OUTSIDE;
  int bigSpan = big.rightTop.x - big.leftTop.x;
  int smallSpan = post.rightTop.x - post.leftTop.x;
  if (POSTDEBUG) {
    cout << "Determining relationship of blobs " << bigSpan << " " << smallSpan << endl;
    drawBlob(big, ORANGE);
  }
  if (smallSpan > bigSpan * 2 / 3) return TIGHT;
  if (post.leftTop.x - big.leftTop.x < 10 || post.leftTop.x - big.leftTop.x < bigSpan / 4) return CLOSELEFT;
  if (big.rightTop.x - post.rightTop.x < 10 || big.rightTop.x - post.rightTop.x < bigSpan / 4) return CLOSERIGHT;
  return MURKY;
}

#endif

/*  Sometimes we process posts differently depending on how big they are.  This just characterizes a post's
 * size such that we can make that determination.
 * @param b   the post in question
 * @return    a constant indicating size - SMALL, MEDIUM, or LARGE
 */

int ObjectFragments::characterizeSize(blob b) {
  int w = b.rightTop.x - b.leftTop.x + 1;
  int h = b.leftBottom.y - b.leftTop.y + 1;
  if (h > 60) return LARGE;
  if (h < 20 || w < 10) return SMALL;
  if (h < 40 || w < 20) return MEDIUM;
  return LARGE;
}

/* Make sure this isn't the blue ring of death.
 * @param b the post in question
 * @param c the color of the post
 * @return do we think it is ok?
 */

bool ObjectFragments::goodPost(blob b, int c) {
  int good = 0, bad = 0;
  int pix = GREEN;
  for (int i = b.leftTop.x; i < b.rightTop.x; i++)
    for (int j = b.leftTop.y; j < b.leftBottom.y; j++)
      pix = thresh->thresholded[j][i];
      if (pix == c)
        good++;
      else if (pix == GREEN)
        bad++;
  if (ARCDEBUG) {
    print("Good %i %d", good,
          ((b.rightTop.x - b.leftTop.x) * (b.leftBottom.y - b.leftTop.y) / 3));
  }
  if  (good < (b.rightTop.x - b.leftTop.x) * (b.leftBottom.y - b.leftTop.y) / 2)
    return false;
  if (bad > (b.rightTop.x - b.leftTop.x) * (b.leftBottom.y - b.leftTop.y) / 5)
    return false;
  return true;

}

/* Make sure that the "post" we are looking at isn't just a colored field arc.  Just make sure that
 * it isn't mostly another color (green probably).
 * @param b   the post in question
 * @param c   the color of the post
 * @return    do we think its ok?
 */

bool ObjectFragments::coloredArc(blob b, int c) {
  int good = 0, green = 0, pix = GREEN;
  for (int i = b.leftTop.x; i < b.rightTop.x; i++) {
    for (int j = b.leftTop.y; j < b.leftBottom.y; j++) {
      pix = thresh->thresholded[j][i];
      if (pix == c)
        good++;
      else if (pix == GREEN)
        green++;
    }
  }
  int area = blobArea(b);
  if (ARCDEBUG) {
    drawBlob(b, BROWN);
    cout << "Good " <<  good << " " << green << " " <<
      (area / 3) << endl;
  }
  if (green < area / 3)
    return false;
  if  (good < area / 3)
    return true;
  // could still be an arc
  if (c == BLUE && b.leftBottom.y > IMAGE_HEIGHT - 4 && (b.leftTop.x < 15 || b.rightTop.x > IMAGE_WIDTH - 15) &&
      blobWidth(b) < 40) {
    return false;
  }
  if (ARCDEBUG) {
    print("%i %i %i %i %i", b.leftBottom.y, b.leftTop.x, b.rightTop.x,
          (b.rightTop.x - b.leftTop.x), IMAGE_HEIGHT);
  }
  good = 0;
  for (int i = b.leftTop.x; i < b.rightTop.x; i++)
    for (int j = b.leftTop.y; j < b.leftTop.y + 2; j++)
      if (thresh->thresholded[j][i] == GREEN)
        good++;
  if (ARCDEBUG) {
    print("Good 2 %i %i", good, (b.rightTop.x - b.leftTop.x));
  }
  return good > (b.rightTop.x - b.leftTop.x);
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



/*  Sometimes we have a potential post that doesn't quite seem right - e.g. it is too fat and might just be a
 * backstop.  So we need to check it out.  What we do is grab the big color blob that it is a part of and analyze
 * its relationship to that blob.  In some cases we can call it a post and a backstop.  Otherwise we say that we
 * don't know and the temptation will be to call it a backstop.  This works with "contains" which has a very similar
 * description.
 * @param rat         the height-width ratio that is in question
 * @param beaconFound   have we found a beacon?  If not we may be looking at one
 * @param c           the color we're processing
 * @param c2          its neighbor color (e.g. blue - bluegreen,  yellow - yellowwhite).
 * @param horizon     the green horizon line
 * @param left        the left post of that color
 * @param mid         the backstop of that color
 * @param right       the right post of that color
 * @param post        the potential post
 * @param big         the big blob
 * @return            whether or not we concluded we had a viable post
 */
#if ROBOT(AIBO)

bool ObjectFragments::checkPostAndBlob(double rat, bool beaconFound, int c, int c2, int horizon, VisualFieldObject* left, VisualFieldObject* mid, VisualFieldObject* right, blob post, blob big) {
  // check for a T nearby
  const list <VisualCorner>* corners = vision->fieldLines->getCorners();
  for (list <VisualCorner>::const_iterator k = corners->begin();
       k != corners->end(); k++) {
    if (k->getShape() == T) {
      if (POSTDEBUG) {
	cout << "Got a T in checkPostBlob" << endl;
      }
      int x = k->getX();
      int y = k->getY();
      if (!beaconFound && y > post.leftBottom.y + (post.rightBottom.x - post.leftBottom.x)) {
	if (POSTDEBUG) {
	  cout << "We have ourselves a beacon" << endl;
	}
	if (c == YELLOW) {
	  inferBeaconFromBlob(post, vision->by);
	} else {
	  inferBeaconFromBlob(post, vision->yb);
	}
	return false;
      } else if (y < post.leftBottom.y) {
	if (POSTDEBUG) {
	  cout << "Got a T corner nearby" << endl;
	}
	if (x > post.rightBottom.x && big.rightTop.y < 1) {

	}
      }
    } else {
      if (POSTDEBUG) {
	cout << "Got a corner in post and blob" << endl;
      }
    }
  }
  // not a post - probably between element
  if (POSTDEBUG) {
    cout << "Checking squat post" << endl;
    drawBlob(post, ORANGE);
  }
  int spanX = blobWidth(post);
  int spanY = blobHeight(post);
  if (rat < SQUATRAT)  {
    if (!beaconFound && rat > 0.8) {
      if (POSTDEBUG) {
	cout << "Checking for beacon" << endl;
      }
      // let's see if we can find the post this way
      int lb = min(post.leftTop.x, post.leftBottom.x);
      int rb = max(post.rightTop.x, post.rightBottom.x);
      int greens = 0;
      if (c == YELLOW) {
	beaconFound = processBeacon(lb, rb, midPoint(lb, rb), midPoint(post.leftTop.y, post.leftBottom.y), true, false);
	if (!beaconFound) {
	  beaconFound = processBeacon(lb, rb, midPoint(lb, rb), midPoint(post.leftTop.y, post.leftBottom.y), false, false);
	}
      } else {
	beaconFound = processBeacon(lb, rb, midPoint(lb, rb), post.leftTop.y, true, false);
	if (!beaconFound) {
	  beaconFound = processBeacon(lb, rb, midPoint(lb, rb), post.leftBottom.y, false, false);
	}
      }
      if (beaconFound) {
	if (POSTDEBUG) {
	  cout << "Found a beacon!" << endl;
	}
	return true;
      }
    }
    if (contains(big, post, c, c2, left, right, mid)) {
      if (POSTDEBUG) {
	cout << "Contains was true" << endl;
      }
      return true;
    }
  }
  if (POSTDEBUG) {
    cout << "Determining relationships" << endl;
  }
  int spanY2 = blobHeight(big);
  int spanX2 = blobWidth(big);
  if (spanY2 / 2 > spanY) {
    if (POSTDEBUG) {
      drawBlob(post, ORANGE);
      cout << "Possible post was much shorter than biggest blob" << endl;
    }
    if (locationOk(big, horizon) && spanY2 < spanX2) {
      updateBackstop(mid, big);
    }
    return true;
  }
  int where = determineRelationship(post, big);
  int last = 0, first = 0, lastFirst = 0, lastLast = 0, greeng = 0, white = 0;
  bool stop = false;
  switch (where) {
  case OUTSIDE:
    if (POSTDEBUG) {
      cout << "Found a bad ratio blob, but it isn't in the biggest blob" << endl;
    }
    if (spanY / 2 < spanX2 && !post.leftTop.y < 1) {
      if (locationOk(big, horizon)) {
	updateBackstop(mid, big);
	return false;
      }
    }
    break;
  case CLOSELEFT:
    if (POSTDEBUG) {
      cout << "Found a bad ratio blob on the left side of the biggest blob" << endl;
    }
    if (locationOk(big, horizon) && greenCheck(big)) {
      // before we declare a backstop, let's poke around a little and see if we can find a post
      // what we have is a wide swath of goal - if it ends abruptly on either side it could signal a post
      // scan along the bottom looking for abrupt change
      if (POSTDEBUG) {
	cout << "Checking " << post.leftTop.y << " " << post.rightTop.y << endl;
      }
      if (post.leftTop.y < 2 && post.rightTop.y < 2 && abs(post.leftTop.x - big.leftTop.x) < 3) {
	if (POSTDEBUG) {
	  cout << "Its at the top, so it is probably a goal and backstop" << endl;
	}
	// Theoretically our squareGoal method handled the separation well
	big.leftTop.x = post.rightTop.x - 1;
	big.leftBottom.x = post.rightTop.x - 1;
	updateObject(right, post, SURE, HEIGHT_UNSURE);
	updateBackstop(mid, big);
	return true;
      }
      lastLast = big.leftBottom.y; lastFirst = -1;
      int stopper = min(IMAGE_WIDTH - 1, max(midPoint(big.leftTop.x, big.rightTop.x), post.rightTop.x + 10));
      //cout << stopper << endl;
      for (int i = big.leftTop.x + 10; !stop && i < stopper; i++) {
	first = -1;
	greeng = 0;
	for (int j = big.leftTop.y; j < big.leftBottom.y; j++) {
	  int pix = thresh->thresholded[j][i];
	  if (pix == c || pix == c2) {
	    last = j;
	    if (first == -1)
	      first = j;
	  } else if (pix == GREEN) {
	    greeng++;
	  } else if (pix == WHITE && j > midPoint(post.leftTop.y, post.leftBottom.y) && i > post.rightTop.x) {
	    white++;
	    if (white > 5 && greeng == 0)
	      stop = true;
	  }
	}
	//cout << last << " " << first << " " << lastLast << " " << lastFirst << endl;
	if ((first - lastFirst > 4 || lastLast - last > 4) && greeng > 1 && lastFirst != -1 && !stop) {
	  if (POSTDEBUG) {
	    cout << "We have ourselves a post!" << endl;
	    drawPoint(i, last, PINK);
	  }
	  int tbr = big.rightTop.x;
	  big.rightTop.x = i - 1;
	  big.rightBottom.x = i - 1;
	  updateObject(right, big, SURE, HEIGHT_UNSURE);
	  big.rightTop.x = tbr;
	  big.rightBottom.x = tbr;
	  big.leftTop.x = i;
	  big.leftBottom.x = i;
	  for (int j = big.leftTop.y; j < big.leftBottom.y; j++) {
	    int pix = thresh->thresholded[j][midPoint(big.leftTop.x, big.rightTop.x)];
	    if (pix == c || pix == c2) {
	      last = j;
	      if (first == -1)
		first = j;
	    }
	  }
	  big.leftTop.y = first;
	  big.rightTop.y = first;
	  big.leftBottom.y = last;
	  big.rightBottom.y = last;
	  updateBackstop(mid, big);
	  return true;
	}
	lastLast = last; lastFirst = first;
      }
      first = -1;
      for (int j = big.leftTop.y; j < big.leftBottom.y; j++) {
	int pix = thresh->thresholded[j][midPoint(big.leftTop.x, big.rightTop.x)];
	if (pix == c || pix == c2) {
	  last = j;
	  if (first == -1)
	    first = j;
	}
      }
      big.leftTop.y = first;
      big.rightTop.y = first;
      big.leftBottom.y = last;
      big.rightBottom.y = last;
      updateBackstop(mid, big);
      return true;
    }
    break;
  case CLOSERIGHT:
    if (POSTDEBUG) {
      cout << "Found a bad ratio blob on the right side of the biggest blob" << endl;
    }
    if (locationOk(big, horizon) && greenCheck(big)) {
      // before we declare a backstop, let's poke around a little and see if we can find a post
      // what we have is a wide swath of goal - if it ends abruptly on either side it could signal a post
      // scan along the bottom looking for abrupt change
      if (POSTDEBUG) {
	cout << "Checking " << post.leftTop.y << " " << post.rightTop.y << endl;
      }
      if (post.leftTop.y < 2 && post.rightTop.y < 2 ) {
	if (POSTDEBUG) {
	  cout << "Its at the top, so it is probably a goal and backstop" << endl;
	}
	// Theoretically our squareGoal method handled the separation well
	big.rightTop.x = post.leftTop.x - 1;
	big.rightBottom.x = post.leftTop.x - 1;
	updateObject(left, post, SURE, HEIGHT_UNSURE);
	updateBackstop(mid, big);
	return true;
      }
      lastLast = big.leftBottom.y; lastFirst = -1;

      for (int i = big.rightTop.x - 10; !stop && i > big.leftTop.x + blobWidth(big) / 3; i--) {
	first = -1;
	greeng = 0;
	for (int j = big.leftTop.y; j < big.leftBottom.y; j++) {
	  int pix = thresh->thresholded[j][i];
	  if (pix == c || pix == c2) {
	    last = j;
	    if (first == -1)
	      first = j;
	  } else if (pix == GREEN) {
	    greeng++;
	  } else if (pix == WHITE && j > midPoint(big.leftTop.y, big.leftBottom.y) && i < post.leftTop.x && j < big.leftBottom.y - 5) {
	    white++;
	    if (white > 5 && greeng == 0) {
	      stop = true;
	      if (POSTDEBUG) {
		drawPoint(i, j, PINK);
	      }
	    }
	  }
	}
	//cout << last << " " << first << " " << lastLast << " " << lastFirst << endl;
	if ((first - lastFirst > 4 || lastLast - last > 4) && greeng > 1 && lastFirst != -1 && !stop) {
	  if (POSTDEBUG) {
	    cout << "We have ourselves a post!" << endl;
	    drawPoint(i, last, PINK);
	  }
	  int tbl = big.leftTop.x;
	  big.leftTop.x = i + 1;
	  big.leftBottom.x = i + 1;
	  updateObject(left, big, SURE, HEIGHT_UNSURE);
	  big.leftTop.x = tbl;
	  big.leftBottom.x = tbl;
	  big.rightTop.x = i;
	  big.rightBottom.x = i;
	  first = -1;
	  for (int j = big.leftTop.y; j < big.leftBottom.y; j++) {
	    int pix = thresh->thresholded[j][midPoint(big.leftTop.x, big.rightTop.x)];
	    if (pix == c || pix == c2) {
	      last = j;
	      if (first == -1)
		first = j;
	    }
	  }
	  big.leftTop.y = first;
	  big.rightTop.y = first;
	  big.leftBottom.y = last;
	  big.rightBottom.y = last;
	  // one last check
	  double newrat = (double)blobWidth(big) / (double)blobHeight(big);
	  if (blobHeight(big) > spanY * 2 / 3 || newrat > SQUATRAT) {
	    // whoops!
	    left->init();
	  } else {
	    updateBackstop(mid, big);
	    return true;
	  }
	}
	lastLast = last; lastFirst = first;
      }
      first = -1;
      for (int j = big.leftTop.y; j < big.leftBottom.y; j++) {
	int pix = thresh->thresholded[j][midPoint(big.leftTop.x, big.rightTop.x)];
	if (pix == c || pix == c2) {
	  last = j;
	  if (first == -1)
	    first = j;
	}
      }
      big.leftTop.y = first;
      big.rightTop.y = first;
      big.leftBottom.y = last;
      big.rightBottom.y = last;
      updateBackstop(mid, big);
      return true;
    }
    break;
  case TIGHT:
  case MURKY:
    if (POSTDEBUG) {
      cout << "Found a bad ratio blob, and its sort of in the middle of a big blob" << endl;
    }
    // let's check if it is at the top
    if (post.leftTop.y <= 0 || post.rightTop.y <= 0) {
      // scan the top and see if there is a big-gap - sort of like what we did above
      if (POSTDEBUG) {
	cout << "Checking top" << endl;
      }
      bool firstrun = false;
      int run = 0, lastindex = BADVALUE, index = BADVALUE, badrun = 0;
      int stopper = 0;
      for (int i = min(post.leftTop.x, post.leftBottom.x); i < max(post.rightTop.x, post.rightBottom.x); i++) {
	int pix = thresh->thresholded[0][i];
	int pix2 = thresh->thresholded[1][i];
	if (pix == c || pix == c2 || pix2 == c || pix2 == c2) {
	  run++;
	  if (run > 1)
	    badrun = 0;
	  if (!firstrun && run > 3) {
	    firstrun = true;
	    index = i - run;
	  }
	  if (run > 3)
	    lastindex = i;
	} else {
	  badrun++;
	  if (badrun > 1)
	    run = 0;
	}
      }
      if (POSTDEBUG) {
	cout << "indices " << post.leftTop.x << " " << post.rightTop.x << " " << index << " " << lastindex << endl;
      }
      if (index > post.leftTop.x + (post.rightTop.x - post.leftTop.x) / 2 || (index > big.leftTop.x + 4 && abs(lastindex - post.rightTop.x) < 3)) {
	// post on right side?
	if (POSTDEBUG) {
	  drawPoint(index, 0, RED);
	  cout << "Found a post on the right" << endl;
	}
	int old = post.leftTop.x;
	post.leftTop.x = index;
	post.leftBottom.x = index;
	updateObject(left, post, SURE, HEIGHT_UNSURE);
	big.rightTop.x = index - 1;
	big.rightBottom.x = index - 1;
	updateBackstop(mid, big);
	return true;
      } else if (lastindex != BADVALUE && lastindex < post.leftTop.x + blobWidth(post) / 2 ||
		 (index < post.leftTop.x + 3 && big.rightTop.x - post.rightTop.x > 5)) {
	// post on left side?
	if (POSTDEBUG) {
	  drawPoint(lastindex, 0, BLACK);
	  cout << "Got a left sider" << endl;
	}
	int old = post.rightTop.x;
	post.rightTop.x = lastindex;
	post.rightBottom.x = lastindex;
	updateObject(right, post, SURE, HEIGHT_UNSURE);
	big.leftTop.x = lastindex + 1;
	big.leftBottom.x = lastindex + 1;
	updateBackstop(mid, big);
      }
    }
    // let's not give up yet

    if (locationOk(big, horizon)) {
      if (POSTDEBUG) {
	cout << "Setting backstop" << endl;
      }
      updateBackstop(mid, big);
      return true;
    } else if (POSTDEBUG) {
      cout << "Bad location" << endl;
    }
    break;
  }
  if (spanX > 20 && spanY > 10 && greenCheck(post)) {
    // there is still a chance its a post - check the side-bottoms for green
    //cout << "Checking here " << rat << endl;
    if (rat < SQUATRAT) {
      if (greenSide(post)) {
	// we may need to do a different blob calculation later
	if (POSTDEBUG) {
	  cout << "Got green" << endl;
	}
	numBlobs = 0;
      } else if (locationOk(big, horizon) && greenCheck(big)) {
	first = -1;
	for (int j = big.leftTop.y; j < big.leftBottom.y; j++) {
	  int pix = thresh->thresholded[j][midPoint(big.leftTop.x, big.rightTop.x)];
	  if (pix == c || pix == c2) {
	    last = j;
	    if (first == -1)
	      first = j;
	  }
	}
	big.leftTop.y = first;
	big.rightTop.y = first;
	big.leftBottom.y = last;
	big.rightBottom.y = last;
	updateBackstop(mid, big);
	return true;
      }
    } else if (locationOk(big, horizon) && greenCheck(big)) {
      first = -1;
      for (int j = big.leftTop.y; j < big.leftBottom.y; j++) {
	int pix = thresh->thresholded[j][midPoint(big.leftTop.x, big.rightTop.x)];
	if (pix == c || pix == c2) {
	  last = j;
	  if (first == -1)
	    first = j;
	}
      }
      big.leftTop.y = first;
      big.rightTop.y = first;
      big.leftBottom.y = last;
      big.rightBottom.y = last;
      updateBackstop(mid, big);
      return true;
    }
  } else {
    if (POSTDEBUG) {
      cout << "Squat conversion2" << endl;
    }
    if (big.rightTop.x - big.leftTop.x > 20 && locationOk(big, horizon) && greenCheck(big)) {
      first = -1;
      for (int j = big.leftTop.y; j < big.leftBottom.y; j++) {
	int pix = thresh->thresholded[j][midPoint(big.leftTop.x, big.rightTop.x)];
	if (pix == c || pix == c2) {
	  last = j;
	  if (first == -1)
	    first = j;
	}
      }
      big.leftTop.y = first;
      big.rightTop.y = first;
      big.leftBottom.y = last;
      big.rightBottom.y = last;
      if (POSTDEBUG)
	cout << "Putting out backstop" << endl;
      updateBackstop(mid, big);
      return true;
    }
  }
  if (POSTDEBUG)
    cout << "Returning false from post/blob" << endl;
  return false;
}
#endif

/* Shooting.
 */

/* Determines shooting information.  Basically scans down from backstop and looks for occlusions.
 * Sets the information found within the backstop data structure.
 * @param one     the backstop
 */

void ObjectFragments::setShot(VisualFieldObject* one) {
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
  for ( ;r2 > bx && r2 <= rx  && shoot[r2]; r2++) {}
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
    cout << "Backstop info: Left Col: " << r1 << " Right Col: " << r2 << " Dir: " << one->getBackDir();
    if (one->shotAvailable())
      cout << " Take the shot!" << endl;
    else
      cout << " Don't shoot!" << endl;
  }
}

void ObjectFragments::bestShot(VisualFieldObject* left, VisualFieldObject* right, VisualFieldObject* middle) {
  // start by setting boundaries
  int leftb = 0, rightb = IMAGE_WIDTH - 1, bottom = 0;
  int rl = 0, rr = 0;
  bool screen[IMAGE_WIDTH];
  for (int i = 0; i < IMAGE_WIDTH; i++) {
    screen[i] = false;
  }
  if (left->getDist() != 0) {
    if (left->getCertainty() != SURE) return;
    leftb = left->getRightBottomX();
    bottom = left->getLeftBottomY();
    rightb = min(rightb, left->getRightBottomX() + (int)left->getHeight());
  }
  if (right->getDist() != 0) {
    rightb = right->getLeftBottomX();
    bottom = right->getRightBottomY();
    if (leftb == 0) {
      leftb = max(0, rightb - (int)right->getHeight());
    }
  }
  if (vision->red1->getDist() > 0) {
    rl = vision->red1->getLeftBottomX();
    rr = vision->red1->getRightBottomX();
    if (rr >= leftb && rl <= rightb) {
      for (int i = rl; i <= rr; i++) {
	screen[i] = true;
      }
    }
  }
  if (vision->red2->getDist() > 0) {
    rl = vision->red2->getLeftBottomX();
    rr = vision->red2->getRightBottomX();
    if (rr >= leftb && rl <= rightb) {
      for (int i = rl; i <= rr; i++) {
	screen[i] = true;
      }
    }
  }
  if (vision->navy1->getDist() > 0) {
    rl = vision->navy1->getLeftBottomX();
    rr = vision->navy1->getRightBottomX();
    if (rr >= leftb && rl <= rightb) {
      for (int i = rl; i <= rr; i++) {
	screen[i] = true;
      }
    }
  }
  if (vision->navy2->getDist() > 0) {
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
    middle->setDist(1);

  }
}

/* Determines what is the most open part of the field.  Basically scans up and looks for occlusions.
 */

void ObjectFragments::openDirection(int horizon, Pose *pose) {
#if ROBOT(AIBO)
  const int divider = 5;
#elif ROBOT(NAO)
  const int divider = 10;
#endif
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
#if ROBOT(AIBO)
	  run++;
#endif
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
#if ROBOT(AIBO)
      if (run > 10 && (white > 20 || bad > 3 || y < sixty)) {
	open2[(int)x / divider] = y + run;
	y = 0;
      }
#elif ROBOT(NAO)
      if (run > 10 && (bad > 3 || y < sixty)) {
	open2[(int)x / divider] = y + run;
	y = 0;
      }
#endif
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
  squareGoal(startX, startY, c, c2, 0, IMAGE_WIDTH - 1, POST); //starts a scan in the middle of the tallest run.
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

int ObjectFragments::classifyFirstPost(int horizon, int c, int c2, bool beaconFound, VisualFieldObject* left, VisualFieldObject* right, VisualFieldObject* mid) {
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
    if (topBlob.leftTop.x != BADVALUE && topBlob.rightTop.x - topBlob.leftTop.x > 10 && locationOk(topBlob, horizon) && greenCheck(topBlob)) {
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
#if ROBOT(NAO)
  // start the long process of figuring out which post we've got - fortunately with the Naos it is easier
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

  //post = triangle(pole);        // look for the triangle
  if (post != NOPOST) {
    if (POSTLOGIC) {
      cout << "Found triangle " << post << endl;
    }
    return post;
  }

#elif ROBOT(AIBO)
  // try to use the blob information we got to avoid the post logic altogether
  int poster = thresh->postCheck(c == BLUE, trueLeft, trueRight);
  poster = 0;
  if (poster > 10 || poster < -10) {
    if (POSTLOGIC)
      cout << "Found from poster" << endl;
    if (poster > 10) post = RIGHT;
    else post = LEFT;
    return post;
  }
  post = checkIntersection(pole);
  if (post != NOPOST) {
    if (POSTLOGIC)
      cout << "Found from Intersection" << endl;
    return post;
  }
  post = checkPostBlob(topBlob, pole);
  if (post != NOPOST) {
    if (POSTLOGIC)
      cout << "Found from checkPostBlob" << endl;
    return post;
  }
  post = checkOther(trueLeft, trueRight, fakeBottom - trueTop, horizon);
  if (post != NOPOST) {
    if (POSTLOGIC)
      cout << "Found from checkOther" << endl;
    return post;
  }

  if (spanY < IMAGE_HEIGHT - 30) {
    post = scanLogic(characterizeSize(pole));
    if (post != NOPOST) {
      if (POSTLOGIC)
	cout << "Found from scanOut" << endl;
      return post;
    }
  }
  // test for side goal case
  int myHor = horizonAt(pole.leftTop.x);
  /*
  if (pole.leftBottom.y - myHor > pole.rightTop.x - pole.leftTop.x && (pole.rightTop.x - pole.leftTop.x) > 20 && pole.leftTop.x > 1 && pole.rightTop.x < IMAGE_WIDTH - 1) {
    if (post == LEFT)
      post = RIGHT;
    else
      post = LEFT;
    if (POSTLOGIC)
      cout << "Side goal case " << endl;
    return post;
    }*/

  post = checkCorners(pole);
  if (post != NOPOST) {
    if (POSTLOGIC)
      cout << "Found from corner check" << endl;
    return post;
  }

  post = projectBoxes(spanX,spanY,howbig, fakeBottom, trueRight, trueLeft, trueTop);
  if (post != NOPOST) {
    if (POSTLOGIC)
      cout << "Found from projectBoxes" << endl;
    return post;
  }
  if (rightBlobColor(pole, QUESTIONABLEPOST) && spanX < 100) {
    if (trueLeft < min(spanX, 15))
      post = RIGHT;
    else if (trueRight > IMAGE_WIDTH - min(spanX,  15))
      post = LEFT;
    if (POSTLOGIC)
      cout << "Side of the image" << endl;
  }
#endif
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
void ObjectFragments::goalScan(VisualFieldObject* left, VisualFieldObject* right, VisualFieldObject* mid, int c, int c2, bool beaconFound, int horizon) {
  //cout << horizon << " " << slope << endl;
  // if we don't have any runs there is nothing to do
  if (numberOfRuns <= 1) return;
  int nextX = 0;
  //int nextY = 0;
  int nextH = 0, dc = BOTH_UNSURE;
#if ROBOT(AIBO)
  // start by blobbing - we often want to do it anyway
  for (int i = 0; i < numberOfRuns; i++) {
    blobIt(runs[i].x, runs[i].y, runs[i].h);
  }
  //cout << "Blobs " << numBlobs << endl;
  getTopAndMerge(horizon);
#endif
  int isItAPost = grabPost(c, c2, horizon, IMAGE_WIDTH, -1);
  //drawBlob(obj, ORANGE);
  // make sure we're looking at something big enough to be a post
  if (isItAPost == NOPOST) {
#if ROBOT(AIBO)
    // hold on!
    if (topBlob.leftTop.x != BADVALUE && topBlob.rightTop.x - topBlob.leftTop.x > 10 && locationOk(topBlob, horizon) && greenCheck(topBlob)) {
      if (goodPost(topBlob, c) && !coloredArc(topBlob, c)) {
	updateBackstop(mid, topBlob);
      }
    }
    if (!locationOk(pole, horizon) && coloredArc(topBlob, c) && !horizonTopOk(topBlob.leftTop.y, horizon)) {
      // FIXME - need to add this stuff to Nao Vision, or unify Vision
      if (c == YELLOW) {
	updateArc(vision->yellowArc, topBlob, 0);
      } else {
	updateArc(vision->blueArc, topBlob, 0);
      }
    }
#endif
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
#if ROBOT(AIBO)
    //cout << "Bad here" << endl;
    //printBlob(pole);
    // once again check if the blob is better
    if (topBlob.leftTop.x != BADVALUE && topBlob.rightTop.x - topBlob.leftTop.x > 10 && locationOk(topBlob, horizon) && greenCheck(topBlob) && goodPost(topBlob, c) && !coloredArc(topBlob, c)) {
      updateBackstop(mid, topBlob);
    }
    // looks like we have a colored arc
    if (coloredArc(topBlob, c) && !horizonTopOk(topBlob.leftTop.y, horizonLeft)) {
      // FIXME - need to add this stuff to Nao Vision, or unify Vision
      if (c == YELLOW) {
	updateArc(vision->yellowArc, topBlob, 0);
      } else {
	updateArc(vision->blueArc, topBlob, 0);
      }
    }
    //drawBlob(topBlob, ORANGE);
    //drawBlob(pole, RED);
    if (spanX > 25 && trueTop < 5 && spanY > 20 && !beaconFound) {
      // perhaps its a beacon? See if we have a nearby T Corner
      const list <VisualCorner>* corners = vision->fieldLines->getCorners();
      for (list <VisualCorner>::const_iterator k = corners->begin();
	   k != corners->end(); k++) {
	if (k->getShape() == T) {
	  if (k->getY() > trueBottom + spanX && (spanX > k->getX() - trueRight || spanX > trueLeft - k->getX())) {
	    if (c == YELLOW) {
	      inferBeaconFromBlob(pole, vision->by);
	    } else {
	      inferBeaconFromBlob(pole, vision->yb);
	    }
	    if (beaconFound) {
	      if (POSTDEBUG) {
		cout << "Found a beacon!" << endl;
	      }
	      return;
	    }
	  }
	}
      }
    }
#endif
    if (POSTLOGIC)
      cout << "Bad location on post" << endl;
    return;
  }
  if (spanY + 1 == 0) return;
  double rat = (double)(spanX) / (double)(spanY);
  if (!postRatiosOk(rat) && spanY < IMAGE_HEIGHT / 2) {
#if ROBOT(NAO)
    return;
#elif ROBOT(AIBO)
    // not a post - probably between element
    bool done = checkPostAndBlob(rat, beaconFound, c, c2, horizon, left, mid, right, pole, topBlob);
    if (done) {
      if (POSTLOGIC) {
	cout << "Exit after post and blob" << endl;
	drawBlob(pole, PINK);
      }
      return;
    }
#endif
  }
#if ROBOT(AIBO)
  if (!beaconFound && !postRatiosOk(rat) && rat < SQUATRAT) {
    // check and make sure we aren't looking at the bottom of a beacon
    squareGoal(midPoint(pole.leftBottom.x, pole.rightBottom.x), midPoint(pole.leftBottom.y, pole.rightBottom.y) + 3, WHITE, ORANGEYELLOW, max(0, trueLeft - 5), min(IMAGE_WIDTH - 1, trueRight), BEACON);
    //cout << "checking " << obj.leftTop.x << " " << (obj.leftBottom.y - obj.leftTop.y) << endl;
    //drawPoint(pole.leftBottom.x + (pole.rightBottom.x - pole.leftBottom.x) / 2, pole.leftBottom.y + (pole.rightBottom.y - pole.leftBottom.y) / 2 + 3, GREEN);
    if (obj.leftTop.x > BADVALUE && obj.leftBottom.y - obj.leftTop.y > spanY) {
      // someday we may want to recognize this as a beacon - but it isn't today
      if (POSTLOGIC)
	cout << "Bad ratio" << endl;
      return;
    } else {
      vertScan(midPoint(pole.leftBottom.x, pole.rightBottom.x), midPoint(pole.leftBottom.y, pole.rightBottom.y) + 3, 1, 5, WHITE, ORANGEYELLOW);
      if (scan.good > spanY / 2 && trueTop > 1) {
	// well it could just be a field line - especially now that they are thick
	if (POSTLOGIC) {
	  drawBlob(pole, ORANGE);
	  cout << "Bad ratio 2 " << rat << endl;
	}
	return;
      }
    }
  }
#endif
  dc = checkDist(trueLeft, trueRight, trueTop, trueBottom);
  // first characterize the size of the possible pole
  int howbig = characterizeSize(pole);
  int post = classifyFirstPost(horizon, c, c2, beaconFound, left, right, mid);
  if (post == LEFT) {
    updateObject(right, pole, SURE, dc);
  } else if (post == RIGHT) {
    updateObject(left, pole, SURE, dc);
  } else if (post == BACKSTOP) {
#if ROBOT(AIBO)
    updateBackstop(mid, topBlob);
#endif
  } else if (pole.rightTop.x - pole.leftTop.x > IMAGE_WIDTH - 3) {
#if ROBOT(AIBO)
    updateBackstop(mid, pole);
#endif
  } else {
    if (howbig == LARGE)
      updateObject(right, pole, NOTSURE, dc);
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
	if (right->getCertainty() != SURE)
	  right->init();
      } else {
	if (left->getCertainty() != SURE)
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
    rat = (double)spanX2 / (double)spanY2;
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
	updateObject(left, pole, SURE, dc);
	// make sure the certainty was set on the other post
	right->setCertainty(SURE);
      } else {
	second = 1;
	updateObject(right, pole, SURE, dc);
	left->setCertainty(SURE);
      }
    } else {
      if (SANITY) {
	drawBlob(pole, ORANGE);
      }
      if (locationOk(pole, horizon) && ratOk && goodSecondPost && pole.leftTop.x > trueRight) {
	// maybe it really is a side-goal situation
	if (abs(trueTop - trueTop2) < 10 && qualityPost(pole, c)) {
	  if (post == LEFT) {
	    second = 1;
	    updateObject(left, pole, SURE, dc);
	    // make sure the certainty was set on the other post
	    right->setCertainty(SURE);
	  } else {
	    second = 1;
	    updateObject(right, pole, SURE, dc);
            left->setCertainty(SURE);
	  }
	}
      } else if (!locationOk(pole, horizon) && secondPostFarEnough(leftP, rightP, pole.leftTop, pole.rightTop, post)) {
#if ROBOT(AIBO)
	if (coloredArc(topBlob, c) && topBlob.leftTop.y > horizonAt(topBlob.leftTop.x) &&
	    secondPostFarEnough(leftP, rightP, topBlob.leftTop, topBlob.rightTop, post)) {
	  //!horizonTopOk(pole.leftTop.y, horizon)) {
      // FIXME - need to add this stuff to Nao Vision, or unify Vision
	  if (POSTDEBUG) {
	    cout << "Checking for arc" << endl;
	  }
	  if (c == YELLOW) {
	    updateArc(vision->yellowArc, topBlob, spanY);
	  } else {
	    updateArc(vision->blueArc, topBlob, spanY);
	  }
	  // we have probably mis-identified the post thanks to the arc
	  if (post == LEFT) {
	    postSwap(left, right);
	  } else {
	    postSwap(right, left);
	  }
	  return;
	}
#endif
      }
    }
  }
#if ROBOT(NAO)

#elif ROBOT(AIBO)
  // at this point we have found one or two posts.  Let's see if we can find some open goal between them
  int leftRange = -1;
  int rightRange = IMAGE_WIDTH;
  bool certain = true;
  if (second) {
    if (post == LEFT) {
      leftRange = trueRight;
      rightRange = trueLeft2;
    } else {
      leftRange = trueRight2;
      rightRange = trueLeft;
      //cout << "Ranges " << leftRange << " " << rightRange << endl;
    }
  } else {
    if (post == LEFT) {
      leftRange = trueRight;
    } else {
      rightRange = trueLeft;
    }
  }
  point <int> backRange  = thresh->backStopCheck(c == BLUE, leftRange, rightRange);
  if (backRange.x != BADVALUE && backRange.y - backRange.x > spanX) {
    // we have the backstop, so let's just get the particulars
    int lastX = -1;
    numBlobs = 0;
    int tryy = -1;
    for (int i = 0; i < numberOfRuns; i++) {
      nextX = runs[i].x;
      nextY = runs[i].y;
      nextH = runs[i].h;
      //cout << nextX << " " << nextY << " " << nextH << endl;
      if (nextX >= backRange.x && nextX <= backRange.y && nextH > 0 && (nextH < spanY || rat < SQUATRAT) && nextX != lastX &&
	  nextY + nextH < horizon + 10) { //
	blobIt(nextX, nextY, nextH);
	if (nextX == backRange.x)
	  tryy = nextY;
      }
      lastX = nextX;
    }
    transferToChecker(topBlob);
    getTopAndMerge(trueTop);
    constrainedSquare(backRange.x, tryy, c, c2, leftRange, rightRange);
    if (obj.leftTop.x != BADVALUE && obj.leftTop.y > trueTop) {
      if (POSTDEBUG) {
	cout << "Getting backstop from constrainedSquare" << endl;
      }
      updateBackstop(mid, obj);
      return;
    }
    else if (topBlob.leftTop.x > -1 && topBlob.rightTop.x - topBlob.leftTop.x > 5 && !second) {
      // let's not give up on that square we found just yet
      if (pole.rightTop.x - pole.leftTop.x > topBlob.rightTop.x - topBlob.leftTop.x) {
	if (POSTDEBUG) {
	  cout << "Using 2d post attempt as backstop" << endl;
	}
	// make sure to trim the blob
	if (pole.rightTop.x > rightRange) {
	  pole.rightTop.x = rightRange;
	  pole.rightBottom.x = rightRange;
	}
	if (pole.leftTop.x < leftRange) {
	  pole.leftBottom.x = leftRange;
	  pole.leftTop.x = leftRange;
	}
	updateBackstop(mid, pole);
      } else {
	if (POSTDEBUG) {
	  cout << "Using blobbing for backstop" << endl;
	}
	updateBackstop(mid, topBlob);
      }
      return;
    }
    if (POSTDEBUG) {
      cout << "RETURNING" << endl;
    }
    //return;
  }
  if (POSTDEBUG) {
    cout << "Old methods for backstop" << endl;
  }
  numBlobs = 0;
  for (int i = 0; i < numberOfRuns; i++) {
    nextX = runs[i].x;
    nextY = runs[i].y;
    nextH = runs[i].h;
    int hor = horizonAt(nextX);
    //cout << nextX << " " << nextY << " " << nextH << " " << leftRange << " " << rightRange << " " << spanY <<  endl;
    if (nextX >= leftRange + 5 && nextX <= rightRange - 5 && nextH > 0 && (nextH < spanY || rat < SQUATRAT) && nextY < hor + 10 && nextY + nextH > hor - 10) { //
      //              && nextH < spanY && (nextY + nextH + 5 > yProject(0, horizon, nextX)) ) {
      //cout << nextX << " " << nextY << " " << nextH << endl;
      //cout << nextX << endl;
      blobIt(nextX, nextY, nextH);
    }
  }
  // screen out blobs that are above other blobs
  screenCrossbar();
  transferToChecker(topBlob);
  getWidest();
  int relat  = determineRelationship(topBlob, checker);
  switch(relat) {
  case OUTSIDE:
  case TIGHT:
  case MURKY:
    break;
  case CLOSELEFT:
    if (post == LEFT) {
      //topBlob.rightTop.x = checker.rightTop.x;
      //topBlob.rightBottom.x = checker.rightBottom.x;
    }
    break;
  case CLOSERIGHT:
    if (post == RIGHT) {
      //topBlob.leftTop.x = checker.leftTop.x;
      //topBlob.leftBottom.x = checker.leftTop.x;
    }
    break;
  }
  bool under = false;
  if (!certain) {
    // we don't know which post it is, if we can see some green under here, then that helps
    if (greenCheck(topBlob)) {
      if (post == LEFT)
	right->setCertainty(SURE);
      else
	left->setCertainty(SURE);
      certain = true;
      under = true;
    }
    // if it is a small goal then punt
    if (post == LEFT)
      right->init();
    else
      left->init();
  }
  // before we leave, check for side goal situations
  /*
  if (!second && howbig != SMALL && topBlob.rightTop.x - topBlob.leftTop.x < 2 * spanX && !greenCheck(topBlob)) {
    if (POSTLOGIC) {
      cout << "Checking for side case" << endl;
    }
    if (post == RIGHT && topBlob.leftTop.x > 0 || post == LEFT && topBlob.rightTop.x < IMAGE_WIDTH - 1) {
      if (POSTLOGIC)
	cout << "Passed second test" << endl;
      // look for a white blob below the backstop
      int wx = midPoint(topBlob.leftTop.x, topBlob.rightTop.x);
      int wy = midPoint(topBlob.leftBottom.y, topBlob.rightBottom.y) + 5;
      squareGoal(wx, wy, WHITE, ORANGEYELLOW, topBlob.leftBottom.x, topBlob.rightBottom.x, BEACON);
      //drawPoint(wx, wy, RED);
      if (obj.leftTop.x > BADVALUE) {
	// could be one
	int wspan = obj.rightTop.x - obj.leftTop.x;
	if (wspan > 10) {
	  if (POSTLOGIC)
	    cout << "Swapping posts!" << endl;
	  if (post == LEFT) {
	    postSwap(left, right);
	  } else {
	    postSwap(right, left);
	  }
	  return;
	}
      } else {
	// let's try again - sometimes our backstops are too low
	wy = wy - 15;
	squareGoal(wx, wy, WHITE, ORANGEYELLOW, topBlob.leftBottom.x, topBlob.rightBottom.x, BEACON);
	drawPoint(wx, wy, RED);
	if (obj.leftTop.x > BADVALUE) {
	  // could be one
	  int wspan = obj.rightTop.x - obj.leftTop.x;
	  if (wspan > 10) {
	    if (POSTLOGIC)
	      cout << "Swapping posts! 2" << endl;
	    if (post == LEFT) {
	      postSwap(left, right);
	    } else {
	      postSwap(right, left);
	    }
	    return;
	  }
	}
      }
    }
    } */
  // sometimes we find a "post" in the backstop
  int tSize = topBlob.leftBottom.y - topBlob.leftTop.y + 1;
  int tWidth = topBlob.rightTop.x - topBlob.leftTop.x + 1;
  if (!second && tSize > ySpan * 2 / 3 && (tWidth > spanX || (tWidth > 20 && (ySpan > 100 || topBlob.leftTop.x < 8 || topBlob.rightTop.x > IMAGE_WIDTH - 8))) && (abs(lx - topBlob.rightTop.x) < 5 || abs(topBlob.leftTop.x - rx < 5))) {
    if (POSTDEBUG)
      cout << "Have we misidentified a post as a backstop?" << endl;
    int lastFirst = ly;
    int lastLast = ly + ySpan;
    bool notThere = false;
    int start = 0, finish = 0;
    int first = 0, last = 0;;
    int dir = 1;
    if (post == LEFT) {
      start = topBlob.leftTop.x;
      finish = topBlob.rightTop.x;
    } else {
      start = topBlob.rightTop.x;
      finish = topBlob.leftTop.x;
      dir = -1;
    }
    for (int i = start; i != finish && !notThere; i = i + dir) {
      first = 0; last = 0;
      for (int j = topBlob.leftTop.y; j <= topBlob.leftBottom.y; j++) {
	int pix = thresh->thresholded[j][i];
	if (pix == c || pix == c2) {
	  last = j;
	  if (first == 0)
	    first = j;
	}
      }
      if (POSTDEBUG) {
	cout << first << " " << last << " " << lastFirst << " " << lastLast << endl;
      }
      if (lastLast - last  > 5 || first - lastFirst > 5) {
	notThere = true;
	// while we're at it - adjust the blob a bit
	if (post == LEFT) {
	  topBlob.leftTop.x = i;
	  topBlob.leftBottom.x = i;
	} else {
	  topBlob.rightTop.x = i;
	  topBlob.rightBottom.x = i;
	}
	// make Henry really happy
	first = 0; last = 0;
	for (int j = topBlob.leftTop.y; j <= topBlob.leftBottom.y; j++) {
	  int pix = thresh->thresholded[j][midPoint(topBlob.leftTop.x, topBlob.rightTop.x)];
	  if (pix == c || pix == c2) {
	    last = j;
	    if (first == 0)
	      first = j;
	  }
	}
	topBlob.leftBottom.y = last;
	topBlob.rightBottom.y = last;
	topBlob.leftTop.y = first;
	topBlob.rightTop.y = first;
      }
      lastLast = last;
      lastFirst = first;
    }
    if (notThere) {
      if (POSTDEBUG) {
	cout << "Nope" << endl;
      }
    } else {
      // we seem to have found that case
      if (POSTDEBUG) {
	cout << "YEs, I found one.  Merging the blobs" << endl;
      }
      if (post == LEFT) {
	topBlob.leftTop.x = lx;
	topBlob.leftBottom.x = lx;
	right->init();
      } else {
	topBlob.rightTop.x = rx;
	topBlob.rightBottom.x = rx;
	left->init();
      }
    }
  }
  if (POSTDEBUG) {
    cout << "Finishing up" << endl;
  }
  if (topBlob.rightTop.x - topBlob.leftTop.x > 10) {
    if (POSTDEBUG) {
      cout << "Backstop at end" << endl;
    }
    updateBackstop(mid, topBlob);
  } else if (POSTDEBUG) {
    drawBlob(topBlob, ORANGE);
  }
#endif
}

/* We misidentified the first post.  Now that we've figured that out we need to switch it to the
 * correct post.  Just transfer the information and reinit the previously IDd post.
 * @param p1    the correct post
 * @param p2    the wrong one
 */

 void ObjectFragments::postSwap(VisualFieldObject * p1, VisualFieldObject * p2) {
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
   p1->setCertainty(SURE);
   p1->setDistCertainty(p2->getDistCertainty());
   p1->setDist(1);
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
void ObjectFragments::transferTopBlob(VisualFieldObject * one, int cert, int distCert) {
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
  one->setWidth(dist(topBlob.leftTop.x, topBlob.leftTop.y, topBlob.rightTop.x, topBlob.rightTop.y));
  one->setHeight(dist(topBlob.leftTop.x, topBlob.leftTop.y, topBlob.leftBottom.x, topBlob.rightBottom.y));
  one->setCenterX(one->getLeftTopX() + ROUND2(one->getWidth() / 2));
  one->setCenterY(one->getRightTopY() + ROUND2(one->getHeight() / 2));
  one->setCertainty(cert);
  one->setDistCertainty(distCert);
  one->setDist(1);
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
double ObjectFragments::rightHalfColor(blob tempobj) {
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
  double percent = (double)max(max(good, good1), good2) / (double) (spanX * spanY / 2);
  //cout << "Tossed because of low percentage " << percent << " " << color << endl;
  return percent;
}

/* Checks out how much of the current blob is orange.  Also looks for too much red.
 * @param tempobj     the candidate ball blob
 * @param col         ???
 * @return            the percentage (unless a special situation occurred)
 */

double ObjectFragments::rightColor(blob tempobj, int col) {
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
  if (blobArea(tempobj) > 1000) return (double) good / (double) blobArea(tempobj);
  //if (ogood < 2 * orgood) return 0.1; // at least two thirds of the "orange" pixels should be orange
  if (red > spanX * spanY * 0.10) return 0.1;
  if (ogood < spanX * spanY * 0.20) return 0.1;
  if (tempobj.area > 1000 && ogood+oygood > (spanX * spanY * 0.4)  && good < (spanX * spanY * 0.65)) return 0.65;
  double percent = (double)good / (double) (spanX * spanY);
  if (col == GREEN)
    return (double)good;
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
int ObjectFragments::scanOut(int start_x, int start_y, double slopel, int dir){
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
    y = start_y  +ROUND2(slopel*(double)(x - (start_x)));
  }else{
    y = y + dir;
    x = start_x  +ROUND2(slopel*(double)(y - (start_y)));
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
  double ratio = (double)w / (double)h;
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
      int d = ROUND2((double)max(w, h) / 6.0);
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
    if ((double)goodPix / (double)badPix < 2) {
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

/*  Check the information surrounding the ball and look to see if it might be a false ball.  Since our main
 * candidate for false balls is the red uniform, the main thing we worry about is a preponderance of red.
 * @param b    our ball candidate
 * @return     true if the surround looks bad, false if its ok
 */

bool ObjectFragments::badSurround(blob b) {
  // basically check around the blob and see if it is ok - ideally we'd have some green, worrisome would be lots of RED
#if ROBOT(AIBO)
  static const int surround = 5;
#elif ROBOT(NAO)
  static const int surround = 12;
#endif
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
#if ROBOT(AIBO)
  if (orange - borange > 5) return true;
#endif
  return false;
}

/*  Is the ball at the boundary of the screen?
 * @param b    the ball
 * @return     whether or not it borders a boundary
 */
bool ObjectFragments::atBoundary(blob b) {
  return b.leftTop.x == 0 || b.rightTop.x >= IMAGE_WIDTH -1 || b.leftTop.y == 0 || b.leftBottom.y >= IMAGE_HEIGHT - 1;
}


#if ROBOT(AIBO)
/* See if there is a ball onscreen.  Basically we get all of the orange blobs and test them for
 * viability.  Once we've screened all of the obviously bad ones we then pick the biggest one
 * and check it some more.
 * @param  horizon   the horizon intercept
 * @param  thisBall  the ball object
 * @return           we always return 0 - the return is an artifact of other methods
 */
int ObjectFragments::balls(int horizon, Ball *thisBall) {
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
  //cout << "BLobs" << numBlobs << endl;
  // pre-screen blobs that don't meet our criteria
  for (int i = 0; i < numBlobs; i++) {
    //drawBlob(blobs[i],BLUE);
    int bx = blobs[i].leftBottom.x;
    int by = blobs[i].leftBottom.y;
    int bxm = bx + (blobs[i].rightBottom.x - bx) / 2;   // mid X point of ball
    int bym = by - (by - blobs[i].leftTop.y) / 2;       // mid Y point of ball
    int bh = horizonAt(bx);
    int br = roundness(blobs[i]);
    double colPer = rightColor(blobs[i], ORANGE);
    int horizonOffset = bh - by;
    int hei = blobs[i].leftBottom.y - blobs[i].leftTop.y;
    int wid = blobs[i].rightTop.x - blobs[i].leftTop.x;
    int brad = max(hei, wid) * 2;
    // ball should never be more than about their radius below the horizon unless they are on the screen edge
    if (brad + brad/2 + horizonOffset < -5) {
      // check if we're occluded
      //print("Ball height %d width %d leftX %d rightX %d",);
      if ((bx == 0 ||  blobs[i].rightTop.x >= IMAGE_WIDTH - 1) && hei > wid) {
      } else if ((by == 0 || blobs[i].leftBottom.y >= IMAGE_WIDTH - 1) && hei < wid) {
      } else {
	if (BALLDEBUG) {
	  if (blobs[i].area > 50) {
	    cout << "Screening blob because it is far below the horizon for its size " << by << " " << bh << " " << brad << endl;
	    drawBlob(blobs[i], MAROON);
	  }
	  cout << "Setting blob to zero" << endl;
	}
#if ROBOT(AIBO)
	blobs[i].area = 0;
#endif
      }
    }
    if (blobs[i].area > 0) {
      //print("Ball area %d\n",blobs[i].area);
      if (BALLDEBUG) {
	cout << "Passed horizon test " << brad << " " << horizonOffset << endl;
      }
      // balls should never be very much above the horizon, they should be roundish, and should be mostly orange
      if (colPer < MINORANGEPERCENT && (blobs[i].area > 1000 || (colPer > 0.5 && blobs[i].area > 250) || (atBoundary(blobs[i]) && blobs[i].area > 300)) ) {
	// do a check to see if the ball is actually occluded because it is being trapped
	colPer = rightHalfColor(blobs[i]) + 0.05;
	if (BALLDEBUG)
	  cout << "******New colPer " << colPer << endl;
      }
      if (horizonOffset > 5 || br < 0 || colPer < MINORANGEPERCENT) {
	if (BALLDEBUG) {
	  cout << "Blob screen " << br << " " << colPer << " " << blobs[i].area << endl;
	  //drawPoint(bx, by, RED);
	  //drawPoint(bx, bh, BLACK);
	  drawBlob(blobs[i], BLACK);
	  }
	blobs[i].area = 0;
      }
    }
    if (BALLDEBUG) {
      if (blobs[i].area > 0) {
	cout << "-----------Viable candidate --------- " << colPer << endl;
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
  int w = topBlob.rightTop.x - topBlob.leftTop.x + 1;
  int h = topBlob.leftBottom.y - topBlob.leftTop.y + 1;
  //printBlob(topBlob);
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
  //int cenY = thisBall -> getCenterX();
  //int cenX = thisBall -> getCenterY();
  /*int numBadPoints = 0;
  int angle = PI;
  scanOut(cenX,cenY,tan(angle), 1);*/

  for(double angle = 0; angle < PI; angle +=PI/NUM_EDGE_POINTS){
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
    /*if (whereIsGreen == NOGREEN) {
      // before we get too serious about it, let's explore more - aftter all green can be hard to see on the horizon
      if (badSurround(topBlob)) {
	if (BALLDEBUG) {
	  drawBlob(topBlob, BLACK);
	}
	return 0;
      } else confidence -= 2;

      } */
  }
  if (whereIsGreen == NOGREEN) {
    confidence-= 2;
    /*if (w <  SMALLBALLDIM || h < SMALLBALLDIM) {
      if (BALLDEBUG)
	cout << "screening small ball because it isn't near enough to green";
      return 0;
      } */
  }

  if (w < SMALLBALLDIM || h < SMALLBALLDIM) {
    // small balls should be near the horizon - this check makes extra sure
    if (topBlob.leftBottom.y > horb + 15) {
      if (BALLDEBUG)
	cout << "Screening small ball for horizon" << endl;
      return 0;
    }
    //if (whereIsGreen % GREENBELOW != 0) {
    //if (BALLDEBUG)
    //	cout << "Screening small ball because it doesn't have green below it" << endl;
    //return 0;
    //}
    if (BALLDEBUG)
      cout << "Small ball passed green and horizon tests" << endl;
  }
  double colPer = rightColor(topBlob, ORANGE);

  if (w * h > 1) {
    confidence -= ROUND2((0.85 - colPer) * 10);
    //cout << (ROUND2((0.85 - colPer) * 10)) << " " << confidence << endl;
    if (topBlob.area < 25) {
      confidence -= 3;
    } else if (topBlob.area < SMALLBALL) {
      confidence -= 2;
    } else if (topBlob.area < 100) {
      confidence -= 1;
    } else if (topBlob.area > 400) {
      confidence += 2;
    } else if (topBlob.area > 200) {
      confidence += 1;
    }
    if (confidence < 0 && colPer < 0.75) {
      if (BALLDEBUG)
	cout << "Screening ball due to too many bad matches " << confidence << endl;
      return 0;
    }
    if (BALLDEBUG) {
      printBall(topBlob, confidence, colPer, occlusion, whereIsGreen);
    }

    if (confidence > 10 && min(w, h) > 40 && min(w, h) < 65) {
      // check for poking situation
    }


    // SORT OUT BALL INFORMATION
    // start out by figuring out whether we're using blobs or inferred information
    //double rat = (double) w / (double) h;
    if (!atBoundary(topBlob)) {
	//if (occlusion == NOOCCLUSION || (occlusion <= BOTTOMOCCLUSION && (rat > MINGOODBALL && rat < MAXGOODBALL))) {
      if (BALLDISTDEBUG) {
	cout << "Using blobs to calculate distance,  width/height" << w << " " << h << endl;
	circleFit(thisBall);
	thisBall->setFocalDistance();
	thisBall->setDistance();
	cout << "Inferred dist would have been" << thisBall->getDist() << " conf: " << inferredConfidence << endl;
      }
      // x, y, width, and height. Not up for debate.
      thisBall->setX(topBlob.leftTop.x);
      thisBall->setY(topBlob.leftTop.y);

      thisBall->setWidth(w);
      thisBall->setHeight(h);
      thisBall->setRadius((double)max((double)w/2.0, (double)h/2.0));
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
	//thisBall->setY(topBlob.leftBottom.y - amount * 2);
      }
      thisBall->setConfidence(SURE);
    } else {
      // INFERRED MEASUREMENTS
       if (BALLDEBUG) {
	 for(int i = 0; i < numPoints; i = i + 2)
	   drawPoint((int)points[i], (int)points[i+1], BLACK);
       }
      if (BALLDISTDEBUG) {
	cout << "Inferred Confidence " << inferredConfidence << endl;
	thisBall->setRadius((double)max(w/2, h/2));
	thisBall->setFocalDistance();
	thisBall->setDistance();
	cout << "Blob dist would have been" << thisBall->getDist() << endl;
      }
      circleFit(thisBall);
      //cout<<"AFter circle fit radius"<<thisBall->getRadius()<<endl;
      //cout<<"after circle fit, what is distance?"<<thisBall->getDist()<<endl;
      if (inferredConfidence > 4 && (occlusion <= BOTTOMOCCLUSION || topBlob.area > 400))
	thisBall->setConfidence(SURE);
      else if (inferredConfidence > 3)
	thisBall->setConfidence(MILDLYSURE);
      else
	thisBall->setConfidence(NOTSURE);
      if (thisBall->getRadius() < max(w,h) / 2 || thisBall->getRadius() > max(w,h) * 2) {
	// crap we must have gotten bad points
	thisBall->setX(topBlob.leftTop.x);
	thisBall->setY(topBlob.leftTop.y);

	thisBall->setWidth(w);
	thisBall->setHeight(h);
	thisBall->setRadius((double)max((double)w/2.0, (double)h/2.0));
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
	  //thisBall->setY(topBlob.leftBottom.y - amount * 2);
	}
	thisBall->setConfidence(MILDLYSURE);
      }
      //else if (occlusion <= BOTTOMOCCLUSION || w * h > BIGGERAREA)
      //thisBall->setConfidence(MILDLYSURE);
    }
    thisBall->findAngles();
    thisBall->setFocalDistance();
    thisBall->setDistance();
    if (BALLDISTDEBUG) {
      cout << "Distance is " << thisBall->getDist() << " " << thisBall->getFocDist() << endl;
      cout<< "Radius"<<thisBall->getRadius()<<endl;
    }
  } else if (topBlob.area > 0){
  }
  return 0;
}
#elif ROBOT(NAO)
/* See if there is a ball onscreen.  Basically we get all of the orange blobs and test them for
 * viability.  Once we've screened all of the obviously bad ones we then pick the biggest one
 * and check it some more.
 * @param  horizon   the horizon intercept
 * @param  thisBall  the ball object
 * @return           we always return 0 - the return is an artifact of other methods
 */
int ObjectFragments::balls(int horizon, Ball *thisBall) {
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
    double perc = rightColor(blobs[i], ORANGE);
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

  for(double angle = 0; angle < PI; angle +=PI/NUM_EDGE_POINTS){
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
  double colPer = rightColor(topBlob, ORANGE);

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
  //double rat = (double) w / (double) h;
  // x, y, width, and height. Not up for debate.
  thisBall->setX(topBlob.leftTop.x);
  thisBall->setY(topBlob.leftTop.y);

  thisBall->setWidth(w);
  thisBall->setHeight(h);
  thisBall->setRadius((double)max((double)w/2.0, (double)h/2.0));
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
  thisBall->setFocalDistance();
  thisBall->setDistance();
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
    cout << "Distance is " << thisBall->getDist() << " " << thisBall->getFocDist() << " " << es.dist << endl;
    cout<< "Radius"<<thisBall->getRadius()<<endl;
  }
  return 0;
}

#endif
/**
   This method calculates the best fit circle to a number of given points. Uses
   least squares regression of some sort. (see url below). Works by filling two
   matrices with various expressions, summing as we go for each point.
   Then we invert matrix J (by hand), and multiply it with K to get the 3x1
   matrix of coefficients which will help us find the circle parmeters.
   Then we write the information into the ball object.
   Returns 0 on succes, 1 on fail.
   @param thisBall    the ball object
   @return            0 for success, 1 for fail
 */
int ObjectFragments::circleFit(Ball * thisBall){
  //thisBall->init();
    inferredConfidence = 0;
    if(numPoints < 3){
      if (DEBUGCIRCLEFIT) printf("ERR in Circle fit: insufficient points!");
      return 1;
    }
  //Init the matrices j,k and a (j^-1)
  double
    j00 = 0.0,j01 = 0.0, j02 = 0.0,
    j10 = 0.0,j11 = 0.0,j12 = 0.0,
    j20 = 0.0,j21 = 0.0,j22 = 0.0,

    k0 = 0.0, k1 = 0.0, k2 = 0.0,

    a00 = 0.0,a01 = 0.0,a02 = 0.0,
    a10 = 0.0,a11 = 0.0,a12 = 0.0,
    a20 = 0.0,a21 = 0.0,a22 = 0.0;

  //look through all the points, and do the analysis
  /*see http://www.orbitals.com/self/least/least.htm , 1st example*/
  for(int p = 0; p < 2*numPoints;){
    double x = points[p++];
    double y = points[p++];
    double xySqrd = (x*x + y*y);
    //cout<<x<<endl;
    //print "New point(",x,y,"),xySqrd = ",xySqrd
    //get Jt*J

    // first row
    j00 +=xySqrd*xySqrd;
    j01 +=xySqrd*x;
    j02 +=xySqrd*y;
    //second row
    //skip (1,0) because it is similar to (0,2)
    j11+= x*x;
    j12+= x*y;

    //third row
    //skip (2,0),(2,1) bc of smiilar reason above
    j22 +=y*y;

    //get K
    k0 +=xySqrd;
    k1 +=x;
    k2 +=y;
  }
  j10=j01;
  j20=j02;
  j21=j12;

  //printf("Num points %d \n",numPoints);
  /*http://en.wikipedia.org/wiki/Standard_deviation*/
  /*
  double meanX = k1/numPoints;
  double meanY = k2/numPoints;

  printf("Sum squared of X %f   mean X^2 %f \n",j11/numPoints,meanX*meanX);
  printf("Sum squared of X %f   mean X^2 %f \n",j22/numPoints,meanY*meanY);

  double stdDevX =sqrt((j11/numPoints)-(meanX*meanX));
  double stdDevY =sqrt((j22/numPoints)-(meanY*meanY));

  printf("Std dev xy (%f,%f)\n",stdDevX,stdDevY);*/

  //find the inverse
  double jDet = det3(j00,j01,j02,
		     j10,j11,j12,
		     j20,j21,j22);
  //printf("The determinant:%g\n",jDet);
  if(jDet == 0){
    if (DEBUGCIRCLEFIT)
      printf("ERR in Circle fit: the determinant was zero, no circle!");
    return 1;

  }

  a00 =  det2(j11,j12,
	      j21,j22)/jDet;
  a01 = -det2(j10,j12
	      ,j20,j22)/jDet;
  a02 =  det2(j10,j11,
	      j20,j21)/jDet;

  a10 = -det2(j01,j02,
	      j21,j22)/jDet;
  a11 =  det2(j00,j02,
	      j20,j22)/jDet;
  a12 = -det2(j00,j01,
	      j20,j21)/jDet;

  a20 =  det2(j01,j02,
	      j11,j12)/jDet;
  a21 = -det2(j00,j02,
	      j10,j12)/jDet;
  a22 =  det2(j00,j01,
	      j10,j11)/jDet;

  //multiply the matrices to find the coefficients
  //which help to solve the circle
  double a = a00*k0+a01*k1+a02*k2;
  double b = a10*k0+a11*k1+a12*k2;
  double c = a20*k0+a21*k1+a22*k2;
  if(a == 0){
    if (DEBUGCIRCLEFIT)
      printf("ERR in circle fit: got a zero denominator, circle fit failed");
    return 1; //fail

  }

  if ((4*a + b*b + c*c) < 0) {
    if (DEBUGCIRCLEFIT)
      print("ERR is negative sqrt");
    return 1;
  }

  //cout<<"a "<<a<<"b "<<b<<"c "<<c<<endl;
  //calc all the infered dimensions
  int inferedX = ROUND2(-b/(2*a));
  int inferedY = ROUND2(-c/(2*a));
  double determinantThing = 4*a + b*b + c*c;
  if(determinantThing < 0){
    if (DEBUGCIRCLEFIT)
      printf("ERR in circle fit: PROCESSOR NOT ACCURATE ENOUGH\n");
    return 1;
  }
  double inferedR = abs(sqrt(determinantThing)/(2*a));


  if(inferedX < (0 - IMAGE_WIDTH) || inferedX > 2*IMAGE_WIDTH ||
     inferedY < (0 - IMAGE_HEIGHT) || inferedY > 2*IMAGE_HEIGHT){
    if (DEBUGCIRCLEFIT)
      print("ERR in circle fit: got a bad center");
    return 1; //fail
  }

  //write info to the ball class
  thisBall->setCenterX(inferedX);
  thisBall->setCenterY(inferedY);
  thisBall->setRadius(inferedR);
  thisBall->setWidth(thisBall->getRadius() * 2);
  thisBall->setHeight(thisBall->getRadius() * 2);
  thisBall->setX(ROUND2(inferedX - thisBall->getRadius()));
  thisBall->setY(ROUND2(inferedY - thisBall->getRadius()));
  if (DEBUGCIRCLEFIT)
    printf("Circle fit: xyr (%d,%d,%f)\n",inferedX,inferedY,inferedR);

  if (DEBUGCIRCLEFIT)
    drawPoint(inferedX,inferedY,YELLOW);
  double stDev = 0;
  double *distances = new double[numPoints];
  double *pointer = distances;
  for(int p = 0; p < 2*numPoints;){
    double x = points[p++];
    double y = points[p++];
    double newRad = sqrt(pow(x-inferedX,2) + pow(y-inferedY,2));
    stDev +=pow(newRad-inferedR,2);
    *pointer = newRad;
    pointer++;
  }
  stDev/=numPoints;
  stDev = sqrt(stDev);
  //cout<<"STDEV "<<stDev<<endl;

  double distErr = 0;
  int badPoints = 0;

  for (int i=0; i< numPoints; i++) {
    double error = fabs(distances[i] - inferedR);
    //cout<<"error: "<<error<<endl;
    if (error > stDev) {
      badPoints++;
      continue;
    }
    distErr += error;
  }

  delete distances;


  if(badPoints==numPoints){
    if (DEBUGCIRCLEFIT)
      printf("ERR in circle fit: no good points");
    return 1; //fail
  }
  if(inferedR==0){
    if (DEBUGCIRCLEFIT)
      printf("ERR in circle fit: resultant radius = 0");
    return 1; //fail
  }
  distErr /= numPoints-badPoints;
  distErr /= inferedR;
  distErr *= 100;
  //cout<<"Radius"<<inferedR<<endl;
  //cout<<"sum of dist errors: "<<distErr<<endl;
  //  float confidence = sqrt(pow(stdDevX,2.0) + pow(stdDevY,2.0))/inferedR;
  //cout<<"Confidence"<<confidence<<endl;

  //print("Ball err %f",distErr);
  inferredConfidence = 5 - ROUND2(distErr);
  return 0; //success
}



/* Sanity check routines for beacons and posts
 */


/* Checks out how much of the blob is of the right color.  If it is enough returns true, if not false.
 * @param tempobj     the blob we're checking (usually a post)
 * @param minpercent  how good it needs to be
 * @return            was it good enough?
 */
bool ObjectFragments::rightBlobColor(blob tempobj, double minpercent) {
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
  double percent = (double)good / (double) (total);
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
#if ROBOT(AIBO)
    //int fudge = 5;
#elif ROBOT(NAO)
  //int fudge = 20;
#endif
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
void ObjectFragments::addPoint(double x, double y){
  if (DEBUGCIRCLEFIT)
    printf("ADDING a point:#%d,(%f,%f)\n",numPoints,x,y);
  if(numPoints < MAX_POINTS){
    points[numPoints*2] = x;
    points[numPoints*2+1] = y;
    numPoints++;
  }else{
    if (DEBUGCIRCLEFIT)
      printf("Too many points!!!\n");
  }
}


/*  Is the ratio of width to height ok for the second post?  We use a different criteria here than for
 * the first post because we have lots of other ways to verify if this is a good post.
 * @param ratio     the height/width ratio
 * @return          is it a legal value?
 */
bool ObjectFragments::postRatiosOk(double ratio) {
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

/* Calculate the euclidian distance between two points.
 * @param x    x value of point 1
 * @param y    y value of point 1
 * @param x1   x value of point 2
 * @param y1   y value of point 2
 * @return      the distance between the objects
 */
 double ObjectFragments::dist(int x, int y, int x1, int y1) {
   return sqrt((double)abs(x - x1) * abs(x - x1) + abs(y - y1) * abs(y - y1));
 }

/* Finds and returns the midpoint of two numbers.
 * @param a   one number
 * @param b   the other
 * @return    the number halfway between (as int)
 */
int ObjectFragments::midPoint(int a, int b) {
  return a + (b - a) / 2;
}





/*  The next group of functions are for debugging only.  They are set up so that debugging information
 *  will only appear when processing is done off-line.
 */

/*  Print debugging information for a field object.
 * @param objs     the object in question
 */

void ObjectFragments::printObject(VisualFieldObject * objs) {
#if defined OFFLINE
  cout << objs->getLeftTopX() << " " << objs->getLeftTopY() << " " << objs->getRightTopX() << " " << objs->getRightTopY() << endl;
  cout << objs->getLeftBottomX() << " " << objs->getLeftBottomY() << " " << objs->getRightBottomX() << " " << objs->getRightBottomY() << endl;
  cout << "Height is " << objs->getHeight() << " Width is " << objs->getWidth() << endl;
  if (objs->getCertainty() == SURE)
    cout << "Very sure" << endl;
  else
    cout << "Not sure" << endl;
  int dc = objs->getDistCertainty();
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
    if (vision->bgBackstop->getWidth() >  0) {
      cout << "Vision found blue backstop " << endl;
      printObject(vision->bgBackstop);
    }
    if (vision->ygBackstop->getWidth() >  0) {
      cout << "Vision found yellow backstop " << endl;
      printObject(vision->ygBackstop);
    }
#if ROBOT(AIBO)
    if (vision->yb->getWidth() >  0) {
      cout << "Vision found yellow blue post " << endl;
      printObject(vision->yb);
    }
    if (vision->by->getWidth() >  0) {
      cout << "Vision found blue yellow post " << endl;
      printObject(vision->by);
    }
#endif
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

void ObjectFragments::printBall(blob b, int c, double p, int o, int bg) {
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



