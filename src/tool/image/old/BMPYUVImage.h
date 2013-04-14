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
#include "Images.h"

namespace tool {
namespace image {

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

public:
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

    ADD_SHARED_PTR(BMPYUVImage);

public:
    BMPYUVImage(memory::MRawImages::const_ptr roboImages,
                man::corpus::Camera::Type which,
                ChannelType type = RGB, QObject* parent = 0);
    virtual ~BMPYUVImage() {};

    void buildBitmap();

    ChannelType getCurrentBitmapType() const { return bitmapType; }
    void setBitmapType(ChannelType type) { bitmapType = type; updateBitmap();}

    const YUVImage* getYUVImage() const { return &yuvImage; }

    unsigned getWidth() const { return bitmap.width(); }
    unsigned getHeight() const { return bitmap.height(); }


protected:
    bool needToResizeBitmap() const;

protected:
    ChannelType bitmapType;
    YUVImage yuvImage;

};

}
}
