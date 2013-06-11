#include "WorldViewPainter.h"

namespace tool {
namespace worldview {

static const int PARTICLE_WIDTH = 8;

WorldViewPainter::WorldViewPainter(QWidget* parent, float scaleFactor_) :
    PaintField(parent, scaleFactor_),
    shouldPaintLocation(true)
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

/**
 *  NOTE: Comm information is coming from behaviors -> is in degrees
 *
 */
void WorldViewPainter::paintRobotLocation(QPaintEvent* event,
                                            messages::WorldModel loc,
                                            bool red)
{
    QPainter painter(this);
    painter.translate(0, FIELD_GREEN_HEIGHT);
    painter.scale(1, -1);

    if (red)
        painter.setBrush(Qt::red);

    QPoint locCenter(loc.my_x(), loc.my_y());

    //draw myself
    painter.drawEllipse(locCenter,
                        PARTICLE_WIDTH,
                        PARTICLE_WIDTH);

    //draw my heading
    painter.drawLine(loc.my_x(),
                     loc.my_y(),
                     PARTICLE_WIDTH * std::cos(TO_RAD*loc.my_h()) + loc.my_x(),
                     PARTICLE_WIDTH * std::sin(TO_RAD*loc.my_h()) + loc.my_y());

    //draw my uncertainty
    painter.drawEllipse(locCenter,
                        (int)loc.my_x_uncert(),
                        (int)loc.my_y_uncert());

    if(loc.ball_on()){
        //draw where I think the ball is
        painter.setBrush(Qt::darkYellow); //Orange isn't a thing??
        painter.drawEllipse(loc.my_x()+loc.ball_dist()*std::cos(TO_RAD*loc.my_h()+TO_RAD*loc.ball_bearing()),
                            loc.my_y()+loc.ball_dist()*std::sin(TO_RAD*loc.my_h()+TO_RAD*loc.ball_bearing()),
                            25,
                            25);

        //draw how sure I am about where the ball is
        //TODO
    }

}

void WorldViewPainter::updateWithLocationMessage(messages::WorldModel newLoc)
{
    curLoc = newLoc;
    if(shouldPaintLocation) {
        update();
    }
}

} // namespace viewer
} // namespace tool
