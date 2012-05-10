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


#ifndef Robots_h_DEFINED
#define Robots_h_DEFINED

#include <stdlib.h>

class Robots; // forward reference
#include "Threshold.h"
#include "Context.h"
#include "VisionStructs.h"
#include "VisualLine.h"
#include "Blob.h"
#include "Blobs.h"
#include "Field.h"
#include "Cross.h"


class Robots {
public:
    Robots(Vision* vis, Threshold* thr, Field* fie, Context* con,
           unsigned char c);
    virtual ~Robots() {}

    void init();
    void findRobots(Cross* cross);
    void findUniforms();
    void findRobotParts();
    Blob correctBlob(Blob area);
    bool checkWhiteAllignment(Blob candidate);
    Blob createAreaOfInterest(Blob robotBlob);
    bool noWhite(Blob b);
    void updateRobots(int w, int i);
    void mergeBigBlobs();
    void checkMerge(int i, int j);
    bool closeEnough(int i, int j);
    bool sanityChecks(int index);
    bool notGreen(Blob a);
    bool whiteBelow(Blob a);
    bool whiteAbove(Blob b);
    void allocateColorRuns();
    int  distance(int x, int x1, int x2, int x3);
    void printBlob(Blob a);
    void setImageBox(int i, int j, int value);

#ifdef OFFLINE
    void setDebugRobots(bool debug) {debugRobots = debug;}
#endif

    //cant be accessed if private
    static const int widthScale = 5;
    static const int heightScale = 5;


private:
    // class pointers
    Vision* vision;
    Threshold* thresh;
    Field* field;
    Context* context;
    
    int imageBoxes[IMAGE_WIDTH/widthScale][IMAGE_HEIGHT/heightScale];
    //depends on image size, however ratio will be correct, given a 4:3 image.
    Blobs* blobs;
    Blobs* whiteBlobs;
    int numberOfRuns, runsize;
    unsigned char color;
    run* runs;
#ifdef OFFLINE
    bool debugRobots;
#else
    static const bool debugRobots = false;
#endif
};
#endif
