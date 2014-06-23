#include "ImageView.h"

namespace tool{
namespace visionsim{

// For drawing the ball! Qt's orange is pathetic.
static const QColor ORANGE(255, 127, 0);

/*
 * @param i -- the image instance we will get all of the object info from
 */
ImageView::ImageView(Image& i, QWidget* parent) :
    QWidget(parent),
    image(i)
{
}

/*
 * Overrode the following two messages so that the images are always shown
 * full-size.
 */
QSize ImageView::minimumSizeHint() const
{
    return QSize(IMAGE_WIDTH, IMAGE_HEIGHT);
}

QSize ImageView::sizeHint() const
{
    return QSize(IMAGE_WIDTH, IMAGE_HEIGHT);
}

/*
 * Handles all of the redrawing of objects when a PaintEvent happens, ie
 * when the view is told to update.
 */
void ImageView::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    // Gray background
    painter.fillRect(0, 0, IMAGE_WIDTH,
                     IMAGE_HEIGHT, Qt::darkGray);

    QPen pen(Qt::white);

    // Draw locations of visible corners with dots
    CornerVector visible = image.visibleCorners;
    for (CornerVector::iterator i = visible.begin();
         i != visible.end(); i++)
    {
        // Make sure we're drawing the corners the right color
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

        // Make sure we're drawing the corners the right color
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

    // If the ball is not behind the image plane, ie not back-projecting,
    // draw it as well
    if (!image.ball.behind())
    {
        pen.setColor(ORANGE);
        pen.setWidth(1);
        painter.setPen(pen);
        painter.setBrush(ORANGE);
        painter.drawEllipse(QPoint(image.ball.x(),
                                   image.ball.y()),
                            image.ball.getVisualRadius(),
                            image.ball.getVisualRadius());
    }

    // Finally, draw the posts as yellow rectangles
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
