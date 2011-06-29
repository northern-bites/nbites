/*
 * V4L2ImageTranscriber.h
 *
 *  Created on: Jun 27, 2011
 *      Author: oneamtu
 *      Credits go to Colin Graf and Thomas Rofer of BHuman from which this
 *      code is mostly inspired from
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include "ThreadedImageTranscriber.h"
#include "Camera.h"

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
};
} /* namespace corpus */
} /* namespace man */
