#pragma once

#include "V4L2ImageTranscriber.h"
#include "ImageAcquisition.h"
#include "ColorParams.h"
//#include "Images.h"
//#include "RoboGrams.h"
#include "ThresholdedImage.h"

// Constants used to init color table -- see initTable()
#define y0 0
#define u0 0
#define v0 0

#define y1 256
#define u1 256
#define v1 256

#define yLimit 128
#define uLimit 128
#define vLimit 128

#define tableByteSize (yLimit * uLimit * vLimit)

namespace man {
namespace image {

class ImageTranscriberModule : public portals::Module {

public:
    ImageTranscriberModule();
    virtual ~ImageTranscriberModule() {}
    portals::OutPortal<messages::YUVImage> topYUVImageOut;
    portals::OutPortal<messages::YUVImage> bottomYUVImageOut;
    portals::OutPortal<messages::ThresholdedImage> topImageOut;
    portals::OutPortal<messages::ThresholdedImage> bottomImageOut;

protected:
    virtual void run_();
    void initTable(const std::string& filename, int which);
    V4L2ImageTranscriber topImageTranscriber;
    V4L2ImageTranscriber bottomImageTranscriber;
    ColorParams params;
    unsigned char *topTable;
    unsigned char *bottomTable;
};

}
}
