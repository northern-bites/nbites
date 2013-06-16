/**
 * Class responsible for drawing more things on the field, inherits from PaintField
 * so that the field itself is already drawn. Based on specs from the FieldViewer
 * Class then this class will render other images
 *
 * @author EJ Googins April 2013
 * @modified Brian Jacobel April 2013
 * @modified Wils Dawson June 2013
 *
 */

#pragma once

#include <QtGui>
#include <vector>
#include <iostream>

#include "FieldConstants.h"

#include "common/PaintField.h"

#include "RobotLocation.pb.h"
#include "WorldModel.pb.h"

#include "Common.h"
#include "NBMath.h"

namespace tool {
namespace worldview {

class WorldViewPainter : public tool_common::PaintField
{
    Q_OBJECT;

public:
    WorldViewPainter(QWidget* parent = 0, float scaleFactor_ = 1.f);

    void updateWithLocationMessage(messages::WorldModel newLoc, int index);

protected:
    // Paint the field
    void paintEvent(QPaintEvent* event);

    // Paint a RobotLocation
    void paintRobotLocation(QPaintEvent* event,
                            messages::WorldModel loc,
                            QString playerNum,
                            bool red = false);

private:
    messages::WorldModel curLoc[NUM_PLAYERS_PER_TEAM];
};

} // namespace worldview
} // namespace tool
