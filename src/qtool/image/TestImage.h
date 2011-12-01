/**
 * @class TestImage
 * 
 * Tests drawing an image
 *
 */

#pragma once

#include "BMPImage.h"
#include <QImage>

namespace qtool {
namespace image {


class TestImage : public BMPImage
{

 public:
    TestImage();
    virtual ~TestImage() {};

    void updateBitmap();

    unsigned getWidth() {return 640;}
    unsigned getHeight() {return 480;}

    
};

}
}
