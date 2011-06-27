#ifndef RENDERAREA_H
#define RENDERAREA_H

#include <QWidget>
#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QLabel>
#include "BMPYUVImage.h"

class RoboImageViewer: public QWidget
{
    Q_OBJECT
public:

    RoboImageViewer(const man::memory::RoboImage* roboImage,
    		QLabel *infoLabel = NULL,
    		QWidget *parent = NULL);
    virtual ~RoboImageViewer();
    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    void updateBitmap();

protected:
    void paintEvent(QPaintEvent *event);

private:
    BMPYUVImage *image;
    QLabel* infoLabel;

};

#endif // RENDERAREA_H
