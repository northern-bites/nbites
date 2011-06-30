/*
 * V4L2ImageTranscriber.h
 *
 *  Created on: Jun 27, 2011
 *      Author: oneamtu
 *      Credits go to Colin Graf and Thomas Rofer of BHuman from where this
 *      code is mostly inspired from
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <linux/videodev2.h>

#include "ThreadedImageTranscriber.h"
#include "Camera.h"
#include "ColorParams.h"

namespace man {
namespace corpus {

class V4L2ImageTranscriber: public ThreadedImageTranscriber {

public:

    V4L2ImageTranscriber(boost::shared_ptr<Synchro> synchro,
            boost::shared_ptr<Sensors> s);
    virtual ~V4L2ImageTranscriber();

    void setSettings(const Camera::Settings& settings);
    const Camera::Settings* getSettings() const {
        return &settings;
    }

    int start();
    void run();
    void stop();
    bool waitForImage();
    void releaseImage(){}
    /**
     * Note: this method blocks until it gets a new image
     */
    bool captureNew();
    const unsigned char* getImage() const;
    unsigned long long getTimeStamp() const;

    Camera switchToUpper();
    Camera switchToLower();
    Camera switchCamera(Camera camera);
    void assertCameraSettings();

    /**
     * Unconditional write of the camera settings
     */
    void writeCameraSettings();

    void initTable(std::string path);
    void initTable(unsigned char* buffer);

private:
    Camera::Settings settings;

#ifndef NO_NAO_EXTENSIONS
    int cameraAdapterFd;
#endif

    enum {
        frameBufferCount = 3, /**< Amount of available frame buffers. */
        WIDTH = 640, HEIGHT = 480, SIZE = WIDTH * HEIGHT * 2
    };
    int fd;
    void* mem[frameBufferCount]; /**< Frame buffer addresses. */
    int memLength[frameBufferCount]; /**< The length of each frame buffer. */
    struct v4l2_buffer* buf; /**< Reusable parameter struct for some ioctl calls. */
    struct v4l2_buffer* currentBuf; /**< The last dequeued frame buffer. */
    unsigned long long timeStamp;

    uint16_t* image;
    unsigned char *table;
    ColorParams params;

    int getControlSetting(unsigned int id);
    bool setControlSetting(unsigned int id, int value);

    void initOpenI2CAdapter();
    void initSelectCamera();
    void initOpenVideoDevice();
    void initSetCameraDefaults();
    void initSetImageFormat();
    void initSetFrameRate();
    void initRequestAndMapBuffers();
    void initQueueAllBuffers();
    void initDefaultControlSettings();
    void startCapturing();

    enum {
        y0 = 0,
        u0 = 0,
        v0 = 0,

        y1 = 256,
        u1 = 256,
        v1 = 256,

        yLimit = 128,
        uLimit = 128,
        vLimit = 128,

        tableByteSize = yLimit * uLimit * vLimit
    };
};
} /* namespace corpus */
} /* namespace man */
