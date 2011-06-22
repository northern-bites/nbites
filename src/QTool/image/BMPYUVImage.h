/**
 * @class BMPYUVImage
 *
 * Extends YUVImage in that it keeps an instance of a QImage that we update
 * each time the image gets updated
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
    BMPYUVImage(const RoboImage* _roboImage);
    virtual ~BMPYUVImage() {};
    virtual void updateFromRoboImage();
    void updateBitmap();

    BitmapType getCurrentBitmapType() const { return bitmapType; }
    void setBitmapType(BitmapType type) { bitmapType = type; updateBitmap();}

    QImage getBitmap(BitmapType type = Color) const { return bitmap; }


private:
    BitmapType bitmapType;
    QImage bitmap;

};
