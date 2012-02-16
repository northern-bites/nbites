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
#include "memory/MImage.h"
#include "Color.h"

namespace qtool {
namespace image {

class ThresholdedImage : public BMPImage
{

public:
    ThresholdedImage(
            boost::shared_ptr<const man::memory::proto::PImage> rawImage, QObject* parent = 0);
    virtual ~ThresholdedImage() {};

    void buildBitmap();

    unsigned getWidth() const { return rawImage->width(); }
    unsigned getHeight() const { return rawImage->height(); }

protected:
    bool needToResizeBitmap() const;

protected:
    boost::shared_ptr<const man::memory::proto::PImage> rawImage;

};

}
}
