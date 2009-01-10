/* This file and a related file (ObjectFragments.cc)
   do the bulk of the Vision processing in a given frame.

   The algorithm used is (relatively) straightforward -
   the visual signal comes in as a 2D array of YUV values.

   In principle the algorithm would work like this:
   1) convert each YUV value to a color (this is called thresholding)
   2) find groups of connected pixels of similar colors (run length encoding)
   3) extract objects from the blobs found

   We need to be faster and smarter than that so 2) kind of blends into 1)
   and 3).
   Our version of vision scans the image starting at the bottom left corner
   and doing vertical slices up and across the image.  Each pixel is checked
   against a color table to do a basic thresholding.  If there are multiple
   pixels in a row of the same color they are collected together into a "run"
   and sent to the object detection module.  There are variations of this basic
   theme in accordance with the structure of the field and the importance of
   the potential object (i.e. balls are extra important).

   Once the scan is complete then run length encoding is done on each color
   object separately and potential field objects are extracted.

   These are then sanity checked based on things like where the horizon
   of the field is, the structure of the field
   (i.e. posts can't be too close to goals), etc.
*/

#include "ifdefs.h"
#include "Common.h"

#include <math.h>
#include <assert.h>
#if ROBOT(NAO_SIM)
#  include <aldefinitions.h>
#endif

#include "Threshold.h"
#include "debug.h"


// Constructor for Threshold class. passed an instance of Vision and Pose
Threshold::Threshold(Vision *vis, Pose *posPtr)
  : inverted(false)
{

  // storing locally
  vision = vis;
  pose = posPtr;
#ifdef OFFLINE
  visualHorizonDebug = false;
#endif

#ifdef USE_CHROMATIC_CORRECTION
#  if ROBOT(AIBO)
#    if defined(OFFLINE)
  initChromeTable("../tables/chrome/chrome.all");
#    else
  initChromeTable("/MS/chrome.all");
#    endif

#  elif ROBOT(NAO)
  // So far as we know there is no chromatic distortion
#    error No chromatic correction for the Nao robots

#  else
#    error Undefined robot type

#  endif // ROBOT(...)
#endif // USE_CHROMATIC_CORRECTION


  // loads the color table on the MS into memory
#if ROBOT(AIBO)
#  ifndef OFFLINE
  initCompressedTable("/MS/table.mtb");
#  endif
#elif ROBOT(NAO_RL)
# ifndef OFFLINE
  initTable("/opt/naoqi/modules/etc/table.mtb");
#endif
#elif ROBOT(NAO_SIM)
#  error Compressed table not set up for simulator Nao yet
#else
#  error Undefined robot type
#endif // OFFLINE
  // Set up object recognition object pointers
  blue = new ObjectFragments(vision,this);
  blue->setColor(BLUE);
  yellow = new ObjectFragments(vision,this);
  yellow->setColor(YELLOW);
  navyblue = new ObjectFragments(vision,this);
  navyblue->setColor(NAVY);
  red = new ObjectFragments(vision,this);
  red->setColor(RED);
  orange = new ObjectFragments(vision,this);
  orange->setColor(ORANGE);
  green = new ObjectFragments(vision,this);
  green->setColor(GREEN);

#ifdef USE_PINK_BALL
  ballPink = new ObjectFragments(vision,this);
  ballPink->setColor(PINK);
#endif

  // initialize the thresholded array to BLACK
  /*for (int i = 0; i < IMAGE_WIDTH; i++) {
    for (int j = 0; j < IMAGE_HEIGHT; j++) {
      thresholded[j][i] = BLACK;
    }
    }*/

}

#ifdef OFFLINE
void Threshold::setConstant(int c) {
  blue->setConstant(c);
  yellow->setConstant(c);
  orange->setConstant(c);
  navyblue->setConstant(c);
  red->setConstant(c);
  green->setConstant(c);
}
#endif

/* Main vision loop, called by Vision.cc
 */
void Threshold::visionLoop() {

  // threshold image and create runs
  thresholdAndRuns();


  // do line recognition (in FieldLines.cc)
  // This will form all lines and all corners. After this call, fieldLines
  // will be able to supply information about them through getLines() and
  // getCorners().
  PROF_ENTER(vision->profiler, P_OBJECT);
  vision->fieldLines->lineLoop();
  // do recognition
  objectRecognition();
  PROF_EXIT(vision->profiler, P_OBJECT);


  PROF_ENTER(vision->profiler, P_LINES);
  vision->fieldLines->afterObjectFragments();
  if (vision->bgBackstop->getWidth() > 0) {
    blue->setShot(vision->bgBackstop);
  }
  if (vision->ygBackstop->getWidth() > 0) {
    yellow->setShot(vision->ygBackstop);
  }
  yellow->openDirection(horizon, pose);
  PROF_EXIT(vision->profiler, P_LINES);



#ifdef OFFLINE
  if (visualHorizonDebug) {
    drawVisualHorizon();
  }
  transposeDebugImage();
#endif
}

#ifndef UNROLLED_LOOPS_THRESHOLD

/* Threshold and runs.  The goal here is to scan the image and collect up "runs" of color.
 * The ones we're particularly interested in are blue, yellow, orange and green (also red and dark blue).
 * Then we send the runs off to the object recognition system.  We scan the image from bottom to top.
 * Along the way we keep track of things like: where we saw blue-yellow and yellow-blue transitions,
 * where the green horizon line is, etc.
 */
void Threshold::thresholdAndRuns() {
  PROF_ENTER(vision->profiler, P_THRESHRUNS);
  initColors();


  /*for (int i=0; i < IMAGE_WIDTH; i++) {
    for (int j=0; j < IMAGE_HEIGHT; j++) {
      thresholded[j][i] = YELLOW;
    }
    }*/
  // these variables are used to detect posts
  // -they track when certain color combinations first occur
  firstYellowBlue = 0;
  firstBlueYellow = 0;

  // Determine where the field horizon is
  findGreenHorizon();

  int hor = 0;        // check horizon
  int previousRunSize = 0;
  float horizonSlope = pose->getHorizonSlope();
  register unsigned int address = ADDRESS_START;

  unsigned char pixel;
#ifdef USE_CHROMATIC_CORRECTION
  unsigned char radius;
#endif


  //printf("Horizon (l,r,pan):\t%d\t%d\t%g\n",pose->getLeftHorizonY(),pose->getRightHorizonY(),RAD2DEG(pose->getPan()));

  for (register int i = 0; i < IMAGE_WIDTH; i++) {//scan across
    // the color of the last pixel before the current one
    greenBlue[i] = false;
    greenYellow[i] = false;
    yellowWhite[i] = BADVALUE;
    blueWhite[i] = BADVALUE;
    lastPixel = GREEN;
    // how big is the current run of like colored pixels?
    currentRun = 0;
    // where in the column did it begin?
    //currentRunStart = IMAGE_HEIGHT - 1;
    // the color of the last run of pixels - useful for object detection
    previousRun = GREEN;
    // potential yellow post location
    sawYellowBlue = false;
    sawBlueYellow = false;
    //int horizonJ = pose->getHorizonY(i);
    int firstRed = -1, lastRed = -1;

    for (register int j = IMAGE_HEIGHT; j--; ) {//j > -1 ; j--) { //scan up
#ifdef USE_CHROMATIC_CORRECTION
      radius = xLUT[i][j];
      pixel = bigTable[corrY[yplane[address]][radius]>>YSHIFT]
                      [corrU[uplane[address]][radius]>>USHIFT]
                      [corrV[vplane[address]][radius]>>VSHIFT];
#else
#if ROBOT(NAO_SIM)
      pixel = bigTable[yplane[j*IMAGE_WIDTH*3 + 3*i + 0]>>YSHIFT]
                      [yplane[j*IMAGE_WIDTH*3 + 3*i + 1]>>USHIFT]
                      [yplane[j*IMAGE_WIDTH*3 + 3*i + 2]>>VSHIFT];

#else
      pixel = bigTable[yplane[address]>>YSHIFT]
                      [uplane[address]>>USHIFT]
                      [vplane[address]>>VSHIFT];
#endif
#endif
      newPixel = thresholded[j][i] = pixel;

      /* to move to the next pixel we just need to jump a row up
	 since the image array goes top to bottom and we're scanning
	 bottom to top, we need to subtract a row_offset instead of add one */
      address -= IMAGE_ROW_OFFSET;

      // check thresholded point with last thresholded point.
      // if the same, increment the current run

      if (lastPixel == newPixel) {
	currentRun++;
      }
      // otherwise, do stuff according to color
      if (lastPixel != newPixel || j == 0) { // j == 0 means end of column
	// switch for last thresholded pixel

	switch (lastPixel) {
	  // possible horizon detection and for postID (not that impt)
	case GREEN:
	  greenYellow[i] = false;
	  greenBlue[i] = false;
	  break;
	case WHITE:
	  if (currentRun > 2 && previousRunSize > 2 && j > horizon) {
	    if (previousRun == YELLOW)
	      yellowWhite[i] = j + currentRun;
	    else if (previousRun == BLUE)
	      blueWhite[i] = j + currentRun;
	  }
	  break;
	case BLUE:
	  // add to Blue data structure
	  hor = blue->horizonAt(i);

	  if (currentRun >= MIN_RUN_SIZE && (j < hor )) { // || currentRun > MIN_RUN_SIZE + 1) ) { // && j < hor + 15) { // noise eliminator
	    if (j > hor)
	      blueWhite[i] = BADVALUE;
	    // add run: x of start, y of start, height of run
	    if (i > 5 && i < IMAGE_WIDTH - 5) {
	      blue->newRun(i, j, currentRun);
	      // checks to see if this blue run was preceded by a yellow run
	      // which was preceded by a white run --> potential pb post
	      if (sawYellowBlue && previousRun == YELLOW) {
		if (firstYellowBlue == 0) {
		  firstYellowBlue = i;
		  firstYellowBlueY = j + currentRun;
		  //vision->drawVerticalLine(firstYellowBlue,PINK);
		} else if (j + currentRun > firstYellowBlueY) {
		  firstYellowBlueY = j + currentRun;
		}
	      }
	      // this initiates a possible blue yellow post: a blue run preceded
	      // by a white run
	      if (!sawBlueYellow && previousRun == WHITE) {
		sawBlueYellow = true;
	      }
	      if (previousRun == GREEN)
		greenBlue[i] = true;
	    }
	  }
	  break;

	case RED:
	  if (firstRed == -1)
	    firstRed = j + currentRun;
	  lastRed = j;
	  break;
	case NAVY:
	  break;
	case ORANGEYELLOW:
	  if (currentRun > 1 && previousRun != YELLOW && j + currentRun > blue->horizonAt(i))
	    orange->newRun(i, j, currentRun);
	  break;
	case ORANGE:
	case ORANGERED:
	  // add to Ball data structure
	  if (currentRun > 1 && j + currentRun > blue->horizonAt(i) - 10)
	    orange->newRun(i, j, currentRun);
	  break;
#ifdef USE_PINK_BALL
	case PINK:

	  if(currentRun > 1){
	    ballPink->newRun(i,j,currentRun);
	  }
	  break;
#endif
	case YELLOW:
	  // add to Yellow data structure
	  if (currentRun >= MIN_RUN_SIZE && (j < blue->horizonAt(i)  || currentRun > MIN_RUN_SIZE + 1)) {
	    hor = horizon + (int)(horizonSlope * (float)(i));
	    if (j > hor)
	      yellowWhite[i] = BADVALUE;

	    yellow->newRun(i, j, currentRun);
	    // checks to see if this yellow run was preceded by a blue run
	    // which was preceded by a white run --> potential by post
	    if (sawBlueYellow && previousRun == BLUE) {
	      if (firstBlueYellow == 0) {
		firstBlueYellow = i;
		firstBlueYellowY = j;
	      }
	    }
	    // this initiates a possible yellow-blue post scan
	    if (!sawYellowBlue && previousRun == WHITE) {
	      sawYellowBlue = true;
	    }
	    if (previousRun == GREEN)
	      greenYellow[i] = true;
	  }
	  break;
	  }
	// store the last run
	if (currentRun > MIN_RUN_SIZE || (currentRun > 2 && (lastPixel == BLUE || lastPixel == YELLOW))) previousRun = lastPixel;
	previousRunSize = currentRun;
	// since this loop runs when a run ends, restart # pixels in run counter
	currentRun = 1;
	// store the position of the start of the run in the column (y-value)
	//currentRunStart = j;
      }
      // every pixel.
      lastPixel = newPixel;
    }//end j loop
    if (lastRed > -1) {
      red->newRun(i, lastRed,  firstRed - lastRed);
    }
    address += ADDRESS_JUMP;//(IMAGE_HEIGHT-1)*row_skip + 1;
  }//end i loop

  PROF_EXIT(vision->profiler, P_THRESHRUNS);
}





/*
 * Check the left side of the image, looking to find an edge of the field.  Our goal is to determine the basic
 * shape of the field.  If we find the edge on the left side, then we can use it to find any landmarks - which
 * ought to be right on the edge.
 */

void Threshold::findGreenHorizon() {
#ifdef JOHO_DEBUG
print("   Theshold::SweepLeft");
#endif
  // a useful point is where we stop seeing green in the first few pixels

  // if the pose estimated horizon is less than 0, then just use it directly
  int pH = pose->getHorizonY(0);
  //if (pH < 0) {
  //horizon = pH;
  //return;
  //}

#if ROBOT(NAO_SIM)
  //robot_console_printf("pH=%d\n",pH);
#endif

  horizon = -1;
  int run = 0;                 // how many consecutive green pixels have I seen?
  int greenPixels = 0;
  int scanY = 0;
  int i, j;
  //int bluepix = 0, lastBlue = 0;
  //int yellowpix = 0, lastYellow = 0;

  // we're going to do this backwards of how we used to - we start at the pose horizon and scan down
  for (j = max(0,pH); j < IMAGE_HEIGHT && horizon == -1; j++) {
    greenPixels = 0;
    run = 0;
    scanY = 0;
    //lastBlue = bluepix;
    //lastYellow = yellowpix;
    for (i = 0; i < IMAGE_WIDTH && scanY < IMAGE_HEIGHT && scanY > -1; i++) {
	int address = scanY * IMAGE_ROW_OFFSET + i;
#ifdef USE_CHROMATIC_CORRECTION
	unsigned char radius = xLUT[i][j];
	unsigned char pixel = bigTable
	  [corrY[yplane[address]][radius]>>YSHIFT]
	  [corrU[uplane[address]][radius]>>USHIFT]
	  [corrV[vplane[address]][radius]>>VSHIFT];

#else
	unsigned char pixel = bigTable
	  [yplane[address]>>YSHIFT]
	  [uplane[address]>>USHIFT]
	  [vplane[address]>>VSHIFT];
#endif

	newPixel = pixel;

	if (newPixel == GREEN) {
	  run++;
	  greenPixels++;
	  //thresholded[scanY][i] = RED;
	  if (run > MIN_GREEN_SIZE || greenPixels > 20) {
	    if (j == pH) {
	      // either our horizon estimate was perfect, or it was an underestimate.  Let's find out
	      int k, l;
	      for (k = j; k > -1 && horizon == -1; k--) {
		greenPixels = 0;
		run = 0;
		int whites = 0;
		scanY = 0;
		for (l = 0; l < IMAGE_WIDTH && scanY < IMAGE_HEIGHT && scanY > -1 && run < MIN_GREEN_SIZE && greenPixels < 20; l++) {
		  //drawPoint(l, scanY, BLACK);
		  int address = scanY * IMAGE_ROW_OFFSET + l;
#ifdef USE_CHROMATIC_CORRECTION
		  unsigned char radius = xLUT[l][k];
		  unsigned char pixel = bigTable
		    [corrY[yplane[address]][radius]>>YSHIFT]
		    [corrU[uplane[address]][radius]>>USHIFT]
		    [corrV[vplane[address]][radius]>>VSHIFT];

#else
		  unsigned char pixel = bigTable
		    [yplane[address]>>YSHIFT]
		    [uplane[address]>>USHIFT]
		    [vplane[address]>>VSHIFT];
#endif

		  newPixel = pixel;
		  if (newPixel == GREEN) {
		    run++;
		    greenPixels++;
		  } else if (newPixel == WHITE) {
		    run = 0;
		    whites++;
		  } else {
		    run = 0;
		  }
		  scanY = blue->yProject(0, k, l);
		}
		if (run < MIN_GREEN_SIZE && greenPixels < 20 && whites + greenPixels < IMAGE_WIDTH - 20) {
		  //cout << "Found horizon " << k << " " << run << " " << greenPixels << " " << whites << endl;
		  //drawPoint(100, k + 1, BLACK);
		  //drawLine(0, k+1, IMAGE_WIDTH - 1, blue->yProject(0, k+1, IMAGE_WIDTH - 1), MAROON);
		  horizon = k + 1;
		  return;
		}
	      }
	      horizon = pH;
	      return;
	    }
	    if (j == 0 && pH < j) {
	      horizon = pH;
	      return;
	    }
	    //cout << "Horizon is " << j << " estimate was " << pH << endl;
	    //drawLine(0, j, IMAGE_WIDTH - 1, blue->yProject(0, j, IMAGE_WIDTH - 1), MAROON);
	    horizon = j;
	    return;
	  }
	} else if (newPixel != GREY) {
	  //if (newPixel == BLUE) bluepix++;
	  //else if (newPixel == YELLOW) yellowpix++;
	  run = 0;
	  //thresholded[scanY][i] = BLUE;
	}
	scanY = blue->yProject(0, j, i);
      }
  }
  // so we've scanned all the way to the bottom of the image and haven't found a horizon.  There are several
  // possibilities:  1) we're off the field, 2) an object or robot is blocking our view, 3) we have problems
  // Let's try and figure out if we're off the field or being blocked
  horizon = pH;
  j = IMAGE_HEIGHT / 2;
  run = 0;
  for (i = 0; i < IMAGE_WIDTH; i++) {
    int address = j * IMAGE_ROW_OFFSET + i;
#ifdef USE_CHROMATIC_CORRECTION
    unsigned char radius = xLUT[i][j];
    unsigned char pixel = bigTable
      [corrY[yplane[address]][radius]>>YSHIFT]
      [corrU[uplane[address]][radius]>>USHIFT]
      [corrV[vplane[address]][radius]>>VSHIFT];

#else
    unsigned char pixel = bigTable
      [yplane[address]>>YSHIFT]
      [uplane[address]>>USHIFT]
      [vplane[address]>>VSHIFT];
#endif
    newPixel = pixel;
    switch (newPixel) {
    case GREEN:
    case WHITE:
    case BLUE:
    case YELLOW:
    case ORANGE:
    case RED:
    case NAVY:
      run++;
      break;
    }
  }
  if (run > IMAGE_WIDTH / 4) {
    horizon = pH;
    //cout << "Blocked" << endl;
  } else {
    horizon = IMAGE_HEIGHT;
    //cout << "Unblocked" << endl;
  }
}

#else // UNROLLED_LOOPS_THRESHOLD

/* Thresholding.  Since there's no real benefit (and in fact can it can be a
 * detriment with compiler optimizations on) to combine the thresholding and
 * the runs loops, I (Jeremy) have split out the thresholding into it's own
 * method here.  This also allows for my incredibly complex unrolled loops for
 * each robot to be separated from other, clearer, code.
 */
void Threshold::threshold() {
  // My loop variables
  int m;
  unsigned char *tPtr, *tEnd, *tOff; // pointers into thresholded array
  const unsigned char *yPtr, *uPtr, *vPtr; // pointers into image array
#ifdef USE_CHROMATIC_CORRECTION
  const unsigned char *rPtr; // pointer into xLUT array
#endif
#if ROBOT(AIBO)
  const unsigned char *uStart;
#endif

  // My loop variable initializations
  yPtr = &yplane[0];
  uPtr = &uplane[0];
  vPtr = &vplane[0];

  tPtr = &thresholded[0][0];
  tEnd = &thresholded[IMAGE_HEIGHT-1][IMAGE_WIDTH-1] + 1;

#ifdef USE_CHROMATIC_CORRECTION
  rPtr = &xLUT[0][0];
#endif

#if ROBOT(AIBO)
  m = IMAGE_WIDTH % 8;

  while (tPtr < tEnd) {

    // number of non-unrolled offset from beginning of row
    tOff = tPtr + m;
    // start of U data (end of row reached)
    uStart = uPtr;

    // here is non-unrolled loop
    while (tPtr < tOff)
#ifdef USE_CHROMATIC_CORRECTION
      *tPtr++ = bigTable[corrY[*yPtr++][*rPtr  ]>>YSHIFT]
                        [corrU[*uPtr++][*rPtr  ]>>USHIFT]
                        [corrV[*vPtr++][*rPtr++]>>VSHIFT];
#else
      *tPtr++ = bigTable[*yPtr++>>YSHIFT][*uPtr++>>USHIFT][*vPtr++>>VSHIFT];
#endif

    // here is the unrolled loop
    while (yPtr < uStart && tPtr < tEnd) {
      // Eight unrolled table lookups
#ifdef USE_CHROMATIC_CORRECTION
      *tPtr++ = bigTable[corrY[*yPtr++][*rPtr  ]>>YSHIFT]
                        [corrU[*uPtr++][*rPtr  ]>>USHIFT]
                        [corrV[*vPtr++][*rPtr++]>>VSHIFT];
      *tPtr++ = bigTable[corrY[*yPtr++][*rPtr  ]>>YSHIFT]
                        [corrU[*uPtr++][*rPtr  ]>>USHIFT]
                        [corrV[*vPtr++][*rPtr++]>>VSHIFT];
      *tPtr++ = bigTable[corrY[*yPtr++][*rPtr  ]>>YSHIFT]
                        [corrU[*uPtr++][*rPtr  ]>>USHIFT]
                        [corrV[*vPtr++][*rPtr++]>>VSHIFT];
      *tPtr++ = bigTable[corrY[*yPtr++][*rPtr  ]>>YSHIFT]
                        [corrU[*uPtr++][*rPtr  ]>>USHIFT]
                        [corrV[*vPtr++][*rPtr++]>>VSHIFT];
      *tPtr++ = bigTable[corrY[*yPtr++][*rPtr  ]>>YSHIFT]
                        [corrU[*uPtr++][*rPtr  ]>>USHIFT]
                        [corrV[*vPtr++][*rPtr++]>>VSHIFT];
      *tPtr++ = bigTable[corrY[*yPtr++][*rPtr  ]>>YSHIFT]
                        [corrU[*uPtr++][*rPtr  ]>>USHIFT]
                        [corrV[*vPtr++][*rPtr++]>>VSHIFT];
      *tPtr++ = bigTable[corrY[*yPtr++][*rPtr  ]>>YSHIFT]
                        [corrU[*uPtr++][*rPtr  ]>>USHIFT]
                        [corrV[*vPtr++][*rPtr++]>>VSHIFT];
      *tPtr++ = bigTable[corrY[*yPtr++][*rPtr  ]>>YSHIFT]
                        [corrU[*uPtr++][*rPtr  ]>>USHIFT]
                        [corrV[*vPtr++][*rPtr++]>>VSHIFT];
#else
      *tPtr++ = bigTable[*yPtr++>>YSHIFT][*uPtr++>>USHIFT][*vPtr++>>VSHIFT];
      *tPtr++ = bigTable[*yPtr++>>YSHIFT][*uPtr++>>USHIFT][*vPtr++>>VSHIFT];
      *tPtr++ = bigTable[*yPtr++>>YSHIFT][*uPtr++>>USHIFT][*vPtr++>>VSHIFT];
      *tPtr++ = bigTable[*yPtr++>>YSHIFT][*uPtr++>>USHIFT][*vPtr++>>VSHIFT];
      *tPtr++ = bigTable[*yPtr++>>YSHIFT][*uPtr++>>USHIFT][*vPtr++>>VSHIFT];
      *tPtr++ = bigTable[*yPtr++>>YSHIFT][*uPtr++>>USHIFT][*vPtr++>>VSHIFT];
      *tPtr++ = bigTable[*yPtr++>>YSHIFT][*uPtr++>>USHIFT][*vPtr++>>VSHIFT];
      *tPtr++ = bigTable[*yPtr++>>YSHIFT][*uPtr++>>USHIFT][*vPtr++>>VSHIFT];
#endif
    }
    // Jump over
    yPtr += ROW_OFFSET - IMAGE_WIDTH;
    uPtr += ROW_OFFSET - IMAGE_WIDTH;
    vPtr += ROW_OFFSET - IMAGE_WIDTH;
  }

#elif ROBOT(NAO_SIM)
    m = (IMAGE_WIDTH * IMAGE_HEIGHT) % 8;

    // number of non-unrolled offset from beginning of row
    tOff = tPtr + m;

    // here is non-unrolled loop
    while (tPtr < tOff)
#ifdef USE_CHROMATIC_CORRECTION
      *tPtr++ = bigTable[corrY[*yPtr++][*rPtr  ]>>YSHIFT]
                        [corrU[*yPtr++][*rPtr  ]>>USHIFT]
                        [corrV[*yPtr++][*rPtr++]>>VSHIFT];
#else
      *tPtr++ = bigTable[*yPtr++>>YSHIFT][*yPtr++>>USHIFT][*yPtr++>>VSHIFT];
#endif

    // here is the unrolled loop
    while (tPtr < tEnd) {
      // Eight unrolled table lookups
#ifdef USE_CHROMATIC_CORRECTION
      *tPtr++ = bigTable[corrY[*yPtr++][*rPtr  ]>>YSHIFT]
                        [corrU[*yPtr++][*rPtr  ]>>USHIFT]
                        [corrV[*yPtr++][*rPtr++]>>VSHIFT];
      *tPtr++ = bigTable[corrY[*yPtr++][*rPtr  ]>>YSHIFT]
                        [corrU[*yPtr++][*rPtr  ]>>USHIFT]
                        [corrV[*yPtr++][*rPtr++]>>VSHIFT];
      *tPtr++ = bigTable[corrY[*yPtr++][*rPtr  ]>>YSHIFT]
                        [corrU[*yPtr++][*rPtr  ]>>USHIFT]
                        [corrV[*yPtr++][*rPtr++]>>VSHIFT];
      *tPtr++ = bigTable[corrY[*yPtr++][*rPtr  ]>>YSHIFT]
                        [corrU[*yPtr++][*rPtr  ]>>USHIFT]
                        [corrV[*yPtr++][*rPtr++]>>VSHIFT];
      *tPtr++ = bigTable[corrY[*yPtr++][*rPtr  ]>>YSHIFT]
                        [corrU[*yPtr++][*rPtr  ]>>USHIFT]
                        [corrV[*yPtr++][*rPtr++]>>VSHIFT];
      *tPtr++ = bigTable[corrY[*yPtr++][*rPtr  ]>>YSHIFT]
                        [corrU[*yPtr++][*rPtr  ]>>USHIFT]
                        [corrV[*yPtr++][*rPtr++]>>VSHIFT];
      *tPtr++ = bigTable[corrY[*yPtr++][*rPtr  ]>>YSHIFT]
                        [corrU[*yPtr++][*rPtr  ]>>USHIFT]
                        [corrV[*yPtr++][*rPtr++]>>VSHIFT];
      *tPtr++ = bigTable[corrY[*yPtr++][*rPtr  ]>>YSHIFT]
                        [corrU[*yPtr++][*rPtr  ]>>USHIFT]
                        [corrV[*yPtr++][*rPtr++]>>VSHIFT];
#else
      *tPtr++ = bigTable[*yPtr++>>YSHIFT][*yPtr++>>USHIFT][*yPtr++>>VSHIFT];
      *tPtr++ = bigTable[*yPtr++>>YSHIFT][*yPtr++>>USHIFT][*yPtr++>>VSHIFT];
      *tPtr++ = bigTable[*yPtr++>>YSHIFT][*yPtr++>>USHIFT][*yPtr++>>VSHIFT];
      *tPtr++ = bigTable[*yPtr++>>YSHIFT][*yPtr++>>USHIFT][*yPtr++>>VSHIFT];
      *tPtr++ = bigTable[*yPtr++>>YSHIFT][*yPtr++>>USHIFT][*yPtr++>>VSHIFT];
      *tPtr++ = bigTable[*yPtr++>>YSHIFT][*yPtr++>>USHIFT][*yPtr++>>VSHIFT];
      *tPtr++ = bigTable[*yPtr++>>YSHIFT][*yPtr++>>USHIFT][*yPtr++>>VSHIFT];
      *tPtr++ = bigTable[*yPtr++>>YSHIFT][*yPtr++>>USHIFT][*yPtr++>>VSHIFT];
#endif
    }

#elif ROBOT(NAO_RL)
    m = (IMAGE_WIDTH * IMAGE_HEIGHT) % 8;

    // number of non-unrolled offset from beginning of row
    tOff = tPtr + m;

    // due to YUV422 data, we can only increment u & v every two assigments
    // thus, we need to do different stuff if we start off with even or odd
    // remainder.  However, we won't get odd # of pixels in 422 (not valid), so
    // lets ignore that

    // here is non-unrolled loop (unrolled by 2, actually)
    while (tPtr < tOff) {
      // we increment Y by 2 every time, and U and V by 4 every two times
#ifdef USE_CHROMATIC_CORRECTION
      *tPtr++ = bigTable[corrY[*yPtr][*rPtr  ]>>YSHIFT]
                        [corrU[*uPtr][*rPtr  ]>>USHIFT]
                        [corrV[*vPtr][*rPtr++]>>VSHIFT];
      yPtr+=2;
      *tPtr++ = bigTable[corrY[*yPtr][*rPtr  ]>>YSHIFT]
                        [corrU[*uPtr][*rPtr  ]>>USHIFT]
                        [corrV[*vPtr][*rPtr++]>>VSHIFT];
      yPtr+=2; uPtr+=4; vPtr+=4;
#else
      *tPtr++ = bigTable[*yPtr>>YSHIFT][*uPtr>>USHIFT][*vPtr>>VSHIFT];
      yPtr+=2;
      *tPtr++ = bigTable[*yPtr>>YSHIFT][*uPtr>>USHIFT][*vPtr>>VSHIFT];
      yPtr+=2; uPtr+=4; vPtr+=4;
#endif
    }

    // here is the unrolled loop
    while (tPtr < tEnd) {
      // Eight unrolled table lookups
#ifdef USE_CHROMATIC_CORRECTION
      *tPtr++ = bigTable[corrY[*yPtr][*rPtr  ]>>YSHIFT]
                        [corrU[*uPtr][*rPtr  ]>>USHIFT]
                        [corrV[*vPtr][*rPtr++]>>VSHIFT];
      yPtr+=2;
      *tPtr++ = bigTable[corrY[*yPtr][*rPtr  ]>>YSHIFT]
                        [corrU[*uPtr][*rPtr  ]>>USHIFT]
                        [corrV[*vPtr][*rPtr++]>>VSHIFT];
      yPtr+=2; uPtr+=4; vPtr+=4;
      *tPtr++ = bigTable[corrY[*yPtr][*rPtr  ]>>YSHIFT]
                        [corrU[*uPtr][*rPtr  ]>>USHIFT]
                        [corrV[*vPtr][*rPtr++]>>VSHIFT];
      yPtr+=2;
      *tPtr++ = bigTable[corrY[*yPtr][*rPtr  ]>>YSHIFT]
                        [corrU[*uPtr][*rPtr  ]>>USHIFT]
                        [corrV[*vPtr][*rPtr++]>>VSHIFT];
      yPtr+=2; uPtr+=4; vPtr+=4;
      *tPtr++ = bigTable[corrY[*yPtr][*rPtr  ]>>YSHIFT]
                        [corrU[*uPtr][*rPtr  ]>>USHIFT]
                        [corrV[*vPtr][*rPtr++]>>VSHIFT];
      yPtr+=2;
      *tPtr++ = bigTable[corrY[*yPtr][*rPtr  ]>>YSHIFT]
                        [corrU[*uPtr][*rPtr  ]>>USHIFT]
                        [corrV[*vPtr][*rPtr++]>>VSHIFT];
      yPtr+=2; uPtr+=4; vPtr+=4;
      *tPtr++ = bigTable[corrY[*yPtr][*rPtr  ]>>YSHIFT]
                        [corrU[*uPtr][*rPtr  ]>>USHIFT]
                        [corrV[*vPtr][*rPtr++]>>VSHIFT];
      yPtr+=2;
      *tPtr++ = bigTable[corrY[*yPtr][*rPtr  ]>>YSHIFT]
                        [corrU[*uPtr][*rPtr  ]>>USHIFT]
                        [corrV[*vPtr][*rPtr++]>>VSHIFT];
      yPtr+=2; uPtr+=4; vPtr+=4;
#else
      *tPtr++ = bigTable[*yPtr>>YSHIFT][*uPtr>>USHIFT][*vPtr>>VSHIFT];
      yPtr+=2;
      *tPtr++ = bigTable[*yPtr>>YSHIFT][*uPtr>>USHIFT][*vPtr>>VSHIFT];
      yPtr+=2; uPtr+=4; vPtr+=4;
      *tPtr++ = bigTable[*yPtr>>YSHIFT][*uPtr>>USHIFT][*vPtr>>VSHIFT];
      yPtr+=2;
      *tPtr++ = bigTable[*yPtr>>YSHIFT][*uPtr>>USHIFT][*vPtr>>VSHIFT];
      yPtr+=2; uPtr+=4; vPtr+=4;
      *tPtr++ = bigTable[*yPtr>>YSHIFT][*uPtr>>USHIFT][*vPtr>>VSHIFT];
      yPtr+=2;
      *tPtr++ = bigTable[*yPtr>>YSHIFT][*uPtr>>USHIFT][*vPtr>>VSHIFT];
      yPtr+=2; uPtr+=4; vPtr+=4;
      *tPtr++ = bigTable[*yPtr>>YSHIFT][*uPtr>>USHIFT][*vPtr>>VSHIFT];
      yPtr+=2;
      *tPtr++ = bigTable[*yPtr>>YSHIFT][*uPtr>>USHIFT][*vPtr>>VSHIFT];
      yPtr+=2; uPtr+=4; vPtr+=4;
#endif // USE_CHROMATIC_CORRECTION
    }

#endif // ROBOT(...)
}

/* Image runs.  As explained in the comments for the threshold() method, I
 * (Jeremy) have split the thresholdAndRuns() method into parts.  This also
 * helped with working out the slow sections of code.
 */
void Threshold::runs() {
  // variable declarations
  int hor;
  float horizonSlope;
  register int address, i, j;
  unsigned char pixel, lastPixel;
#ifdef USE_CHROMATIC_CORRECTION
  unsigned char radius;
#endif

  // variable initializations
  hor = 0;
  address = ADDRESS_START;
  // these variables are used to detect posts
  // -they track when certain color combinations first occur

  horizonSlope = pose->getHorizonSlope();

  // split up the loops
  for (i = 0; i < IMAGE_WIDTH; i = i + 2) {//scan across
    // the color of the last pixel before the current one
    lastPixel = GREEN;
    // how big is the current run of like colored pixels?
    currentRun = 0;
    // where in the column did it begin?
    //currentRunStart = IMAGE_HEIGHT - 1;
    // the color of the last run of pixels - useful for object detection
    previousRun = GREEN;
    int previousRunStop = IMAGE_HEIGHT;
    navyTops[i] = -1;
    redTops[i] = -1;
    redBottoms[i] = -1;
    navyBottoms[i] = -1;
    // potential yellow post location
    int lastGoodPixel = 0;
    //int horizonJ = pose->getHorizonY(i);

    for (j = IMAGE_HEIGHT - 1; j--; ) { //scan up
      if (j < horizon && lastGoodPixel - j > 10 && currentRun == 1) break;
      pixel = thresholded[j][i];

      // check thresholded point with last thresholded point.
      // if the same, increment the current run

      if (lastPixel == pixel) {
	currentRun++;
      }
      // otherwise, do stuff according to color
      if (lastPixel != pixel || j == 0) { // j == 0 means end of column
	// switch for last thresholded pixel

	switch (lastPixel) {
	  // possible horizon detection and for postID (not that impt)
	case NAVY:
	  if (currentRun > 8 && (previousRun == WHITE || previousRun == NAVY)) {
	    //navyblue->newRun(i, j, currentRun);
	    lastGoodPixel = j;
	  }
	  if (currentRun > 3 &&
	      (previousRun == WHITE || previousRun == NAVY ||
	       (previousRun == GREEN || currentRun > 20))) { //&& previousRunStop - j - currentRun < 10))) {
	    navyTops[i] = j;
	    //drawPoint(i, j, YELLOW);
	  }
	  if (navyBottoms[i] == -1 && currentRun > 3 && (previousRun == WHITE || previousRun == GREEN || currentRun > 20)) {
	    navyBottoms[i] = j + currentRun;
	    //drawPoint(i, j + currentRun, ORANGE);
	  }
	  break;
	case RED:
	  if (currentRun > 8 && (previousRun == WHITE || previousRun == RED || previousRun == ORANGE ||
				 (previousRun == GREEN ) || currentRun > 20)) {//&& previousRunStop - j - currentRun < 10))) {
	    lastGoodPixel = j;
	  }
	  if (currentRun > 3 && (previousRun == WHITE || previousRun == RED || previousRun == ORANGE ||
				 previousRun == GREEN || currentRun > 20)) {
	    redTops[i] = j;
	  }
	  if (redBottoms[i] == -1 && currentRun > 3 && (previousRun == WHITE || previousRun == GREEN || previousRun == ORANGE)) {
	    redBottoms[i] = j + currentRun;
	  }
	  break;
	case GREEN:
	  if (currentRun > 20) {
	    redBottoms[i] = -1;
	    navyBottoms[i] = -1;
	  }

	  break;
	case BLUE:
	  // add to Blue data structure
	  hor = horizon + (int)(horizonSlope * (float)(i));

	  if (currentRun > MIN_RUN_SIZE) { // noise eliminator
	    lastGoodPixel = j;
	    // add run: x of start, y of start, height of run
	    if (i > 5 && i < IMAGE_WIDTH - 5) {
	      blue->newRun(i, j, currentRun);
	    }
	  }
	  break;

	case ORANGE:
	case ORANGERED:
	  // add to Ball data structure
	  if (currentRun > 3 && j+currentRun > horizon - 30) {
	    orange->newRun(i, j, currentRun);
	    lastGoodPixel = j;
	  }
	  break;
#ifdef USE_PINK_BALL
	case PINK:

	  if(currentRun > 1){
	    ballPink->newRun(i,j,currentRun);
	  }
	  break;
#endif
	case YELLOW:
	  // add to Yellow data structure
	  if (currentRun >= MIN_RUN_SIZE) {
	    lastGoodPixel = j;
	    yellow->newRun(i, j, currentRun);
	  }
	  break;
	case WHITE:
	  if (currentRun >= MIN_RUN_SIZE) {
	    lastGoodPixel = j;
	  }
	  break;
	}
	// store the last run
	if (currentRun > MIN_RUN_SIZE) {
	  previousRun = lastPixel;
	  previousRunStop = j;
	}
	// since this loop runs when a run ends, restart # pixels in run counter
	currentRun = 1;
	// store the position of the start of the run in the column (y-value)
	//currentRunStart = j;
      }
      // every pixel.
      lastPixel = pixel;
    }//end j loop
    //if (j > 1) {
    //for (j = j - 10 ; j > -1; j--) {
    //thresholded[j][i] = BROWN;
    //}
    //}
  }//end i loop
  int bigh = IMAGE_HEIGHT, firstn = -1, lastn = -1, bot = -1;
  for (i = 0; i < IMAGE_WIDTH - 1; i+= 2) {
    if (navyTops[i] != -1) {
      firstn = i;
      lastn = 0;
      bigh = navyTops[i];
      bot = navyBottoms[i];
      while ((navyTops[i] != -1 || navyTops[i+2] != -1) && i < IMAGE_WIDTH - 3) {
	if (navyTops[i] < bigh && navyTops[i] != -1) {
	  bigh = navyTops[i];
	}
	if (navyBottoms[i] > bot) {
	  bot = navyBottoms[i];
	}
	i+=2;
	lastn+= 2;
      }
      for (int k = firstn; k < firstn + lastn; k+= 2) {
	navyblue->newRun(k, bigh, bot - bigh);
	//cout << "Runs " << k << " " << bigh << " " << (bot - bigh) << endl;
      }
      //cout << "Last " << lastn << " " << bigh << " " << bot << endl;
      //drawRect(firstn, bigh, lastn, bot - bigh, RED);
    }
  }
  for (i = 0; i < IMAGE_WIDTH - 1; i+= 2) {
    if (redTops[i] != -1) {
      firstn = i;
      lastn = 0;
      bigh = redTops[i];
      bot = redBottoms[i];
      while ((redTops[i] != -1 || redTops[i+2] != -1) && i < IMAGE_WIDTH - 3) {
	if (redTops[i] < bigh && redTops[i] != -1) {
	  bigh = redTops[i];
	}
	if (redBottoms[i] > bot) {
	  bot = redBottoms[i];
	}
	i+=2;
	lastn+= 2;
      }
      for (int k = firstn; k < firstn + lastn; k+= 2) {
	red->newRun(k, bigh, bot - bigh);
	//cout << "Runs " << k << " " << bigh << " " << (bot - bigh) << endl;
      }
      //cout << "Last " << lastn << " " << bigh << " " << bot << endl;
      //drawRect(firstn, bigh, lastn, bot - bigh, RED);
    }
  }

}


/* Threshold and runs.  The goal here is to scan the image and collect up "runs" of color.
 * The ones we're particularly interested in are blue, yellow, orange and green (also red and dark blue).
 * Then we send the runs off to the object recognition system.  We scan the image from bottom to top.
 * Along the way we keep track of things like: where we saw blue-yellow and yellow-blue transitions,
 * where the green horizon line is, etc.
 */
void Threshold::thresholdAndRuns() {
  PROF_ENTER(vision->profiler, P_THRESHRUNS); // profiling

  // Perform image thresholding
  PROF_ENTER(vision->profiler, P_THRESHOLD);
  threshold();
  PROF_EXIT(vision->profiler, P_THRESHOLD);

  // Determine where the field horizon is
  PROF_ENTER(vision->profiler, P_FGHORIZON);
  findGreenHorizon();
  PROF_EXIT(vision->profiler, P_FGHORIZON);

  // 'Run' up the image to find color-grouped pixel sequences
  PROF_ENTER(vision->profiler, P_RUNS);
  runs();
  PROF_EXIT(vision->profiler, P_RUNS);

  PROF_EXIT(vision->profiler, P_THRESHRUNS);
}


/*
 * Check the left side of the image, looking to find an edge of the field.  Our goal is to determine the basic
 * shape of the field.  If we find the edge on the left side, then we can use it to find any landmarks - which
 * ought to be right on the edge.
 */

void Threshold::findGreenHorizon() {
#ifdef JOHO_DEBUG
print("   Theshold::SweepLeft");
#endif
  // a useful point is where we stop seeing green in the first few pixels

  //variable definitions
  int pH, run, greenPixels, scanY;
  register int i, j;
  unsigned char pixel; //, lastPixel;
  initColors();

  // if the pose estimated horizon is less than 0, then just use it directly
  pH = pose->getHorizonY(0);
  //if (pH < 0) {
  //horizon = pH;
  //return;
  //}

  horizon = -1;
  run = 0;                 // how many consecutive green pixels have I seen?
  greenPixels = 0;
  scanY = 0;
  int firstpix = 0;
  // we're going to do this backwards of how we used to - we start at the pose horizon and scan down
  for (j = pH; j < IMAGE_HEIGHT && horizon == -1; j+=4) {
    greenPixels = 0;
    run = 0;
    scanY = 0;
    //lastBlue = bluepix;
    //lastYellow = yellowpix;
    for (i = 0; i < IMAGE_WIDTH && scanY < IMAGE_HEIGHT && scanY > -1 && greenPixels < 3; i+= 10) {
        pixel = thresholded[scanY][i];
	if (pixel == GREEN) {
	  greenPixels++;
	}
	scanY = blue->yProject(0, j, i);
    }
    if (greenPixels > 2) {
      break;
    }
  }
  // we should have a base estimate, let's move it up
  int k, l, minpix = IMAGE_WIDTH, minpixrow = -1;
  //cout << "initial estimate is " << j << " " << pH << endl;
  horizon = j;
  for (k = min(horizon, IMAGE_HEIGHT - 2); k > -1; k-=4) {
    greenPixels = 0;
    run = 0;
    scanY = 0;
    for (l = max(0, firstpix - 5), firstpix = -1; l < IMAGE_WIDTH && scanY < IMAGE_HEIGHT && scanY > -1 && run < MIN_GREEN_SIZE && greenPixels < 20; l+=3) {
      //drawPoint(l, scanY, BLACK);
      newPixel = thresholded[scanY][l];
      if (newPixel == GREEN) {
	if (firstpix == -1) {
	  //cout << "firstpix " << i << endl;;
	  firstpix = l;
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
      scanY = blue->yProject(0, k, l);
    }
    if (run < MIN_GREEN_SIZE && greenPixels < 20) {
      // first make sure we didn't get fooled by firstpix
      run = 0;
      scanY = firstpix;
      for (j = firstpix - 1; j >= 0; j--) {
	newPixel = thresholded[scanY][j];
	//drawPoint(j, scanY, BLACK);
	if (newPixel == GREEN) {
	  run++;
	  greenPixels++;
	  firstpix = j;
	}
	scanY = blue->yProject(0, k, j);
      }
      if (run < MIN_GREEN_SIZE && greenPixels < 20) {
	//cout << "Found horizon " << k << " " << run << " " << greenPixels << endl;
	//drawPoint(100, k + 1, BLACK);
	//drawLine(0, k+2, IMAGE_WIDTH - 1, green->yProject(0, k+2, IMAGE_WIDTH - 1), MAROON);
	//drawLine(minpix, minpixrow, firstpix, k + 2, RED);
	horizon = k + 2;
	return;
      }
    }
  }
  horizon = 0;
}

#endif // UNROLLED_LOOPS_THRESHOLD

point <int> Threshold::findIntersection(int col, int dir, int c) {
  point <int> ret;
  ret.x = BADVALUE; ret.y = BADVALUE;
  for (int i = col; i > -1 && i < IMAGE_WIDTH; i += dir) {
    if (c == BLUE) {
      if (blueWhite[i] != BADVALUE) {
	ret.x = i;
	ret.y = blueWhite[i];
	return ret;
      }
    } else {
      if (yellowWhite[i] != BADVALUE) {
	ret.x = i;
	ret.y = yellowWhite[i];
	return ret;
      }
    }
  }
  return ret;
}

point <int> Threshold::backStopCheck(bool which, int leftRange, int rightRange) {
  int left = -1, right = -1, total = 0;
  int bestLeft = -1, bestRight = -1, bestTotal = 0;
  int bads = 0;
  for (int i = leftRange + 1; i < rightRange; i++) {
    if ((which && greenBlue[i]) || (!which && greenYellow[i])) {
      bads = 0;
      if (total == 0)
	left = i;
      right = i;
      total++;
      if (total > bestTotal) {
	bestTotal = total;
	bestLeft = left;
	bestRight = right;
      }
    } else {
      bads++;
      if (bads > 1)
	total = 0;
    }
  }
  point <int> result;
  result.x = bestLeft; result.y = bestRight;
  return result;
}

int Threshold::postCheck(bool which, int left, int right) {
  int rc = 0, lc = 0;
  if (which) {
    for (int i = 0; i < left; i++) {
      if (greenBlue[i])
	lc++;
    }
    for (int i = right; i < IMAGE_WIDTH; i++) {
      if (greenBlue[i])
	rc++;
    }
  } else {
    for (int i = 0; i < left; i++) {
      if (greenYellow[i])
	lc++;
    }
    for (int i = right; i < IMAGE_WIDTH; i++) {
      if (greenYellow[i])
	rc++;
    }
  }
  return lc - rc;
}


/*  Makes the calls to the vision system to recognize objects.  Then performs some extra
 * sanity checks to make sure we don't have weird cases like 2 beacons.
 */

void Threshold::objectRecognition() {
#ifdef JOHO_DEBUG
print("   Theshold::objectRecognition");
#endif
  // Chown-RLE
  initObjects();
  // now get the posts and goals
  yellow->createObject(horizon);
  blue->createObject(horizon);
#if ROBOT(NAO)
  red->createObject(horizon);
  navyblue->createObject(horizon);
#endif

  bool ylp = vision->yglp->getWidth() > 0;
  bool yrp = vision->ygrp->getWidth() > 0;
  bool blp = vision->bglp->getWidth() > 0;
  bool brp = vision->bgrp->getWidth() > 0;

#if ROBOT(AIBO)
  // check if we saw a post - if so, then screen goals that are too close
  if (vision->by->getDistance() > 0 || vision->yb->getDistance() > 0) {
    // we can't see the yb and by posts at the same time, so pick the larger one
    if (vision->yb->getWidth() > 0 && vision->by->getWidth() > 0) {
      if (vision->yb->getWidth() > vision->by->getWidth())
	vision->by->init();
      else
	vision->yb->init();
    }
    // nothing can be too close
    int byX = vision->by->getX();
    int byX2 = vision->by->getRightTopX();
    if (vision->yb->getDistance() > 0) {
      byX = vision->yb->getX();
      byX2 = vision->yb->getRightTopX();
    }
    if (ylp && distance(byX, byX2,
			vision->yglp->getX(), vision->yglp->getRightTopX()) <
	MIN_SPLIT) {
      vision->yglp->init();
      ylp = false;
    }
    if (yrp && distance(byX, byX2,
			vision->ygrp->getX(), vision->ygrp->getRightTopX()) <
	MIN_SPLIT) {
      vision->ygrp->init();
      yrp = false;
    }
    if (blp && distance(byX, byX2,
			vision->bglp->getX(), vision->bglp->getRightTopX()) <
	MIN_SPLIT) {
      vision->bglp->init();
      blp = false;
    }
    if (brp && distance(byX, byX2,
			vision->bgrp->getX(), vision->bgrp->getRightTopX()) <
	MIN_SPLIT) {
      vision->bgrp->init();
      brp = false;
    }
    if (vision->ygBackstop->getWidth() > 0 &&
	distance(byX, byX2,
		 vision->ygBackstop->getX(), vision->ygBackstop->getRightTopX())
	< MIN_SPLIT) {
      vision->ygBackstop->init();
    }
    if (vision->bgBackstop->getWidth() > 0 &&
	distance(byX, byX2,
		 vision->bgBackstop->getX(), vision->bgBackstop->getRightTopX())
	< MIN_SPLIT) {
      vision->bgBackstop->init();
    }
  }
#endif

  if ((ylp || yrp) && (blp || brp)) {
    // we see blue and yellow goal posts!
    // if we see two of either then use that color'
    if (ylp && yrp) {
      vision->bglp->init();
      vision->bgrp->init();
      vision->bgBackstop->init();
      blp = false;
      brp = false;
    } else if (blp && brp) {
      vision->yglp->init();
      vision->ygrp->init();
      vision->ygBackstop->init();
      ylp = false;
      yrp = false;
    } else {
      // we see one of each, so pick the biggest one
      float ylpw = vision->yglp->getWidth();
      float yrpw = vision->ygrp->getWidth();
      float blpw = vision->bglp->getWidth();
      float brpw = vision->bgrp->getWidth();
      if (ylpw > yrpw) {
	if (blpw > brpw) {
	  if (ylpw > blpw)
	    vision->bglp->init();
	  else
	    vision->yglp->init();
	} else {
	  if (ylpw > brpw)
	    vision->bgrp->init();
	  else
	    vision->yglp->init();
	}
      } else {
	if (blpw > brpw) {
	  if (yrpw > blpw)
	    vision->bglp->init();
	  else
	    vision->ygrp->init();
	} else {
	  if (yrpw > brpw)
	    vision->bgrp->init();
	  else
	    vision->ygrp->init();
	}
      }
    }
  }


  // throw blue goal objects through a filter to eliminate noise in corners
  //chromeFilter(vision->bglp);
  //chromeFilter(vision->bgrp);
  chromeFilter(vision->bgBackstop);
  //reset these bools, incase we changed them above
  ylp = vision->yglp->getWidth() > 0;
  yrp = vision->ygrp->getWidth() > 0;
  blp = vision->bglp->getWidth() > 0;
  brp = vision->bgrp->getWidth() > 0;

  // make sure we don't see a false backstop when looking at the other goal
  if (ylp || yrp) {
      vision->bgBackstop->init();
  }
  if (blp || brp) {
      vision->ygBackstop->init();
  }

  if (horizon < IMAGE_HEIGHT)
    orange->createObject(horizon);
  else
    orange->createObject(pose->getHorizonY(0));

#ifdef USE_PINK_BALL
    if (horizon < IMAGE_HEIGHT)
      ballPink->createObject(horizon);
    else
      ballPink->createObject(pose->getHorizonY(0));
#endif

#if ROBOT(NAO)
    if (ylp || yrp) {
      yellow->bestShot(vision->ygrp, vision->yglp, vision->ygBackstop);
    }
    if (blp || brp) {
      blue->bestShot(vision->bgrp, vision->bglp, vision->bgBackstop);
    }
#endif

  // sanity check: if pose estimated horizon is completely above the image,
  // shouldn't find any posts or goals
  if (pose->getLeftHorizonY() < 0 && pose->getRightHorizonY() < 0) {
    vision->yglp->init();
    vision->ygrp->init();
    //vision->ygBackstop->init();
    vision->bglp->init();
    vision->bgrp->init();
    //vision->bgBackstop->init();
    vision->yb->init();
    vision->by->init();
  }

  storeFieldObjects();

}

//right post

/*
   RLE Helper Methods
*/

/* Calculates and stores all landmark field objects info.
 */
void Threshold::storeFieldObjects() {

#if ROBOT(AIBO)
  setFieldObjectInfo(vision->yb);
  setFieldObjectInfo(vision->by);
#endif
  setFieldObjectInfo(vision->yglp);
  setFieldObjectInfo(vision->ygrp);
  setFieldObjectInfo(vision->bglp);
  setFieldObjectInfo(vision->bgrp);
  vision->ygBackstop->setFocDist(0.0); // sometimes set to 1.0 for some reason
  vision->bgBackstop->setFocDist(0.0); // sometimes set to 1.0 for some reason
  vision->ygBackstop->setDistance(0.0); // sometimes set to 1.0 for some reason
  vision->bgBackstop->setDistance(0.0); // sometimes set to 1.0 for some reason
#if ROBOT(AIBO)
  // FIXME - need to add this stuff to Nao Vision, or unify Vision
  setFieldObjectInfo(vision->blueArc);
  setFieldObjectInfo(vision->yellowArc);
#elif ROBOT(NAO)
  setFieldObjectInfo(vision->red1);
  setFieldObjectInfo(vision->red2);
  setFieldObjectInfo(vision->navy1);
  setFieldObjectInfo(vision->navy2);
#endif

}

/* This filter checks to see if field object points are within the
 * region of chromatic distortion -- and if they mostly are, we throw them out
 * @param obj       The field object to check
*/
void Threshold::chromeFilter(VisualFieldObject *obj) {

  if (obj->getHeight() <= 0 && obj->getWidth() <= 0) {
    return;
  }


  // chromatic distortion filter only for blue goal objects
  point <int> leftTop(obj->getLeftTopX(), obj->getLeftTopY());
  point <int> rightTop(obj->getRightTopX(), obj->getRightTopY());
  point <int> leftBottom(obj->getLeftBottomX(), obj->getLeftBottomY());
  point <int> rightBottom(obj->getRightBottomX(), obj->getRightBottomY());

  int score = 0;

  if (getEuclidianDist(CENTER_IMAGE_COORD,leftTop) >
      CHROME_FILTER_RADIAL_DIST) {
    score++;
  }
  if (getEuclidianDist(CENTER_IMAGE_COORD,rightTop) >
      CHROME_FILTER_RADIAL_DIST) {
    score++;
  }
  if (getEuclidianDist(CENTER_IMAGE_COORD,leftBottom) >
      CHROME_FILTER_RADIAL_DIST) {
    score++;
  }
  if (getEuclidianDist(CENTER_IMAGE_COORD,rightBottom) >
      CHROME_FILTER_RADIAL_DIST) {
    score++;
  }

  if (score >= CHROME_FILTER_SCORE) {
    //print("chromeFilter filters out object with score: %d", score);
    obj->init();
  }
}

/* Figures out center x,y, angle x,y, and foc/body dists for field objects.
 * @param objPtr    the field object to study
 */
void Threshold::setFieldObjectInfo(VisualFieldObject *objPtr) {
  // if the object is on screen, basically
  if (objPtr->getHeight() > 0) {
    // set center x,y
    objPtr->setCenterX(objPtr->getX() + ROUND(objPtr->getWidth()/2));
    objPtr->setCenterY(objPtr->getY() + ROUND(objPtr->getHeight()/2));

    // find angle x/y (relative to camera)
    objPtr->setAngleX((HALF_IMAGE_WIDTH - objPtr->getCenterX())/MAX_BEARING);
    objPtr->setAngleY((HALF_IMAGE_HEIGHT - objPtr->getCenterY())/MAX_ELEVATION);

    // if object is a goal post
    if (objPtr == vision->yglp ||
	objPtr == vision->ygrp ||
	objPtr == vision->bglp ||
	objPtr == vision->bgrp) {
      //print("we've got a post!");
      float dist = 0.0;
      float width = objPtr->getWidth(); float height = objPtr->getHeight();
      distanceCertainty cert = objPtr->getDistanceCertainty();
      float distw = getGoalPostDistFromWidth(width);
      float disth = getGoalPostDistFromHeight(height);

      switch (cert) {
      case HEIGHT_UNSURE:
	// the height is too small - it can still be used as a ceiling though
	if (disth < distw)
	  dist = disth;
	else
	  dist = distw;
	break;
      case WIDTH_UNSURE:
	dist = disth;
	break;
      case BOTH_UNSURE:
	dist = max(disth, distw);
	break;
      case BOTH_SURE:
	dist = disth;
	break;
      }
#if defined OFFLINE && defined PRINT_VISION_INFO
      print("goal post dist: %g %g %g", dist, distw, disth);
#endif

      // sanity check: throw ridiculous distance estimates out
      // constants in Threshold.h
      if (dist < POST_MIN_FOC_DIST ||
	  dist > POST_MAX_FOC_DIST) {
	dist = 0.0;
      }
      objPtr->setDistance(dist);
    }
#if ROBOT(AIBO)
    // if object is a beacon
    else if (objPtr == vision->yb || objPtr == vision->by) {
      float dist = 0.0;

      // get beacon distance always from height
      dist = getBeaconDistFromHeight(objPtr->getHeight());

      // sanity check: throw out ridiculous post estimates
      // constants in Threshold.h
      if (dist < BEACON_MIN_FOC_DIST ||
	  dist > BEACON_MAX_FOC_DIST) {
	dist = 0.0;
      }

      //ANDREW- temp fix, should track down why bad distances are occuring
      if ((vision->getPlayerNumber()) == 1){
	if(dist < GOALIE_BEACON_MIN_FOC_DIST ||
	   dist > GOALIE_BEACON_MAX_FOC_DIST){
	  dist = 0.0;
	}
      }


      objPtr->setDistance(dist);
    }
#endif
    // if object is an arc
#if ROBOT(AIBO)
    else if (objPtr == vision->blueArc || objPtr == vision->yellowArc) {
      objPtr->setDistance(10.0);
    }
#endif

#if ROBOT(NAO)
    else if (objPtr == vision->red1 || objPtr == vision->red2 || objPtr == vision->navy1 || objPtr == vision->navy2) {
      objPtr->setDistance(10.0);
    }
#endif
    // don't know what object it is
    else {
      //print("setFieldObjectInfo: unrecognized FieldObject");
      return;
    }

    // sets focal distance of the field object
    objPtr->setFocDist(objPtr->getDistance());
    // convert dist + angle estimates to body center
    estimate obj_est = pose->bodyEstimate(objPtr->getCenterX(),
					  objPtr->getCenterY(),
					  objPtr->getDistance());
<<<<<<< HEAD:vision/Threshold.cpp
    objPtr->setDistanceWithSD(obj_est.dist);
    objPtr->setBearingWithSD(obj_est.bearing);
=======
    objPtr->setDistance(obj_est.dist);
    objPtr->setBearing(obj_est.bearing);
>>>>>>> Lots of changes to make VisualFieldObject work as a subclass of VisualLandmark.:vision/Threshold.cpp
    objPtr->setElevation(obj_est.elevation);
  }
  else {
    objPtr->setFocDist(0.0);
<<<<<<< HEAD:vision/Threshold.cpp
    objPtr->setDistanceWithSD(0.0);
    objPtr->setBearingWithSD(0.0);
=======
    objPtr->setDistance(0.0);
    objPtr->setBearing(0.0);
>>>>>>> Lots of changes to make VisualFieldObject work as a subclass of VisualLandmark.:vision/Threshold.cpp
    objPtr->setElevation(0.0);
  }
}

/* Looks up goal post height in pixels to focal distance function.
 * @param height     the height of the post
 * @return           the distance to the post
 */
float Threshold::getGoalPostDistFromHeight(float height) {
#if ROBOT(NAO_SIM)
  return 17826*pow((double) height,-1.0254);
  //OLD return 100.0*61.0/height;
#elif ROBOT(NAO_RL)
  return 39305*pow((double) height,-0.9245);
#else
  return 6646*pow((double) height,-.9785);
#endif
}

/* Looks up goal post width in pixels to focal distance function.
 * @param width     the width of the post
 * @return          the distance to the post
 */
float Threshold::getGoalPostDistFromWidth(float width) {
#if ROBOT(NAO_SIM)
  //floor distance, seems to be best for the width
  return 2360.1*pow((double) width,-1.0516); //camera dist - 2585.4*pow(width,-1.0678);//OLD return 100.0*13.0/width;
#elif ROBOT(NAO_RL)
  return 10083*pow((double) width,-1.052);
#else
  return 1483.5*pow((double) width,-.934);
#endif
}

/* Looks up beacon height in pixels to focal distance function.
 * @param height     the height of the beacon
 * @return           the distance to the beacon
 */
float Threshold::getBeaconDistFromHeight(float height) {
#if ROBOT(NAO_SIM)
  return 100.0*39.0/height; //there aren't nao beacons, but just in case
#elif ROBOT(NAO_RL)
  return (239.102*235)/height;
#else
  return 3028.7*pow(height,-.9324);
#endif
}

//Beacons in the simmulator are 18pix wide at 1M


/* Sets all the object information back to zero.
 */
void Threshold::initObjects(void) {

  // yellow goal objs
  vision->ygrp->init();
  vision->yglp->init();
  vision->ygBackstop->init();

  // blue goal objs
  vision->bgrp->init();
  vision->bglp->init();
  vision->bgBackstop->init();

  // beacons
  vision->by->init();
  vision->yb->init();
  // arcs
  vision->blueArc->init();
  vision->yellowArc->init();

  vision->red1->init();
  vision->red2->init();
  vision->navy1->init();
  vision->navy2->init();
  // balls
  vision->ball->init();
#ifdef USE_PINK_BALL
  vision->pinkBall->init();
#endif
} // initObjects

/* Initializes all the color data structures.
 */
void Threshold::initColors() {

  orange->init(pose->getHorizonSlope());
  blue->init(pose->getHorizonSlope());
  yellow->init(pose->getHorizonSlope());
  red->init(pose->getHorizonSlope());
  navyblue->init(pose->getHorizonSlope());
#ifdef USE_PINK_BALL
  ballPink->init(pose->getHorizonSlope());
#endif
}

/* This function loads a table file with the given file name
 * into memory(the big Table array)
 * for example, filename can be "/MS/merged.mtb".
 * it means the merged.mtb file in the root directory of the Memory stick
 * @param filename      the file to load
*/
void Threshold::initTable(std::string filename) {

  FILE* fp;
  //cout << filename << endl;
  fp = fopen(filename.c_str(), "r");   //open table for reading
  if (fp == NULL) {
    print("initTable() FAILED to open filename: %s", filename.c_str());
#ifdef OFFLINE
    exit(0);
#elif ROBOT(AIBO)
    print("Exiting..");
    // crash
    //OBootCondition bootCond;
    //OPENR::GetBootCondition(&bootCond);
    //OPENR::Shutdown(bootCond);
    int x = 0;
    x = 1/x;
#else
    return;
#endif
  }

  //actually read the table into memory
  for(int i=0; i< YMAX; i++)
    for(int j=0; j<UMAX; j++){
      fread(bigTable[i][j], sizeof(unsigned char), VMAX, fp);
    }

#ifndef OFFLINE
  print("Loaded colortable %s",filename.c_str());
#endif

  fclose(fp);
}


void Threshold::initTableFromBuffer(byte * tbfr)
{

    byte* source = tbfr;
    for(int i=0; i< YMAX; i++)
        for(int j=0; j<UMAX; j++){
            //pointer to beginning of row:
            byte* dest = bigTable[i][j];
            //copy over a whole row into big table from the buffer
            memcpy(dest,source,YMAX);
            source+=YMAX;//advance the source bugger
            }
}

/* This function loads a table file with the given file name
 * into memory(the big Table array)
 * for example, filename can be "/MS/merged.mtb".
 * it means the merged.mtb file in the root directory of the Memory stick
 * @param filename      the file to load
*/
void Threshold::initCompressedTable(std::string filename){
#ifndef NO_ZLIB
  FILE* fp;
  //cout << filename << endl;
  fp = fopen(filename.c_str(), "r");   //open table for reading
  if (fp == NULL) {
    printf("initTable() FAILED to open filename: %s , exiting",
        filename.c_str());
#ifdef OFFLINE
    exit(0);
#else
    // crash
    int x = 0;
    x = 1/x;
#endif
  }

  unsigned char *fileData = NULL;
  int length;
  fileData = Zlib::readCompressedFile(fp, length);
  if(!fileData) {
    //crash
    print("something went wrong with decompression\n");
#ifdef OFFLINE
    exit(0);
#else
    // crash
    int x = 0;
    x = 1/x;
#endif
  }
  else
    printf("everything went fine\n");


  unsigned char *fileTraverse = fileData;


  for(int i=0; i< YMAX; i++) {
    //printf("vtoro");
    for(int j=0; j<UMAX; j++){
      //fread(bigTable[i][j], sizeof(unsigned char), vMax, fp);  //64 bytes per chunk
      for(int k=0; k<VMAX; k++) {
	bigTable[i][j][k] = *fileTraverse;
	fileTraverse++;
      }
    }
  }

  print("Loaded colortable %s",filename.c_str());
  free(fileData);

  fclose(fp);
#endif /* NO_ZLIB */
}

/* Loads up the chromatic distortion table.
 * @param filename    the file where the data resides
 */
void Threshold::initChromeTable(std::string filename){

#ifdef USE_CHROMATIC_CORRECTION
  for(int x = 0; x< IMAGE_WIDTH; x++){
    for(int y =0; y < IMAGE_HEIGHT; y++){
      xLUT[x][y] = (unsigned char)sqrt(pow(104.0-(float)x,2)+pow(80.0-(float)y,2)); //calc each distance
      //print("(%d,%d) = %d",x,y,xLUT[x][y]);
    }
  }

  FILE* fp;
  printf("loading %s \n",filename.c_str());
  fp = fopen(filename.c_str(), "r");   //open table for reading
  if (fp == NULL) {
    printf("initTable() FAILED to open filename: %s , exiting",
           filename.c_str());
#ifdef OFFLINE
    exit(0);
#else
    // crash
    int x = 0;
    x = 1/x;
#endif
  }


  //read Y channel
  for(int i =0; i < NUM_YUV; i++){
    fread(corrY[i], sizeof(unsigned char), YRAD, fp);
  }

  //U
  for(int i =0; i < NUM_YUV; i++){
    fread(corrU[i], sizeof(unsigned char), URAD, fp);
  }

  //V
  for(int i =0; i < NUM_YUV; i++){
    fread(corrV[i], sizeof(unsigned char), VRAD, fp);
  }
  /*
  //XLUT
  for(int i =0; i < IMAGE_HEIGHT; i++){
    fread(xLUT[i], sizeof(unsigned char), XLUTX, fp);
  }
  */
  fclose(fp);

  #endif // USE_CHROMATIC_CORRECTION

  /*
  FILE *output = fopen("/MS/error.log", "w");
  fprintf(output, "Threshold::initChromeTable('%s')\n", filename.c_str());
  fflush(output);
  fprintf(output, "Before fopen\n");
  fflush(output);
    fprintf(output, "fopen failed.  errno=%i, '%s'\n", errno, strerror(errno));
    fclose(output);
  fprintf(output, "After fopen\n");
  fflush(output);
  fprintf(output, "Done initChromeTable()\n");
  fclose(output);
  */
}

const uchar* Threshold::getYUV() {
  return yuv;
}

/* I haven't a clue what this method is for.
 * @param newyuv     presumably a new yuv value in bytes or something
 */
void Threshold::setYUV(const uchar* newyuv) {

  yuv = newyuv;
  yplane = yuv;

  if (!inverted) {
#if ROBOT(AIBO)
    uplane = yplane + IMAGE_WIDTH;
    vplane = yplane + IMAGE_WIDTH * 2;
#elif ROBOT(NAO_RL)
    // I've reversed the U and V planes, in addition to offsetting them, as the
    // color table format is still reversed
    uplane = yplane + 3; // normally, is 1, but with reversed tables, is 1
    vplane = yplane + 1; // normally, is 3, but with reversed tables, is 1
#elif ROBOT(NAO_SIM)
#else
#    error Undefined robot type
#endif

  }else {
    // inverted
#if ROBOT(AIBO)
    uplane = yplane + IMAGE_WIDTH * 2;
    vplane = yplane + IMAGE_WIDTH;
#elif ROBOT(NAO)
    // this is actually the correct (non-inverted) settings, but again, with
    // our color tables, inverted=non-inverted and non-inverted=inverted
    uplane = yplane + 1;
    vplane = yplane + 3;
#elif ROBO(NAO_SIM)
#else
#    error Undefined robot type
#endif
  }
}

/* Calculate the distance between two objects (x distance only).
 * @param x1   one end point of the first object
 * @param x2   the other end point
 * @param x3   one end point of the second object
 * @param x4   the other end point
 * @return     the distance between them
 */

int Threshold::distance(int x1, int x2, int x3, int x4) {
  if (x2 < x3)
    return x3 - x2;
  if (x1 > x4)
    return x1 - x4;
  return 0;
}

/*  Returns the euclidian distance between two points.
 * @param coord1    the first point
 * @param coord2    the second point
 * @return          the distance between them
 */
float Threshold::getEuclidianDist(point <int> coord1, point <int> coord2) {
  return sqrt(pow((float)coord2.y-coord1.y,2)+pow((float)coord2.x-coord1.x,2));
}

#if defined(NEW_LOGGING) || defined(USE_JPEG)
#if defined(USE_CHROMATIC_CORRECTION)
//fill the corrected image
unsigned char * Threshold::getCorrectedImage(){
  int index = 0;
  for(int y = 0; y < IMAGE_HEIGHT; y++){
    //Y
    for(int x = 0; x < IMAGE_WIDTH; x++){
      corrected[index++] = corrY[yuv[index]][xLUT[x][y]];
    }
    //U
    for(int x = 0; x < IMAGE_WIDTH; x++){
      corrected[index++] = corrU[yuv[index]][xLUT[x][y]];
    }
    //V
    for(int x = 0; x < IMAGE_WIDTH; x++){
      corrected[index++] = corrV[yuv[index]][xLUT[x][y]];
    }
    index += 3*IMAGE_WIDTH; //skip over these for compatability with openrimage
  }
  return corrected;
}
#endif /* USE_CHROMATIC_CORRECTION */
#endif /* NEW_LOGGING || USE_JPEG */

/*  A bunch of methods for offline debugging.  Basically we create an extra image
 * array so that we can draw on it without disturbing the real image.  After we're
 * done processing then we can overlay the drawing array.
 */


/*  Reset the debugging array back to empty.
 */
void Threshold::initDebugImage(){
#ifdef OFFLINE
  for(int x = 0 ; x < IMAGE_WIDTH;x++)
    for(int y = 0; y < IMAGE_HEIGHT;y++)
      debugImage[y][x] = GREY;
#endif
}

/* For any pixels in the drawing image that have been modified, transfer them
 * to the real image.
 */
void Threshold::transposeDebugImage(){
#if defined OFFLINE && defined DEBUG_IMAGE
  for(int x = 0 ; x < IMAGE_WIDTH;x++)
    for(int y = 0; y < IMAGE_HEIGHT;y++)
      if(debugImage[y][x]!=GREY){
	thresholded[y][x] = debugImage[y][x];}
#endif
}

/* Draw a box in the fake image.
 * @param left     x value of left edge
 * @param right    x value of right edge
 * @param bottom   y value of bottom
 * @param top      y value of top
 * @param c        the color we'd like to draw
 */
void Threshold::drawBox(int left, int right, int bottom, int top, int c) {


#ifdef OFFLINE
  if (left < 0) {
    left = 0;
  }
  if (top < 0) {
    top = 0;
  }
  int width = right-left;
  int height = bottom-top;

  for (int i = left; i < left + width; i++) {
    if (top >= 0 &&
	top < IMAGE_HEIGHT &&
	i >= 0 &&
	i < IMAGE_WIDTH) {
      debugImage[top][i] = c;
    }
    if ((top + height) >= 0 &&
	(top + height) < IMAGE_HEIGHT &&
	i >= 0 &&
	i < IMAGE_WIDTH) {
      debugImage[top + height][i] = c;
    }
  }
  for (int i = top; i < top + height; i++) {
    if (i >= 0 &&
	i < IMAGE_HEIGHT &&
	left >= 0 &&
	left < IMAGE_WIDTH) {
      debugImage[i][left] = c;
    }
    if (i >= 0 &&
	i < IMAGE_HEIGHT &&
	(left+width) >= 0 &&
	(left+width) < IMAGE_WIDTH) {
      debugImage[i][left + width] = c;
    }
  }
#endif
} // drawBox


/* Draw a rectangle in the fake image.
 * @param left     x value of left edge
 * @param right    x value of right edge
 * @param bottom   y value of bottom
 * @param top      y value of top
 * @param c        the color we'd like to draw
 */
void Threshold::drawRect(int left, int top, int width, int height, int c) {
#ifdef OFFLINE
  if (left < 0) {
    width += left;
    left = 0;
  }
  if (top < 0) {
    height += top;
    top = 0;
  }

  for (int i = left; i < left + width; i++) {
    if (top >= 0 && top < IMAGE_HEIGHT && i >= 0 && i < IMAGE_WIDTH) {
      debugImage[top][i] = c;
    }
    if ((top + height) >= 0 &&
	(top + height) < IMAGE_HEIGHT &&
	i >= 0 &&
	i < IMAGE_WIDTH) {
      debugImage[top + height][i] = c;
    }
  }
  for (int i = top; i < top + height; i++) {
    if (i >= 0 &&
	i < IMAGE_HEIGHT &&
	left >= 0 &&
	left < IMAGE_WIDTH) {
      debugImage[i][left] = c;
    }
    if (i >= 0 &&
	i < IMAGE_HEIGHT &&
	(left+width) >= 0 &&
	(left+width) < IMAGE_WIDTH) {
      debugImage[i][left + width] = c;
    }
  }
#endif
} // drawRect

void Threshold::drawLine(const point<int> start, const point<int> end,
                         const int color) {
  drawLine(start.x, start.y, end.x, end.y, color);
}

/* Draw a line in the fake image.
 * @param x       start x
 * @param y       start y
 * @param x1      finish x
 * @param y1      finish y
 * @param c       color we'd like to draw
 */
void Threshold::drawLine(int x, int y, int x1, int y1, int c) {

#ifdef OFFLINE
  float slope = (float)(y - y1) / (float)(x - x1);
  int sign = 1;
  if ((abs(y - y1)) > (abs(x - x1))) {
    slope = 1.0 / slope;
    if (y > y1) sign = -1;
    for (int i = y; i != y1; i += sign) {
      int newx = x + (int)(slope * (i - y));
      if (newx >= 0 && newx < IMAGE_WIDTH && i >= 0 && i < IMAGE_HEIGHT)
	debugImage[i][newx] = c;
    }
  } else if (slope != 0) {
    //slope = 1.0 / slope;
    if (x > x1) sign = -1;
    for (int i = x; i != x1; i += sign) {
      int newy = y + (int)(slope * (i - x));
      if (newy >= 0 && newy < IMAGE_HEIGHT && i >= 0 && i < IMAGE_WIDTH)
	debugImage[newy][i] = c;
    }
  }
  else if (slope == 0) {
    int startX = min(x, x1);
    int endX = max(x, x1);
    for (int i = startX; i <= endX; i++) {
      if (y >= 0 && y < IMAGE_HEIGHT && i >= 0 && i < IMAGE_WIDTH) {
	debugImage[y][i] = c;
      }
    }
  }
#endif
}

// Draws the visual horizon on the image
void Threshold::drawVisualHorizon() {
  drawLine(0, horizon, IMAGE_WIDTH, horizon, VISUAL_HORIZON_COLOR);
}

/* drawPoint()
 * Draws a crosshair or a 'point' on the fake image at some given x, y, and with a given color.
 * @param x       center of the point
 * @param y       center y value
 * @param c       color to draw
*/
void Threshold::drawPoint(int x, int y, int c) {

#ifdef OFFLINE
  if (y > 0 && x > 0 && y < (IMAGE_HEIGHT) && x < (IMAGE_WIDTH)) {
    debugImage[y][x] = c;
  }if (y+1 > 0 && x > 0 && y+1 < (IMAGE_HEIGHT) && x < (IMAGE_WIDTH)) {
    debugImage[y+1][x] = c;
  }if (y+2 > 0 && x > 0 && y+2 < (IMAGE_HEIGHT) && x < (IMAGE_WIDTH)) {
    debugImage[y+2][x] = c;
  }if (y-1 > 0 && x > 0 && y-1 < (IMAGE_HEIGHT) && x < (IMAGE_WIDTH)) {
    debugImage[y-1][x] = c;
  }if (y-2 > 0 && x > 0 && y-2 < (IMAGE_HEIGHT) && x < (IMAGE_WIDTH)) {
    debugImage[y-2][x] = c;
  }if (y > 0 && x+1 > 0 && y < (IMAGE_HEIGHT) && x+1 < (IMAGE_WIDTH)) {
    debugImage[y][x+1] = c;
  }if (y > 0 && x+2 > 0 && y < (IMAGE_HEIGHT) && x+2 < (IMAGE_WIDTH)) {
    debugImage[y][x+2] = c;
  }if (y > 0 && x-1 > 0 && y < (IMAGE_HEIGHT) && x-1 < (IMAGE_WIDTH)) {
    debugImage[y][x-1] = c;
  }if (y > 0 && x-2 > 0 && y < (IMAGE_HEIGHT) && x-2 < (IMAGE_WIDTH)) {
    debugImage[y][x-2] = c;
  }
#endif
}

// Prerequisite - point is within bounds of screen
void Threshold::drawX(int x, int y, int c) {
#ifdef OFFLINE
  // Mid point
  debugImage[y-2][x-2] = c;
  debugImage[y-1][x-1] = c;
  debugImage[y][x] = c;
  debugImage[y+1][x+1] = c;
  debugImage[y+2][x+2] = c;

  debugImage[y-2][x+2] = c;
  debugImage[y-1][x+1] = c;

  debugImage[y+1][x-1] = c;
  debugImage[y+2][x-2] = c;

#endif

}



const char* Threshold::getShortColor(int _id) {
  switch (_id) {
  case WHITE: return "W";
  case ORANGE: return "O";
  case BLUE: return "B";
  case GREEN: return "G";
  case YELLOW: return "Y";
  case BLACK: return "b";
  case RED: return "R";
  case NAVY: return "n";
  case GREY: return "U";
  case YELLOWWHITE: return "YW";
  case BLUEGREEN: return "BG";
  case PINK: return "P";
  default: return "No idea what thresh color you have, mate";
  }
}

