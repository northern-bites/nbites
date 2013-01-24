/**
 * Widget for drawing the state of the world.
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

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent* event);

private:
    World& world;
};

}
}
