/**
 * @class BMPImage
 *
 * Wraps a basic bitmap image
 *
 * @author Octavian Neamtu
 */

#pragma once

#include <QImage>
#include "ClassHelper.h"

namespace qtool {
namespace image {

class BMPImage
{

ADD_NULL_INSTANCE(BMPImage);
ADD_SHARED_PTR(BMPImage);

public:
    BMPImage() {}
    virtual ~BMPImage() {};
    virtual void updateBitmap() {};

    QImage getBitmap() const { return bitmap; }

    virtual unsigned getWidth() {return 0;}
    virtual unsigned getHeight() {return 0;}

protected:
    QImage bitmap;

};

}
}
