
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
#include "FieldLines/CornerDetector.h"
#include "FieldLines/FieldLinesDetector.h"

using namespace std;
using namespace ::messages;
using boost::shared_ptr;

static uint8_t global_8_image[IMAGE_BYTE_SIZE];
static uint16_t global_16_image[IMAGE_BYTE_SIZE];

// Vision Class Constructor
Vision::Vision()
    : yImg(&global_16_image[0]),
      linesDetector(new FieldLinesDetector()),
      cornerDetector(new CornerDetector()),
      frameNumber(0), colorTable("table.mtb")
{
    // variable initialization
	std::cout << "constructing visionModule\n";
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
    obstacles = new VisualObstacle();
	
	pose = boost::shared_ptr<NaoPose>(new NaoPose());
    thresh = new Threshold(this, pose);
    fieldLines = boost::shared_ptr<FieldLines>(new FieldLines(this, pose));
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

// void Vision::notifyImage(const uint16_t* y) {
//     yImg = y;
//     uImg = y + AVERAGED_IMAGE_SIZE;
//     vImg = uImg + AVERAGED_IMAGE_SIZE;

//     // Set the current image pointer in Threshold
//     thresh->setYUV(y);
//     notifyImage();
// }

// void Vision::notifyImage(const uint16_t* y_top, const uint16_t* y_bot) {
//     yImg = y_top;
//     uImg = y_top + AVERAGED_IMAGE_SIZE;
//     vImg = uImg + AVERAGED_IMAGE_SIZE;

//     yImg_bot = y_bot;
//     uImg_bot = y_bot + AVERAGED_IMAGE_SIZE;
//     vImg_bot = uImg + AVERAGED_IMAGE_SIZE;

//     // Set the current image pointer in Threshold
//     thresh->setYUV(y_top);
//     thresh->setYUV_bot(y_bot);
//     notifyImage();
// }

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
 *-----------The above is just not true-----------
 *-----------(except for the part about it being important)-------
 *
 */

void Vision::notifyImage(const uint16_t* y_top, const uint16_t* y_bot,
			 const JointAngles& ja, const InertialState& inert) {
	

    yImg = y_top;
    uImg = y_top + AVERAGED_IMAGE_SIZE;
    vImg = uImg + AVERAGED_IMAGE_SIZE;

    yImg_bot = y_bot;
    uImg_bot = y_bot + AVERAGED_IMAGE_SIZE;
    vImg_bot = uImg + AVERAGED_IMAGE_SIZE;

    // Set the current image pointer in Threshold
    thresh->setYUV(y_top);
    thresh->setYUV_bot(y_bot);


    // NORMAL VISION LOOP
    frameNumber++;
    // counts the frameNumber
    if (frameNumber > 1000000) frameNumber = 0;

//    linesDetector->detect(thresh->getVisionHorizon(),
//                         thresh->field->getTopEdge(),
//                         yImg);
//
//    cornerDetector->detect(thresh->getVisionHorizon(),
//                           thresh->field->getTopEdge(),
//                           linesDetector->getLines());

    // Perform image correction, thresholding, and object recognition

    thresh->visionLoop(ja, inert);
    thresh->obstacleLoop(ja, inert);

//    drawEdges(*linesDetector->getEdges());
//    drawHoughLines(linesDetector->getHoughLines());
//    drawVisualLines(linesDetector->getLines());
//    drawVisualCorners(cornerDetector->getCorners());

    thresh->transposeDebugImage();

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


// /**
//  * Vision memory update
//  */

// using namespace proto;

// //helpers
// void update(PVision::PVisualDetection* visual_detection,
//             VisualDetection* visualDetection) {

//     visual_detection->set_intopcam(visualDetection->isTopCam());
//     visual_detection->set_distance(visualDetection->getDistance());
//     visual_detection->set_center_x(visualDetection->getCenterX());
//     visual_detection->set_center_y(visualDetection->getCenterY());
//     visual_detection->set_x(visualDetection->getX());
//     visual_detection->set_y(visualDetection->getY());
//     visual_detection->set_angle_x(visualDetection->getAngleX());
//     visual_detection->set_angle_y(visualDetection->getAngleY());
//     visual_detection->set_bearing(visualDetection->getBearing());
//     visual_detection->set_elevation(visualDetection->getElevation());
//     visual_detection->set_distance_sd(visualDetection->getDistanceSD());
//     visual_detection->set_bearing_sd(visualDetection->getBearingSD());
// }

// void update(PVision::PVisualLandmark* visual_landmark,
//             VisualLandmark* visualLandmark) {

//     visual_landmark->set_id(visualLandmark->getID());
//     visual_landmark->set_id_certainty(visualLandmark->getIDCertainty());
//     visual_landmark->set_distance_certainty(visualLandmark->getDistanceCertainty());
// }

// void update(PVision::PVisualFieldObject* visual_field_object,
//             VisualFieldObject* visualFieldObject) {

//     PVision::PVisualDetection* visual_detection = visual_field_object->mutable_visual_detection();
//     PVision::PVisualLandmark* visual_landmark = visual_field_object->mutable_visual_landmark();
//     update(visual_detection, visualFieldObject);
//     update(visual_landmark, visualFieldObject);
//     visual_field_object->set_left_top_x(visualFieldObject->getLeftTopX());
//     visual_field_object->set_left_top_y(visualFieldObject->getLeftTopY());
//     visual_field_object->set_left_bottom_x(visualFieldObject->getLeftBottomX());
//     visual_field_object->set_left_bottom_y(visualFieldObject->getLeftBottomY());
//     visual_field_object->set_right_top_x(visualFieldObject->getRightTopX());
//     visual_field_object->set_right_top_y(visualFieldObject->getRightTopY());
//     visual_field_object->set_right_bottom_x(visualFieldObject->getRightBottomX());
//     visual_field_object->set_right_bottom_y(visualFieldObject->getRightBottomY());
// }

// void update(PVision::PVisualRobot* visual_robot,
//             VisualRobot* visualRobot) {

//     PVision::PVisualDetection* visual_detection = visual_robot->mutable_visual_detection();
//     update(visual_detection, visualRobot);

//     visual_robot->set_left_top_x(visualRobot->getLeftTopX());
//     visual_robot->set_left_top_y(visualRobot->getLeftTopY());
//     visual_robot->set_left_bottom_x(visualRobot->getLeftBottomX());
//     visual_robot->set_left_bottom_y(visualRobot->getLeftBottomY());
//     visual_robot->set_right_top_x(visualRobot->getRightTopX());
//     visual_robot->set_right_top_y(visualRobot->getRightTopY());
//     visual_robot->set_right_bottom_x(visualRobot->getRightBottomX());
//     visual_robot->set_right_bottom_y(visualRobot->getRightBottomY());
// }

// void update(PVision::PVisualLine* visual_line,
//             boost::shared_ptr<VisualLine> visualLine) {

//     PVision::PVisualLandmark* visual_landmark = visual_line->mutable_visual_landmark();
//     update(visual_landmark, visualLine.get());

//     visual_line->set_start_x(visualLine->getStartpoint().x);
//     visual_line->set_start_y(visualLine->getStartpoint().y);
//     visual_line->set_end_x(visualLine->getEndpoint().x);
//     visual_line->set_end_y(visualLine->getEndpoint().y);
// }

// void update(proto::PVision::PVisualCross* visual_cross,
//         VisualCross* visualCross) {

//     PVision::PVisualDetection* visual_detection= visual_cross->mutable_visual_detection();
//     update(visual_detection, visualCross);

//     PVision::PVisualLandmark* visual_landmark= visual_cross->mutable_visual_landmark();
//     update(visual_landmark, visualCross);

//     visual_cross->set_left_top_x(visualCross->getLeftTopX());
//     visual_cross->set_left_top_y(visualCross->getLeftTopY());
//     visual_cross->set_left_bottom_x(visualCross->getLeftBottomX());
//     visual_cross->set_left_bottom_y(visualCross->getLeftBottomY());
//     visual_cross->set_right_top_x(visualCross->getRightTopX());
//     visual_cross->set_right_top_y(visualCross->getRightTopY());
//     visual_cross->set_right_bottom_x(visualCross->getRightBottomX());
//     visual_cross->set_right_bottom_y(visualCross->getRightBottomY());
// }

// //main update method
// void Vision::updateMVision(man::memory::MVision::ptr mVision) const {

//     //VisualBall
//     PVision::PVisualBall* visual_ball;
//     visual_ball = mVision->get()->mutable_visual_ball();

//     //VisualBall::VisualDetection
//     PVision::PVisualDetection* visual_detection;
//     visual_detection = visual_ball->mutable_visual_detection();
//     update(visual_detection, this->ball);

//     //VisualBall::stuff
//     visual_ball->set_radius(this->ball->getRadius());
//     visual_ball->set_confidence(this->ball->getConfidence());

//     PVision::PVisualFieldObject* yglp;
//     yglp = mVision->get()->mutable_yglp();
//     update(yglp, this->yglp);

//     PVision::PVisualFieldObject* ygrp;
//     ygrp = mVision->get()->mutable_ygrp();
//     update(ygrp, this->ygrp);

//     //VisualRobot
//     PVision::PVisualRobot* red1;
//     red1=mVision->get()->mutable_red1();
//     update(red1, this->red1);

//     PVision::PVisualRobot* red2;
//     red2=mVision->get()->mutable_red2();
//     update(red2, this->red2);

//     PVision::PVisualRobot* red3;
//     red3=mVision->get()->mutable_red3();
//     update(red3, this->red3);

//     PVision::PVisualRobot* navy1;
//     navy1=mVision->get()->mutable_navy1();
//     update(navy1, this->navy1);

//     PVision::PVisualRobot* navy2;
//     navy2=mVision->get()->mutable_navy2();
//     update(navy2, this->navy2);

//     PVision::PVisualRobot* navy3;
//     navy3=mVision->get()->mutable_navy3();
//     update(navy3, this->navy3);

//     PVision::PVisualCross* visual_cross;
//     visual_cross=mVision->get()->mutable_visual_cross();
//     update(visual_cross, this->cross);

//     //VisualLines
//     mVision->get()->clear_visual_line();
//     const vector<boost::shared_ptr<VisualLine> >* visualLines = this->fieldLines->getLines();
//     for(vector<boost::shared_ptr<VisualLine> >::const_iterator i = visualLines->begin();
//             i != visualLines->end(); i++){

//         PVision::PVisualLine* visual_line = mVision->get()->add_visual_line();
//         update(visual_line, (*i));
//     }


//     //VisualCorners
//     mVision->get()->clear_visual_corner();
//     list<VisualCorner>* visualCorners = this->fieldLines->getCorners();
//     for (list<VisualCorner>::iterator i = visualCorners->begin(); i
//     != visualCorners->end(); i++) {
//         //VisualCorner
//         PVision::PVisualCorner* visual_corner =
//                 mVision->get()->add_visual_corner();

//         //VisualCorner::VisualDetection
//         visual_detection = visual_corner->mutable_visual_detection();
//         update(visual_detection, &(*i));

//         //VisualCorner::VisualLandmark
//         PVision::PVisualLandmark* visual_landmark =
//                 visual_corner->mutable_visual_landmark();
//         update(visual_landmark, &(*i));


//         //VisualCorner::stuff
//         visual_corner->set_corner_type(i->getShape());
//         visual_corner->set_secondary_shape(i->getSecondaryShape());
//         visual_corner->set_angle_between_lines(i->getAngleBetweenLines());
//         visual_corner->set_orientation(i->getOrientation());
//         visual_corner->set_physical_orientation(i->getPhysicalOrientation());
//     }
// }

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
    const vector< boost::shared_ptr<VisualLine> >* lines = fieldLines->getLines();

    for (vector< boost::shared_ptr<VisualLine> >::const_iterator i = lines->begin();
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
                drawHoughLine(*line, MAROON);
            }
        }
#endif
    }

void Vision::drawHoughLine(const HoughLine& line, int color)
{
#ifdef OFFLINE
    const double sn = line.getSinT();
    const double cs = line.getCosT();

    double uStart = 0, uEnd = 0;
    HoughLine::findLineImageIntersects(line, uStart, uEnd);

    const double x0 = line.getRadius() * cs + IMAGE_WIDTH/2;
    const double y0 = line.getRadius() * sn + IMAGE_HEIGHT/2;

    for (double u = uStart; u <= uEnd; u+=1.){
        int x = (int)round(x0 + u * sn);
        int y = (int)round(y0 - u * cs); // cs goes opposite direction
        drawDot(x,y, color);
    }
#endif
}

void Vision::drawVisualLines(const vector<HoughVisualLine>& lines)
{
#ifdef OFFLINE
    if (thresh->debugVisualLines){
        vector<HoughVisualLine>::const_iterator line;
        int color = 5;
        for (line = lines.begin(); line != lines.end(); line++){
            pair<HoughLine, HoughLine> lp = line->getHoughLines();
            drawHoughLine(lp.first, color);
            drawHoughLine(lp.second, color);
            color++;
        }
    }
#endif
}

void Vision::drawVisualCorners(const vector<HoughVisualCorner>& corners)
{
#ifdef OFFLINE
    if (thresh->debugVisualCorners) {
        for (vector<HoughVisualCorner>::const_iterator i = corners.begin();
             i != corners.end(); ++i) {
            point<int> pt = i->getImageLocation();
            drawX(pt.x, pt.y, SEA_GREEN);
        }
    }
#endif
}
