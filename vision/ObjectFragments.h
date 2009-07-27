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

    void newRun(int x, int endY, int height);

    // scan operations
    int yProject(int startx, int starty, int newy);
    int yProject(point <int> point, int newy);
    int xProject(int startx, int starty, int newx);
    int xProject(point <int> point, int newx);
    void vertScan(int x, int y, int dir, int stopper, int c, int c2, stop & scan);
    void horizontalScan(int x, int y, int dir, int stopper, int c, int c2, int l,
                        int r, stop & scan);
    int findTrueLineVertical(point <int> top, point <int> bottom, int c, int c2,
                             bool left);
    int findTrueLineHorizontal(point <int> left, point <int> right, int c, int c2,
                               bool up);
    void findVerticalEdge(point <int>& top, point <int>& bottom, int c,
                                    int c2, bool left);
    void findHorizontalEdge(point <int>& left, point <int>& right,
                                      int c, int c2, bool up);
    bool checkEdge(int x, int y, int x1, int y1);
    int horizonAt(int x);

    // finding square objects
    void squareGoal(int x, int y, int c, int c2, Blob & pole);
    float correct(Blob b, int c, int c2);

    // main method
    void createObject(int c);

    // miscelaneous goal processing  methods
    bool qualityPost(Blob b, int c);
    bool checkSize(Blob b, int c);
    int getBigRun(int left, int right, int hor);
    bool updateObject(VisualFieldObject* a, Blob b, certainty _certainty,
                      distanceCertainty _distCertainty);
    distanceCertainty checkDist(int left, int right, int top, int bottom,
								Blob pole);

    // post recognition routines
    int classifyByCrossbar(Blob b);
    int classifyByOtherRuns(int left, int right, int height, int horizon);
    int classifyByLineIntersection(Blob b);
    int classifyByCheckingCorners(Blob b);

    int characterizeSize(Blob b);

    int classifyFirstPost(int horizon, int c, int c2,
                          VisualFieldObject* left, VisualFieldObject* right,
                          VisualCrossbar* mid, Blob pole);

    // the big kahuna
    void goalScan(VisualFieldObject *left, VisualFieldObject *right,
                  VisualCrossbar *mid, int c, int c2,
                  int horizon);
    int grabPost(int c, int c2, int horizon, int left, int right, Blob & pole);
    void postSwap(VisualFieldObject * p1, VisualFieldObject * p2);
    void transferTopBlob(VisualFieldObject * one, certainty cert,
                         distanceCertainty dc);

    // sanity checks
    bool rightBlobColor(Blob obj, float per);
    bool postBigEnough(Blob b);
    bool horizonBottomOk(int spanX, int spanY, int minHeight, int left, int right,
                         int bottom, int top);
    bool horizonTopOk(int top, int hor);
    bool postRatiosOk(float ratio);
    bool secondPostFarEnough(point <int> l1, point <int> r1,
                             point <int> l2, point <int> r2, int p);
    bool blobOk(Blob b);
    bool locationOk(Blob b, int hor);
    bool relativeSizesOk(int x1, int y1, int s2, int y2, int t1, int t2, int f);

    // misc.
    int distance(int x1, int x2, int x3, int x4);
    float getSlope() { return slope; }
	bool greenCheck(Blob b);


    // debugging methods
    void printObjs();
    void drawPoint(int x, int y, int c);
    void drawRect(int x, int y, int w, int h, int c);
    void drawBlob(Blob b, int c);
    void drawLine(int x, int y, int x1, int y1, int c);
    void printBlob(Blob b);
    void printObject(VisualFieldObject * objs);
    void paintRun(int x,int y, int h, int c);
    void drawRun(const run& run, int c);


private:
    // class pointers
    Vision* vision;
    Threshold* thresh;

    int color;
    int runsize;
    int biggestRun;
    int numberOfRuns;
    run* runs;
    float slope;
};

#endif // ObjectFragments_h_DEFINED
