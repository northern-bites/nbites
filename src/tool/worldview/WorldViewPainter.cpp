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
                                          messages::WorldModel msg,
                                          QString playerNum,
                                          bool red)
{
    if (!msg.active())
    {
        return; // Don't paint robots that aren't there.
    }

    QPainter painter(this);
    painter.translate(0, FIELD_GREEN_HEIGHT);
    painter.scale(1, -1);

    Qt::GlobalColor brushColor = Qt::cyan;
    if (red)
        brushColor = Qt::red;

    painter.setBrush(brushColor);

    QPoint locCenter(msg.my_x(), msg.my_y());

    // Draw myself
    painter.drawEllipse(locCenter,
                        PARTICLE_WIDTH,
                        PARTICLE_WIDTH);

    // Draw my heading
    painter.drawLine(msg.my_x(),
                     msg.my_y(),
                     PARTICLE_WIDTH * std::cos(TO_RAD*msg.my_h()) + msg.my_x(),
                     PARTICLE_WIDTH * std::sin(TO_RAD*msg.my_h()) + msg.my_y());

    // Draw my uncertainty
    painter.drawEllipse(locCenter,
                        (int)msg.my_uncert(),
                        (int)msg.my_uncert());

    // Draw my number
    painter.setPen(brushColor);
    painter.scale(1, -1); // Scale y so that the number is right-side up.
    painter.drawText(locCenter.x() + 15, -locCenter.y(), playerNum);
    painter.scale(1,-1);
    painter.setPen(Qt::black);

    // Draw the ball
    if(msg.ball_on()){
        QPoint ballCenter(msg.my_x()+msg.ball_dist()*std::cos(TO_RAD*msg.my_h()+TO_RAD*msg.ball_bearing()),
                          msg.my_y()+msg.ball_dist()*std::sin(TO_RAD*msg.my_h()+TO_RAD*msg.ball_bearing()));

        //draw where I think the ball is
        painter.setBrush(QColor::fromRgb(205,140,0));
        painter.drawEllipse(ballCenter,
                            8,
                            8);

        //draw how sure I am about where the ball is
        //TODO

        // Draw my number
        painter.setPen(brushColor);
        painter.scale(1, -1); // Scale y so that the number is right-side up.
        painter.drawText(ballCenter.x() + 15, -ballCenter.y(), playerNum);
        painter.scale(1,-1);
        painter.setPen(Qt::black);
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
