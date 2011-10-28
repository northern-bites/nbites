/**
 * @class BMPYUVImage
 *
 * Extends YUVImage in that it keeps an instance of a QImage (a bitmap) that we
 * update each time the image gets updated
 *
 */

#include <QGraphicsItem>
#include <QString>
#include <QImage>
#include <QColor>
#include <QPainter>
#include "ColorSpace.h"
#include "YUVImage.h"

enum BitmapType {
	Color,
	Y,
	U,
	V,
	Red,
	Green,
	Blue,
	Hue,
	Saturation,
	Value
};

class BMPYUVImage : public YUVImage
{

public:
    BMPYUVImage(man::memory::MImage::const_ptr rawImage);
    virtual ~BMPYUVImage() {};
    virtual void updateFromRawImage();
    void updateBitmap();

    BitmapType getCurrentBitmapType() const { return bitmapType; }
    void setBitmapType(BitmapType type) { bitmapType = type; updateBitmap();}

    QImage getBitmap() const { return bitmap; }


private:
    BitmapType bitmapType;
    QImage bitmap;

};
