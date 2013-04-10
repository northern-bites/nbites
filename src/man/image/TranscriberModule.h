#pragma once

/*
 * @class ImageTranscriber
 *
 * Provides a way to request the latest image from the camera.
 */

#include <linux/videodev2.h>
#include <stdint.h>
#include "Camera.h"
#include "VisionDef.h"
#include "Images.h"
#include "RoboGrams.h"

namespace portals {

#define MAKE_FINALIZE(msgType)                              \
template<>                                                  \
inline void MessageHolder<messages::msgType>::finalize()    \
{                                                           \
    message = messages::msgType();                          \
}

MAKE_FINALIZE(YUVImage)
MAKE_FINALIZE(ThresholdImage)
MAKE_FINALIZE(PackedImage16)
MAKE_FINALIZE(PackedImage8)
}

namespace man {
namespace image {

class TranscriberBuffer : public messages::VideoPixelBuffer
{
public:
    TranscriberBuffer(void* pixels, int fd, const struct v4l2_buffer&);
protected:
    virtual ~TranscriberBuffer();
private:
    struct v4l2_buffer releaseBuff;
    int fd;
};

class ImageTranscriber
{
public:
    ImageTranscriber(Camera::Type);
    ~ImageTranscriber();

    messages::YUVImage getNextImage();
    uint64_t getTimestamp() const;

private:
    enum
    {
        WIDTH = NAO_IMAGE_WIDTH,
        HEIGHT = NAO_IMAGE_HEIGHT,
        SIZE = WIDTH * HEIGHT * 2,
        NUM_BUFFERS = 4
    };

    // All of the (magical) init methods
    void initSettings();
    void initOpenI2CAdapter();
    void initSelectCamera();
    void initOpenVideoDevice();
    void initSetCameraDefaults();
    void initSetImageFormat();
    void initSetFrameRate();
    void initRequestAndMapBuffers();
    void initQueueAllBuffers();
    void startCapturing();
    void assertCameraSettings();

    // Helpers for controlling the camera's settings
    int getControlSetting(unsigned int id);
    bool setControlSetting(unsigned int id, int value);

    // @see Camera.h
    Camera::Settings settings;
    Camera::Type cameraType;

    int cameraAdapterFd;
    int fd;

    // Frame buffer addresses.
    void* mem[NUM_BUFFERS];
    // The length of each frame buffer.
    int memLength[NUM_BUFFERS];

    // Reusable parameter struct for some ioctl calls.
    struct v4l2_buffer requestBuff;

    uint64_t timeStamp;
};

class TranscriberModule : public portals::Module
{
public :
    TranscriberModule(ImageTranscriber&);
    portals::OutPortal<messages::YUVImage> imageOut;
protected :
    virtual void run_();
private :
    ImageTranscriber& it;
};

}
}
