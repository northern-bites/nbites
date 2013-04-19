#include "WorldViewPainter.h"

namespace tool {
namespace worldview {

static const int PARTICLE_WIDTH = 8;

WorldViewPainter::WorldViewPainter(QWidget* parent, float scaleFactor_) :
    PaintField(parent, scaleFactor_),
    shouldPaintLocation(false)
{
}

void WorldViewPainter::paintLocationAction(bool state) {

    shouldPaintLocation = state;
    repaint();
}

void WorldViewPainter::paintEvent(QPaintEvent* event)
{
    PaintField::paintEvent(event);

    if(shouldPaintLocation) {
        // Paint actual location
        paintRobotLocation(event, curLoc, true);
    }

}

void WorldViewPainter::paintRobotLocation(QPaintEvent* event,
                                            messages::RobotLocation loc,
                                            bool red)
{
    QPainter painter(this);

    if (red)
        painter.setBrush(Qt::red);

    QPoint locCenter(loc.x(), loc.y());

    painter.drawEllipse(locCenter,
                        PARTICLE_WIDTH,
                        PARTICLE_WIDTH);

    painter.drawLine(loc.x(),
                     loc.y(),
                     PARTICLE_WIDTH * std::cos(loc.h()) + loc.x(),
                     PARTICLE_WIDTH * std::sin(loc.h()) + loc.y());
}

void WorldViewPainter::updateWithLocationMessage(messages::RobotLocation newLoc)
{
    curLoc = newLoc;
    if(shouldPaintLocation) {
        repaint();
    }
}

} // namespace viewer
} // namespace tool
