
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


#ifndef Ball_h_DEFINED
#define Ball_h_DEFINED

#include <stdlib.h>

#include "Common.h"
#include "VisionStructs.h"
#include "VisionHelpers.h"
#include "Blob.h"
#include "Blobs.h"

class Ball; // forward reference
#include "Threshold.h"
#include "Context.h"
#include "Field.h"
#include "VisualBall.h"
#include "Vision.h"


//constants to constrain blob and point amounts
static const int MAX_BALLS = 400;
static const int MAX_BALL_POINTS = 100;
static const int MAX_NUM_BALL_RUNS = 500; //never use this!
static const int BALL_RUNS_MALLOC_SIZE = 10000;
static const int BAD_VALUE = -10000;
static const int NOISE_SKIPS = 1;
static const float ORANGE_BALL_RADIUS = 32.5; //mm

class Ball {
public:
    Ball(Vision* vis, Threshold* thr, Field* fie, Context* con,
         unsigned char _color);
    virtual ~Ball() {}

    // SETTERS
    void setColor(unsigned char c);
    void allocateColorRuns();
    void newRun(int x, int y, int h);

    // Making object
    void init(float s);

    int horizonAt(int x);

    // main methods
    void createBall(int c);

    // ball stuff
    bool isOrange(unsigned char pix);
    bool isRed(unsigned char pix);
    bool isYellow(unsigned char pix);
    bool isGreen(unsigned char pix);
    bool isUndefined(unsigned char pix);
    float rightHalfColor(Blob obj);
    void setOcclusionInformation();
    bool ballIsReasonablySquare(int x, int y, int w, int h);
    bool nearImageEdgeX(int x, int margin);
    bool nearImageEdgeY(int y, int margin);
    bool nearEdge(Blob b);
    int roundness(Blob b);
    std::pair<int, int> scanMidlinesForRoundnessInformation(Blob b);
    std::pair<int, int> scanDiagonalsForRoundnessInformation(Blob b);
    bool badSurround(Blob b);
    void setBallInfo(int w, int h, VisualBall *thisBall, estimate e);
    void checkForReflections(int h, int w, VisualBall * thisBall,
                             estimate e);
    bool ballIsClose(VisualBall * thisBall);
    bool ballIsNotSquare(int h, int w);

    int balls(int c, VisualBall *thisBall);

    void adjustBallDimensions();
    int findBallEdgeY(int x, int y, int dir);
    int findBallEdgeX(int x, int y, int dir);

    void setFramesOnAndOff(VisualBall *objPtr);

    // sanity checks
    void preScreenBlobsBasedOnSizeAndColor();
    bool sanityChecks(int w, int h, estimate e, VisualBall * thisBall);
    bool blobOk(Blob b);
    bool blobIsBigEnoughToBeABall(int w, int h);

    // debugging methods
    void printBall(Blob b, int c, float p, int o);
    void drawPoint(int x, int y, int c);
    void drawRect(int x, int y, int w, int h, int c);
    void drawBlob(Blob b, int c);
    void drawLine(int x, int y, int x1, int y1, int c);
    void printBlob(Blob b);
    void paintRun(int x,int y, int h, int c);
    void drawRun(const run& run, int c);
#ifdef OFFLINE
    void setDebugBall(bool debugBall) {BALLDEBUG = debugBall;}
    void setDebugBallDistance(bool debug) {BALLDISTDEBUG = debug;}
#endif

private:
    // class pointers
    Vision* vision;
    Threshold* thresh;
    Field* field;
    Context* context;

    int topSpot; //keeps track of best blob -> global var for debugging purposes
    unsigned char color;
    int runsize;
    int biggestRun;
    int maxHeight;
    int maxOfBiggestRun;
    int numberOfRuns;
    int indexOfBiggestRun;
    run* runs;

    Blob *topBlob, zeroBlob;
    //Blob checker, obj, pole, leftBox, rightBox;
    Blobs *blobs;
    estimate focalDist;
    int inferredConfidence;
    float slope;
    int occlusion;
    point <int> spot;
    int numPoints;
    float points[MAX_BALL_POINTS*2];
#ifdef OFFLINE
    bool BALLDISTDEBUG;
    bool BALLDEBUG;
#else
    static const bool BALLDISTDEBUG = false;
    static const bool BALLDEBUG = false;
#endif
};

#endif // Ball_h_DEFINED
