#include "SharedViewerPainter.h"

#include <cmath>

namespace tool {
namespace sharer {

SharedViewerPainter::SharedViewerPainter(QWidget* parent, float scaleFactor_) :
    PaintField(parent, scaleFactor_)
{
    // initializes arrays
    for (int i = 0; i < NUM_PLAYERS_PER_TEAM; i++) {
        myXCoords[i] = 0.f;
        myYCoords[i] = 0.f;
        myHeadings[i] = 0.f;
        myBallDist[i] = 0.f;
    }
}

void SharedViewerPainter::paintEvent(QPaintEvent* event)
{
    PaintField::paintEvent(event);
    // Paint each robot with info stated
    for (int i = 0; i < NUM_PLAYERS_PER_TEAM; i++) {
        paintRobot(event, myXCoords[i], myYCoords[i], myHeadings[i],
                   myBallDist[i], QString::number(i+1));
    }
    paintSharedBall(event, sharedBallLoc);
}

void SharedViewerPainter::paintSharedBall(QPaintEvent* event,
                                       messages::SharedBall msg)
{
    if (msg.ball_on()) {
        QPainter painter(this);
        painter.translate(0, FIELD_GREEN_HEIGHT);
        painter.scale(scaleFactor, -scaleFactor);
        QPoint ballCenter(msg.x(), msg.y());

        painter.setBrush(QColor::fromRgb(153,0,153));
        painter.drawEllipse(ballCenter, 8, 8);
    }
    return;
}

void SharedViewerPainter::paintRobot(QPaintEvent* event, float x, float y,
                                     float h, float ballD, QString playerNum)
{
    if (x <= 0.f || y <= 0.f)
    {
        return; // Don't paint robots that aren't there.
    }

    QPainter painter(this);
    painter.translate(0, FIELD_GREEN_HEIGHT*scaleFactor);
    painter.scale(scaleFactor, -scaleFactor);

    Qt::GlobalColor brushColor = Qt::cyan;

    const QPoint locCenter(x, y);

    painter.setBrush(brushColor);
    painter.setPen(brushColor);

    // Draw myself
    painter.drawEllipse(locCenter,
                        ROBOT_WIDTH,
                        ROBOT_WIDTH);

    painter.setPen(Qt::black);
    // Draw my heading
    painter.drawLine(x,
                     y,
                     ROBOT_WIDTH * std::cos(TO_RAD*h) + x,
                     ROBOT_WIDTH * std::sin(TO_RAD*h) + y);

    // Draw my number
    painter.setPen(brushColor);
    painter.scale(1, -1); // Scale y so that the number is right-side up.
    painter.drawText(x + 15, -y, playerNum);
    painter.scale(1,-1);
    painter.setPen(Qt::black);

    // Draw the ball
    QPoint ballCenter(x + ballD*std::cos(TO_RAD*h),
                      y + ballD*std::sin(TO_RAD*h));
    painter.setBrush(QColor::fromRgb(205,140,0));
    painter.drawEllipse(ballCenter, 8, 8);

    // Draw my number
    painter.setPen(brushColor);
    painter.scale(1, -1); // Scale y so that the number is right-side up.
    painter.drawText(ballCenter.x() + 15, -ballCenter.y(), playerNum);
    painter.scale(1,-1);
    painter.setPen(Qt::black);
}

void SharedViewerPainter::handleZoomIn()
{
    scaleFactor += .1;
    repaint();
}

void SharedViewerPainter::handleZoomOut()
{
    scaleFactor -= .1;
    repaint();
}

void SharedViewerPainter::updateWithLocationInfo(float xCoord, float yCoord,
                                                 float heading, float ballDistance,
                                                 int index)
{
    myXCoords[index] = xCoord;
    myYCoords[index] = yCoord;
    myHeadings[index] = heading;
    myBallDist[index] = ballDistance;
    update();
}

void SharedViewerPainter::updateWithSharedBallMessage(messages::SharedBall sharedLoc)
{
    sharedBallLoc = sharedLoc;
    update();
}

} // namespace shared
} // namespace tool
