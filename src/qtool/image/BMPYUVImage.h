/**
 * @class BMPYUVImage
 *
 * Extends YUVImage in that it keeps an instance of a QImage (a bitmap) that we
 * update each time the image gets updated
 *
 */

#pragma once

#include "BMPImage.h"
#include "Color.h"
#include "YUVImage.h"

namespace qtool {
namespace image {

enum BitmapType {
	RGB,
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

class BMPYUVImage : public BMPImage
{

public:
    BMPYUVImage(man::memory::MImage::const_ptr rawImage, BitmapType type = RGB);
    virtual ~BMPYUVImage() {};

    void buildBitmap();

    BitmapType getCurrentBitmapType() const { return bitmapType; }
    void setBitmapType(BitmapType type) { bitmapType = type; updateBitmap();}

    unsigned getWidth() const { return yuvImage.getWidth(); }
    unsigned getHeight() const { return yuvImage.getHeight(); }

protected:
    bool needToResizeBitmap() const;

protected:
    BitmapType bitmapType;
    YUVImage yuvImage;

};

}
}
