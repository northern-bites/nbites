
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

#ifndef _Vision_h_DEFINED
#define _Vision_h_DEFINED

// c++ headers
#include <sstream>
#include <fstream>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <stdint.h>

#include  "visionconfig.h"
// including info header files
#include "Common.h"
#include "VisionDef.h"
#include "Profiler.h"
#if defined(OFFLINE) || !ROBOT(NAO_RL)
#  include "MotionDef.h"
#endif
#include "FieldLinesDetector.h"

class Vision;   // forward reference

// including Class header files
#include "VisualCrossbar.h"
#include "VisualFieldObject.h"
#include "ConcreteFieldObject.h"
#include "VisualBall.h"
#include "VisualRobot.h"
#include "VisualCross.h"
#include "VisualFieldEdge.h"
#include "Threshold.h"
#include "NaoPose.h"
#include "FieldLines.h"
#include "VisualCorner.h"

class Vision
{
    friend class Threshold;

public:
    Vision(boost::shared_ptr<NaoPose> _pose);
    ~Vision();

private:
    // DO NOT ever copy or assign vision objects. Copying causes problems
    // because the pointers passed to threshold and fieldlines can become
    // invalid.
    Vision(const Vision& other);
    Vision& operator=(const Vision& other);

    void detectObjects();

public:
    // Main Vision methods
    // copy the data from the given image into the static image pointer
    void copyImage(const byte *image);
    // utilize the given image pointer for vision processing
    //   equivalent to setImage(image), followed by notifyImage()
    void notifyImage(const uint16_t *image);
    // utilize the current image pointer for vision processing
    void notifyImage();
    // set the current image pointer to the given pointer
    void setImage(const uint16_t* image);

    // visualization methods
    void drawBoxes(void);
    void drawFieldObject(VisualFieldObject* obj, int color);
    void drawCrossbar(VisualCrossbar* obj, int color);
    void drawBox(int left, int right, int bottom, int top, int c);
    void drawCenters(void);
    void drawRect(int left, int top, int width, int height, int c);
    void drawLine(int x, int y, int x1, int y1, int c);
    void drawLine(boost::shared_ptr<VisualLine> line, const int color);
    void drawLine(const point<int> start, const point<int> end,
		  const int c);
    void drawDot(int x, int y, int c);
    void drawFieldLines();
    void drawX(int x, int y, int c);
    void drawPoint(int x, int y, int c);


    //
    // SETTERS
    //

    // profiling
    inline void setTimeThresholding(long _t) { timeThresholding = _t; }
    inline void setTimeObject(long _o) { timeObject = _o; }
    inline void setTimeLines(long _l) { timeLines = _l; }

    inline void setColorTablePath(std::string path) { colorTable = path; }

    //
    // GETTERS
    //

    // profiling
    inline long int getFrameNumber() { return frameNumber; }
    inline long getTimeThresholding() { return timeThresholding; }
    inline long getTimeObject() { return timeObject; }
    inline long getTimeLines() { return timeLines; }


    // information
    inline std::string getColorTablePath() { return colorTable; }

    // misc
    std::string getThreshColor(int _id);

public:
    //
    // Public Variables
    //

    // OBJECT RECOGNITION VARIABLES
    // Field Object pointers
    VisualFieldObject *bgrp, *bglp;
    VisualFieldObject *ygrp, *yglp;
    VisualCrossbar *ygCrossbar, *bgCrossbar;
    VisualRobot *red1, *red2, *red3;
    VisualRobot *navy1, *navy2, *navy3;
	VisualCross *cross;
    VisualBall *ball;
	VisualFieldEdge *fieldEdge;
    Threshold *thresh;
    boost::shared_ptr<NaoPose> pose;
    boost::shared_ptr<FieldLines> fieldLines;

    fieldOpening fieldOpenings[3];
#define NUM_OPEN_FIELD_SEGMENTS 3

    const uint16_t * yImg, *uvImg;

    FieldLinesDetector linesDetector;
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
    float transformPercent;
    float thresholdPercent;
    float objectPercent;
    float linesPercent;
    // avg ms
    float transformMs;
    float thresholdMs;
    float objectMs;
    float linesMs;
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
    std::string colorTable;


};

#endif // _Vision_h_DEFINED
