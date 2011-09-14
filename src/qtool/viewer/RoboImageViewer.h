#ifndef RENDERAREA_H
#define RENDERAREA_H

#include <QWidget>
#include <QLabel>
#include "data/DataTypes.h"
#include "man/include/Subscriber.h"
#include "image/BMPYUVImage.h"
#include <iostream>

namespace qtool {
namespace viewer {

class RoboImageViewer: public QWidget,
	public Subscriber<qtool::data::MObject_ID>
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

    void update(qtool::data::MObject_ID);

protected:
    void paintEvent(QPaintEvent *event);

private:
    BMPYUVImage *image;
    QLabel* infoLabel;

};

}
}

#endif // RENDERAREA_H
