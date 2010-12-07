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
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
// GNU Lesser Public License for more details.
//
// You should have received a copy of the GNU General Public License
// and the GNU Lesser Public License along with Man.  If not, see
// <http://www.gnu.org/licenses/>.


/*
 * Context - holds context information on field objects in order to
 * better recognize them and identify them.
 */

#include <iostream>
#include "Context.h"
#include "debug.h"
#include "FieldConstants.h"
#include "Utility.h"
#include <vector>
#include <boost/shared_ptr.hpp>

using namespace std;

Context::Context(Vision *vis, Threshold* thr, Field* fie)
    : vision(vis), thresh(thr), field(fie)
{
    init();
}

void Context::init() {
    rightYellowPost = false;
    leftYellowPost = false;
    unknownYellowPost = false;
    yellowPost = false;
    rightBluePost = false;
    leftBluePost = false;
    unknownBluePost = false;
    bluePost = false;
    tCorner = 0;
    lCorner = 0;
    iCorner = 0;
    oCorner = 0;
    cross = false;
    unknownCross = false;
    yellowCross = false;
    blueCross = false;
    ball = false;
}
