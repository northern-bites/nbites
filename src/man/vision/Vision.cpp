
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
Vision::Vision(shared_ptr<NaoPose> _pose)
    : pose(_pose),
      yImg(&global_16_image[0]), linesDetector(),
      frameNumber(0), colorTable("table.mtb")
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
	red3 = new VisualRobot();
    navy1 = new VisualRobot();
    navy2 = new VisualRobot();
	navy3 = new VisualRobot();
	cross = new VisualCross();
	fieldEdge = new VisualFieldEdge();

    thresh = new Threshold(this, pose);
    fieldLines = shared_ptr<FieldLines>(new FieldLines(this, pose));
    thresh->setYUV(&global_16_image[0]);
}

// Vision Class Deconstructor
Vision::~Vision()
{
    cout << "Vision destructor" << endl;
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

    // NORMAL VISION LOOP
    frameNumber++;
    // counts the frameNumber
    if (frameNumber > 1000000) frameNumber = 0;

    // Transform joints into pose estimations and horizon line
    PROF_ENTER(P_TRANSFORM);
    pose->transform();
    PROF_EXIT(P_TRANSFORM);

    // Perform image correction, thresholding, and object recognition
    thresh->visionLoop();

    // linesDetector.detect(yImg);
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
#ifdef OFFLINE

    // draw field objects
    if(bglp->getDistance() > 0) drawFieldObject(bglp,RED);
    if(bgrp->getDistance() > 0) drawFieldObject(bgrp,BLACK);
    if(yglp->getDistance() > 0) drawFieldObject(yglp,RED);
    if(ygrp->getDistance() > 0) drawFieldObject(ygrp,BLACK);
    if(ygCrossbar->getHeight() > 0) drawCrossbar(ygCrossbar, BLUE);
    if(bgCrossbar->getHeight() > 0) drawCrossbar(bgCrossbar, YELLOW);

    // balls
    // orange
    /*if(ball->getWidth() > 0)
        drawRect(ball->getX(), ball->getY(),
                 NBMath::ROUND(ball->getWidth()),
                 NBMath::ROUND(ball->getHeight()), PINK);*/

    // lines
    drawFieldLines();

    // pose horizon line
    drawLine(pose->getLeftHorizon().x,
             pose->getLeftHorizon().y,
             pose->getRightHorizon().x,
             pose->getRightHorizon().y,
             MAROON);

    // vision horizon line
    drawPoint(IMAGE_WIDTH/2, thresh->getVisionHorizon(), RED);
#endif /* OFFLINE */

} // drawBoxes

// self-explanatory
void Vision::drawFieldObject(VisualFieldObject* obj, int color)
{
#ifdef OFFLINE
    drawLine(obj->getLeftTopX(), obj->getLeftTopY(),
             obj->getRightTopX(), obj->getRightTopY(), color);
    drawLine(obj->getLeftTopX(), obj->getLeftTopY(),
             obj->getLeftBottomX(), obj->getLeftBottomY(), color);
    drawLine(obj->getRightBottomX(), obj->getRightBottomY(),
             obj->getRightTopX(), obj->getRightTopY(), color);
    drawLine(obj->getLeftBottomX(), obj->getLeftBottomY(),
             obj->getRightBottomX(), obj->getRightBottomY(), color);
#endif /* OFFLINE */
}

// self-explanatory
void Vision::drawCrossbar(VisualCrossbar* obj, int color)
{
#ifdef OFFLINE
    drawLine(obj->getLeftTopX(), obj->getLeftTopY(),
             obj->getRightTopX(), obj->getRightTopY(), color);
    drawLine(obj->getLeftTopX(), obj->getLeftTopY(),
             obj->getLeftBottomX(), obj->getLeftBottomY(), color);
    drawLine(obj->getRightBottomX(), obj->getRightBottomY(),
             obj->getRightTopX(), obj->getRightTopY(), color);
    drawLine(obj->getLeftBottomX(), obj->getLeftBottomY(),
             obj->getRightBottomX(), obj->getRightBottomY(), color);
#endif /* OFFLINE */
}

/**
 * Draws a rectangle to the debugging image in the Threshold class.
 *
 * @param left    x value of the left vertices.
 * @param right   x value of the right vertices.
 * @param bottom  y value of the bottom vertices.
 * @param top     y value of the top vertices.
 * @param c       the color of the box.
 */
void Vision::drawBox(int left, int right, int bottom, int top, int c)
{
#ifdef OFFLINE
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
            thresh->debugImage[top][i] = static_cast<unsigned char>(c);
        }
        if ((top + height) >= 0 &&
                (top + height) < IMAGE_HEIGHT &&
                i >= 0 &&
                i < IMAGE_WIDTH) {
            thresh->debugImage[top + height][i] = static_cast<unsigned char>(c);
        }
    }
    for (int i = top; i < top + height; i++) {
        if (i >= 0 &&
                i < IMAGE_HEIGHT &&
                left >= 0 &&
                left < IMAGE_WIDTH) {
            thresh->debugImage[i][left] = static_cast<unsigned char>(c);
        }
        if (i >= 0 &&
                i < IMAGE_HEIGHT &&
                (left+width) >= 0 &&
                (left+width) < IMAGE_WIDTH) {
            thresh->debugImage[i][left + width] = static_cast<unsigned char>(c);
        }
    }
#endif /* OFFLINE */
}


/* drawCenters()
   --draws little crosshairs at the center x,y of identified objects.
   includes checks to make sure the crosshairs don't draw off the screen
   (index array out of bounds error producers)
   --draws:
   -ball
   -goals
   -add more if you'd like
*/
void Vision::drawCenters()
{
#ifdef OFFLINE

    // draws an X at the ball center X and Y.
    if (ball->getCenterX() >= 2 && ball->getCenterY() >= 2
        && ball->getCenterX() <= (IMAGE_WIDTH-2)
        && ball->getCenterY() <= (IMAGE_HEIGHT-2)) {
        drawPoint(ball->getCenterX(), ball->getCenterY(), YELLOW);
    }
#endif /* OFFLINE */
} // drawCenters

/**
 * Draw a rectangle in the debugging image.
 *
 * @param left     x value of left edge
 * @param right    x value of right edge
 * @param bottom   y value of bottom
 * @param top      y value of top
 * @param c        the color we'd like to draw
 */
void Vision::drawRect(int left, int top, int width, int height, int c)
{
#ifdef OFFLINE
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
            thresh->debugImage[top][i] = static_cast<unsigned char>(c);
        }
        if ((top + height) >= 0 &&
                (top + height) < IMAGE_HEIGHT &&
                i >= 0 &&
                i < IMAGE_WIDTH) {
            thresh->debugImage[top + height][i] = static_cast<unsigned char>(c);
        }
    }
    for (int i = top; i < top + height; i++) {
        if (i >= 0 &&
                i < IMAGE_HEIGHT &&
                left >= 0 &&
                left < IMAGE_WIDTH) {
            thresh->debugImage[i][left] = static_cast<unsigned char>(c);
        }
        if (i >= 0 &&
                i < IMAGE_HEIGHT &&
                (left+width) >= 0 &&
                (left+width) < IMAGE_WIDTH) {
            thresh->debugImage[i][left + width] = static_cast<unsigned char>(c);
        }
    }
#endif
}

/**
 * Draw a line at the specified coordinates on the debugging
 * image.
 *
 * @param x       start x
 * @param y       start y
 * @param x1      finish x
 * @param y1      finish y
 * @param c       color we'd like to draw
 */
void Vision::drawLine(int x, int y, int x1, int y1, int c)
{
#ifdef OFFLINE
    float slope = static_cast<float>(y - y1) / static_cast<float>(x - x1);
    int sign = 1;
    if ((abs(y - y1)) > (abs(x - x1))) {
        slope = 1.0f / slope;
        if (y > y1)  {
            sign = -1;
        }
        for (int i = y; i != y1; i += sign) {
            int newx = x +
                    static_cast<int>(slope * static_cast<float>(i - y) );

            if (newx >= 0 && newx < IMAGE_WIDTH && i >= 0 && i < IMAGE_HEIGHT) {
                thresh->debugImage[i][newx] = static_cast<unsigned char>(c);
            }
        }
    } else if (slope != 0) {
        //slope = 1.0 / slope;
        if (x > x1) {
            sign = -1;
        }
        for (int i = x; i != x1; i += sign) {
            int newy = y +
                    static_cast<int>(slope * static_cast<float>(i - x) );

            if (newy >= 0 && newy < IMAGE_HEIGHT && i >= 0 && i < IMAGE_WIDTH) {
                thresh->debugImage[newy][i] = static_cast<unsigned char>(c);
            }
        }
    }
    else if (slope == 0) {
        int startX = min(x, x1);
        int endX = max(x, x1);
        for (int i = startX; i <= endX; i++) {
            if (y >= 0 && y < IMAGE_HEIGHT && i >= 0 && i < IMAGE_WIDTH) {
                thresh->debugImage[y][i] = static_cast<unsigned char>(c);
            }
        }
    }
#endif /* OFFLINE */
}

// Convenience method to draw a VisualLine to the screen.
void Vision::drawLine(boost::shared_ptr<VisualLine> line, const int color)
{
#ifdef OFFLINE
    drawLine(line->getStartpoint().x, line->getStartpoint().y,
             line->getEndpoint().x, line->getEndpoint().y, color);
#endif /* OFFLINE */
}

void Vision::drawLine(const point<int> start,
                      const point<int> end,
                      const int c)
{
    drawLine(start.x, start.y, end.x, end.y, c);
}

void Vision::drawDot(int x, int y, int c)
{
#ifdef OFFLINE
    if (y > 0 && x > 0 && y < (IMAGE_HEIGHT) && x < (IMAGE_WIDTH)) {
        thresh->debugImage[y][x] = static_cast<unsigned char>(c);
    }
#endif /* OFFLINE */
}

void Vision::drawFieldLines()
{
#ifdef OFFLINE
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
                drawPoint(j->x, j->y, BLACK);
            }
            // Horizontally found = red
            else {
                drawPoint(j->x, j->y, RED);
            }
        }
    }

    const list <linePoint>* unusedPoints = fieldLines->getUnusedPoints();
    for (list <linePoint>::const_iterator i = unusedPoints->begin();
         i != unusedPoints->end(); i++) {
        // Unused vertical = PINK
        if (i->foundWithScan == VERTICAL) {
            drawPoint(i->x, i->y, PINK);
        }
        // Unused horizontal = Yellow
        else {
            drawPoint(i->x, i->y, YELLOW);
        }
    }

    const list <VisualCorner>* corners = fieldLines->getCorners();
    for (list <VisualCorner>::const_iterator i = corners->begin();
         i != corners->end(); i++) {
        drawPoint(i->getX(), i->getY(), ORANGE);
    }
#endif /* OFFLINE */
}

// Prerequisite - point is within bounds of screen
void Vision::drawX(int x, int y, int c)
{

#ifdef OFFLINE
    // Mid point
    thresh->debugImage[y-2][x-2] = static_cast<unsigned char>(c);
    thresh->debugImage[y-1][x-1] = static_cast<unsigned char>(c);
    thresh->debugImage[y][x] = static_cast<unsigned char>(c);
    thresh->debugImage[y+1][x+1] = static_cast<unsigned char>(c);
    thresh->debugImage[y+2][x+2] = static_cast<unsigned char>(c);

    thresh->debugImage[y-2][x+2] = static_cast<unsigned char>(c);
    thresh->debugImage[y-1][x+1] = static_cast<unsigned char>(c);

    thresh->debugImage[y+1][x-1] = static_cast<unsigned char>(c);
    thresh->debugImage[y+2][x-2] = static_cast<unsigned char>(c);
#endif

}

/* drawPoint()
 * Draws a crosshair or a 'point' on the fake image at some given x, y, and with
 * a given color.
 * @param x       center of the point
 * @param y       center y value
 * @param c       color to draw
 */
void Vision::drawPoint(int x, int y, int c)
{
#ifdef OFFLINE
    if (y > 0 && x > 0 && y < (IMAGE_HEIGHT) && x < (IMAGE_WIDTH)) {
        thresh->debugImage[y][x] = static_cast<unsigned char>(c);
    }if (y+1 > 0 && x > 0 && y+1 < (IMAGE_HEIGHT) && x < (IMAGE_WIDTH)) {
        thresh->debugImage[y+1][x] = static_cast<unsigned char>(c);
    }if (y+2 > 0 && x > 0 && y+2 < (IMAGE_HEIGHT) && x < (IMAGE_WIDTH)) {
        thresh->debugImage[y+2][x] = static_cast<unsigned char>(c);
    }if (y-1 > 0 && x > 0 && y-1 < (IMAGE_HEIGHT) && x < (IMAGE_WIDTH)) {
        thresh->debugImage[y-1][x] = static_cast<unsigned char>(c);
    }if (y-2 > 0 && x > 0 && y-2 < (IMAGE_HEIGHT) && x < (IMAGE_WIDTH)) {
        thresh->debugImage[y-2][x] = static_cast<unsigned char>(c);
    }if (y > 0 && x+1 > 0 && y < (IMAGE_HEIGHT) && x+1 < (IMAGE_WIDTH)) {
        thresh->debugImage[y][x+1] = static_cast<unsigned char>(c);
    }if (y > 0 && x+2 > 0 && y < (IMAGE_HEIGHT) && x+2 < (IMAGE_WIDTH)) {
        thresh->debugImage[y][x+2] = static_cast<unsigned char>(c);
    }if (y > 0 && x-1 > 0 && y < (IMAGE_HEIGHT) && x-1 < (IMAGE_WIDTH)) {
        thresh->debugImage[y][x-1] = static_cast<unsigned char>(c);
    }if (y > 0 && x-2 > 0 && y < (IMAGE_HEIGHT) && x-2 < (IMAGE_WIDTH)) {
        thresh->debugImage[y][x-2] = static_cast<unsigned char>(c);
    }
#endif
}
