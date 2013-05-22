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

    // Gets an image from the Transcriber module (from the camera)
    portals::InPortal<messages::YUVImage> imageIn;
    // Outs four images for vision, YUV split up and a color segmented image
    portals::OutPortal<messages::PackedImage16> yImage;
    portals::OutPortal<messages::PackedImage16> uImage;
    portals::OutPortal<messages::PackedImage16> vImage;
    portals::OutPortal<messages::ThresholdImage> thrImage;

    // Used offline, table can be anywhere on the file system
    void initTable(unsigned char* otherTable);

protected:
    virtual void run_();

    // Used on the robot, table must be in /home/nao/nbites/lib/table
    void initTable(const std::string& filename);

    Camera::Type whichCamera;
    ColorParams params;
    unsigned char *table;

    // Constants used for reading a color table into memory
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
