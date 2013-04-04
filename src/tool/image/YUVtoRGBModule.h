#pragma once

#include "RoboGrams.h"
#include "Images.h"

typedef unsigned char byte;
typedef unsigned int rgbvalue;
typedef messages::PackedImage<rgbvalue> RGBImage;

namespace tool {
namespace image {

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
