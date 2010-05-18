#ifndef Threshold_h_DEFINED
#define Threshold_h_DEFINED

#include <boost/shared_ptr.hpp>

typedef unsigned char uchar;

class Threshold;  // forward reference

#include "Vision.h"

#include "ObjectFragments.h"
#include "Ball.h"
#include "Field.h"
#include "Cross.h"
#include "Robots.h"
#ifndef NO_ZLIB
#include "Zlib.h"
#endif
#include "Profiler.h"
#include "NaoPose.h"
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

//#define USE_EDGES
#define DARK
#ifdef DARK
#define BLUEV 145
#define YELLOWV 128
#define ORANGEU 135
#define WHITEY  105
#define FUDGEV  2
#define HIGHGREENY 92
#define LOWGREENY  44
#define HIGHGREENU 120
#define LOWGREENU  88
#define HIGHGREENV 148
#define LOWGREENV  128
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

//
// THRESHOLDING CONSTANTS
// Constants pertaining to object detection and horizon detection
static const int MIN_RUN_SIZE = 5;

/* The following two constants are used in the traversal of the image
   inside thresholdAndRuns. We start at the bottom left of the image which
   is (IMAGE_HEIGHT-1)*IMAGE_ROW_OFFSET. ADDRESS_JUMP means we want to move to
   the next column which we do by adding a 1 in there */
static const unsigned int ADDRESS_START = (IMAGE_HEIGHT)*IMAGE_ROW_OFFSET;
static const unsigned int ADDRESS_JUMP = (ADDRESS_START) + 1;

// open field constants
static const int MIN_X_OPEN = 40;

static const int VISUAL_HORIZON_COLOR = BROWN;

static const int UOFFSET=3;
static const int VOFFSET=1;
static const int YOFFSET1=0;
static const int YOFFSET2=2;


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
    inline void threshold();
    inline void runs();
	unsigned char getColor(int x, int y);
	bool isGreen(int x, int y);
	bool isYellow(int x, int y);
	bool isBlue(int x, int y);
	unsigned char getExpandedColor(int x, int y, unsigned char col);
    void thresholdAndRuns();
	void findGoals(int column, int top);
	void findBallsCrosses(int column, int top);
	void detectSelf();
	void setBoundaryPoints(int x1, int y1, int x2, int y2, int x3, int y3);
    void objectRecognition();
    // helper methods
    void initObjects(void);
    void initColors();
    void initTable(std::string filename);
    void initTableFromBuffer(byte* tbfr);
    void initCompressedTable(std::string filename);

    void storeFieldObjects();
    void setFieldObjectInfo(VisualFieldObject *objPtr);
    void setVisualRobotInfo(VisualRobot *objPtr);
	void setVisualCrossInfo(VisualCross *objPtr);
	float chooseGoalDistance(distanceCertainty cert, float height, float width,
							 float poseDist, int bottom);
    float getGoalPostDistFromHeight(float height);
    float getGoalPostDistFromWidth(float width);
    float getBeaconDistFromHeight(float height);
    int distance(int x1, int x2, int x3, int x4);
    float getEuclidianDist(point <int> coord1, point <int> coord2);
    void findGreenHorizon();
    point <int> findIntersection(int col, int dir, int c);
	int greenEdgePoint(int x);
	int getRobotTop(int x, int c);
	int getRobotBottom(int x, int c);
    int postCheck(bool which, int left, int right);
    point <int> backStopCheck(bool which, int left, int right);
    void setYUV(const uchar* newyuv);
    const uchar* getYUV();
    static const char * getShortColor(int _id);

    int getPixelBoundaryLeft();
    int getPixelBoundaryRight();
    int getPixelBoundaryUp();

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


#if ROBOT(NAO_RL)
    inline uchar getY(int x, int y) {
        return yplane[y*IMAGE_ROW_OFFSET+4*(x/2)];
    }
    inline uchar getU(int x, int y) {
		return yplane[y*IMAGE_ROW_OFFSET+4*(x/2) + UOFFSET];
    }
    inline uchar getV(int x, int y) {
		return yplane[y*IMAGE_ROW_OFFSET+4*(x/2) + VOFFSET];
    }
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

	Robots *red, *navyblue;
    Ball* orange;
	Cross* cross;
    // main array
    unsigned char thresholded[IMAGE_HEIGHT][IMAGE_WIDTH];

#ifdef OFFLINE
    //write lines, points, boxes to this array to avoid changing the real image
    unsigned char debugImage[IMAGE_HEIGHT][IMAGE_WIDTH];
#endif

private:

    // class pointers
    Vision* vision;
    boost::shared_ptr<NaoPose> pose;
	Field* field;

    const uchar* yuv;
    const uchar* yplane, *uplane, *vplane;

    unsigned char bigTable[UMAX][VMAX][YMAX];

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
	int greenEdge[IMAGE_WIDTH];

	int lowerBound[IMAGE_WIDTH];

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
#endif
};

#endif // RLE_h_DEFINED
