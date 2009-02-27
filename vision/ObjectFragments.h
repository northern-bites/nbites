
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


#ifndef ObjectFragments_h_DEFINED
#define ObjectFragments_h_DEFINED
using namespace std;

#include <stdlib.h>
#include <vector>

#include "Common.h"
#include "ifdefs.h"
#include "NaoPose.h"
#include "VisionStructs.h"
#include "VisionHelpers.h"

class ObjectFragments; // forward reference
#include "Threshold.h"
#include "VisualFieldObject.h"
#include "ConcreteFieldObject.h"
#include "VisualBall.h"
#include "Vision.h"

//here are defined the lower bounds on the sizes of goals, posts, and balls
//IMPORTANT: they are only guesses right now.

#define MIN_GOAL_HEIGHT	25
#define MIN_GOAL_WIDTH  5

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

static const int MAX_BLOBS = 400;
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
static const int BALLDISTDEBUGN = 59;
static const int PRINTOBJSN = 50;
static const int POSTDEBUGN = 52;
static const int POSTLOGICN = 51;
static const int TOPFINDN = 55;
static const int BALLDEBUGN = 54;
static const int CORNERDEBUGN = 56;
static const int BACKDEBUGN = 57;
static const int SANITYN = 58;
static const int DEBUGCIRCLEFITN = 60;
static const int DEBUGBALLPOINTSN = 61;
static const int CORRECTN = 63;
static const int OPENFIELDN = 64;
#else
static const bool BALLDISTDEBUG = false;
static const bool PRINTOBJS = false;
static const bool POSTDEBUG = false;
static const bool POSTLOGIC = false;
static const bool TOPFIND = false;
static const bool BALLDEBUG = false;
static const bool CORNERDEBUG = false;
static const bool BACKDEBUG = false;
static const bool SANITY = false;
static const bool DEBUGCIRCLEFIT = false;
static const bool DEBUGBALLPOINTS = false;
static const bool CORRECT = false;
static const bool OPENFIELD = false;
#endif

class ObjectFragments {
public:
    ObjectFragments(Vision* vis, Threshold* thr, int _color);
    virtual ~ObjectFragments() {}

    // SETTERS
    void setColor(int c);
    void allocateColorRuns();

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

    // robot recognition routines
    void getRobots(int maxY);
    void expandRobotBlob();
    void mergeBigBlobs();
    void updateRobots(int a, int b);
    bool closeEnough(blob a, blob b);
    int isRobotCentered(int mid, int left, int right);
    bool bigEnough(blob a, blob b);
    bool viableRobot(blob a);

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
    void squareGoal(int x, int y, int c, int c2);
    void correct(blob& b, int c, int c2);

    // main methods
    void createObject(int c);
    void yellow(int pink);
    void blue(int pink);
    void robot(int c);

    // miscelaneous goal processing  methods
    bool qualityPost(blob b, int c);
    bool checkSize(blob b, int c);
    int checkIntersection(blob b);
    int checkCorners(blob b);
    int getBigRun(int left, int right, int hor);
    bool updateObject(VisualFieldObject* a, blob b, certainty _certainty,
                      distanceCertainty _distCertainty);
    distanceCertainty checkDist(int left, int right, int top, int bottom);

    // post recognition routines
    int crossCheck(blob b);
    int crossCheck2(blob b);
    int scanOut(int stopp, int spanX, int c);
    int checkOther(int left, int right, int height, int horizon);
    int characterizeSize(blob b);

    // shooting
    void setShot(VisualCrossbar * one);
    void bestShot(VisualFieldObject * left, VisualFieldObject * right,
                  VisualCrossbar * mid);
    void openDirection(int h, NaoPose *p);
    int classifyFirstPost(int horizon, int c, int c2, bool postFound,
                          VisualFieldObject* left, VisualFieldObject* right,
                          VisualCrossbar* mid);

    // the big kahuna
    void goalScan(VisualFieldObject *left, VisualFieldObject *right,
                  VisualCrossbar *mid, int c, int c2, bool post,
                  int horizon);
    int grabPost(int c, int c2, int horizon, int left, int right);
    void postSwap(VisualFieldObject * p1, VisualFieldObject * p2);
    void transferToChecker(blob b);
    void transferToPole();
    void transferTopBlob(VisualFieldObject * one, certainty cert,
                         distanceCertainty dc);
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
    int balls(int c, VisualBall *thisBall);

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
    int distance(int x1, int x2, int x3, int x4);
    int getPixels(int index);
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
        case DEBUGBALLPOINTSN:
            DEBUGBALLPOINTS = !DEBUGBALLPOINTS;
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

private:
    // class pointers
    Vision* vision;
    Threshold* thresh;

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
    bool BALLDEBUG;
    bool TOPFIND;
    bool CORNERDEBUG;
    bool BACKDEBUG;
    bool SANITY;
    bool BALLDISTDEBUG;
    bool DEBUGBALLPOINTS;
    bool CORRECT;
    bool OPENFIELD;
#endif

};

#endif // ObjectFragments_h_DEFINED
