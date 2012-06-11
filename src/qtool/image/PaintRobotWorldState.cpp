
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
                            const Pose& startPose,
                            QColor color) {

    float distance = visualDetection.distance();
    float bearing = visualDetection.bearing();

    if (distance == 0.0f) return ;

    float relX = distance * cos(bearing + startPose.h());
    float relY = distance * sin(bearing + startPose.h());

    QPoint point(relX + startPose.x(), relY + startPose.y());
    this->paintDot(painter, color, point, 5);
}

void PaintRobotWorldState::draw(QPainter& painter, const Pose& pose,
                                const PoseArea& poseArea, QColor color) {

    QPoint poseLoc = QPoint(pose.x(), pose.y());
    this->paintDot(painter, color, poseLoc);
    this->paintPolarLine(painter, color, 3, poseLoc, 20, pose.h() * TO_DEG);

    //pose area
    this->paintEllipseArea(painter, color, poseLoc, poseArea.x_size(), poseArea.y_size());
    this->paintPolarLine(painter, color, 2, poseLoc, 15,
                         (pose.h() + poseArea.h_size()/2) * TO_DEG);
    this->paintPolarLine(painter, color, 2, poseLoc, 15,
                         (pose.h() - poseArea.h_size()/2) * TO_DEG);

}

void PaintRobotWorldState::buildBitmap()
{
    bitmap.fill(Qt::transparent);
    QPainter painter(&bitmap);

    this->transformPainterToFieldCoordinates(painter);

    this->draw(painter, mLoc->get()->pose(), mLoc->get()->uncertainty(), QColor("Blue"));

    this->draw(painter, mVision->get()->visual_ball().visual_detection(),
               mLoc->get()->pose(), QColor("Orange"));
    this->draw(painter, mVision->get()->yglp().visual_detection(),
               mLoc->get()->pose(), QColor("Yellow"));
    this->draw(painter, mVision->get()->ygrp().visual_detection(),
               mLoc->get()->pose(), QColor("Yellow"));

    this->draw(painter, mVision->get()->visual_cross().visual_detection(),
               mLoc->get()->pose(), QColor("White"));
}

}
}

