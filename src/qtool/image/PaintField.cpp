#include "PaintField.h"
#include <vector>

namespace qtool {
namespace image {

PaintField::PaintField(QObject *parent):
        BMPImage(parent)
{
    bitmap = QPixmap(FIELD_WIDTH, FIELD_HEIGHT);
}

// Paints the field
void PaintField::buildBitmap()
{

    QPainter painter(&bitmap);

    // Field Areas
    QRect field(FIELD_GREEN_LEFT_SIDELINE_X,
            FIELD_GREEN_BOTTOM_SIDELINE_Y,
            FIELD_GREEN_WIDTH, FIELD_GREEN_HEIGHT);
    QRect playArea(FIELD_WHITE_LEFT_SIDELINE_X,
            FIELD_WHITE_BOTTOM_SIDELINE_Y,
            FIELD_WHITE_WIDTH, FIELD_WHITE_HEIGHT);

    // Goal Boxes
    QRect blueGoalBox(FIELD_WHITE_LEFT_SIDELINE_X,
            MIDFIELD_Y -(GOALBOX_WIDTH/2),
            GOALBOX_DEPTH, GOALBOX_WIDTH);
    QRect yellowGoalBox(FIELD_WHITE_RIGHT_SIDELINE_X -
            (GOALBOX_DEPTH),
            MIDFIELD_Y - (GOALBOX_WIDTH/2),
            GOALBOX_DEPTH, GOALBOX_WIDTH);
    // Goals
    QRect blueGoal(LANDMARK_BLUE_GOAL_BOTTOM_POST_X -
            GOAL_DEPTH,
            LANDMARK_BLUE_GOAL_BOTTOM_POST_Y,
            GOAL_DEPTH,
            CROSSBAR_CM_WIDTH);
    QRect yellowGoal(LANDMARK_YELLOW_GOAL_BOTTOM_POST_X,
            LANDMARK_YELLOW_GOAL_BOTTOM_POST_Y,
            GOAL_DEPTH, CROSSBAR_CM_WIDTH);

    QPoint centerField = QPoint(CENTER_FIELD_X, CENTER_FIELD_Y);

    //Posts
    QPoint topBluePost = QPoint(LANDMARK_BLUE_GOAL_TOP_POST_X,
            LANDMARK_BLUE_GOAL_TOP_POST_Y);
    QPoint bottomBluePost = QPoint(LANDMARK_BLUE_GOAL_BOTTOM_POST_X,
            LANDMARK_BLUE_GOAL_BOTTOM_POST_Y);
    QPoint topYellowPost = QPoint(LANDMARK_YELLOW_GOAL_TOP_POST_X,
            LANDMARK_YELLOW_GOAL_TOP_POST_Y);
    QPoint bottomYellowPost = QPoint(LANDMARK_YELLOW_GOAL_BOTTOM_POST_X,
            LANDMARK_YELLOW_GOAL_BOTTOM_POST_Y);

    // Paint Field
    painter.fillRect(field, Qt::darkGreen);

    // Paint Lines
    QPen pen(Qt::white, LINE_WIDTH, Qt::SolidLine,
            Qt::SquareCap, Qt::MiterJoin);
    painter.setPen(pen);
    painter.drawRect(blueGoalBox);
    painter.drawRect(yellowGoalBox);
    painter.drawRect(playArea);

    // Mid-Field Line
    painter.drawLine(((playArea.topLeft()+
            playArea.topRight())/2),
            ((playArea.bottomLeft()+
                    playArea.bottomRight())/2));
    // Center Cirlce
    painter.drawEllipse (centerField,
            (int)CENTER_CIRCLE_RADIUS,
            (int)CENTER_CIRCLE_RADIUS);
    // Field Crosses
    painter.drawLine(LANDMARK_BLUE_GOAL_CROSS_X -
            (LINE_CROSS_LENGTH/2),
            LANDMARK_BLUE_GOAL_CROSS_Y,
            LANDMARK_BLUE_GOAL_CROSS_X +
            (LINE_CROSS_LENGTH/2),
            LANDMARK_BLUE_GOAL_CROSS_Y);
    painter.drawLine(LANDMARK_YELLOW_GOAL_CROSS_X -
            (LINE_CROSS_LENGTH/2),
            LANDMARK_YELLOW_GOAL_CROSS_Y,
            LANDMARK_YELLOW_GOAL_CROSS_X +
            (LINE_CROSS_LENGTH/2),
            LANDMARK_YELLOW_GOAL_CROSS_Y);
    painter.drawLine(LANDMARK_BLUE_GOAL_CROSS_X,
            LANDMARK_BLUE_GOAL_CROSS_Y -
            (LINE_CROSS_LENGTH/2),
            LANDMARK_BLUE_GOAL_CROSS_X ,
            LANDMARK_BLUE_GOAL_CROSS_Y +
            (LINE_CROSS_LENGTH/2));
    painter.drawLine(LANDMARK_YELLOW_GOAL_CROSS_X,
            LANDMARK_YELLOW_GOAL_CROSS_Y-
            (LINE_CROSS_LENGTH/2),
            LANDMARK_YELLOW_GOAL_CROSS_X,
            LANDMARK_YELLOW_GOAL_CROSS_Y +
            (LINE_CROSS_LENGTH/2));

    // Painting the Blue Goal
    painter.setBrush(Qt::yellow);
    painter.setPen(Qt::yellow);
    painter.drawEllipse(topBluePost,
            (int)GOAL_POST_RADIUS,
            (int)GOAL_POST_RADIUS);
    painter.drawEllipse(bottomBluePost,
            (int)GOAL_POST_RADIUS,
            (int)GOAL_POST_RADIUS);

    // Painting Yellow Goal
    painter.setBrush(Qt::yellow);
    painter.setPen(Qt::yellow);
    painter.drawEllipse(topYellowPost,
            (int)GOAL_POST_RADIUS,
            (int)GOAL_POST_RADIUS);
    painter.drawEllipse(bottomYellowPost,
            (int)GOAL_POST_RADIUS,
            (int)GOAL_POST_RADIUS);

    // Nets in Goals
    QBrush goalsBrush = QBrush(Qt::white, Qt::CrossPattern);
    painter.setBrush(goalsBrush);
    painter.setPen(Qt::blue);
    painter.drawRect(blueGoal);
    painter.setPen(Qt::yellow);
    painter.drawRect(yellowGoal);
}

}
}

