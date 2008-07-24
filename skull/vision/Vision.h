/* Vision.h  */

#ifndef _Vision_h_DEFINED
#define _Vision_h_DEFINED

// c++ headers
#include <sstream>
#include <fstream>
#include <vector>

// including info header files
#include "Common.h"
#include "VisionDef.h"
#include "CortexDef.h"
#include "Profiler.h"

#if defined(OFFLINE) || !ROBOT(NAO_RL)
#  include "MotionDef.h"
#endif

class Vision;   // forward reference

// including Class header files
#include "FieldObjects.h"
#include "Ball.h"
#include "Threshold.h"
#include "Pose.h"
#include "FieldLines.h"
#include "VisualCorner.h"

using namespace std;


// helper method for deflate() for shifting bits for bit-wise compression
inline static void setBit(unsigned char &b, int pos, int value) {
  b = (int) b | (value << pos);
} 


class Vision
{
    friend class Threshold;

  public:
    Vision(Pose *_pose, Profiler *_prof);
    virtual ~Vision();
    
    // Main Vision methods
    //   virtual, to allow overloading
    // copy the data from the given image into the static image pointer
    virtual void copyImage(const byte *image);
    // utilize the given image pointer for vision processing
    //   equivalent to setImage(image), followed by notifyImage()
    virtual void notifyImage(const unsigned char *image);
    // utilize the current image pointer for vision processing
    virtual void notifyImage();
    // set the current image pointer to the given pointer
    virtual void setImage(const byte* image);

    // visualization methods
    virtual void drawBoxes(void);
    virtual void drawFieldObject(FieldObjects* obj, int color);
    virtual void drawBox(int left, int right, int bottom, int top, int c);
    virtual void drawCenters(void);
    virtual void drawRect(int left, int top, int width, int height, int c);
    virtual void drawPoint(int x, int y,int c);
    virtual void drawPoint(const linePoint &p, const int color);
    virtual void drawPoint(const VisualCorner &c, const int color);
    virtual void drawLine(int x, int y, int x1, int y1, int c);
    virtual void drawLine(const VisualLine &line, const int color);
    virtual void drawVerticalLine(int x, int c);
    virtual void drawHorizontalLine(int x, int c);
    virtual void drawDot(int x, int y, int c);
    virtual void drawCrossHairs(int x, int y, int c);
    virtual void drawFieldLines();
    
  
    //
    // SETTERS
    //
  
    // profiling
    inline void setTimeThresholding(long _t) { timeThresholding = _t; }
    inline void setTimeObject(long _o) { timeObject = _o; }
    inline void setTimeLines(long _l) { timeLines = _l; }
  
    inline void setColorTablePath(std::string path) { colorTable = path; }
    inline void setPlayerNumber(int n) { player = n; }
    inline void setDogID(int _id) { id = _id; }
    inline void setRobotName(std::string _name) { name = _name; }
    
    //
    // GETTERS
    //

    // profiling
    inline long int getFrameNumber() { return frameNumber; }
    inline long getTimeThresholding() { return timeThresholding; }
    inline long getTimeObject() { return timeObject; }
    inline long getTimeLines() { return timeLines; }

  
    // information
    inline int getDogID() { return id; }
    std::string getRobotName();
    inline int getPlayerNumber() { return player; }
    inline std::string getColorTablePath() { return colorTable; }

    // misc
    std::string getThreshColor(int _id);

  public:
    //
    // Public Variables
    //
    
    // OBJECT RECOGNITION VARIABLES
    // Field Object pointers
    FieldObjects *bgrp, *bglp, *bgBackstop;
    FieldObjects *ygrp, *yglp, *ygBackstop;
    FieldObjects *by, *yb;
    FieldObjects *blueArc, *yellowArc;
    FieldObjects *red1, *red2;
    FieldObjects *navy1, *navy2;
    Ball *ball;
#ifdef USE_PINK_BALL
    Ball *pinkBall; //added for pink ball recognition
#endif
    Threshold *thresh;
    Pose *pose;
    FieldLines *fieldLines;

	fieldOpening fieldOpenings[3];
#define NUM_OPEN_FIELD_SEGMENTS 3

    // Profiling
    Profiler *profiler;

  protected:
    //
    // Protected Variable
    //
    

    // PROFILING VARIABLES
    // time counters
    long timeTransform;
    long timeThresholding;
    long timeObject;
    long timeLines;
    // aggregate totals
    long totalTransform;
    long totalThreshold;
    long totalObject;
    long totalLines;
    // results
    // avg percents
    double transformPercent;
    double thresholdPercent;
    double objectPercent;
    double linesPercent;
    // avg ms
    double transformMs;
    double thresholdMs;
    double objectMs;
    double linesMs;
    // booleans for stop/start profiling
    bool collectFrames;
    bool collectTimes;

 private:
    //
    // Private Variables
    //

    // Random Vision Variables
    long int frameNumber;

    // information
    int id;
    std::string name;
    int player;
    std::string colorTable;

};

#endif // _Vision_h_DEFINED
