/**
 * @class TestImage
 * 
 * Tests drawing an image
 *
 */

#pragma once

#include "BMPImage.h"
#include <QImage>
#include "man/memory/MVision.h"

namespace qtool {
namespace image {


class TestImage : public BMPImage
{

 public:
    TestImage(man::memory::MVision::const_ptr visionData);
    virtual ~TestImage() {};

    void updateBitmap();

    unsigned getWidth() {return 640;}
    unsigned getHeight() {return 480;}

 protected:
    man::memory::MVision::const_ptr visionData;
};

}
}
