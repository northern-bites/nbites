#pragma once

#include "RoboGrams.h"
#include "Images.h"

namespace tool {
namespace image {

typedef unsigned char byte;
typedef unsigned int rgbvalue;
typedef messages::PackedImage<rgbvalue> RGBImage;

class YUVtoRGBModule : public portals::Module
{
public:
    YUVtoRGBModule();

    portals::InPortal<messages::YUVImage> yuvIn;
    portals::OutPortal<RGBImage> rgbOut;

protected:
    virtual void run_();

    RGBImage* convert(messages::YUVImage);
};

}
}
