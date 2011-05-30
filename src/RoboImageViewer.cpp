#include "RoboImageViewer.h"
#include <QMouseEvent>

RoboImageViewer::RoboImageViewer(YUVImage r1, QLabel *inf, QWidget *parent)
    : QWidget(parent),
      r(r1),
      info(inf)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}

void RoboImageViewer::mouseMoveEvent(QMouseEvent *event) {
    int x = event->x() / 2;
    int y = event->y() / 2;
    QString xS;
    xS.setNum(x);
    QString yS;
    yS.setNum(y);
    QString yy;
    yy.setNum(r.getY(x, y));
    QString u;
    u.setNum(r.getU(x, y));
    QString v;
    v.setNum(r.getV(x, y));
    QString temp = "x, y: "+ xS+" "+yS+"\nYUV: "+yy+" "+u+" "+v;
    info->setText(temp);
}

QSize RoboImageViewer::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize RoboImageViewer::sizeHint() const
{
    return QSize(640, 480);
}
