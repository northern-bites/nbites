#include "PlaybookField.h"

#include <QtDebug>

namespace tool {
namespace playbook {

PlaybookField::PlaybookField(QWidget* parent, float scaleFactor_) :
    PaintField(parent, scaleFactor_),
    shouldPaintGoalie(true)
{
}

void PlaybookField::drawGoalie(bool on)
{
    shouldPaintGoalie = on;
    qDebug() << "Drawing goalie " << on;
    update();
}

void PlaybookField::paintEvent(QPaintEvent* event)
{
    PaintField::paintEvent(event);

    if(shouldPaintGoalie)
    {
        paintGoalie(event);
    }
}

void PlaybookField::paintGoalie(QPaintEvent* event)
{
    QPainter painter(this);
    //Move orign to bottom left and scale to flip y axis
    painter.translate(0,FIELD_GREEN_HEIGHT);
    painter.scale(scaleFactor, -scaleFactor);

    paintRobot(event, painter,
               FIELD_WHITE_LEFT_SIDELINE_X + 15, CENTER_FIELD_Y, 0,
               Qt::magenta);
}

void PlaybookField::paintRobot(QPaintEvent* event,
                               QPainter& painter,
                               int x, int y, int h,
                               Qt::GlobalColor c,
                               float sizeX, float sizeY)
{
    painter.save();

    painter.setBrush(c);

    painter.translate(x,y);
    painter.rotate(h);

    QPoint center(0, 0);
    QRect robot(0, 0, sizeX*scaleFactor, sizeY*scaleFactor);

    QPoint lineEnd(0.125*x*scaleFactor,
                   0);

    robot.moveCenter(center);

    painter.drawRect(robot);

    painter.drawLine(center, lineEnd);

    painter.rotate(-h);

    painter.restore();
}

} // namespace playbook
} // namespace tool
