/**
 * Class responsible for drawing more things on the field, inherits from PaintField
 * so that the field itself is already drawn. Based on specs from the FieldViewer
 * Class then this class will render other images
 *
 * @author EJ Googins April 2013
 * @modified Brian Jacobel April 2013
 *
 */

#pragma once

#include <QtGui>
#include <vector>

#include "common/FieldConstants.h"

#include "common/PaintField.h"

#include "RobotLocation.pb.h"
#include "WorldModel.pb.h"

namespace tool {
namespace worldview {

class WorldViewPainter : public tool_common::PaintField
{
    Q_OBJECT;

public:
    WorldViewPainter(QWidget* parent = 0, float scaleFactor_ = 1.f);

    void updateWithLocationMessage(messages::WorldModel newLoc);

protected slots:
    void paintLocationAction(bool state);

protected:
    // Paint the field
    void paintEvent(QPaintEvent* event);

    // Paint a RobotLocation
    void paintRobotLocation(QPaintEvent* event,
                            messages::WorldModel loc,
                            bool red = false);

private:
    bool shouldPaintLocation;

    messages::WorldModel curLoc;
};

} // namespace viewer
} // namespace tool


/*WorldModel proto looks like:

message WorldModel
{
        optional float my_x = 1;
        optional float my_y = 2;
        optional float my_h = 3;

        optional float my_x_uncert = 4;
        optional float my_y_uncert = 5;
        optional float my_h_uncert = 6;

        optional bool  ball_on = 7;

        optional float ball_dist = 8;
        optional float ball_bearing = 9;

        optional float ball_dist_uncert = 10;
        optional float ball_bearing_uncert = 11;

        optional float chase_time = 12;

        optional int32 role = 13;
        optional int32 sub_role = 14;

        optional bool  active = 15;
}

*/
