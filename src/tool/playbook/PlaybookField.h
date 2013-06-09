/**
 * Class responsible for drawing more things on the field,
 * inherits from PaintField so that the field itself is already drawn.
 * Based on specs from the PlaybookCreator Class
 *
 * @author Wils Dawson
 * @date May 2013
 */

#pragma once

#include <QtGui>
#include <vector>

#include "FieldConstants.h"
#include "PlaybookModel.h"
#include "RobotGraphics.h"

#include "common/PaintField.h"

enum Role {
    DEFENDER = 0,
    MIDDIE,
    OFFENDER,
    GOALIE
};

static const Qt::GlobalColor roleColors [] = {Qt::blue, Qt::cyan,
                                              Qt::yellow, Qt::magenta};

namespace tool {
namespace playbook {

class PlaybookField : public PaintField
{
    Q_OBJECT;

public:
    PlaybookField(int b_s, int g_w, int g_h, PlaybookModel* m,
                  QWidget* parent = 0, float scaleFactor_ = 1.f);

protected slots:
    void drawGoalie(bool on);

protected:
    void paintEvent(QPaintEvent* event);

    void paintGrid(QPaintEvent*event);

    void paintGoalie(QPaintEvent* event);
    void paintPlayers(QPaintEvent* event);

    static const float ROBOT_SIZE_X = 18.0f;
    static const float ROBOT_SIZE_Y = 35.0f;

    void paintRobot(QPaintEvent* event,
                    QPainter& painter,
                    int x, int y, int h,
                    Qt::GlobalColor c,
                    float sizeX = 18.0f, float sizeY = 35.0f);

private:
    PlaybookModel* model;

    bool shouldPaintGoalie;

    int BOX_SIZE;
    int GRID_WIDTH;
    int GRID_HEIGHT;

    RobotGraphics* robots[3];
    void updateRobots(PlaybookPosition** positions);
    int parseRole(char role);
};

} // namespace playbook
} // namespace tool
