/**
 * Class responsible for drawing more things on the field, inherits from PaintField
 * so that the field itself is already drawn. Based on specs from the SharedViewer
 * Class, which are used to draw the robots, balls, and sharedball.
 *
 * @author Megan Maher
 * @date   June 2014
 */

#pragma once

#include <QtGui>
#include <vector>
#include <iostream>

#include "FieldConstants.h"
#include "Common.h"

#include "common/PaintField.h"

#include "RobotLocation.pb.h"
#include "BallModel.pb.h"

namespace tool {
namespace sharer {

static const int ROBOT_WIDTH = 8;

class SharedViewerPainter : public tool_common::PaintField
{
    Q_OBJECT;

public:
    SharedViewerPainter(QWidget* parent = 0, float scaleFactor_ = 1.f);

    void updateWithLocationInfo(float xCoord, float yCoord,
                                float heading, float ballDistance, int index);
    void updateWithSharedBallMessage(messages::SharedBall sharedLoc);

protected slots:
    void handleZoomIn();
    void handleZoomOut();

protected:
    // Paint the field
    void paintEvent(QPaintEvent* event);

    // Paint a RobotLocation
    void paintRobot(QPaintEvent* event, float x, float y, float h, float ballD,
                    QString playerNum);

    void paintSharedBall(QPaintEvent* event, messages::SharedBall msg);

    float myXCoords[NUM_PLAYERS_PER_TEAM];
    float myYCoords[NUM_PLAYERS_PER_TEAM];
    float myHeadings[NUM_PLAYERS_PER_TEAM];
    float myBallDist[NUM_PLAYERS_PER_TEAM];

private:
    messages::SharedBall sharedBallLoc;
};

} // namespace shared
} // namespace tool
