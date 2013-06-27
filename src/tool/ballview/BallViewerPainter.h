/**
 * Class responsible for drawing more things on the field, inherits from PaintField
 * so that the field itself is already drawn. Based on specs from the FieldViewer
 * Class then this class will render other images
 *
 * @author EJ Googins April 2013
 *
 */

#pragma once

#include <QtGui>
#include <vector>

#include "FieldConstants.h"

#include "common/PaintField.h"

#include "RobotLocation.pb.h"
#include "BallModel.pb.h"

namespace tool {
namespace ballview {


class BallViewerPainter : public tool_common::PaintField
{
    Q_OBJECT;

public:
    BallViewerPainter(QWidget* parent = 0, float scaleFactor_ = 1.f);
    void updateOdometry(messages::RobotLocation odometry);
    void updateFilteredBall(messages::FilteredBall ball);
    void updateOfflineFilteredBall(messages::FilteredBall ball);

protected slots:
    void handleZoomIn();
    void handleZoomOut();
    void paintLogAction(bool state);
    void paintOfflineAction(bool state);

protected:
    // Paint the field
    void paintEvent(QPaintEvent* event);
    void paintOdometry(QPaintEvent* event);
    void paintBalls(QPaintEvent* event);

private:
    QPoint getGlobalPoint(float dist, float bear);

    messages::RobotLocation lastOdometry;

    messages::RobotLocation curLocation;

    bool stationary;
    QPoint curGlobalStationary;
    QPoint curGlobalMoving;
    QPoint curGlobalVision;

    bool offStationary;
    QPoint offCurGlobalStationary;
    QPoint offCurGlobalMoving;
    QPoint offCurGlobalVision;

    bool paintLog;
    bool paintOffline;
};

} // namespace ballview
} // namespace tool
