#include "WorldViewPainter.h"

namespace tool {
namespace worldview {

static const int PARTICLE_WIDTH = 8;

WorldViewPainter::WorldViewPainter(QWidget* parent, float scaleFactor_) :
    PaintField(parent, scaleFactor_)
{
}

void WorldViewPainter::paintEvent(QPaintEvent* event)
{
    PaintField::paintEvent(event);

    // Paint actual location
    for (int i = 0; i < NUM_PLAYERS_PER_TEAM; ++i)
    {
        paintRobotLocation(event, curLoc[i], QString::number(i+1), true);
    }
}

void WorldViewPainter::paintRobotLocation(QPaintEvent* event,
                                          messages::WorldModel loc,
                                          QString playerNum,
                                          bool red)
{
    if (!loc.active())
    {
        return; // Don't paint robots that aren't there.
    }

    QPainter painter(this);
    painter.translate(0, FIELD_GREEN_HEIGHT);
    painter.scale(1, -1);

    if (red)
        painter.setBrush(Qt::red);

    QPoint locCenter(loc.my_x(), loc.my_y());

    //std::cout<<"Painting at "<<loc.my_x()<<" "<<loc.my_y()<<std::endl;

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

void WorldViewPainter::updateWithLocationMessage(messages::WorldModel newLoc,
                                                 int index)
{
    curLoc[index] = newLoc;
    update();
}

} // namespace worldview
} // namespace tool
