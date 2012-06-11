
#include "PaintRobotWorldState.h"

namespace qtool {
namespace image {

using namespace data;
using namespace memory;

PaintRobotWorldState::PaintRobotWorldState(memory::MVision::const_ptr mVision,
                                           memory::MLocalization::const_ptr mLoc,
                                           QObject *parent)
    : PaintFieldOverlay(1.0f, parent), mVision(mVision), mLoc(mLoc)
{
}

void PaintRobotWorldState::draw(QPainter& painter,
                            const proto::PVision::PVisualDetection& visualDetection,
                            QColor color) {

    float distance = visualDetection.distance();
    float bearing = visualDetection.bearing();

    if (distance == 0.0f) return ;

    float relX = distance * cos(bearing + mLoc->get()->h_est());
    float relY = distance * sin(bearing + mLoc->get()->h_est());

    QPoint point(relX + mLoc->get()->x_est(), relY + mLoc->get()->y_est());
    this->paintDot(painter, color, point, 5);
}

void PaintRobotWorldState::buildBitmap()
{
    bitmap.fill(Qt::transparent);
    QPainter painter(&bitmap);

    this->transformPainterToFieldCoordinates(painter);

    QPoint robotLoc = QPoint(mLoc->get()->x_est(), mLoc->get()->y_est());

    this->paintDot(painter, QColor("Blue"), robotLoc);
    this->paintPolarLine(painter, QColor("blue"), 3, robotLoc, 20, mLoc->get()->h_est() * TO_DEG);

    this->draw(painter, mVision->get()->visual_ball().visual_detection(), QColor("Orange"));
    this->draw(painter, mVision->get()->yglp().visual_detection(), QColor("Yellow"));
    this->draw(painter, mVision->get()->ygrp().visual_detection(), QColor("Yellow"));

    this->draw(painter, mVision->get()->visual_cross().visual_detection(), QColor("White"));
}

}
}

