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
#include "VisionStructs.h"
#include "VisualLine.h"
#include "Blob.h"
#include "Blobs.h"
#include "Field.h"


class Robots {
public:
    Robots(Vision* vis, Threshold* thr, Field* fie, int c);
    virtual ~Robots() {}

	void init();
	void preprocess();
	void robot(int bg);
	void expandRobotBlob(int which);
	bool noWhite(Blob b);
	void expandHorizontally(int which, int dir);
	int expandVertically(int which, int dir);
	bool goodScan(int c, int w, int o, int g, int gr, int t);
	void updateRobots(int w, int i);
	void mergeBigBlobs();
	bool closeEnough(Blob a, Blob b);
	bool bigEnough(Blob a, Blob b);
	bool noGreen(Blob a, Blob b);
	bool checkHorizontal(int l, int r, int t, int b);
	bool checkVertical(int l, int r, int t, int b);
	bool viableRobot(Blob a);
	void createObject();
	void newRun(int x, int y, int h);
	void setColor(int c);
	void allocateColorRuns();
	int distance(int x, int x1, int x2, int x3);
	void printBlob(Blob a);

private:
    // class pointers
    Vision* vision;
    Threshold* thresh;
	Field* field;

	Blobs* blobs;
	int numberOfRuns, runsize;
	int color;
	Blob* topBlob;
	run* runs;
};
#endif
