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
        // Ignore corners behind the image plane
        if(i->bothPointsBehind())
        {
            continue;
        }

        if (i->green())
        {
            pen.setColor(Qt::darkGreen);
            pen.setWidth(3);
            painter.setPen(pen);
        }
        else
        {
            pen.setColor(Qt::white);
            pen.setWidth(3);
            painter.setPen(pen);
        }

        painter.drawLine(QLine(i->point1()[X_VALUE],
                               i->point1()[Y_VALUE],
                               i->point2()[X_VALUE],
                               i->point2()[Y_VALUE]));
    }

    pen.setColor(ORANGE);
    pen.setWidth(1);
    painter.setPen(pen);
    painter.setBrush(ORANGE);
    painter.drawEllipse(QPoint(image.ballCenter[X_VALUE],
                               image.ballCenter[Y_VALUE]),
                        image.ballVisualRadius,
                        image.ballVisualRadius);

    pen.setColor(Qt::yellow);
    pen.setWidth(1);
    painter.setPen(pen);
    painter.setBrush(Qt::yellow);

    PostVector posts = image.allPosts;
    for (PostVector::iterator i = posts.begin();
         i != posts.end(); i++)
    {
        if (i->behind()) continue;
        painter.drawRect(QRect(QPoint(i->x(),
                                      i->y()),
                               QSize(i->getVisualWidth(),
                                     i->getVisualHeight())));
    }

}

}
}
