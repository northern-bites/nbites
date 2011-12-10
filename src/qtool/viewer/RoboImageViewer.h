#ifndef RENDERAREA_H
#define RENDERAREA_H

#include <QWidget>
#include <QPainter>
#include "data/DataTypes.h"
#include "man/include/Subscriber.h"
#include "image/BMPImage.h"
#include "image/TestImage.h"

namespace qtool {
namespace viewer {

class RoboImageViewer: public QWidget,
	public Subscriber<qtool::data::MObject_ID>
{
    Q_OBJECT
public:

    RoboImageViewer(image::BMPImage::ptr image =
		    image::BMPImage::NullInstanceSharedPtr(),
		    image::TestImage::ptr overlay = 
		    image::TestImage::NullInstanceSharedPtr(),
            QWidget *parent = NULL);
    virtual ~RoboImageViewer();
    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    void updateBitmap();

    void update(qtool::data::MObject_ID);
    void setImage(image::BMPImage::ptr _image) { image = _image; }

protected:
    void paintEvent(QPaintEvent *event);

private:
    image::BMPImage::ptr image;
    image::TestImage::ptr overlay;

};

}
}

#endif // RENDERAREA_H
