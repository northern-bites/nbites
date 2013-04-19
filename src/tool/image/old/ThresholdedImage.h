/**
 * @class ThresholdedImage
 *
 * Extends BMPImage and fills a bmp with the contents of a thresholdedimage
 *
 * @author Octavian Neamtu
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include "BMPImage.h"
#include "memory/MObjects.h"
#include "Color.h"

namespace qtool {
namespace image {

class ThresholdedImage : public BMPImage
{
    typedef boost::shared_ptr<const man::memory::proto::PRawImage> ImagePtr;

public:
    ThresholdedImage(
            ImagePtr rawImage,
            QObject* parent = 0, byte filter = 0xFF);
    virtual ~ThresholdedImage() {};

    void buildBitmap();
    void scaleBitmap_640_480();
    void scaleBitmap_320_240();

    void setFilter(byte _filter) { filter = _filter; this->updateBitmap(); }

    unsigned getWidth() const { return rawImage->width(); }
    unsigned getHeight() const { return rawImage->height(); }

protected:
    bool needToResizeBitmap() const;

protected:
    ImagePtr rawImage;
    byte filter;

};

}
}
