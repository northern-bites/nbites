
#include "PaintRobotWorldState.h"

namespace qtool {
namespace image {

using namespace data;
using namespace memory;
using namespace proto;

PaintRobotWorldState::PaintRobotWorldState(DataManager::ptr dataManager,
                                           QObject *parent)
    : PaintFieldOverlay(1.0f, parent), dataManager(dataManager)
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

RobotLocation operator+(const RobotLocation& location1, const RobotLocation& location2) {
    RobotLocation result;

    result.set_h(location1.h() + location2.h());
    result.set_x(location1.x() + 4*location2.x());
    result.set_y(location1.y() + 4*location2.y());
    return result;
}

void PaintRobotWorldState::buildBitmap()
{
//    bitmap.fill(Qt::transparent);
    QPainter painter(&bitmap);

    this->transformPainterToFieldCoordinates(painter);

    MLocalization::const_ptr mLoc = dataManager->getMemory()->get<MLocalization>();
    MVision::const_ptr mVision = dataManager->getMemory()->get<MVision>();
    MMotion::const_ptr mMotion = dataManager->getMemory()->get<MMotion>();

    RobotLocation robotLocation;
    robotLocation.set_x(370);
    robotLocation.set_y(270);
    robotLocation.set_h(0);
    robotLocation = robotLocation + mMotion->get()->odometry();

    this->draw(painter, robotLocation, mLoc->get()->uncertainty(), QColor("Blue"));

//    this->draw(painter, mVision->get()->visual_ball().visual_detection(),
//               robotLocation, QColor("Orange"));
//    this->draw(painter, mVision->get()->yglp().visual_detection(),
//               robotLocation, QColor("Yellow"));
//    this->draw(painter, mVision->get()->ygrp().visual_detection(),
//               robotLocation, QColor("Yellow"));
//
//    this->draw(painter, mVision->get()->visual_cross().visual_detection(),
//               robotLocation, QColor("White"));
}

}
}

