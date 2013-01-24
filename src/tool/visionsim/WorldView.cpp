/**
 * WorldView.
 */

#include "WorldView.h"
#include <iostream>

///// Constants for Drawing /////

// pixel/cm conversion ratios
static const float WIDTH_RATIO = 1.0f;
static const float HEIGHT_RATIO = WIDTH_RATIO;

static const float WIDTH = GREEN_WIDTH*WIDTH_RATIO;
static const float HEIGHT = GREEN_HEIGHT*HEIGHT_RATIO;

static const int LINE_WIDTH = 5;

static const int GOALPOST_RAD = 0.5f*GOALPOST_WIDTH;
static const int GOALPOST_PEN_WIDTH = 3;

static const float CROSS_ARM = 0.5f*CROSS_WIDTH;

static const int BALL_RAD = 5;

static const int ROBOT_X_SIZE = 20;
static const int ROBOT_Y_SIZE = 10;

static const int ARROW_START = -1;
static const int ARROW_WIDTH = 2;
static const int ARROW_DEPTH = 3;

WorldView::WorldView(World& state) : world(state)
{
}

QSize WorldView::minimumSizeHint() const
{
    return QSize(WIDTH, HEIGHT);
}

QSize WorldView::sizeHint() const
{
    return QSize(WIDTH, HEIGHT);
}

void WorldView::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    // Green background
    painter.fillRect(0, 0, WIDTH, HEIGHT, Qt::darkGreen);

    // Flip coordinate system to normal one
    painter.translate(0, HEIGHT);
    painter.scale(WIDTH_RATIO, -HEIGHT_RATIO);

    // White lines
    QPen pen(Qt::white);
    pen.setWidth(LINE_WIDTH);
    pen.setJoinStyle(Qt::MiterJoin);

    painter.setPen(pen);

    // Field
    painter.drawRect(FIELD_LEFT_EDGE_X, FIELD_BOTTOM_Y,
                     FIELD_WIDTH, FIELD_HEIGHT);

    // Goal boxes
    painter.drawRect(FIELD_LEFT_EDGE_X, GOALBOX_BOTTOM_Y,
                     GOALBOX_WIDTH, GOALBOX_HEIGHT);

    painter.drawRect(FIELD_RIGHT_EDGE_X-GOALBOX_WIDTH, GOALBOX_BOTTOM_Y,
                     GOALBOX_WIDTH, GOALBOX_HEIGHT);

    painter.drawLine(HALF_FIELD_X, FIELD_BOTTOM_Y,
                     HALF_FIELD_X, FIELD_TOP_Y);

    // Center circle
    painter.drawEllipse(HALF_FIELD_X-CENTER_CIRCLE_RAD,
                        CROSS_Y-CENTER_CIRCLE_RAD,
                        CENTER_CIRCLE_DIAM,
                        CENTER_CIRCLE_DIAM);

    // // Cross
    // painter.drawLine(convertCoords(CENTER_GOAL_X-CROSS_ARM, CROSS_Y),
    //                  convertCoords(CENTER_GOAL_X+CROSS_ARM, CROSS_Y));
    // painter.drawLine(convertCoords(CENTER_GOAL_X, CROSS_Y-CROSS_ARM),
    //                  convertCoords(CENTER_GOAL_X, CROSS_Y+CROSS_ARM));

    // Goalposts
    pen.setColor(Qt::yellow);
    pen.setWidth(GOALPOST_PEN_WIDTH);
    painter.setPen(pen);
    painter.setBrush(Qt::yellow);

    painter.drawEllipse(FIELD_LEFT_EDGE_X-GOALPOST_RAD,
                        BOTTOM_GOALPOST_Y-GOALPOST_RAD,
                        GOALPOST_WIDTH, GOALPOST_WIDTH);
    painter.drawEllipse(FIELD_LEFT_EDGE_X-GOALPOST_RAD,
                        TOP_GOALPOST_Y-GOALPOST_RAD,
                        GOALPOST_WIDTH, GOALPOST_WIDTH);

    painter.drawEllipse(FIELD_RIGHT_EDGE_X-GOALPOST_RAD,
                        BOTTOM_GOALPOST_Y-GOALPOST_RAD,
                        GOALPOST_WIDTH, GOALPOST_WIDTH);
    painter.drawEllipse(FIELD_RIGHT_EDGE_X-GOALPOST_RAD,
                        TOP_GOALPOST_Y-GOALPOST_RAD,
                        GOALPOST_WIDTH, GOALPOST_WIDTH);

    painter.drawLine(FIELD_LEFT_EDGE_X, BOTTOM_GOALPOST_Y,
                     LEFT_GOAL_BACK_X, BOTTOM_GOALPOST_Y);
    painter.drawLine(FIELD_LEFT_EDGE_X, TOP_GOALPOST_Y,
                     LEFT_GOAL_BACK_X, TOP_GOALPOST_Y);
    painter.drawLine(LEFT_GOAL_BACK_X, BOTTOM_GOALPOST_Y,
                     LEFT_GOAL_BACK_X, TOP_GOALPOST_Y);

    painter.drawLine(FIELD_RIGHT_EDGE_X, BOTTOM_GOALPOST_Y,
                     RIGHT_GOAL_BACK_X, BOTTOM_GOALPOST_Y);
    painter.drawLine(FIELD_RIGHT_EDGE_X, TOP_GOALPOST_Y,
                     RIGHT_GOAL_BACK_X, TOP_GOALPOST_Y);
    painter.drawLine(RIGHT_GOAL_BACK_X, BOTTOM_GOALPOST_Y,
                     RIGHT_GOAL_BACK_X, TOP_GOALPOST_Y);

    // Ball-Data from world instance
    QColor orange(255, 140, 0);
    pen.setColor(orange);
    painter.setPen(pen);
    painter.setBrush(orange);

    painter.drawEllipse(world.ballX()-BALL_RAD, world.ballY()-BALL_RAD,
                        2*BALL_RAD, 2*BALL_RAD);


    // Robot-Data from world instance
    pen.setColor(Qt::darkGray);
    painter.setPen(pen);
    painter.setBrush(Qt::darkGray);

    // To get the right orientation for the goalie ellipse, translates
    // coordinate system to center of the goalie and rotates before
    // drawing
    painter.translate(world.robotX(), world.robotY());
    painter.rotate(-world.robotH());
    painter.drawEllipse(-(0.5*ROBOT_X_SIZE), -(0.5*ROBOT_Y_SIZE),
                        ROBOT_X_SIZE, ROBOT_Y_SIZE);

    // Arrow specifies direction the goalie is facing
    pen.setColor(Qt::darkBlue);
    painter.setPen(pen);
    painter.drawLine(0, ARROW_START, 0, ROBOT_Y_SIZE);
    painter.drawLine(0, ROBOT_Y_SIZE, ARROW_WIDTH,
                     ROBOT_Y_SIZE-ARROW_DEPTH);
    painter.drawLine(0, ROBOT_Y_SIZE, -ARROW_WIDTH,
                     ROBOT_Y_SIZE-ARROW_DEPTH);
}
