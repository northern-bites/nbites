#ifndef Threshold_h_DEFINED
#define Threshold_h_DEFINED

#include <boost/shared_ptr.hpp>

typedef unsigned char uchar;

class Threshold;  // forward reference

#include "Vision.h"

#include "ObjectFragments.h"
#include "NaoPose.h"
#ifndef NO_ZLIB
#include "Zlib.h"
#endif
#include "Profiler.h"
//
// COLOR TABLE CONSTANTS
// remember to change both values when chaning the color tables

//these must be changed everytime we load a new table
#ifdef SMALL_TABLES
#define YSHIFT  3
#define USHIFT  2
#define VSHIFT  2
#define YMAX  32
#define UMAX  64
#define VMAX  64
#else
#define YSHIFT  1
#define USHIFT  1
#define VSHIFT  1
#define YMAX  128
#define UMAX  128
#define VMAX  128
#endif

//radial chrome distortion
#define YRAD  140
#define URAD  140
#define VRAD  140

//Chromatic distorition constants
static const int NUM_YUV = 256;

// channel constants
static const int Y = 0;
static const int U = 1;
static const int V = 2;


//
// THRESHOLDING CONSTANTS
// Constants pertaining to object detection and horizon detection
#if ROBOT(AIBO)
static const int MIN_RUN_SIZE = 2;
#elif ROBOT(NAO)
static const int MIN_RUN_SIZE = 25;
#endif
// we're more demanding of Green because there is so much
static const int MIN_GREEN_SIZE = 10;

/* The following two constants are used in the traversal of the image
   inside thresholdAndRuns. We start at the bottom left of the image which
   is (IMAGE_HEIGHT-1)*IMAGE_ROW_OFFSET. ADDRESS_JUMP means we want to move to
   the next column which we do by adding a 1 in there */
static const unsigned int ADDRESS_START = (IMAGE_HEIGHT)*IMAGE_ROW_OFFSET;
static const unsigned int ADDRESS_JUMP = (ADDRESS_START) + 1;

// open field constants
static const int MIN_X_OPEN = 40;

static const int VISUAL_HORIZON_COLOR = BROWN;

//
// DISTANCE ESTIMATES CONSTANTS
// based on Height and Width
static const float POST_MIN_FOC_DIST = 10.0f; // goal posts
static const float POST_MAX_FOC_DIST = 800.0f;
static const float BEACON_MIN_FOC_DIST = 15.0f;
static const float BEACON_MAX_FOC_DIST = 650.0f;

//Andrew: restrict acceptable beacon dist for goalie
static const float GOALIE_BEACON_MIN_FOC_DIST = 10.0f;
static const float GOALIE_BEACON_MAX_FOC_DIST = 500.0f;

//
// CHROMATIC DISTORTION CONSTANTS
// center of image
static const point <int> CENTER_IMAGE_COORD(IMAGE_WIDTH / 2,
					    IMAGE_HEIGHT / 2);
// radial dist for chromeFilter()
static const int CHROME_FILTER_RADIAL_DIST = 110;
// score at which you throw out values
static const int CHROME_FILTER_SCORE = 3;

class Threshold
{
    friend class Vision;
 public:
  Threshold(Vision* vis, boost::shared_ptr<NaoPose> posPtr);
  virtual ~Threshold() {}

  // main methods
  void visionLoop();
#ifdef UNROLLED_LOOPS_THRESHOLD
  inline void threshold();
  inline void runs();
#endif
  void thresholdAndRuns();
  void objectRecognition();
#if defined(NEW_LOGGING) || defined(USE_JPEG)
#ifdef USE_CHROMATIC_CORRECTION
  unsigned char * getCorrectedImage();
#endif
#endif
  // helper methods
  void initObjects(void);
  void initColors();
  void initTable(std::string filename);
  void initTableFromBuffer(byte* tbfr);
  void initChromeTable(std::string filename);
  void initCompressedTable(std::string filename);

  void storeFieldObjects();
  void setFieldObjectInfo(VisualFieldObject *objPtr);
  float getGoalPostDistFromHeight(float height);
  float getGoalPostDistFromWidth(float width);
  float getBeaconDistFromHeight(float height);
  int distance(int x1, int x2, int x3, int x4);
  float getEuclidianDist(point <int> coord1, point <int> coord2);
  void findGreenHorizon();
  point <int> findIntersection(int col, int dir, int c);
  int postCheck(bool which, int left, int right);
  point <int> backStopCheck(bool which, int left, int right);
  void chromeFilter(VisualFieldObject *obj);
  void setYUV(const uchar* newyuv);
  const uchar* getYUV();
  static const char * getShortColor(int _id);

  void swapUV() { inverted = !inverted; setYUV(yuv); }
  void swapUV(bool _inverted) { inverted = _inverted; setYUV(yuv); }


#ifdef OFFLINE
  void setConstant(int c);
  void setHorizonDebug(bool _bool) { visualHorizonDebug = _bool; }
  bool getHorizonDebug() { return visualHorizonDebug; }
#endif

  void initDebugImage();
  void transposeDebugImage();
  void drawX(int x, int y, int c);
  void drawPoint(int x, int y, int c);
  void drawLine(const point<int> start, const point<int> end,
                const int color);
  void drawVisualHorizon();
  void drawLine(int x, int y, int x1, int y1, int c);
  void drawBox(int left, int right, int bottom, int top, int c);
  void drawRect(int left, int top, int width, int height, int c);

#ifdef USE_CHROMATIC_CORRECTION
  inline unsigned char getCorrY(int x, int y) { return corrY[yplane[y*IMAGE_ROW_OFFSET+x]][xLUT[x][y]]; }
  inline unsigned char getCorrU(int x, int y) { return corrU[uplane[y*IMAGE_ROW_OFFSET+x]][xLUT[x][y]]; }
  inline unsigned char getCorrV(int x, int y) { return corrV[vplane[y*IMAGE_ROW_OFFSET+x]][xLUT[x][y]]; }

  inline uchar getY(int x, int y) { return corrY[yplane[y*IMAGE_ROW_OFFSET+x]][xLUT[x][y]]; }
  inline uchar getU(int x, int y) { return corrU[uplane[y*IMAGE_ROW_OFFSET+x]][xLUT[x][y]]; }
  inline uchar getV(int x, int y) { return corrV[vplane[y*IMAGE_ROW_OFFSET+x]][xLUT[x][y]]; }

  /*
  inline unsigned char getCorrY(int x, int y) { return corrY[yplane[y*IMAGE_ROW_OFFSET+x]>>YSHIFT][xLUT[x][y]]<<YSHIFT; }
  inline unsigned char getCorrU(int x, int y) { return corrU[uplane[y*IMAGE_ROW_OFFSET+x]>>USHIFT][xLUT[x][y]]<<USHIFT; }
  inline unsigned char getCorrV(int x, int y) { return corrV[vplane[y*IMAGE_ROW_OFFSET+x]>>VSHIFT][xLUT[x][y]]<<VSHIFT; }
*/
#else

#if ROBOT(AIBO)
  inline uchar getCorrY(int x, int y) { return yplane[y*IMAGE_ROW_OFFSET+x]; }
  inline uchar getCorrU(int x, int y) { return uplane[y*IMAGE_ROW_OFFSET+x]; }
  inline uchar getCorrV(int x, int y) { return vplane[y*IMAGE_ROW_OFFSET+x]; }

  inline uchar getY(int x, int y) { return yplane[y*IMAGE_ROW_OFFSET+x]; }
  inline uchar getU(int x, int y) { return uplane[y*IMAGE_ROW_OFFSET+x]; }
  inline uchar getV(int x, int y) { return vplane[y*IMAGE_ROW_OFFSET+x]; }
#elif ROBOT(NAO_RL)
  inline uchar getCorrY(int x, int y) { return yplane[y*IMAGE_ROW_OFFSET+2*x]; }
  inline uchar getCorrU(int x, int y) { return uplane[y*IMAGE_ROW_OFFSET+4*(x/2)]; }
  inline uchar getCorrV(int x, int y) { return vplane[y*IMAGE_ROW_OFFSET+4*(x/2)]; }

  inline uchar getY(int x, int y) { return yplane[y*IMAGE_ROW_OFFSET+2*x]; }
  inline uchar getU(int x, int y) { return uplane[y*IMAGE_ROW_OFFSET+4*(x/2)]; }
  inline uchar getV(int x, int y) { return vplane[y*IMAGE_ROW_OFFSET+4*(x/2)]; }
#elif ROBOT(NAO_SIM)
#  error NAO_SIM robot type not implemented
#else
#  error Undefined robot type
#endif

#endif

  int getVisionHorizon() { return horizon; }

  inline static int ROUND(float x) {
    if ((x-(int)x) >= 0.5) return ((int)x+1);
    if ((x-(int)x) <= -0.5) return ((int)x-1);
    else return (int)x;
  }


  ObjectFragments *blue;
  ObjectFragments *yellow;
  ObjectFragments *orange;
  ObjectFragments *green;
  ObjectFragments *navyblue;
  ObjectFragments *red;
#ifdef USE_PINK_BALL
  ObjectFragments *ballPink;
#endif

  // can keep these arrays at hi-res size ...at any resolution
  //int sawYellowBlue[IMAGE_WIDTH];
  //int sawBlueYellow[IMAGE_WIDTH];
  bool sawYellowBlue, sawBlueYellow;
  int firstYellowBlue, firstYellowBlueY;
  int firstBlueYellow, firstBlueYellowY;

  // main array
  unsigned char thresholded[IMAGE_HEIGHT][IMAGE_WIDTH];

#ifdef OFFLINE//write lines, points, boxes to this array to avoid changing the real image
  unsigned char debugImage[IMAGE_HEIGHT][IMAGE_WIDTH];
#endif

  bool inverted;

 private:

  // class pointers
  Vision* vision;
  boost::shared_ptr<NaoPose> pose;

  const uchar* yuv;
  const uchar* yplane, *uplane, *vplane;

  unsigned char bigTable[YMAX][UMAX][VMAX];

  // open field variables
  int openField[IMAGE_WIDTH];
  int closePoint;
  int closePoint1;
  int closePoint2;
  bool stillOpen;

  bool greenBlue[IMAGE_WIDTH];
  bool greenYellow[IMAGE_WIDTH];
  int yellowWhite[IMAGE_WIDTH];
  int blueWhite[IMAGE_WIDTH];
  int navyTops[IMAGE_WIDTH];
  int redTops[IMAGE_WIDTH];
  int navyBottoms[IMAGE_WIDTH];
  int redBottoms[IMAGE_WIDTH];

#ifdef USE_CHROMATIC_CORRECTION
  unsigned char corrY[NUM_YUV][YRAD],corrU[NUM_YUV][URAD],corrV[NUM_YUV][VRAD];
  unsigned char xLUT[IMAGE_WIDTH][IMAGE_HEIGHT];
#endif

#if defined(NEW_LOGGING) || defined(USE_JPEG)
#if defined(USE_CHROMATIC_CORRECTION)
  unsigned char corrected[IMAGE_WIDTH*IMAGE_HEIGHT*6];
#endif
#endif
  // thresholding variables
  int horizon;
  int lastPixel;
  int currentRun;
  int previousRun;
  int newPixel;

#ifdef OFFLINE
  // Visual horizon debugging
  bool visualHorizonDebug;
#endif
};

#endif // RLE_h_DEFINED
