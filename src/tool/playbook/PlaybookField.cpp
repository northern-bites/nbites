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
    if (shouldPaintGoalie)
    {
        update();
    }
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

    painter.setBrush(Qt::magenta);

    QPoint goalieCenter(FIELD_WHITE_LEFT_SIDELINE_X, CENTER_FIELD_Y);
    QRect goalie(FIELD_WHITE_LEFT_SIDELINE_X,
                 CENTER_FIELD_Y,
                 18*scaleFactor,
                 35*scaleFactor);

    goalie.moveCenter(goalieCenter);

    painter.drawRect(goalie);

    QPoint lineEnd( 9.0*scaleFactor * std::cos(0) + FIELD_WHITE_LEFT_SIDELINE_X,
                   17.5*scaleFactor * std::sin(0) + CENTER_FIELD_Y);

    painter.drawLine(goalieCenter, lineEnd);
}

} // namespace playbook
} // namespace tool
