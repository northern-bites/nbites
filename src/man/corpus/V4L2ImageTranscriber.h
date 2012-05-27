/*
 * V4L2ImageTranscriber.h
 *
 *  Created on: Jun 27, 2011
 *      Author: Octavian Neamtu
 *     Updated: April 18, 2012
 *      Author: Lizzie Mamantov
 *
 * Code initially borrowed from B-Human in 2011. (NaoCamera)
 * License included below.
 * Was not used in the Northern Bites code base until April 2012,
 * when it was reworked for the Nao V4 cameras.
 *
 * LICENSE
 * ------------------------------------------------------------------
 * Copyright (c) 2011 B-Human.  All rights reserved.
 *
 * Parts of this distribution were not developed by B-Human.
 * This licence doesn't apply to these parts, the rights of the
 * copyright owners remain.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright  notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * 3. The end-user documentation included with the redistribution, if
 *    any, must include the following acknowledgment:
 *    "This product includes software developed by B-Human
 *     (http://www.b-human.de)."
 *    Alternately, this acknowledgment may appear in the software
 *    itself, if and wherever such third-party acknowledgments
 *    normally appear.
 *
 * 4. If the source code or parts of the source code shall be used
 *    for a RoboCup competition, the competing program must differ in
 *    at least several major parts from the original distribution.
 *    Additionally the usage shall be announced in the SPL mailing
 *    list (currently robocup-nao@cc.gatech.edu) one month before the
 *    competition. The announcement shall name which the parts of
 *    this code are used. It shall also contain a description of the
 *    own contribution.
 *
 * THIS SOFTWARE IS PROVIDED BY B-HUMAN ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * B-HUMAN NOR ITS MEMBERS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ------------------------------------------------------------------
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <linux/videodev2.h>

#include "ThreadedImageTranscriber.h"
#include "Camera.h"
#include "ColorParams.h"

namespace man {
namespace corpus {

class V4L2ImageTranscriber: public ImageTranscriber {

public:

    V4L2ImageTranscriber(boost::shared_ptr<Sensors> s, Camera::Type which);
    virtual ~V4L2ImageTranscriber();

    void setNewSettings(const Camera::Settings& settings);
    const Camera::Settings* getSettings() const {
        return &settings;
    }

    bool waitForImage();
    bool releaseBuffer();
    void releaseImage(){}

    /**
     * Note: this method blocks until it gets a new image
     */
    bool captureNew();
    unsigned long long getTimeStamp() const;

    void assertCameraSettings();

    /**
     * Unconditional write of the camera settings
     */
    void writeCameraSettings();

    void initTable(const std::string& path);

private:
    Camera::Settings settings;
    Camera::Type cameraType;

    int cameraAdapterFd;

    enum {
        frameBufferCount = 4, /**< Amount of available frame buffers. */
        WIDTH = 640, HEIGHT = 480, SIZE = WIDTH * HEIGHT * 2
    };
    int fd;
    void* mem[frameBufferCount]; /**< Frame buffer addresses. */
    int memLength[frameBufferCount]; /* The length of each frame buffer. */
    /* Reusable parameter struct for some ioctl calls. */
    struct v4l2_buffer* buf;

    struct v4l2_buffer* currentBuf; /**< The last dequeued frame buffer. */
    unsigned long long timeStamp;

    uint16_t* image;
    unsigned char *table;
    ColorParams params;

    int getControlSetting(unsigned int id);
    bool setControlSetting(unsigned int id, int value);

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

    // Can be used to get info about controls
    void enumerate_menu();
    void enumerate_controls();
    struct v4l2_queryctrl queryctrl;
    struct v4l2_querymenu querymenu;

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
