#ifndef ObjectFragments_h_DEFINED
#define ObjectFragments_h_DEFINED
using namespace std;

#include <stdlib.h>
#include "Common.h"
#include "ifdefs.h"
#include "Pose.h"

#include <vector>


class ObjectFragments; // forward reference
#include "Threshold.h"
//#include "FieldObjects.h"
#include "VisualFieldObject.h"
#include "ConcreteFieldObject.h"
#include "Ball.h"
#include "Vision.h"

//here are defined the lower bounds on the sizes of goals, posts, and balls
//IMPORTANT: they are only guesses right now.
#if ROBOT(AIBO)
#define MIN_GOAL_HEIGHT		8
#define MIN_GOAL_WIDTH		2
#elif ROBOT(NAO)
#define MIN_GOAL_HEIGHT		50
#define MIN_GOAL_WIDTH		10
#endif
// ID information on goal post constant
static const int NOPOST = 0;   // don't know which
static const int RIGHT = 1;
static const int LEFT = 2;
static const int USEBIG = 3;   //
static const int BACKSTOP = 4;
static const int MAX_NUM_BALL_RUNS = 500; //never use this!
static const int BALL_RUNS_MALLOC_SIZE = 10000;

static const int MAX_POINTS = 100;

// Comparison of spatial relationships of two blobs
static const int OUTSIDE = 0;      // one is outside the other
static const int TIGHT = 1;        // they are basically the same blob
static const int CLOSELEFT = 2;    // the small one is way on the left side of the big one
static const int CLOSERIGHT = 3;   // the small one is way on the right side of the big one
static const int MURKY = 4;        // the small one is sort of in the middle of the big one

static const int BIGGAP = 80;
static const int SIMILARSIZE = 5;

// Relative size of posts
static const int LARGE = 2;
static const int MEDIUM = 1;
static const int SMALL = 0;

// Am I looking at a post or a beacon?
static const int BEACON = 0;
static const int POST = 1;

// Universal bad value used to id whether or not we successfully did something
static const int BADVALUE = -100;

#if ROBOT(AIBO)
static const int MAX_BLOBS = 100;                   // how many blobs can we handle
#elif ROBOT(NAO)
static const int MAX_BLOBS = 400;
#endif
static const int NOISE_SKIP = 3;                    // actually just skips 2 pixel noise in runs
static const int MIN_SPLIT = 40;                    // minimum distance between goal and post - changed from 50 to 40 JS
static const float PERCENTMATCH = 0.65f;            //  Amount of post that has to match (so backstop isn't counted in posts)
static const float HALFISH = 0.49f;                 // threshold for expanding sides back out
static const float GOODRAT = 0.75f;                 // highest ratio of width over height for posts
static const float SQUATRAT = 1.2f;                 // indicator that post may be salvagable
static const int MIN_POST_SEPARATION = 12;          // goal posts of the same color have to be this far apart
static const int BIGPOST = 50;                      // how big a post is to be declared a big post
static const float NORMALPOST = 0.6f;
static const float QUESTIONABLEPOST = 0.85f;

// Ball constants
#if ROBOT(NAO)
static const int SMALLBALLDIM = 8;                 // below this size balls are considered small
#else
static const int SMALLBALLDIM = 10;
#endif
static const int SMALLBALL = SMALLBALLDIM * SMALLBALLDIM;
static const float BALLTOOFAT = 1.5f;               // ratio of width/height worse than this is a very bad sign
static const float BALLTOOTHIN = 0.75f;             // ditto
static const float OCCLUDEDTHIN = 0.2f;             // however, if the ball is occluded we can go thinner
static const float OCCLUDEDFAT = 4.0f;              // or fatter
static const float MIDFAT = 3.0f;
static const float MIDTHIN = 0.3f;
static const float MINORANGEPERCENT = 0.59f;        // at least this much of the blob should be orange normally
static const float MINGOODBALL = 0.5f;
static const float MAXGOODBALL = 3.0f;
static const int BIGAREA = 400;
static const int BIGGERAREA = 600;
static const float FATBALL = 2.0f;
static const float THINBALL = 0.5f;

static const int DIST_POINT_FUDGE = 5;

#ifdef OFFLINE
static const int BALLDISTDEBUGN = 59;
static const int PRINTOBJSN = 50;
static const int POSTDEBUGN = 52;
static const int POSTLOGICN = 51;
static const int TOPFINDN = 55;
static const int BEACONDEBUGN = 53;
static const int BALLDEBUGN = 54;
static const int CORNERDEBUGN = 56;
static const int BACKDEBUGN = 57;
static const int SANITYN = 58;
static const int DEBUGCIRCLEFITN = 60;
static const int DEBUGBALLPOINTSN = 61;
static const int ARCDEBUGN = 62;
static const int CORRECTN = 63;
static const int OPENFIELDN = 64;
#else
static const bool BALLDISTDEBUG = false;
static const bool PRINTOBJS = false;
static const bool POSTDEBUG = false;
static const bool POSTLOGIC = false;
static const bool TOPFIND = false;
static const bool BEACONDEBUG = false;
static const bool BALLDEBUG = false;
static const bool CORNERDEBUG = false;
static const bool BACKDEBUG = false;
static const bool SANITY = false;
static const bool DEBUGCIRCLEFIT = false;
static const bool DEBUGBALLPOINTS = false;
static const bool ARCDEBUG = false;
static const bool CORRECT = false;
static const bool OPENFIELD = false;
#endif

// a blob structure that holds information about its own location, and
// information involving its larger blob structure
struct blob {
  // bounding coordinates of the blob
  point <int> leftTop;
  point <int> rightTop;
  point <int> leftBottom;
  point <int> rightBottom;
  int pixels; // the total number of correctly colored pixels in our blob
  int area;
};

struct run {
  int x;
  int y;
  int h;
};

struct stop {
  int x;
  int y;
  int bad;
  int good;
  int span;
};

//struct colorRect {
//};

inline int ROUND2(float x) {
  if ((x-(int)x) >= 0.5) return ((int)x+1);
  if ((x-(int)x) <= -0.5) return ((int)x-1);
  return (int)x;
}


class ObjectFragments {
 public:
  ObjectFragments(Vision *vis, Threshold *thr);
  virtual ~ObjectFragments() {}

  // SETTERS
  void setColor(int c);

  // Making object
  void init(float s);

  // blobbing
  void getTopAndMerge(int maxY);
  void getWidest();
  void zeroTheBlob(int which);
  void mergeBlobs(int first, int second);
  void blobIt(int x, int y, int h);
  void newRun(int x, int endY, int height);
  int blobArea(blob a);
  int blobWidth(blob a);
  int blobHeight(blob a);

#if ROBOT(NAO)
  // robot recognition routines
  void getRobots(int maxY);
  void expandRobotBlob();
  void mergeBigBlobs();
  void updateRobots(int a, int b);
  bool closeEnough(blob a, blob b);
  void splitBlob(int which);
  int isRobotCentered(int mid, int left, int right);
  void detectOrientation(int which);
  bool bigEnough(blob a, blob b);
  bool viableRobot(blob a);
#endif

  // scan operations
  int yProject(int startx, int starty, int newy);
  int yProject(point <int> point, int newy);
  int xProject(int startx, int starty, int newx);
  int xProject(point <int> point, int newx);
  void vertScan(int x, int y, int dir, int stopper, int c, int c2);
  void horizontalScan(int x, int y, int dir, int stopper, int c, int c2, int l,
                      int r);
  int findTrueLineVertical(point <int> top, point <int> bottom, int c, int c2,
                           bool left);
  int findTrueLineHorizontal(point <int> left, point <int> right, int c, int c2,
                             bool up);
  void findTrueLineVerticalSloped(point <int>& top, point <int>& bottom, int c,
                                  int c2, bool left);
  void findTrueLineHorizontalSloped(point <int>& left, point <int>& right,
                                    int c, int c2, bool up);
  bool checkEdge(int x, int y, int x1, int y1);
  int horizonAt(int x);

  // finding square objects
  void constrainedSquare(int x, int y, int c, int c2, int left, int right);
  void aiboSquareGoal(int x, int y, int c, int c2);
  void squareGoal(int x, int y, int c, int c2, int l, int r, int whichOne);
  void correct(blob& b, int c, int c2);
  //void expansion(point <int> start, int axis, int dir, int span, int check, int c, int c2);
  //bool topFind(int c, int c2, int l, int r, int w);

  // main methods
  void createObject(int c);
  void yellow(int pink);
  void blue(int pink);
  void robot(int c);

  // beacons
#if ROBOT(AIBO)
  bool beaconCheck(int a, int c, int d, int e);
  bool processBeacon(int l, int r, int x, int y, bool yellowOnTop, bool normal);
  void expandSpot(int x1, int y1, int dir, int c, int c2, int c3, int c4);
  void findSpot(int x, int y, int x2, int y2, int dir, int c, int c2, int c3
                int c4, int cases);
  bool setCorners(int leftx, int lefty, int rightx, int righty, int c, int c2,
                  int c3, int c4, VisualFieldObject* post);
  void findCorner(int oneX, int oneY, int dir, int myHeight, float check,
                  int x, int y, int stopper, int c, int c2);
  bool inferBeaconFromBlob(blob b, VisualFieldObject* post);
#endif

  // miscelaneous goal processing  methods
  bool coloredArc(blob b, int c);
  bool goodPost(blob b, int c);
  bool qualityPost(blob b, int c);
  bool checkSize(blob b, int c);
  int checkIntersection(blob b);
  int checkCorners(blob b);
  int getBigRun(int left, int right, int hor);
  bool updateObject(VisualFieldObject* a, blob b, int certainty, int dc);
  void updateRobot(VisualFieldObject* a, blob b);
  bool updateArc(VisualFieldObject* a, blob b, int sawGoal);
  int checkDist(int left, int right, int top, int bottom);
  void updateBackstop(VisualFieldObject* a, blob b);

  // post recognition routines
#if ROBOT(NAO)
  int crossCheck(blob b);
  int crossCheck2(blob b);
  int triangle(blob b);
#elif ROBOT(AIBO)
  int checkPostBlob(blob b, blob s);
  bool contains(blob b, blob s, int c, int c2, VisualFieldObject* left,
                VisualFieldObject* right, VisualFieldObject* mid);
  bool inBlob(blob b, int x, int y, int h);
  void makeBoxes(int sx, int xy, int hb);
  bool checkPostAndBlob(float rat, bool postFound, int c, int c2,
			int horizon, VisualFieldObject* left, VisualFieldObject* mid,
			VisualFieldObject* right, blob post, blob big);
  int projectBoxes(int spanX, int spanY, int howbig, int fakeBottom,
                   int trueRight, int trueLeft, int trueTop);
  int determineRelationship(blob b, blob s);
#endif
  int scanOut(int stopp, int spanX, int c);
  int checkOther(int left, int right, int height, int horizon);
  int characterizeSize(blob b);
  int  scanLogic(int howbig);

  // shooting
  void setShot(VisualFieldObject * one);
  void bestShot(VisualFieldObject * one, VisualFieldObject * two,
                VisualFieldObject * three);
  void openDirection(int h, Pose *p);

  int classifyFirstPost(int horizon, int c, int c2, bool postFound,
                        VisualFieldObject* left, VisualFieldObject* right,
                        VisualFieldObject* mid);

  // the big kahuna
  void goalScan(VisualFieldObject* left, VisualFieldObject* mid,
                VisualFieldObject* right, int c, int c2, bool post,
                int horizon);
  int grabPost(int c, int c2, int horizon, int left, int right);
  void postSwap(VisualFieldObject * p1, VisualFieldObject * p2);
  void transferToChecker(blob b);
  void transferToPole();
  void transferTopBlob(VisualFieldObject * one, int cert, int dc);
  void transferBlob(blob from, blob & to);

  // ball stuff
  float rightColor(blob obj, int c);
  float rightHalfColor(blob obj);
  bool greenCheck(blob b);
  bool greenSide(blob b);
  int scanOut(int start_x, int start_y, float slope,int dir);
  int ballNearGreen(blob b);
  int roundness(blob b);
  bool badSurround(blob b);
  bool atBoundary(blob b);
  int balls(int c, Ball *thisBall);
  int circleFit(Ball * thisBall);

  // sanity checks
  bool rightBlobColor(blob obj, float per);
  void screenCrossbar();
  bool postBigEnough(blob b);
  bool horizonBottomOk(int spanX, int spanY, int minHeight, int left, int right,
                       int bottom, int top);
  bool horizonTopOk(int top, int hor);
  bool postRatiosOk(float ratio);
  bool secondPostFarEnough(point <int> l1, point <int> r1,
			   point <int> l2, point <int> r2, int p);
  bool blobOk(blob b);
  bool locationOk(blob b, int hor);
  bool relativeSizesOk(int x1, int y1, int s2, int y2, int t1, int t2, int f);
  void addPoint(float x, float y);

  // misc.
  float dist(int x, int y, int x1, int y1);
  int distance(int x1, int x2, int x3, int x4);
  int getPixels(int index);
  int midPoint(int a, int b);
  float getSlope() { return slope; }


  // debugging methods
  void printObjs();
  void printBall(blob b, int c, float p, int o, int bg);
  void drawPoint(int x, int y, int c);
  void drawRect(int x, int y, int w, int h, int c);
  void drawBlob(blob b, int c);
  void drawLine(int x, int y, int x1, int y1, int c);
  void printBlob(blob b);
  void printObject(VisualFieldObject * objs);
  void paintRun(int x,int y, int h, int c);
  void drawRun(const run& run, int c);
  void drawLess(int x, int y, int c);
  void drawMore(int x, int y, int c);

#ifdef OFFLINE
  void setConstant(int c) {
    switch (c) {
    case PRINTOBJSN:
      PRINTOBJS = !PRINTOBJS;
      break;
    case POSTLOGICN:
      POSTLOGIC = !POSTLOGIC;
      break;
    case  POSTDEBUGN:
      POSTDEBUG = !POSTDEBUG;
      break;
    case BEACONDEBUGN:
      BEACONDEBUG = !BEACONDEBUG;
      break;
    case BALLDEBUGN:
      BALLDEBUG = !BALLDEBUG;
      break;
    case TOPFINDN:
      TOPFIND = !TOPFIND;
      break;
    case CORNERDEBUGN:
      CORNERDEBUG = !CORNERDEBUG;
      break;
    case  BACKDEBUGN:
      BACKDEBUG = !BACKDEBUG;
      break;
    case SANITYN:
      SANITY = !SANITY;
      break;
    case BALLDISTDEBUGN:
      BALLDISTDEBUG = !BALLDISTDEBUG;
      break;
    case DEBUGCIRCLEFITN:
      DEBUGCIRCLEFIT = !DEBUGCIRCLEFIT;
      break;
    case DEBUGBALLPOINTSN:
      DEBUGBALLPOINTS = !DEBUGBALLPOINTS;
      break;
    case ARCDEBUGN:
      ARCDEBUG = !ARCDEBUG;
      break;
    case CORRECTN:
      CORRECT = !CORRECT;
      break;
    case OPENFIELDN:
      OPENFIELD = !OPENFIELD;
      break;
    }
  }

#endif

  //find the determinant of a 2by2
  float det2(float a00,float a01,float a10,float a11){
    return (a00*a11)-(a01*a10);}
  //find the determinant of a 3by3
  float det3(float a00,float a01,float a02,
	    float a10,float a11,float a12,
	    float a20,float a21,float a22){
    return ((a00*det2(a11,a12,
		      a21,a22))
	    -(a01*det2(a10,a12,
		       a20,a22))
	    +(a02*det2(a10,a11,
		       a20,a21)));}

 private:
  // class pointers
  Vision *vision;
  Threshold *thresh;

  int topSpot; //keeps track of best blob -> global var for debugging purposes
  int color;
  int runsize;
  int biggestRun;
  int maxHeight;
  int maxOfBiggestRun;
  int numberOfRuns;
  int indexOfBiggestRun;
  run* runs;

  blob topBlob, secondBlob, crossBlob;
  int numBlobs;
  blob checker, obj, pole, leftBox, rightBox;
  stop scan, scan1, scan2;
  blob blobs[MAX_BLOBS];
  int projx[5], projy[5];
  //int goodX[4];
  //int goodY[4];
  int candidateX[4];
  int candidateY[4];
  bool shoot[IMAGE_WIDTH];
  int goodP;
  int inferredConfidence;
  blob zeroBlob;
  float slope;
  int occlusion;
  point <int> spot;
  int numPoints;
  float points[MAX_POINTS*2];

#ifdef OFFLINE
 bool PRINTOBJS;
 bool POSTLOGIC;
 bool POSTDEBUG;
 bool BEACONDEBUG;
 bool BALLDEBUG;
 bool TOPFIND;
 bool CORNERDEBUG;
 bool BACKDEBUG;
 bool SANITY;
 bool BALLDISTDEBUG;
 bool DEBUGCIRCLEFIT;
 bool DEBUGBALLPOINTS;
 bool ARCDEBUG;
 bool CORRECT;
 bool OPENFIELD;
#endif

};

#endif // ObjectFragments_h_DEFINED
