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
#include "ColorSpace.h"
#include "memory/MImage.h"

namespace qtool {
namespace image {

class ThresholdedImage : public BMPImage
{

public:
    ThresholdedImage(
            boost::shared_ptr<const man::memory::proto::PImage> rawImage);
    virtual ~ThresholdedImage() {};

    void updateBitmap();

    unsigned getWidth() const { return rawImage->width(); }
    unsigned getHeight() const { return rawImage->height(); }

protected:
    bool needToResizeBitmap() const;

protected:
    boost::shared_ptr<const man::memory::proto::PImage> rawImage;

};

}
}
