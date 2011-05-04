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
#include "VisionStructs.h"
#include "VisionHelpers.h"

class ObjectFragments; // forward reference
#include "Threshold.h"
#include "Context.h"
#include "VisualFieldObject.h"
#include "ConcreteFieldObject.h"
#include "VisualBall.h"
#include "Vision.h"
#include "Field.h"

static const int MAX_BLOBS = 400;
static const int MAX_POINTS = 100;


class ObjectFragments {
public:
    ObjectFragments(Vision* vis, Threshold* thr, Field* fie, Context* con,
                    int _color);
    virtual ~ObjectFragments() {}

    // SETTERS
    void setColor(int c);
    void allocateColorRuns();

    // Making object
    void init(float s);

    void newRun(int x, int endY, int height);
	bool colorsEqual(unsigned char x, unsigned char y);

    // scan operations
    int yProject(int startx, int starty, int newy);
    int yProject(point <int> point, int newy);
    int xProject(int startx, int starty, int newx);
    int xProject(point <int> point, int newx);
    void vertScan(int x, int y, int dir, int stopper, int c, stop & scan);
	int pickNth(int val[], int n, int s);
    void horizontalScan(int x, int y, int dir, int stopper, int c, int l,
                        int r, stop & scan);
    int findTrueLineVertical(point <int> top, point <int> bottom, int c,
                             bool left);
    int findTrueLineHorizontal(point <int> left, point <int> right, int c,
                               bool up);
    void findVerticalEdge(point <int>& top, point <int>& bottom, int c,
                                    bool left);
    void findHorizontalEdge(point <int>& left, point <int>& right,
                                      int c, bool up);
    bool checkEdge(int x, int y, int x1, int y1);
    int horizonAt(int x);

    // finding square objects
    void squareGoal(int x, int y, int left, int right, int top, int bottom,
					int c, Blob & pole);
    float correct(Blob & b, int c);

    // main method
    void createObject();

    // miscelaneous goal processing  methods
    bool qualityPost(Blob b, int c);
    bool checkSize(Blob b, int c);
    int getBigRun(int left, int right);
    bool updateObject(VisualFieldObject* a, Blob b, certainty _certainty,
                      distanceCertainty _distCertainty);
    distanceCertainty checkDist(Blob pole);

    // post recognition routines
    int classifyByCrossbar(Blob b);
    int classifyByOtherRuns(int left, int right, int height);
    int classifyByTCorner(Blob b);
    int classifyByCheckingCorners(Blob b);
    int cornerClassifier(float diff, float dist, int x, int y,int class1,
                         int class2, bool goal);
    int classifyGoalBoxLineThatAbutsPost(int y, float diff, float dist,
                                         int classification);
    bool withinEdgeMargin(int x, int margin);
    bool withinVerticalEdgeMargin(int y, int margin);
    int classifyByLengthOfGoalline(float dist, int x, int y,
                                   int class1, int class2);
    int classifyByGoalline(const point<int> linel, const point<int> liner,
                           point<int> left, point<int> right);
    int classifyByGoalBoxFrontline(pair<int, int> foo,
                                   point<int> left, point<int> right);
    int getFrontlineClassification(point<int> post,
                                   pair<int, int> foo,
                                   int classification);




	int classifyByCheckingLines(Blob post);

    int characterizeSize(Blob b);

    int classifyFirstPost(int c, Blob pole);

    // the big kahuna
    void lookForFirstPost(VisualFieldObject *left, VisualFieldObject *right,
                  VisualCrossbar *mid, int c);
    void lookForSecondPost(Blob pole, int post,
                                            VisualFieldObject* left,
                                            VisualFieldObject* right,
                           VisualCrossbar* mid, int c);

    void updateRunsAfterFirstPost(Blob pole, int post);
    int grabPost(int c, int left, int right, Blob & pole);
    void postSwap(VisualFieldObject * p1, VisualFieldObject * p2);
    void transferTopBlob(VisualFieldObject * one, certainty cert,
                         distanceCertainty dc);

    // sanity checks
    bool isPostReasonableSizeShapeAndPlace(Blob post);

    bool rightBlobColor(Blob obj, float per);
    bool postBigEnough(Blob b);
    bool horizonBottomOk(int spanX, int spanY, int minHeight, int left, int right,
                         int bottom, int top);
    bool postRatiosOk(float ratio);
    bool secondPostFarEnough(Blob a, Blob b, int p);
    bool blobOk(Blob b);
	bool badDistance(Blob b);
    bool locationOk(Blob b);
    bool relativeSizesOk(Blob a, Blob b);

    // misc.
    bool withinMargin(float n, float n1, float n2);
    bool withinMarginInt(int n, int n1, int n2);
    int distance(int x1, int x2, int x3, int x4);
	float realDistance(int x1, int y1, int x2, int y2);
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
#ifdef OFFLINE
	void setPrintObjs(bool debug) {PRINTOBJS = debug;}
	void setPostDebug(bool debug) {POSTDEBUG = debug;}
	void setPostLogic(bool debug) {POSTLOGIC = debug;}
	void setSanity(bool debug) {SANITY = debug;}
	void setCorrect(bool debug) {CORRECT = debug;}
#endif


private:
    // class pointers
    Vision* vision;
    Threshold* thresh;
	Field* field;
    Context* context;

    int color;
    int runsize;
    int biggestRun;
    int numberOfRuns;
    run* runs;
    float slope;
#ifdef OFFLINE
	bool PRINTOBJS;
	bool POSTDEBUG;
	bool POSTLOGIC;
	bool SANITY;
	bool CORRECT;
#else
	static const bool PRINTOBJS = false;
	static const bool POSTDEBUG = false;
	static const bool POSTLOGIC = false;
	static const bool SANITY = false;
	static const bool CORRECT = false;
#endif

};

#endif // ObjectFragments_h_DEFINED
