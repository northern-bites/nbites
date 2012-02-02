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
#include "ClassHelper.h"

namespace qtool {
namespace image {

enum ChannelType {
	RGB,
	Y,
	U,
	V,
	Red,
	Green,
	Blue,
	Hue,
	Saturation,
	Value,
	NUM_CHANNELS
};

static const std::string ChannelType_label[] = {
    "RGB",
    "Y",
    "U",
    "V",
    "Red",
    "Green",
    "Blue",
    "Hue",
    "Saturation",
    "Value"
};

class BMPYUVImage : public BMPImage
{

    ADD_SHARED_PTR(BMPYUVImage);

public:
    BMPYUVImage(man::memory::MImage::const_ptr rawImage, ChannelType type = RGB);
    virtual ~BMPYUVImage() {};

    void buildBitmap();

    ChannelType getCurrentBitmapType() const { return bitmapType; }
    void setBitmapType(ChannelType type) { bitmapType = type; updateBitmap();}

    unsigned getWidth() const { return yuvImage.getWidth(); }
    unsigned getHeight() const { return yuvImage.getHeight(); }

protected:
    bool needToResizeBitmap() const;

protected:
    ChannelType bitmapType;
    YUVImage yuvImage;

};

}
}
