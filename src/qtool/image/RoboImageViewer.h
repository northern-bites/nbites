#ifndef RENDERAREA_H
#define RENDERAREA_H

#include <QWidget>
#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QLabel>
#include "data/DataTypes.h"
#include "man/include/Subscriber.h"
#include "BMPYUVImage.h"
#include <iostream>

//TODO: add this to a namespace
class RoboImageViewer: public QWidget,
	public Subscriber<qtool::data::DataEvent>
{
    Q_OBJECT
public:

    RoboImageViewer(man::memory::RoboImage::const_ptr roboImage,
    		QLabel *infoLabel = NULL,
    		QWidget *parent = NULL);
    virtual ~RoboImageViewer();
    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    void updateBitmap();

    void update(qtool::data::DataEvent) {
    	this->updateBitmap();
    }

protected:
    void paintEvent(QPaintEvent *event);

private:
    BMPYUVImage *image;
    QLabel* infoLabel;

};

#endif // RENDERAREA_H
