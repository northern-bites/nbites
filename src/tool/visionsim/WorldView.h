/**
 * @class WorldView
 *
 * Widget for drawing the state of the world. Draws an overhead view of the
 * field based on the world model it contains. Can draw lines, posts, the
 * ball (movable) and the robot (movable).
 *
 * @author Lizzie Mamantov
 */

#pragma once

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include "World.h"
#include "WorldConstants.h"

namespace tool{
namespace visionsim{

class WorldView : public QWidget
{

Q_OBJECT

public:
    WorldView(World& state);
    ~WorldView() {};

    // Keeps the widget the right size
    QSize minimumSizeHint() const;
    QSize sizeHint() const;

protected:
    // Handles all of the drawing
    void paintEvent(QPaintEvent* event);

private:
    World& world;
};

}
}
