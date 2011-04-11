
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


/**
 * Vision.cc  -- The Main Vision Module Class file.
 *
 * This file does the main initialization for the entire module, including:
 *   - picking the camera settings
 *   - declaring all the class variables
 *   - starting AiboConnect TCP Server
 *   - other important things
 *
 * Then, this file contains the main processing loop for the module, called
 * notifyImage(), which performs all the vision processing.
 */

#include <boost/shared_ptr.hpp>
#include "Vision.h" // Vision Class Header File

using namespace std;
using boost::shared_ptr;

static uint8_t global_8_image[IMAGE_BYTE_SIZE];
static uint16_t global_16_image[IMAGE_BYTE_SIZE];

// Vision Class Constructor
Vision::Vision(shared_ptr<NaoPose> _pose, shared_ptr<Profiler> _prof)
    : pose(_pose), profiler(_prof),
      yImg(&global_16_image[0]), linesDetector(profiler),
      frameNumber(0), id(-1), name(), player(1),
      colorTable("table.mtb")
{
    // variable initialization

    /* declaring class pointers for field objects, ball, leds, lines*/
    ygrp = new VisualFieldObject(YELLOW_GOAL_RIGHT_POST);
    yglp = new VisualFieldObject(YELLOW_GOAL_LEFT_POST);
    bgrp = new VisualFieldObject(BLUE_GOAL_RIGHT_POST);
    bglp = new VisualFieldObject(BLUE_GOAL_LEFT_POST);
    ygCrossbar = new VisualCrossbar();
    bgCrossbar = new VisualCrossbar();
    ball = new VisualBall();
    red1 = new VisualRobot();
    red2 = new VisualRobot();
    navy1 = new VisualRobot();
    navy2 = new VisualRobot();
	cross = new VisualCross();
	fieldEdge = new VisualFieldEdge();

    thresh = new Threshold(this, pose);
    fieldLines = shared_ptr<FieldLines>(new FieldLines(this, pose, profiler));
    thresh->setYUV(&global_16_image[0]);
}

// Vision Class Deconstructor
Vision::~Vision()
{
    delete thresh;
    delete navy2;
    delete navy1;
    delete red2;
    delete red1;
    delete ball;
    delete bgCrossbar;
    delete ygCrossbar;
    delete bglp;
    delete bgrp;
    delete yglp;
    delete ygrp;
}

void Vision::copyImage(const byte* image) {
    memcpy(&global_16_image[0], image, IMAGE_BYTE_SIZE);
    thresh->setYUV(&global_16_image[0]);
}

void Vision::notifyImage(const uint16_t* y) {
    yImg = y;
    uvImg = y + AVERAGED_IMAGE_SIZE;

    // Set the current image pointer in Threshold
    thresh->setYUV(y);
    notifyImage();
}

/* notifyImage() -- The Image Loop
 *
 * This is the most important loop, ever, really.  This is what the operating
 * system calls when there is a new image.  Here is--in order--what we do next:
 *
 * -Do Chromatic Distortion filtering on the Y,Cr,Cb values
 * -this is done by the setYUV() method and in CorrectedImage.cc
 * -Vision Processing->thresholding and object recognition.
 * -this is done in ChownRLE.cc (thresholding) and in ObjectFragments (rle and
 *  recognition)
 * -Behavior processing
 * -processFrame() in PyEmbed.cc, and then in the PyCode brain
 * -Handle image arrays if AiboConnect is requesting them
 * -Calculate Frames Per Second.
 *
 */
void Vision::notifyImage() {
    PROF_ENTER(profiler, P_VISION);

    // NORMAL VISION LOOP
    frameNumber++;
    // counts the frameNumber
    if (frameNumber > 1000000) frameNumber = 0;

    // Transform joints into pose estimations and horizon line
    PROF_ENTER(profiler, P_TRANSFORM);
    pose->transform();
    PROF_EXIT(profiler, P_TRANSFORM);

    // Perform image correction, thresholding, and object recognition
    thresh->visionLoop();

    linesDetector.detect(thresh->getVisionHorizon(), yImg);

    drawEdges(*linesDetector.getEdges());
    drawHoughLines(linesDetector.getHoughLines());
    drawVisualLines(*linesDetector.getLines());
    PROF_EXIT(profiler, P_VISION);
}

void Vision::setImage(const uint16_t *image) {
    thresh->setYUV(image);
}

std::string Vision::getThreshColor(int _id) {
    switch (_id) {
    case WHITE: return "WHITE";
    case ORANGE: return "ORANGE";
    case BLUE: return "BLUE";
    case GREEN: return "GREEN";
    case YELLOW: return "YELLOW";
    case BLACK: return "BLACK";
    case RED: return "RED";
    case NAVY: return "NAVY";
    case GREY: return "GREY";
    case YELLOWWHITE: return "YELLOWWHITE";
    case BLUEGREEN: return "BLUEGREEN";
    case PINK: return "PINK";
        //case BOTHWHITE: return "BOTHWHITE";
        //case TRUE_BLUE: return "TRUE_BLUE";
    default: return "No idea what thresh color you have, mate";
    }
}

std::string Vision::getRobotName() {
    return name;
}



/*******************************|
| Vision visualization methods. |
|*******************************/


/* draBoxes() --
   draws bounding boxes around all the objects we identify:
   -Goals (also draws top and bottom points)
   -Posts
   -Ball
   -Dogs
*/
void Vision::drawBoxes(void)
{

    // draw field objects
    if(bglp->getDistance() > 0) drawFieldObject(bglp,RED);
    if(bgrp->getDistance() > 0) drawFieldObject(bgrp,BLACK);
    if(yglp->getDistance() > 0) drawFieldObject(yglp,RED);
    if(ygrp->getDistance() > 0) drawFieldObject(ygrp,BLACK);
    if(ygCrossbar->getHeight() > 0) drawCrossbar(ygCrossbar, BLUE);
    if(bgCrossbar->getHeight() > 0) drawCrossbar(bgCrossbar, YELLOW);

    // balls
    // orange
    if(ball->getWidth() > 0)
        drawRect(ball->getX(), ball->getY(),
                 NBMath::ROUND(ball->getWidth()),
                 NBMath::ROUND(ball->getHeight()), PINK);

    // lines
    drawFieldLines();

    // pose horizon line
    drawLine(pose->getLeftHorizon().x,
             pose->getLeftHorizon().y,
             pose->getRightHorizon().x,
             pose->getRightHorizon().y,
             BLUE);

    // vision horizon line
    thresh->drawPoint(IMAGE_WIDTH/2, thresh->getVisionHorizon(), RED);

} // drawBoxes

// self-explanatory
void Vision::drawFieldObject(VisualFieldObject* obj, int color) {
    drawLine(obj->getLeftTopX(), obj->getLeftTopY(),
             obj->getRightTopX(), obj->getRightTopY(), color);
    drawLine(obj->getLeftTopX(), obj->getLeftTopY(),
             obj->getLeftBottomX(), obj->getLeftBottomY(), color);
    drawLine(obj->getRightBottomX(), obj->getRightBottomY(),
             obj->getRightTopX(), obj->getRightTopY(), color);
    drawLine(obj->getLeftBottomX(), obj->getLeftBottomY(),
             obj->getRightBottomX(), obj->getRightBottomY(), color);
}

// self-explanatory
void Vision::drawCrossbar(VisualCrossbar* obj, int color) {
    drawLine(obj->getLeftTopX(), obj->getLeftTopY(),
             obj->getRightTopX(), obj->getRightTopY(), color);
    drawLine(obj->getLeftTopX(), obj->getLeftTopY(),
             obj->getLeftBottomX(), obj->getLeftBottomY(), color);
    drawLine(obj->getRightBottomX(), obj->getRightBottomY(),
             obj->getRightTopX(), obj->getRightTopY(), color);
    drawLine(obj->getLeftBottomX(), obj->getLeftBottomY(),
             obj->getRightBottomX(), obj->getRightBottomY(), color);
}

/* drawBox()
   --helper method for drawing rectangles on the thresholded array
   for AiboConnect visualization.
   --takes as input the left,right (x1,x2),bottom,top (y1,y2)
   --the rectangle drawn is a non-filled box.
*/
void Vision::drawBox(int left, int right, int bottom, int top, int c) {
    if (left < 0) {
        left = 0;
    }
    if (top < 0) {
        top = 0;
    }
    int width = right-left;
    int height = bottom-top;

    for (int i = left; i < left + width; i++) {
        if (top >= 0 &&
            top < IMAGE_HEIGHT &&
            i >= 0 &&
            i < IMAGE_WIDTH) {
            thresh->setThresholded(top,i, static_cast<unsigned char>(c));
        }
        if ((top + height) >= 0 &&
            (top + height) < IMAGE_HEIGHT &&
            i >= 0 &&
            i < IMAGE_WIDTH) {
            thresh->setThresholded(top + height,i, static_cast<unsigned char>(c));
        }
    }
    for (int i = top; i < top + height; i++) {
        if (i >= 0 &&
            i < IMAGE_HEIGHT &&
            left >= 0 &&
            left < IMAGE_WIDTH) {
            thresh->setThresholded(i,left, static_cast<unsigned char>(c));
        }
        if (i >= 0 &&
            i < IMAGE_HEIGHT &&
            (left+width) >= 0 &&
            (left+width) < IMAGE_WIDTH) {
            thresh->setThresholded(i,left + width, static_cast<unsigned char>(c));
        }
    }
} // drawBox


/* drawCenters()
   --draws little crosshairs at the center x,y of identified objects.
   includes checks to make sure the crosshairs don't draw off the screen
   (index array out of bounds error producers)
   --draws:
   -ball
   -goals
   -add more if you'd like
*/
void Vision::drawCenters() {
    // draws an X at the ball center X and Y.
    if (ball->getCenterX() >= 2 && ball->getCenterY() >= 2
        && ball->getCenterX() <= (IMAGE_WIDTH-2)
        && ball->getCenterY() <= (IMAGE_HEIGHT-2)) {
        thresh->drawPoint(ball->getCenterX(), ball->getCenterY(), YELLOW);
    }

} // drawCenters


/* drawRect()
   --helper method for drawing rectangles on the thresholded array
   for AiboConnect visualization.
   --takes as input the getX,getY (top left x,y coords),
   the width and height of the object,
   and lastly the color of the rectangle you want to use.
   --the rectangle drawn is a non-filled box.
*/
void Vision::drawRect(int left, int top, int width, int height, int c) {
    if (left < 0) {
        width += left;
        left = 0;
    }
    if (top < 0) {
        height += top;
        top = 0;
    }

    for (int i = left; i < left + width; i++) {
        if (top >= 0 && top < IMAGE_HEIGHT && i >= 0 && i < IMAGE_WIDTH) {
            thresh->setThresholded(top,i, static_cast<unsigned char>(c));
        }
        if ((top + height) >= 0 &&
            (top + height) < IMAGE_HEIGHT &&
            i >= 0 &&
            i < IMAGE_WIDTH) {
            thresh->setThresholded(top + height,i, static_cast<unsigned char>(c));
        }
    }
    for (int i = top; i < top + height; i++) {
        if (i >= 0 &&
            i < IMAGE_HEIGHT &&
            left >= 0 &&
            left < IMAGE_WIDTH) {
            thresh->setThresholded(i,left, static_cast<unsigned char>(c));
        }
        if (i >= 0 &&
            i < IMAGE_HEIGHT &&
            (left+width) >= 0 &&
            (left+width) < IMAGE_WIDTH) {
            thresh->setThresholded(i,left + width, static_cast<unsigned char>(c));
        }
    }
#if ROBOT(NAO)
    left--;
    width+=2;
    height+=2;
    top--;
    if (left < 0) {
        width += left;
        left = 0;
    }
    if (top < 0) {
        height += top;
        top = 0;
    }
    for (int i = left; i < left + width; i++) {
        if (top >= 0 && top < IMAGE_HEIGHT && i >= 0 && i < IMAGE_WIDTH) {
            thresh->setThresholded(top,i, static_cast<unsigned char>(c));
        }
        if ((top + height) >= 0 &&
            (top + height) < IMAGE_HEIGHT &&
            i >= 0 &&
            i < IMAGE_WIDTH) {
            thresh->setThresholded(top + height,i, static_cast<unsigned char>(c));
        }
    }
    for (int i = top; i < top + height; i++) {
        if (i >= 0 &&
            i < IMAGE_HEIGHT &&
            left >= 0 &&
            left < IMAGE_WIDTH) {
            thresh->setThresholded(i,left, static_cast<unsigned char>(c));
        }
        if (i >= 0 &&
            i < IMAGE_HEIGHT &&
            (left+width) >= 0 &&
            (left+width) < IMAGE_WIDTH) {
            thresh->setThresholded(i,left + width, static_cast<unsigned char>(c));
        }
    }
#endif
} // drawRect

/* drawLine()
   --helper visualization method for drawing a line given two points and a color
*/
void Vision::drawLine(int x, int y, int x1, int y1, int c) {
    float slope = (float)(y - y1) / (float)(x - x1);
    int sign = 1;
    if ((abs(y - y1)) > (abs(x - x1))) {
        slope = 1.0f / slope;
        if (y > y1) sign = -1;
        for (int i = y; i != y1; i += sign) {
            int newx = x + static_cast<int>(slope * static_cast<float>(i - y) );
            if (newx >= 0 && newx < IMAGE_WIDTH && i >= 0 && i < IMAGE_HEIGHT)
                thresh->setThresholded(i,newx, static_cast<unsigned char>(c));
        }
    } else if (slope != 0) {
        //slope = 1.0 / slope;
        if (x > x1) sign = -1;
        for (int i = x; i != x1; i += sign) {
            int newy = y + static_cast<int>(slope * static_cast<float>(i - x));
            if (newy >= 0 && newy < IMAGE_HEIGHT && i >= 0 && i < IMAGE_WIDTH)
                thresh->setThresholded(newy,i, static_cast<unsigned char>(c));
        }
    }
    else if (slope == 0) {
        int startX = min(x, x1);
        int endX = max(x, x1);
        for (int i = startX; i <= endX; i++) {
            if (y >= 0 && y < IMAGE_HEIGHT && i >= 0 && i < IMAGE_WIDTH) {
                thresh->setThresholded(y,i, static_cast<unsigned char>(c));
            }
        }
    }
/*
    // This code is supposed to, I guess, make the line thicker since the Nao's
    // image was 640x480 at some point and you couldn't see the drawing
    // but it's not done well. if you look closely it looks like two blue
    // lines are drawn next to each other but it's clearly not one solid line
#if ROBOT(NAO)
    y--;
    y1--;
    x--;
    x1--;
    if ((abs(y - y1)) > (abs(x - x1))) {
        slope = 1.0 / slope;
        if (y > y1) sign = -1;
        for (int i = y; i != y1; i += sign) {
            int newx = x + (int)(slope * (i - y));
            if (newx >= 0 && newx < IMAGE_WIDTH && i >= 0 && i < IMAGE_HEIGHT)
                thresh->thresholded[i][newx] = c;
        }
    } else if (slope != 0) {
        //slope = 1.0 / slope;
        if (x > x1) sign = -1;
        for (int i = x; i != x1; i += sign) {
            int newy = y + (int)(slope * (i - x));
            if (newy >= 0 && newy < IMAGE_HEIGHT && i >= 0 && i < IMAGE_WIDTH)
                thresh->thresholded[newy][i] = c;
        }
    }
    else if (slope == 0) {
        int startX = min(x, x1);
        int endX = max(x, x1);
        for (int i = startX; i <= endX; i++) {
            if (y >= 0 && y < IMAGE_HEIGHT && i >= 0 && i < IMAGE_WIDTH) {
                thresh->thresholded[y][i] = c;
            }
        }
    }
#endif
*/
}

// Convenience method to draw a VisualLine to the screen.
void Vision::drawLine(boost::shared_ptr<VisualLine> line, const int color) {
    drawLine(line->getStartpoint().x, line->getStartpoint().y, line->getEndpoint().x, line->getEndpoint().y, color);
}

/* drawVerticalLine()
   --helper method to visualize a vertical line on the screen given a x-value
   --given a color as well
*/
void Vision::drawVerticalLine(int x, int c) {
    if (x >= 0 && x < IMAGE_WIDTH) {
        for (int i = 0; i < IMAGE_HEIGHT; i++) {
            thresh->setThresholded(i,x, static_cast<unsigned char>(c));
        }
    }
}

/* drawHorizontalLine()
   --helper method to visualize a vertical line on the screen given a x-value
   --given a color as well
*/
void Vision::drawHorizontalLine(int y, int c) {
    if (y >= 0 && y < IMAGE_HEIGHT) {
        for (int i = 0; i < IMAGE_WIDTH; i++) {
            thresh->setThresholded(y,i, static_cast<unsigned char>(c));
            if (y + 1 < IMAGE_HEIGHT - 1) {
                thresh->setThresholded(y+1,i, static_cast<unsigned char>(c));
            }
        }
    }
}

// Draws gigantic lines across the screen centered on one x,y coordinate
void Vision::drawCrossHairs(int x, int y, int c) {
    drawHorizontalLine(y,c);
    drawVerticalLine(x,c);
}


/* drawVerticalLine()
   --helper method to visualize a single point on the screen given a x-value
   --given a color as well
   --use thresh->drawPoint() if you really want to see the point well.
*/
void Vision::drawDot(int x, int y, int c) {
    if (y > 0 && x > 0 && y < (IMAGE_HEIGHT) && x < (IMAGE_WIDTH)) {
        thresh->setThresholded(y,x, static_cast<unsigned char>(c));
    }
}

void Vision::drawFieldLines() {
    const vector< shared_ptr<VisualLine> >* lines = fieldLines->getLines();

    for (vector< shared_ptr<VisualLine> >::const_iterator i = lines->begin();
         i != lines->end(); i++) {
        drawLine(*i, BLUE);

        // Draw all the line points in the line
		const vector<linePoint> points = (*i)->getPoints();
        for (vector<linePoint>::const_iterator j = points.begin();
             j != points.end(); j++) {
            // Vertically found = black
            if (j->foundWithScan == VERTICAL) {
                thresh->drawPoint(j->x, j->y, BLACK);
            }
            // Horizontally found = red
            else {
                thresh->drawPoint(j->x, j->y, RED);
            }
        }
    }

    const list <linePoint>* unusedPoints = fieldLines->getUnusedPoints();
    for (list <linePoint>::const_iterator i = unusedPoints->begin();
         i != unusedPoints->end(); i++) {
        // Unused vertical = PINK
        if (i->foundWithScan == VERTICAL) {
            thresh->drawPoint(i->x, i->y, PINK);
        }
        // Unused horizontal = Yellow
        else {
            thresh->drawPoint(i->x, i->y, YELLOW);
        }
    }

    const list <VisualCorner>* corners = fieldLines->getCorners();
    for (list <VisualCorner>::const_iterator i = corners->begin();
         i != corners->end(); i++) {
        thresh->drawPoint(i->getX(), i->getY(), ORANGE);
    }
}

void Vision::drawEdges(Gradient& g)
{
#ifdef OFFLINE
    if (thresh->debugEdgeDetection){
        for (int i=0; g.isPeak(i); ++i) {
            drawDot(g.getAnglesXCoord(i) + IMAGE_WIDTH/2,
                    g.getAnglesYCoord(i) + IMAGE_HEIGHT/2,
                    PINK);
        }
    }
#endif
}

void Vision::drawHoughLines(const list<HoughLine>& lines)
{
#ifdef OFFLINE
    if (thresh->debugHoughTransform){

        list<HoughLine>::const_iterator line;
        for (line = lines.begin() ; line != lines.end(); line++){
            const double sn = line->getSinT();
            const double cs = line->getCosT();

            double uStart = 0, uEnd = 0;
            HoughLine::findLineImageIntersects(*line, uStart, uEnd);

            const double x0 = line->getRadius() * cs + IMAGE_WIDTH/2;
            const double y0 = line->getRadius() * sn + IMAGE_HEIGHT/2;

            for (double u = uStart; u <= uEnd; u+=1.){
                int x = (int)round(x0 + u * sn);
                int y = (int)round(y0 - u * cs); // cs goes opposite direction
                drawDot(x,y, BLUE);
            }
        }
    }
#endif
}

void Vision::drawVisualLines(const vector<VisualLine>& lines)
{
    if (thresh->debugVisualLines){
        vector<VisualLine>::const_iterator line;
        for (line = lines.begin(); line != lines.end(); line++){
            thresh->drawLine(line->tr, line->tl, MAROON);
            thresh->drawLine(line->br, line->bl, MAROON);
        }
    }
}
