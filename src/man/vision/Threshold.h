#ifndef Threshold_h_DEFINED
#define Threshold_h_DEFINED

#include <boost/shared_ptr.hpp>

typedef unsigned char uchar;

class Threshold;  // forward reference
class Gradient;

#include "Vision.h"

#include "ObjectFragments.h"
#include "Ball.h"
#include "Field.h"
#include "VisualFieldEdge.h"
#include "Cross.h"
#include "Robots.h"
#include "Context.h"
#include "Profiler.h"
#include "NaoPose.h"

//#define SOFTCOLORS

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

//#define SHOULDERS

//#define USE_EDGES
//#define ROOM223
#ifdef ROOM223
#define BLUEV 141
#define YELLOWV 108
#define ORANGEU 145
#define WHITEY  105
#define FUDGEV 0
#define HIGHGREENY 80
#define LOWGREENY  32
#define HIGHGREENU 122
#define LOWGREENU  106
#define HIGHGREENV 140
#define LOWGREENV  124
#else
#define DARK
#ifdef DARK
#define BLUEV 148
#define YELLOWV 128
#define ORANGEU 135
#define WHITEY  130
#define FUDGEV 0
#define HIGHGREENY 124
#define LOWGREENY  64
#define HIGHGREENU 116
#define LOWGREENU  86
#define HIGHGREENV 144
#define LOWGREENV  126
#else
#define BLUEV 145
#define YELLOWV 120
#define ORANGEU 145
#define WHITEY  145
#define FUDGEV 5
#define HIGHGREENY 92
#define LOWGREENY  44
#define HIGHGREENU 120
#define LOWGREENU  88
#define HIGHGREENV 148
#define LOWGREENV  128
#endif
#endif

//
// THRESHOLDING CONSTANTS
// Constants pertaining to object detection and horizon detection
static const int MIN_RUN_SIZE = 5;

static const int DEFAULT_EDGE_VALUE = 30;

/* The following two constants are used in the traversal of the image
   inside thresholdAndRuns. We start at the bottom left of the image which
   is (IMAGE_HEIGHT-1)*IMAGE_ROW_OFFSET. ADDRESS_JUMP means we want to move to
   the next column which we do by adding a 1 in there */
static const unsigned int ADDRESS_START = (IMAGE_HEIGHT)*IMAGE_ROW_OFFSET;
static const unsigned int ADDRESS_JUMP = (ADDRESS_START) + 1;

// open field constants
static const int MIN_X_OPEN = 40;

static const int VISUAL_HORIZON_COLOR = BROWN;

static const int NUMBLOCKS = 3;

//
// DISTANCE ESTIMATES CONSTANTS
// based on Height and Width
static const float POST_MIN_FOC_DIST = 10.0f; // goal posts
static const float POST_MAX_FOC_DIST = 800.0f;

const float HORIZONTAL_SHOULDER_THRESH_LEFT = 1.05f;
const float HORIZONTAL_SHOULDER_THRESH_RIGHT = -HORIZONTAL_SHOULDER_THRESH_LEFT;
const float VERTICAL_SHOULDER_THRESH = -0.38f;


class Threshold
{
    friend class Vision;
public:
    Threshold(Vision* vis, boost::shared_ptr<NaoPose> posPtr);
    virtual ~Threshold() {}


    // main methods
    void visionLoop();
    // inline void threshold();
    void thresholdOldImage(const uint8_t *oldImg, uint16_t* newImg);
    inline void runs();
    unsigned char getColor(int x, int y);
    unsigned char getExpandedColor(int x, int y, unsigned char col);
    int getHorizontalEdge(int x1, int y1, int dir);
    void thresholdAndRuns();
	void lowerRuns();
    void findGoals(int column, int top);
    void findBallsCrosses(int column, int top);
	void findBallLowerCamera(int column, int topEdge);
    void detectSelf();
    void setBoundaryPoints(int x1, int y1, int x2, int y2, int x3, int y3);
    void objectRecognition();
    void newFindRobots(); //ben's function
    // helper methods
    void initObjects(void);
    void initColors();
    void initTable(std::string filename);
    void initTableFromBuffer(byte* tbfr);
    void initCompressedTable(std::string filename);

    void storeFieldObjects();
    void setFramesOnAndOff(VisualDetection *objPtr);
    void setFieldObjectInfo(VisualFieldObject *objPtr);
    void setVisualRobotInfo(VisualRobot *objPtr);
    void setVisualCrossInfo(VisualCross *objPtr);
    void setShot(VisualCrossbar * one);
    void setOpenFieldInformation();
    float chooseGoalDistance(distanceCertainty cert, float height, float width,
                             float poseDist, int bottom);
    float getGoalPostDistFromHeight(float height);
    float getGoalPostDistFromWidth(float width);
    float getBeaconDistFromHeight(float height);
    int distance(int x1, int x2, int x3, int x4);
    float realDistance(int x, int y, int x1, int y1);
    float getEuclidianDist(point <int> coord1, point <int> coord2);
    void findGreenHorizon();
    point <int> findIntersection(int col, int dir, int c);
    int greenEdgePoint(int x);
    int getRobotTop(int x, int c);
    int getRobotBottom(int x, int c);
    int postCheck(bool which, int left, int right);
    point <int> backStopCheck(bool which, int left, int right);
    void setYUV(const uint16_t* newyuv);
    void setYUV_bot(const uint16_t* newyuv);
    const uint16_t* getYUV();
    static const char * getShortColor(int _id);

    int getPixelBoundaryLeft();
    int getPixelBoundaryRight();
    int getPixelBoundaryUp();
    float getPixDistance(int y) {return pixDistance[y];}

#ifdef OFFLINE
    void setConstant(int c);
    void setDebugHorizon(bool _bool) { visualHorizonDebug = _bool; }
    bool getDebugHorizon() { return visualHorizonDebug; }
    void setDebugShooting(bool _bool) {debugShot = _bool;}
    void setDebugOpenField(bool _bool) {debugOpenField = _bool;}
    void setDebugEdgeDetection(bool _bool) {debugEdgeDetection = _bool;}
    void setDebugHoughTransform(bool _bool) {debugHoughTransform = _bool;}
    void setDebugRobots(bool _bool);
    void setDebugVisualLines(bool _bool) {debugVisualLines = _bool;}
    void setDebugVisualCorners(bool _bool) {debugVisualCorners = _bool;}
#endif

    void initDebugImage();
    void transposeDebugImage();
    void drawDetectedEdges(boost::shared_ptr<Gradient> g);
    void drawVisualHorizon();
    void setEdgeThreshold(int _thresh);
    int getEdgeThreshold();
    void setHoughAcceptThreshold(int _thresh);

#if ROBOT(NAO_RL)
    int getY(int j, int i) const;
    int getU(int x, int y) const;
    int getV(int j, int i) const;
#elif ROBOT(NAO_SIM)
#  error NAO_SIM robot type not implemented
#else
#  error Undefined robot type
#endif

    int getVisionHorizon() { return horizon; }

    inline static int ROUND(float x) {
        return static_cast<int>( std::floor(x + 0.5f) );
    }


    boost::shared_ptr<ObjectFragments> blue;
    boost::shared_ptr<ObjectFragments> yellow;

    Robots *red, *navyblue, *unid;
    Ball* orange;
    Cross* cross;
    // main array
    uint8_t* thresholded;
    uint8_t* thresholdedBottom;
    inline uint8_t getThresholded(int i, int j){
      if (usingTopCamera)
        return thresholded[i * IMAGE_WIDTH + j];
      else {
	return thresholdedBottom[i * IMAGE_WIDTH + j];
      }
    }
    inline void setThresholded(int i, int j, uint8_t value){
        thresholded[i * IMAGE_WIDTH + j] = value;
    }

	Field* field;
    Context* context;

#ifdef OFFLINE
    //write lines, points, boxes to this array to avoid changing the real image
    uint8_t debugImage[IMAGE_HEIGHT][IMAGE_WIDTH];
#endif

    bool usingTopCamera;

private:

    // class pointers
    Vision* vision;
    boost::shared_ptr<NaoPose> pose;

    const uint16_t* yuv;
    const uint16_t* yplane;
    const uint16_t* yuv_bot;
    const uint16_t* yplane_bot;

    unsigned char bigTable[UMAX][VMAX][YMAX];

    // open field variables
    int openField[IMAGE_WIDTH];
    int closePoint;
    int closePoint1;
    int closePoint2;
    bool stillOpen;


    bool greenBlue[IMAGE_WIDTH];
    bool greenYellow[IMAGE_WIDTH];
    bool shoot[IMAGE_WIDTH];
    int yellowWhite[IMAGE_WIDTH];
    int blueWhite[IMAGE_WIDTH];
    int navyTops[IMAGE_WIDTH];
    int redTops[IMAGE_WIDTH];
    int navyBottoms[IMAGE_WIDTH];
    int redBottoms[IMAGE_WIDTH];
    int greenEdge[IMAGE_WIDTH];

    int lowerBound[IMAGE_WIDTH];
    int block[NUMBLOCKS];
    int evidence[NUMBLOCKS];

	float pixDistance[IMAGE_HEIGHT];

    // thresholding variables
    int horizon;
    int lastPixel;
    int currentRun;
    int previousRun;
    int newPixel;

#ifdef OFFLINE
    // Visual horizon debugging
    bool visualHorizonDebug;
    bool debugSelf;
    bool debugShot;
    bool debugOpenField;
    bool debugEdgeDetection;
    bool debugHoughTransform;
    bool debugRobots;
    bool debugVisualLines;
    bool debugVisualCorners;
#else
    static const bool debugSelf = false;
    static const bool debugShot = false;
    static const bool debugOpenField = false;
    static const bool debugEdgeDetection = false;
    static const bool debugHoughTransform = false;
    static const bool debugRobots = false;
    static const bool debugVisualLines = false;
#endif
};

#endif // RLE_h_DEFINED
