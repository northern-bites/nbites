#include "BallViewerPainter.h"

#include <cmath>

namespace tool {
namespace ballview {

BallViewerPainter::BallViewerPainter(QWidget* parent, float scaleFactor_) :
    PaintField(parent, scaleFactor_)
{
    // Start at midfield
    curLocation.set_x(CENTER_FIELD_X);
    curLocation.set_y(CENTER_FIELD_Y);
}

void BallViewerPainter::paintOdometry(QPaintEvent* event)
{
    QPainter painter(this);
    //Move origin to bottem left and scale to flip the y axis
    painter.translate(0,FIELD_GREEN_HEIGHT*scaleFactor);
    painter.scale(scaleFactor, -scaleFactor);

    painter.setPen(Qt::white);
    painter.setBrush(Qt::black);

    // paint the new estimate
    QPoint loc(curLocation.x(), curLocation.y());
    painter.drawEllipse(loc, 10, 10);
    painter.drawLine(curLocation.x(), curLocation.y(),
                     10.f * std::cos(curLocation.h()) + curLocation.x(),
                     10.f * std::sin(curLocation.h()) + curLocation.y());
}

void BallViewerPainter::paintBalls(QPaintEvent* event)
{
    QPainter painter(this);
    //Move origin to bottem left and scale to flip the y axis
    painter.translate(0,FIELD_GREEN_HEIGHT*scaleFactor);
    painter.scale(scaleFactor, -scaleFactor);

    if(paintLog) {
        if(stationary) {
            painter.setPen(Qt::blue);
            painter.setBrush(Qt::blue);
            painter.drawEllipse(curGlobalMoving, 6, 6);

            painter.setPen(Qt::black);
            painter.setBrush(Qt::red);
            painter.drawEllipse(curGlobalStationary, 6, 6);
        }

        else {
            painter.setPen(Qt::black);
            painter.setBrush(Qt::black);
            painter.drawEllipse(curGlobalStationary, 6, 6);

            painter.setPen(Qt::blue);
            painter.setBrush(Qt::red);
            painter.drawEllipse(curGlobalMoving, 6, 6);
        }

        painter.setBrush(Qt::magenta);
        if((curGlobalVision.x() > 0) && (curGlobalVision.y() > 0))
            painter.drawEllipse(curGlobalVision, 6, 6);
        else
            painter.drawEllipse(QPoint(3,3), 6, 6);
    }

    if (paintOffline) {
        if(offStationary) {
            painter.setPen(Qt::blue);
            painter.setBrush(Qt::blue);
            painter.drawEllipse(offCurGlobalMoving, 6, 6);

            painter.setPen(Qt::black);
            painter.setBrush(Qt::red);
            painter.drawEllipse(offCurGlobalStationary, 6, 6);
        }

        else {
            painter.setPen(Qt::black);
            painter.setBrush(Qt::black);
            painter.drawEllipse(offCurGlobalStationary, 6, 6);

            painter.setPen(Qt::blue);
            painter.setBrush(Qt::red);
            painter.drawEllipse(offCurGlobalMoving, 6, 6);
        }

        painter.setBrush(Qt::magenta);
        if((offCurGlobalVision.x() > 0) && (offCurGlobalVision.y() > 0))
            painter.drawEllipse(offCurGlobalVision, 6, 6);
        else
            painter.drawEllipse(QPoint(3,3), 6, 6);
    }
}

void BallViewerPainter::paintEvent(QPaintEvent* event)
{
    PaintField::paintEvent(event);

    paintOdometry(event);

    paintBalls(event);
}

void BallViewerPainter::updateOdometry(messages::RobotLocation curOdometry)
{
    // Calculate the difference in odometry and store the cur as last
    float dX = curOdometry.x() - lastOdometry.x();
    lastOdometry.set_x(curOdometry.x());
    float dY = curOdometry.y() - lastOdometry.y();
    lastOdometry.set_y(curOdometry.y());
    float dH = curOdometry.h() - lastOdometry.h();
    lastOdometry.set_h(curOdometry.h());

    // Global frame and robot frame line up!
    curLocation.set_x(curLocation.x() + dX);
    curLocation.set_y(curLocation.y() + dY);
    curLocation.set_h(curLocation.h() + dH);

    // Paint it
    repaint();
}

void BallViewerPainter::updateFilteredBall(messages::FilteredBall ball)
{
    // Paint the moving and stationary filters
    curGlobalStationary = getGlobalPoint(ball.stat_distance(), ball.stat_bearing());
    curGlobalMoving = getGlobalPoint(ball.mov_distance(), ball.mov_bearing());
    stationary = ball.is_stationary();
    if(ball.vis().on())
        curGlobalVision   = getGlobalPoint(ball.vis().distance(), ball.vis().bearing());
    else
        curGlobalVision = QPoint(-1,-1);

    repaint();
}

void BallViewerPainter::updateOfflineFilteredBall(messages::FilteredBall ball)
{
    // Paint the moving and stationary filters
    offCurGlobalStationary = getGlobalPoint(ball.stat_distance(), ball.stat_bearing());
    offCurGlobalMoving = getGlobalPoint(ball.mov_distance(), ball.mov_bearing());
    offStationary = ball.is_stationary();
    if(ball.vis().on())
        offCurGlobalVision   = getGlobalPoint(ball.vis().distance(), ball.vis().bearing());
    else
        offCurGlobalVision = QPoint(-1,-1);

    repaint();
}

void BallViewerPainter::handleZoomIn()
{
    scaleFactor += .1;
    repaint();
}

void BallViewerPainter::handleZoomOut()
{
    scaleFactor -= .1;
    repaint();
}

void BallViewerPainter::paintLogAction(bool state)
{
    paintLog = state;
    repaint();
}

void BallViewerPainter::paintOfflineAction(bool state)
{
    paintOffline = state;
    repaint();
}

QPoint BallViewerPainter::getGlobalPoint(float dist, float bear)
{
    float sin, cos;
    float ninetyDeg = 1.5707963;
    sincosf((curLocation.h() + bear), &sin, &cos);

    float relX = dist*cos + curLocation.x();
    float relY = dist*sin + curLocation.y();
    QPoint relLoc(relX,relY);
    return relLoc;
}


} // namespace ballview
} // namespace tool
