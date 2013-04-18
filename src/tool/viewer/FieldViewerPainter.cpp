#include "FieldViewerPainter.h"

namespace tool {
namespace viewer {

static const int PARTICLE_WIDTH = 8;

FieldViewerPainter::FieldViewerPainter(QWidget* parent, float scaleFactor_) :
    PaintField(parent, scaleFactor_),
    shouldPaintParticles(false),
    shouldPaintLocation(false)
{
}

void FieldViewerPainter::paintParticleAction(bool state) {

    shouldPaintParticles = state;
    repaint();
}

void FieldViewerPainter::paintLocationAction(bool state) {

    shouldPaintLocation = state;
    repaint();
}



void FieldViewerPainter::paintEvent(QPaintEvent* event)
{
    PaintField::paintEvent(event);

    if(shouldPaintParticles) {
        // Paint all particles
        messages::RobotLocation test;
        test.set_x(50.f);
        test.set_y(50.f);
        test.set_h(0.f);

        paintRobotLocation(event, test);
    }

    if(shouldPaintLocation) {
        // Paint actual location
        paintRobotLocation(event, curLoc, true);
    }

}

void FieldViewerPainter::paintRobotLocation(QPaintEvent* event,
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

void FieldViewerPainter::updateWithLocationMessage(messages::RobotLocation newLoc)
{
    shouldPaintLocation = true;
    curLoc = newLoc;
    repaint();
}


} // namespace viewer
} // namespace tool
