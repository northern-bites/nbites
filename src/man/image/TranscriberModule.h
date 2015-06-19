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
#include "PMotion.pb.h"
#include "InertialState.pb.h"
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

// Specialized pixel buffer for holding pixels in video memory
// IMPORTANT see images.h for more information on pixel buffers
// fd and v4l2_buffer needed to access video (kernel-land) memory
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

// The transcriber itself, returns image class of pixels from video memory
class ImageTranscriber
{
public:
    ImageTranscriber(Camera::Type);
    ~ImageTranscriber();

    // The heart of the transcriber, clients calls this for new image
    messages::YUVImage getNextImage();
    uint64_t getTimestamp() const;
    Camera::Type type() { return cameraType; }

    struct NewSettings {
        bool hflip;
        bool vflip;
        int auto_exposure;
        int brightness;
        int contrast;
        int saturation;
        int hue;
        int sharpness;
        int gamma;
        bool auto_whitebalance;
        int backlight_compensation;
        int exposure;
        int gain;
        int white_balance;
        bool fade_to_black;
    };

    void initSettings(); //one of the magical init methods

    NewSettings updated_settings; //struct containing updated settings

private:
    enum
    {
        WIDTH_TOP_CAMERA = NAO_IMAGE_WIDTH,
        HEIGHT_TOP_CAMERA = NAO_IMAGE_HEIGHT,
        NUM_BUFFERS = 4
    };

    // All of the (magical) init methods
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
    void testControlSettings();

    // Helpers for controlling the camera's settings
    int getControlSetting(unsigned int id);
    bool setControlSetting(unsigned int id, int value);

    // @see Camera.h
    Camera::Settings settings;
    Camera::Type cameraType;

    int width;
    int height;
    int size;

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

// Module that wraps Transcriber's functionality
// Outportals image from video memory
class TranscriberModule : public portals::Module
{
public :
    TranscriberModule(ImageTranscriber&);

    portals::InPortal<messages::JointAngles> jointsIn;
    portals::InPortal<messages::InertialState> inertsIn;

    portals::OutPortal<messages::YUVImage> imageOut;
    portals::OutPortal<messages::JointAngles> jointsOut;
    portals::OutPortal<messages::InertialState> inertsOut;

    time_t file_mod_time;
    int first_time;

protected :
    virtual void run_();
private :
    ImageTranscriber& it;
    size_t image_index;
};

}
}
