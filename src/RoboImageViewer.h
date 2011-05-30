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
    enum Shape { Y, U, V, Blue, Red, Green, H, S, Z, EDGE, WHEEL, Pixmap };

    RoboImageViewer(const YUVImage* yuvImage, QLabel *lab, QWidget *parent = 0);

//    void  setYUVImage(YUVImage img) { r = img;}
    void mouseMoveEvent(QMouseEvent *event);
    int max(int a, int b) {if (a < b) return b; return a;}
    QSize minimumSizeHint() const;
    QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent *event);

private:
    QLabel *info;
    bool antialiased;
    bool transformed;
    QPixmap pixmap;
    const YUVImage *yuvImage;

};

#endif // RENDERAREA_H
