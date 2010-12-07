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


#ifndef Context_h_DEFINED
#define Context_h_DEFINED

#include <stdlib.h>

#include "Common.h"
#include "VisionStructs.h"
#include "VisionHelpers.h"

class Context; // forward reference
#include "ObjectFragments.h"
#include "Threshold.h"
#include "VisualFieldObject.h"
#include "ConcreteFieldObject.h"
#include "ConcreteCorner.h"
#include "ConcreteLine.h"
#include "VisualBall.h"
#include "Vision.h"
#include "Field.h"


class Context {
public:
    Context(Vision* vis, Threshold* thr, Field* fie);
    virtual ~Context() {}

    void init();
    void setContext();

    // setters
    void setTCorner() {tCorner++;}
    void setLCorner() {lCorner++;}
    void setOLCorner() {oCorner++; lCorner++;}
    void setILCorner() {iCorner++; lCorner++;}
    void setRightYellowPost() {rightYellowPost = true; yellowPost = true;}
    void setLeftYellowPost() {leftYellowPost = true; yellowPost = true;}
    void setUnknownYellowPost() {unknownYellowPost = true; yellowPost = true;}
    void setRightBluePost() {rightBluePost = true; bluePost = true;}
    void setLeftBluePost() {leftBluePost = true; bluePost = true;}
    void setUknownBluePost() {unknownBluePost = true; bluePost = true;}
    void setUnknownCross() {unknownCross = true; cross = true;}
    void setYellowCross() {yellowCross = true; cross = true;}
    void setBlueCross() {blueCross = true; cross = true;}
    void setCross() {cross = true;}
    void setBall() {ball = true;}

    // getters
    int  getTCorner() {return tCorner;}
    int  getLCorner() {return lCorner;}
    int  getILCorner() {return iCorner;}
    int  getOLCorner() {return oCorner;}
    bool getRightYellowPost() {return rightYellowPost;}
    bool getLeftYellowPost() {return leftYellowPost;}
    bool getUnknownYellowPost() {return unknownYellowPost;}
    bool getYellowPost() {return yellowPost;}
    bool getRightBluePost() {return rightBluePost;}
    bool getLeftBluePost() {return leftBluePost;}
    bool getUnknownBluePost() {return unknownBluePost;}
    bool getCross() {return cross;}
    bool getUnknownCross() {return unknownCross;}
    bool getYellowCross() {return yellowCross;}
    bool getBlueCross() {return blueCross;}
    bool getBall() {return ball;}

private:
    Vision* vision;
    Threshold* thresh;
    Field* field;

    bool rightYellowPost;
    bool leftYellowPost;
    bool unknownYellowPost;
    bool yellowPost;
    bool rightBluePost;
    bool leftBluePost;
    bool unknownBluePost;
    bool bluePost;
    int  tCorner;
    int  lCorner;
    int  iCorner;
    int  oCorner;
    bool cross;
    bool unknownCross;
    bool yellowCross;
    bool blueCross;
    bool ball;
};

#endif // Context_h_DEFINED
