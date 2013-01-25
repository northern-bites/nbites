#include "ImageView.h"
#include <iostream>

namespace tool{
namespace visionsim{

// For drawing the ball!
static const QColor ORANGE(255, 127, 0);

ImageView::ImageView(Image& i, QWidget* parent) :
    QWidget(parent),
    image(i)
{
}

QSize ImageView::minimumSizeHint() const
{
    return QSize(IMAGE_WIDTH, IMAGE_HEIGHT);
}

QSize ImageView::sizeHint() const
{
    return QSize(IMAGE_WIDTH, IMAGE_HEIGHT);
}

void ImageView::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    painter.fillRect(0, 0, IMAGE_WIDTH,
                     IMAGE_HEIGHT, Qt::darkGray);

    QPen pen(Qt::white);

    // Draw locations of visible corners
    CornerVector visible = image.visibleCorners;
    for (CornerVector::iterator i = visible.begin();
         i != visible.end(); i++)
    {
        if (i->green())
        {
            pen.setColor(Qt::darkGreen);
            pen.setWidth(6);
            painter.setPen(pen);
        }
        else
        {
            pen.setColor(Qt::white);
            pen.setWidth(6);
            painter.setPen(pen);
        }

        if (!i->behind())
            painter.drawPoint(i->x(), i->y());
    }

    // Draw locations of lines
    LineVector lines = image.allLines;
    for (LineVector::iterator i = lines.begin();
         i != lines.end(); i++)
    {
        painter.drawLine(QLine(i->getCorner1()->x(), i->getCorner1()->y(),
                               i->getCorner2()->x(), i->getCorner2()->y()));
    }

}

}
}
