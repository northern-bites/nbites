#include "WorldViewPainter.h"

namespace tool {
namespace worldview {

static const int ROBOT_WIDTH = 8;

// Degree of field of vision
static const int VISION_SPAN = 70;

// Distance of robot vision, for testing
static const int VISION_DISTANCE = 50;

WorldViewPainter::WorldViewPainter(QWidget* parent, float scaleFactor_) :
    PaintField(parent, scaleFactor_)
{
    flipped = 0;
}

void WorldViewPainter::paintEvent(QPaintEvent* event)
{
    PaintField::paintEvent(event);

    // Paint actual location
    for (int i = 0; i < NUM_PLAYERS_PER_TEAM; ++i)
    {
        paintRobotLocation(event, curLoc[i], QString::number(i+1));
    }

    paintSharedBallLocation(event, sharedballLoc);
}

void WorldViewPainter::paintSharedBallLocation(QPaintEvent* event,
                                               messages::SharedBall msg)
{
    if (msg.ball_on()) {
        QPainter painter(this);
        if (flipped)
        {
            painter.translate(FIELD_GREEN_WIDTH*scaleFactor, 0);
            painter.scale(-scaleFactor, scaleFactor);
        }
        else
        {
            painter.translate(0, FIELD_GREEN_HEIGHT*scaleFactor);
            painter.scale(scaleFactor, -scaleFactor);
        }
        QPoint ballCenter(msg.x(),
                          msg.y());

        //draw the weighted averaged location of the ball
        painter.setBrush(QColor::fromRgb(153,0,153));
        painter.drawEllipse(ballCenter,
                            8,
                            8);
    }
    return;
}

void WorldViewPainter::paintRobotLocation(QPaintEvent* event,
                                          messages::WorldModel msg,
                                          QString playerNum)
{
    if (!msg.active())
    {
        return; // Don't paint robots that aren't there.
    }

    QPainter painter(this);
    if (flipped)
    {
        painter.translate(FIELD_GREEN_WIDTH*scaleFactor, 0);
        painter.scale(-scaleFactor, scaleFactor);
    }
    else
    {
        painter.translate(0, FIELD_GREEN_HEIGHT*scaleFactor);
        painter.scale(scaleFactor, -scaleFactor);
    }

    Qt::GlobalColor brushColor = Qt::cyan;
    if (msg.fallen())
        brushColor = Qt::red;
    else if (msg.in_kicking_state())
        brushColor = Qt::blue;

    const QPoint locCenter(msg.my_x(), msg.my_y());

    painter.setBrush(Qt::Dense7Pattern);

    // Draw my uncertainty
    painter.drawEllipse(locCenter,
                        (int)msg.my_uncert(),
                        (int)msg.my_uncert());


    if (!msg.fallen()){
        // Draw boundaries of my field of vision
        painter.setPen(QPen(Qt::cyan, 3));
        painter.drawLine(msg.my_x(),
                         msg.my_y(),
                         VISION_DISTANCE * std::cos(TO_RAD*(msg.my_h()+VISION_SPAN/2)) + msg.my_x(),
                         VISION_DISTANCE * std::sin(TO_RAD*(msg.my_h()+VISION_SPAN/2)) + msg.my_y());
        painter.drawLine(msg.my_x(),
                         msg.my_y(),
                         VISION_DISTANCE * std::cos(TO_RAD*(msg.my_h()-VISION_SPAN/2)) + msg.my_x(),
                         VISION_DISTANCE * std::sin(TO_RAD*(msg.my_h()-VISION_SPAN/2)) + msg.my_y());

        // Draw where I'm walking to
        painter.setPen(QPen(Qt::black, 2));
        const QPoint walkingEnd(msg.walking_to_x(), msg.walking_to_y());
        painter.drawLine(locCenter, walkingEnd);
    }

    painter.setBrush(brushColor);
    painter.setPen(brushColor);

    // Draw myself
    painter.drawEllipse(locCenter,
                        ROBOT_WIDTH,
                        ROBOT_WIDTH);

    painter.setPen(Qt::black);
    // Draw my heading
    painter.drawLine(msg.my_x(),
                     msg.my_y(),
                     ROBOT_WIDTH * std::cos(TO_RAD*msg.my_h()) + msg.my_x(),
                     ROBOT_WIDTH * std::sin(TO_RAD*msg.my_h()) + msg.my_y());

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

        // Draw my number
        painter.setPen(brushColor);
        painter.scale(1, -1); // Scale y so that the number is right-side up.
        painter.drawText(ballCenter.x() + 15, -ballCenter.y(), playerNum);
        painter.scale(1,-1);
        painter.setPen(Qt::black);
    }

    // Draw where I am kicking
    if(msg.in_kicking_state()){
        const QPoint ballCenter(msg.my_x()+msg.ball_dist()*std::cos(TO_RAD*msg.my_h()+TO_RAD*msg.ball_bearing()),
                                msg.my_y()+msg.ball_dist()*std::sin(TO_RAD*msg.my_h()+TO_RAD*msg.ball_bearing()));
        const QPoint kickingEnd(msg.kicking_to_x(), msg.kicking_to_y());

        // Draw line to where I am kicking
        painter.setPen(QPen(Qt::blue, 2));
        painter.drawLine(ballCenter, kickingEnd);
    }
}

void WorldViewPainter::updateWithLocationMessage(messages::WorldModel newLoc,
                                                 int index)
{
    curLoc[index] = newLoc;
    update();
}

void WorldViewPainter::updateWithSharedBallMessage(messages::SharedBall sharedLoc)
{
    sharedballLoc = sharedLoc;
    update();
}

void WorldViewPainter::flipScreen()
{
    if (flipped) {
        flipped = 0;
    }
    else {
        flipped = 1;
    }
    update();
}

} // namespace worldview
} // namespace tool
