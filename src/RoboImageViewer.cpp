#include "RoboImageViewer.h"
#include <QMouseEvent>

RoboImageViewer::RoboImageViewer(const YUVImage* yuvImage, QLabel *inf, QWidget *parent)
    : QWidget(parent),
      yuvImage(yuvImage),
      info(inf)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    setMouseTracking(true);
}

void RoboImageViewer::mouseMoveEvent(QMouseEvent *event) {
//    int x = event->x() / 2;
//    int y = event->y() / 2;
//    QString xS;
//    xS.setNum(x);
//    QString yS;
//    yS.setNum(y);
//    QString yy;
//    yy.setNum(r.getY(x, y));
//    QString u;
//    u.setNum(r.getU(x, y));
//    QString v;
//    v.setNum(r.getV(x, y));
//    QString temp = "x, y: "+ xS+" "+yS+"\nYUV: "+yy+" "+u+" "+v;
//    info->setText(temp);
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

    QRect rect(10, 20, 80, 60);

    QPainterPath path;
    /*path.moveTo(20, 80);
    path.lineTo(20, 30);
    path.cubicTo(80, 0, 50, 50, 80, 80);*/

    QPainter painter(this);

    QTextStream out(stdout);
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
