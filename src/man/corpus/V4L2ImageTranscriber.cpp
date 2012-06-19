/*
 * V4L2ImageTranscriber.cpp
 *
 *  Created on: Jun 27, 2011
 *      Author: Octavian Neamtu
 *     Updated: April 18, 2012
 *      Author: Lizzie Mamantov
 *
 * Based on B-Human 2011 code release.
 * See header file for license and further information.
 */

#include "V4L2ImageTranscriber.h"

#include "corpusconfig.h" //for CAN_SAVE_FRAMES

#include <cstring>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#undef __STRICT_ANSI__
#include <linux/version.h>
#include <bn/i2c/i2c-dev.h>
#define __STRICT_ANSI__

#include <cerrno>

#include "ImageAcquisition.h"
#include "Profiler.h"

// For checking the ioctls; prints error if one occurs
#define VERIFY(x, str) {                               \
        if( (x) < 0) {                                 \
            printf("CAMERA ERROR::");                  \
            printf(str);                               \
            printf("\n");                              \
            printf("System Error Message: %s\n",       \
                   strerror(errno));                   \
        }                                              \
    }

namespace man {
namespace corpus {

using boost::shared_ptr;
using namespace memory;

V4L2ImageTranscriber::V4L2ImageTranscriber(boost::shared_ptr<Sensors> s,
                                           Camera::Type which, MRawImages::ptr rawImages) :
    ImageTranscriber(s),
    settings(Camera::getSettings(which)),
    cameraType(which),
    currentBuf(0),
    timeStamp(0),
    image(reinterpret_cast<uint16_t*>(new uint8_t[IMAGE_BYTE_SIZE])),
    table(new unsigned char[yLimit * uLimit * vLimit]),
    params(y0, u0, v0, y1, u1, v1, yLimit, uLimit, vLimit),
    rawImages(rawImages) {

    initOpenI2CAdapter();
    initSelectCamera();
    initOpenVideoDevice();
    initSetCameraDefaults();
    initSetImageFormat();
    initSetFrameRate();
    initRequestAndMapBuffers();
    initQueueAllBuffers();

    // Uncomment if you want info about control settings printed!
    //enumerate_controls();

    initSettings();

    assertCameraSettings();

    startCapturing();
}

V4L2ImageTranscriber::~V4L2ImageTranscriber() {
    // disable streaming
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    VERIFY((ioctl(fd, VIDIOC_STREAMOFF, &type)),
           "Capture stop failed.");

    // unmap buffers
    for (int i = 0; i < frameBufferCount; ++i)
        munmap(mem[i], memLength[i]);

    // close the device
    close(cameraAdapterFd);
    close(fd);
    free(buf);
}

void V4L2ImageTranscriber::initTable(const string& filename)
{
    FILE *fp = fopen(filename.c_str(), "r");   //open table for reading

    if (fp == NULL) {
        printf("CAMERA::ERROR::initTable() FAILED to open filename: %s\n",
               filename.c_str());
#ifdef OFFLINE
        exit(0);
#else
        return;
#endif
    }

    // actually read the table into memory
    // Color table is in VUY ordering
    int rval;
    for(int v=0; v < vLimit; ++v){
        for(int u=0; u< uLimit; ++u){
            rval = fread(&table[v * uLimit * yLimit + u * yLimit],
                         sizeof(unsigned char), yLimit, fp);
        }
    }

#ifndef OFFLINE
    printf("CAMERA::Loaded colortable %s.\n",filename.c_str());
#endif

    fclose(fp);
}

void V4L2ImageTranscriber::initOpenI2CAdapter() {
    if(cameraType == Camera::TOP)
        cameraAdapterFd = open("/dev/i2c-camera0", O_RDWR);
    else
        cameraAdapterFd = open("/dev/i2c-camera1", O_RDWR);

    if(cameraAdapterFd == -1)
        printf("CAMERA::ERROR::Camera adapter FD is WRONG.\n");
    VERIFY((ioctl(cameraAdapterFd, 0x703, 8)),
           "Opening I2C adapter failed.");
}

void V4L2ImageTranscriber::initSelectCamera() {
    unsigned char cmd[2] = { (unsigned char) cameraType, 0 };
    i2c_smbus_write_block_data(cameraAdapterFd, 220, 1, cmd);
}

void V4L2ImageTranscriber::initOpenVideoDevice() {
    // open device
    if(cameraType == Camera::TOP)
        fd = open("/dev/video0", O_RDWR);
    else
        fd = open("/dev/video1", O_RDWR);

    if(fd == -1)
        printf("CAMERA::ERROR::Video Device FD is WRONG.\n");
}

void V4L2ImageTranscriber::initSetCameraDefaults() {
    v4l2_std_id esid0 = WIDTH == 320 ? 0x04000000UL : 0x08000000UL;
    VERIFY((ioctl(fd, VIDIOC_S_STD, &esid0)),
           "Setting default parameters failed.");
}

void V4L2ImageTranscriber::initSetImageFormat() {
    // set format
    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(struct v4l2_format));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    VERIFY((ioctl(fd, VIDIOC_S_FMT, &fmt)),
           "Setting image format failed.");

    if(fmt.fmt.pix.sizeimage != (unsigned int)SIZE)
        printf("CAMERA ERROR::Size setting is WRONG.\n");
}

void V4L2ImageTranscriber::initSetFrameRate() {
    // set frame rate
    struct v4l2_streamparm fps;
    memset(&fps, 0, sizeof(struct v4l2_streamparm));
    fps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    VERIFY((ioctl(fd, VIDIOC_G_PARM, &fps)),
           "Getting FPS failed.");
    fps.parm.capture.timeperframe.numerator = 1;
    fps.parm.capture.timeperframe.denominator = 30;
    VERIFY((ioctl(fd, VIDIOC_S_PARM, &fps)),
           "Setting FPS failed.");
}

void V4L2ImageTranscriber::initRequestAndMapBuffers() {
    // request buffers
    struct v4l2_requestbuffers rb;
    memset(&rb, 0, sizeof(struct v4l2_requestbuffers));
    rb.count = frameBufferCount;
    rb.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    rb.memory = V4L2_MEMORY_MMAP;
    VERIFY((ioctl(fd, VIDIOC_REQBUFS, &rb)),
           "Requesting buffers failed.");

    if(rb.count != (unsigned int)frameBufferCount)
        printf("CAMERA ERROR::Buffer count is WRONG.\n");

    // map or prepare the buffers
    buf = static_cast<struct v4l2_buffer*>(calloc(1,
            sizeof(struct v4l2_buffer)));
    for(int i = 0; i < frameBufferCount; ++i)
    {
        buf->index = i;
        buf->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf->memory = V4L2_MEMORY_MMAP;
        VERIFY((ioctl(fd, VIDIOC_QUERYBUF, buf)),
               "Querying buffer failed.");
        memLength[i] = buf->length;
        mem[i] = mmap(0, buf->length, PROT_READ | PROT_WRITE,
                      MAP_SHARED, fd, buf->m.offset);
        if(mem[i] == MAP_FAILED)
            printf("CAMERA ERROR::Map failed.\n");
    }
}

void V4L2ImageTranscriber::initQueueAllBuffers() {
    // queue the buffers
    for (int i = 0; i < frameBufferCount; ++i) {
        buf->index = i;
        buf->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf->memory = V4L2_MEMORY_MMAP;
        if(ioctl(fd, VIDIOC_QBUF, buf) == -1)
            printf("Queueing a buffer failed.\n");
    }
}

// Taken from V4L2 specs example
// If you need to determine info about driver, use this method
void V4L2ImageTranscriber::enumerate_controls()
{
    memset (&queryctrl, 0, sizeof (queryctrl));

    printf("Public controls:\n");
    for (queryctrl.id = V4L2_CID_BASE;
         queryctrl.id < V4L2_CID_LASTP1;
         queryctrl.id++) {
        if (0 == ioctl (fd, VIDIOC_QUERYCTRL, &queryctrl)) {
            if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
                continue;

            printf ("Control %s", queryctrl.name);
            printf (" has id %d,", queryctrl.id);
            printf (" steps %d,", queryctrl.step);
            printf (" and min %d, max %d.\n\n", queryctrl.minimum,
                    queryctrl.maximum);

            if (queryctrl.type == V4L2_CTRL_TYPE_MENU)
                enumerate_menu ();
        } else {
            if (errno == EINVAL)
                continue;

            perror ("VIDIOC_QUERYCTRL");
            exit (EXIT_FAILURE);
        }
    }

    printf("Private controls:\n");
    for (queryctrl.id = V4L2_CID_PRIVATE_BASE;;
         queryctrl.id++) {
        if (0 == ioctl (fd, VIDIOC_QUERYCTRL, &queryctrl)) {
            if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
                continue;

            printf ("Control %s\n", queryctrl.name);

            if (queryctrl.type == V4L2_CTRL_TYPE_MENU)
                enumerate_menu ();
        } else {
            if (errno == EINVAL)
                break;

            perror ("VIDIOC_QUERYCTRL");
            exit (EXIT_FAILURE);
        }
    }

    /* have to look for auto exposure separately
       some controls may be much further than the loop looks!
       this is just the most important right now */
    queryctrl.id = V4L2_CID_EXPOSURE_AUTO;

    if (0 == ioctl (fd, VIDIOC_QUERYCTRL, &queryctrl))
    {
            if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
                printf("Disabled.\n");
    }

    printf ("Control %s", queryctrl.name);
    printf (" has id %d", queryctrl.id);
    printf (" and min %d, max %d.\n\n", queryctrl.minimum,
            queryctrl.maximum);

    if (queryctrl.type == V4L2_CTRL_TYPE_MENU)
                enumerate_menu ();
}

void V4L2ImageTranscriber::enumerate_menu ()
{
    printf ("  Menu items:\n");

    memset (&querymenu, 0, sizeof (querymenu));
    querymenu.id = queryctrl.id;

    for (querymenu.index = queryctrl.minimum;
         querymenu.index <= (unsigned)queryctrl.maximum;
         querymenu.index++) {
        if (0 == ioctl (fd, VIDIOC_QUERYMENU, &querymenu)) {
            printf ("  %s\n", querymenu.name);
        }
    }
}

void V4L2ImageTranscriber::initSettings()
{
    // DO NOT SCREW UP THE ORDER BELOW

    setControlSetting(V4L2_CID_HFLIP, settings.hflip);
    setControlSetting(V4L2_CID_VFLIP, settings.vflip);

    // Auto exposure on (buggy driver, blah)
    setControlSetting(V4L2_CID_EXPOSURE_AUTO, 1);

    // Set most settings with auto exposure off
    setControlSetting(V4L2_CID_BRIGHTNESS, settings.brightness);
    setControlSetting(V4L2_CID_CONTRAST, settings.contrast);
    setControlSetting(V4L2_CID_SATURATION, settings.saturation);
    setControlSetting(V4L2_CID_HUE, settings.hue);
    setControlSetting(V4L2_CID_SHARPNESS, settings.sharpness);

    // Auto white balance and backlight comp off!
    setControlSetting(V4L2_CID_AUTO_WHITE_BALANCE,
                      settings.auto_whitebalance);
    setControlSetting(V4L2_CID_BACKLIGHT_COMPENSATION,
                      settings.backlight_compensation);

    // Auto exposure back off
    setControlSetting(V4L2_CID_EXPOSURE_AUTO, settings.auto_exposure);

    setControlSetting(V4L2_CID_EXPOSURE, settings.exposure);
    setControlSetting(V4L2_CID_GAIN, settings.gain);

    // This is actually just the white balance setting!
    setControlSetting(V4L2_CID_DO_WHITE_BALANCE, settings.white_balance);
}

void V4L2ImageTranscriber::startCapturing() {
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    VERIFY((ioctl(fd, VIDIOC_STREAMON, &type)),
           "Start capture failed.");
}

bool V4L2ImageTranscriber::waitForImage() {
    PROF_ENTER(P_DQBUF);
    this->captureNew();
    PROF_EXIT(P_DQBUF);
    uint8_t* current_image = static_cast<uint8_t*>(mem[currentBuf->index]);
    if (current_image) {

        PROF_ENTER(P_ACQUIRE_IMAGE);
#ifdef CAN_SAVE_FRAMES
        //copy the image first into MRawImages so we can stream it
        if (rawImages.get()) {

            proto::PRawImage* rawImage;
            if (cameraType == Camera::TOP) {
                rawImage = rawImages->get()->mutable_topimage();
            } else {
                rawImage = rawImages->get()->mutable_bottomimage();
            }

            if (rawImage->image().size() < (int) SIZE) {
                //allocate the size needed if it's not big enough
                //Note: if we assign too much it might mess up stuff later on
                rawImage->mutable_image()->assign(SIZE * sizeof(byte), 'A');
            }

            //terrible, but necessary to get the image to copy
            //if we don't copy it right into the string, initializing
            //the string from the image byte array will copy it over again,
            //which would slow us down unnecessarily
            //TODO: look more into rawImage.set_image(const char*) and see if it slows us down
            _copy_image(current_image, (uint8_t *)(rawImage->mutable_image()->data()));

            rawImage->set_width(WIDTH);
            rawImage->set_height(HEIGHT);

            // acquire the image directly from the buffer we just copied over; faster
            // than from the original source
            ImageAcquisition::acquire_image_fast(table, params,
                                                 reinterpret_cast<const uint8_t *>(rawImage->image().data()),
                                                 image);
        } else
#else   //syntax magic for the hanging else one line up, don't know if bad or awesome - Octavian
        {
        ImageAcquisition::acquire_image_fast(table, params,
                current_image, image);
        }
#endif
        PROF_EXIT(P_ACQUIRE_IMAGE);

        PROF_ENTER(P_QBUF);
        this->releaseBuffer();
        PROF_EXIT(P_QBUF);
        sensors->setImage(image, cameraType);
        return true;
    } else {
        printf("Warning - the buffer we dequeued was NULL\n");
    }
    return false;
}

bool V4L2ImageTranscriber::captureNew() {
    // dequeue a frame buffer (this call blocks when there is
    // no new image available)

    VERIFY((ioctl(fd, VIDIOC_DQBUF, buf)),
           "Dequeueing the frame buffer failed.");
    if(buf->bytesused != (unsigned int)SIZE)
        printf("CAMERA::ERROR::Wrong buffer size!.\n");
    currentBuf = buf;

    static bool shout = true;
    if (shout) {
        shout = false;
        printf("CAMERA::Camera is working.\n");
    }

    return true;
}

bool V4L2ImageTranscriber::releaseBuffer() {
    if (currentBuf) {
        VERIFY((ioctl(fd, VIDIOC_QBUF, currentBuf)),
               "Releasing buffer failed.");
    }
    return true;
}

// These two methods now assume the control is valid!
int V4L2ImageTranscriber::getControlSetting(unsigned int id) {
    struct v4l2_control control_s;
    control_s.id = id;
    if (ioctl(fd, VIDIOC_G_CTRL, &control_s) < 0)
    {
        printf("CAMERA::Warning::Getting control failed.\n");
        return -1;
    }
    return control_s.value;
}

bool V4L2ImageTranscriber::setControlSetting(unsigned int id, int value) {
    struct v4l2_control control_s;
    control_s.id = id;
    control_s.value = value;

    int counter = 0;

    // Have to make sure the setting "sticks"
    while(getControlSetting(id) != value)
    {
        if (ioctl(fd, VIDIOC_S_CTRL, &control_s) < 0)
        {
            printf("CAMERA::Warning::Control setting failed.\n");
            return false;
        }
	counter++;
	if(counter > 10)
	  {
	    printf("CAMERA::Warning::Timeout while setting a parameter.\n");
	    return false;
	  }
    }
    return true;
}

void V4L2ImageTranscriber::assertCameraSettings() {
    bool allFine = true;
    // check frame rate
    struct v4l2_streamparm fps;
    memset(&fps, 0, sizeof(fps));
    fps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    VERIFY((ioctl(fd, VIDIOC_G_PARM, &fps)),
           "Getting settings failed");

    if (fps.parm.capture.timeperframe.numerator != 1) {
        printf("CAMERA::fps.parm.capture.timeperframe.numerator is wrong.\n");
        allFine = false;
    }
    if (fps.parm.capture.timeperframe.denominator != 30) {
        printf("CAMERA::fps.parm.capture.timeperframe.denominator is wrong.\n");
        allFine = false;
    }

    // check camera settings against what the driver has
    int hflip = getControlSetting(V4L2_CID_HFLIP);
    int vflip = getControlSetting(V4L2_CID_VFLIP);
    int brightness = getControlSetting(V4L2_CID_BRIGHTNESS);
    int contrast = getControlSetting(V4L2_CID_CONTRAST);
    int saturation = getControlSetting(V4L2_CID_SATURATION);
    int hue = getControlSetting(V4L2_CID_HUE);
    int sharpness = getControlSetting(V4L2_CID_SHARPNESS);
    int gain = getControlSetting(V4L2_CID_GAIN);
    int exposure = getControlSetting(V4L2_CID_EXPOSURE);
    int whitebalance = getControlSetting(V4L2_CID_DO_WHITE_BALANCE);

    if (hflip != settings.hflip)
    {
        printf("CAMERA::WARNING::Horizontal flip setting is wrong:");
        printf(" is %d, not %d.\n", hflip, settings.hflip);
        allFine = false;
    }
    if (vflip != settings.vflip)
    {
        printf("CAMERA::WARNING::Vertical flip setting is wrong:");
        printf(" is %d, not %d.\n", vflip, settings.vflip);
        allFine = false;
    }
    if (brightness != settings.brightness)
    {
        printf("CAMERA::WARNING::Brightness setting is wrong:");
        printf(" is %d, not %d.\n", brightness, settings.brightness);
        allFine = false;
    }
    if (contrast != settings.contrast)
    {
        printf("CAMERA::WARNING::Contrast setting is wrong:");
        printf(" is %d, not %d.\n", contrast, settings.contrast);
        allFine = false;
    }
    if (saturation != settings.saturation)
    {
        printf("CAMERA::WARNING::Saturation setting is wrong:");
        printf(" is %d, not %d.\n", saturation, settings.saturation);
        allFine = false;
    }
    if (hue != settings.hue)
    {
        printf("CAMERA::WARNING::Hue setting is wrong:");
        printf(" is %d, not %d.\n", hue, settings.hue);
        allFine = false;
    }
   if (sharpness != settings.sharpness)
    {
        printf("CAMERA::WARNING::Sharpness setting is wrong:");
        printf(" is %d, not %d.\n", sharpness, settings.sharpness);
        allFine = false;
    }
   if (gain != settings.gain)
    {
        printf("CAMERA::WARNING::Gain setting is wrong:");
        printf(" is %d, not %d.\n", gain, settings.gain);
        allFine = false;
    }
   if (exposure != settings.exposure)
    {
        printf("CAMERA::WARNING::Exposure setting is wrong:");
        printf(" is %d, not %d.\n", exposure, settings.exposure);
        allFine = false;
    }
   if (whitebalance != settings.white_balance)
    {
        printf("CAMERA::WARNING::Whitebalance setting is wrong:");
        printf(" is %d, not %d.\n", whitebalance, settings.white_balance);
        allFine = false;
    }

    if (allFine) {
        printf("CAMERA::");
        if(cameraType == Camera::BOTTOM)
            printf("Bottom camera settings were set correctly.\n");
        else printf("Top camera settings were set correctly.\n");
    }
}

} /* namespace corpus */
} /* namespace man */
