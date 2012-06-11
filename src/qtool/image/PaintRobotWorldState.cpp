
#include "PaintRobotWorldState.h"

namespace qtool {
namespace image {

using namespace data;
using namespace memory;
using namespace proto;

PaintRobotWorldState::PaintRobotWorldState(memory::MVision::const_ptr mVision,
                                           memory::MLocalization::const_ptr mLoc,
                                           QObject *parent)
    : PaintFieldOverlay(1.0f, parent), mVision(mVision), mLoc(mLoc)
{
}

void PaintRobotWorldState::draw(QPainter& painter,
                            const PVision::PVisualDetection& visualDetection,
                            const RobotLocation& startPose,
                            QColor color) {

    float distance = visualDetection.distance();
    float bearing = visualDetection.bearing();

    if (distance == 0.0f) return ;

    float relX = distance * cos(bearing + startPose.h());
    float relY = distance * sin(bearing + startPose.h());

    QPoint point(relX + startPose.x(), relY + startPose.y());
    this->paintDot(painter, color, point, 5);
}

void PaintRobotWorldState::draw(QPainter& painter, const RobotLocation& location,
                                const RobotArea& area, QColor color) {

    QPoint poseLoc = QPoint(location.x(), location.y());
    this->paintDot(painter, color, poseLoc);
    this->paintPolarLine(painter, color, 3, poseLoc, 20, location.h() * TO_DEG);

    //location area
    this->paintEllipseArea(painter, color, poseLoc, area.x_size(), area.y_size());
    this->paintPolarLine(painter, color, 2, poseLoc, 15,
                         (location.h() + area.h_size()/2) * TO_DEG);
    this->paintPolarLine(painter, color, 2, poseLoc, 15,
                         (location.h() - area.h_size()/2) * TO_DEG);

}

void PaintRobotWorldState::buildBitmap()
{
    bitmap.fill(Qt::transparent);
    QPainter painter(&bitmap);

    this->transformPainterToFieldCoordinates(painter);

    this->draw(painter, mLoc->get()->location(), mLoc->get()->uncertainty(), QColor("Blue"));

    this->draw(painter, mVision->get()->visual_ball().visual_detection(),
               mLoc->get()->location(), QColor("Orange"));
    this->draw(painter, mVision->get()->yglp().visual_detection(),
               mLoc->get()->location(), QColor("Yellow"));
    this->draw(painter, mVision->get()->ygrp().visual_detection(),
               mLoc->get()->location(), QColor("Yellow"));

    this->draw(painter, mVision->get()->visual_cross().visual_detection(),
               mLoc->get()->location(), QColor("White"));
}

}
}

