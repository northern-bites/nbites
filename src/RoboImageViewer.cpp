#include "RoboImageViewer.h"
#include <QMouseEvent>

RoboImageViewer::RoboImageViewer(const YUVImage* yuvImage, QLabel *inf, QWidget *parent)
    : QWidget(parent),
      yuvImage(yuvImage),
      info(inf)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}

QSize RoboImageViewer::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize RoboImageViewer::sizeHint() const
{
    return QSize(640, 480);
}

void RoboImageViewer::paintEvent(QPaintEvent * /* event */)
{

    QPainter painter(this);

    QRect draw;
    int red, green, blue, edge;
    bool found;
    for (int i = 0; i < yuvImage->getHeight(); i++)
    {
        for (int j = 0; j < yuvImage->getWidth(); j++)
        {
        	red = yuvImage->getRed(j, i);
            green = yuvImage->getGreen(j, i);
            blue = yuvImage->getBlue(j, i);

            QColor col(red, green, blue);
            painter.setPen(col);
            draw.setCoords(j, i, j + 1, i + 1);
            painter.fillRect(draw, col);
        }
    }
}
