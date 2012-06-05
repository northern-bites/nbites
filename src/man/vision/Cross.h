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


#ifndef Cross_h_DEFINED
#define Cross_h_DEFINED

#include <stdlib.h>

class Cross; // forward reference
#include "Threshold.h"
#include "Context.h"
#include "VisionStructs.h"
#include "VisualLine.h"
#include "Blob.h"
#include "Blobs.h"

static const int NOISE = 4;

class Cross {
public:
    Cross(Vision* vis, Threshold* thr, Field* fie, Context* con);
    virtual ~Cross() {}

    void init();
    void createObject();
    void checkForCrosses();
	bool checkForRobotBlobs(Blob b);
    bool checkSizeAgainstPixEstimate(Blob b);
    bool scanAroundPerimeter(Blob b);
    bool checkForLineIntersection(Blob b);
    void checkForX(Blob b);
    void newRun(int x, int y, int h);
    void allocateColorRuns();
    bool rightBlobColor(Blob b, float perc);
    Blobs* getBlobs();

#ifdef OFFLINE
    void setCrossDebug(bool debug) {CROSSDEBUG = debug;}
#endif

private:
    // class pointers
    Vision* vision;
    Threshold* thresh;
    Field* field;
    Context* context;

    Blobs* blobs;
    int numberOfRuns, runsize;
    run* runs;
#ifdef OFFLINE
    bool CROSSDEBUG;
#else
    static const bool CROSSDEBUG = false;
#endif
};
#endif
