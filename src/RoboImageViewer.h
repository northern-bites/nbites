#ifndef RENDERAREA_H
#define RENDERAREA_H

#include <QWidget>
#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QLabel>
#include "YUVImage.h"

class RoboImageViewer: public QWidget
{
    Q_OBJECT
public:

    RoboImageViewer(const YUVImage* yuvImage, QLabel *lab, QWidget *parent = 0);
    void  setYUVImage(YUVImage* img) { yuvImage = img;}
    QSize minimumSizeHint() const;
    QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent *event);

private:
    QLabel *info;
    QPixmap pixmap;
    const YUVImage *yuvImage;

};

#endif // RENDERAREA_H
