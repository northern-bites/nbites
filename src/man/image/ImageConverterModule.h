#pragma once

#include "ImageAcquisition.h"
#include "Images.h"
#include "RoboGrams.h"
#include "Camera.h"
#include "ColorParams.h"

namespace man {
namespace image {

class ImageConverterModule : public portals::Module {

public:
    ImageConverterModule(Camera::Type);
    virtual ~ImageConverterModule() {}

    portals::InPortal<messages::YUVImage> imageIn;
    portals::OutPortal<messages::PackedImage16> yImage;
    portals::OutPortal<messages::PackedImage16> uImage;
    portals::OutPortal<messages::PackedImage16> vImage;
    portals::OutPortal<messages::ThresholdImage> thrImage;

    // Used offline
    void initTable(unsigned char* otherTable);

protected:
    virtual void run_();

    // Used on the robot
    void initTable(const std::string& filename);

    Camera::Type whichCamera;
    ColorParams params;
    unsigned char *table;

    enum InitTableConstants {
        y0 = 0,
        u0 = 0,
        v0 = 0,
        y1 = 256,
        u1 = 256,
        v1 = 256,
        yLimit = 128,
        uLimit = 128,
        vLimit = 128,
        tableByteSize = (yLimit * uLimit * vLimit)
    };
};

}
}
