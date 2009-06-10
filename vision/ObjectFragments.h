
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

#include <stdlib.h>

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


//constants to constrain blob and point amounts
static const int MAX_BLOBS = 400;
static const int MAX_POINTS = 100;


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
    float correct(blob b, int c, int c2);

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
    void printBall(blob b, int c, float p, int o);
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
