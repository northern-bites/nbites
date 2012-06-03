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
//#include <linux/i2c.h>
#define __STRICT_ANSI__

#include <cerrno>

#include "ImageAcquisition.h"
#include "Profiler.h"

#ifndef V4L2_CID_AUTOEXPOSURE
#  define V4L2_CID_AUTOEXPOSURE     (V4L2_CID_BASE+32)
#endif

#ifndef V4L2_CID_CAM_INIT
#  define V4L2_CID_CAM_INIT         (V4L2_CID_BASE+33)
#endif

#ifndef V4L2_CID_POWER_LINE_FREQUENCY
#  define V4L2_CID_POWER_LINE_FREQUENCY  (V4L2_CID_BASE+24)

#define V4L2_CID_HUE_AUTO      (V4L2_CID_BASE+25)
#define V4L2_CID_WHITE_BALANCE_TEMPERATURE  (V4L2_CID_BASE+26)
//#define V4L2_CID_SHARPNESS      (V4L2_CID_BASE+27)
//#define V4L2_CID_BACKLIGHT_COMPENSATION   (V4L2_CID_BASE+28)

#define V4L2_CID_CAMERA_CLASS_BASE     (V4L2_CTRL_CLASS_CAMERA | 0x900)

#define V4L2_CID_EXPOSURE_AUTO      (V4L2_CID_CAMERA_CLASS_BASE+1)
enum v4l2_exposure_auto_type {
    V4L2_EXPOSURE_MANUAL = 0,
    V4L2_EXPOSURE_AUTO = 1,
    V4L2_EXPOSURE_SHUTTER_PRIORITY = 2,
    V4L2_EXPOSURE_APERTURE_PRIORITY = 3
};

//#define V4L2_CID_EXPOSURE_ABSOLUTE    (V4L2_CID_CAMERA_CLASS_BASE+2)
//#define V4L2_CID_EXPOSURE_AUTO_PRIORITY    (V4L2_CID_CAMERA_CLASS_BASE+3)

#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26) */

// For checking the ioctls; prints error if one occurs
#define VERIFY(x, str) {                               \
        if( (x) < 0) {                                 \
            printf("CAMERA ERROR::");                  \
            printf(str);                               \
            printf("\n");                              \
            printf("System Error Message: %s\n",        \
                   strerror(errno));                   \
        }                                              \
    }

namespace man {
namespace corpus {

using boost::shared_ptr;

V4L2ImageTranscriber::V4L2ImageTranscriber(shared_ptr<Sensors> s,
                                           Camera::Type which) :
    ImageTranscriber(s),
    settings(Camera::getDefaultSettings()),
    cameraType(which),
    currentBuf(0),
    timeStamp(0),
    image(reinterpret_cast<uint16_t*>(new uint8_t[IMAGE_BYTE_SIZE])),
    table(new unsigned char[yLimit * uLimit * vLimit]),
    params(y0, u0, v0, y1, u1, v1, yLimit, uLimit, vLimit){

    initTable("/home/nao/nbites/lib/table/table.mtb");

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
    unsigned char cmd[2] = { cameraType, 0 };
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

    if(fmt.fmt.pix.sizeimage != SIZE)
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

    if(rb.count != frameBufferCount)
        printf("CAMERA ERROR::Buffer count is WRONG.\n");

    // map or prepare the buffers
    buf = static_cast<struct v4l2_buffer*>(calloc(1,
            sizeof(struct v4l2_buffer)));
    for(    int i = 0; i < frameBufferCount; ++i)
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
    // The following should be 1 if we're using the top camera (?)
    if(cameraType == Camera::TOP)
    {
        setControlSetting(V4L2_CID_HFLIP, 1);
        setControlSetting(V4L2_CID_VFLIP, 1);
    }
    else
    {
        setControlSetting(V4L2_CID_HFLIP, 0);
        setControlSetting(V4L2_CID_VFLIP, 0);
    }

    // Auto exposure on (buggy driver, blah)
    setControlSetting(V4L2_CID_EXPOSURE_AUTO, 1);

    // Set most settings with auto exposure off
    setControlSetting(V4L2_CID_BRIGHTNESS, settings.brightness);
    setControlSetting(V4L2_CID_CONTRAST, settings.contrast);
    setControlSetting(V4L2_CID_SATURATION, settings.saturation);
    setControlSetting(V4L2_CID_HUE, settings.hue);
    setControlSetting(V4L2_CID_SHARPNESS, 3);

    // Auto white balance and backlight comp off!
    setControlSetting(V4L2_CID_AUTO_WHITE_BALANCE, 0);
    setControlSetting(V4L2_CID_BACKLIGHT_COMPENSATION, 0x00);

    // Auto exposure back off
    setControlSetting(V4L2_CID_EXPOSURE_AUTO, 0);

    setControlSetting(V4L2_CID_EXPOSURE, settings.exposure);
    setControlSetting(V4L2_CID_GAIN, settings.gain);

    // This is actually just the white balance setting!
    setControlSetting(V4L2_CID_DO_WHITE_BALANCE, -60);
}

void V4L2ImageTranscriber::startCapturing() {
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    VERIFY((ioctl(fd, VIDIOC_STREAMON, &type)),
           "Start capture failed.");
}

void V4L2ImageTranscriber::setNewSettings(const Camera::Settings& newset) {
    if (newset.exposure != settings.exposure)
    {
        setControlSetting(V4L2_CID_EXPOSURE,
                (settings.exposure = newset.exposure));
    }
    if (newset.brightness != settings.brightness)
    {
        setControlSetting(V4L2_CID_BRIGHTNESS,
                (settings.brightness = newset.brightness));
    }
    if (newset.contrast != settings.contrast)
    {
        setControlSetting(V4L2_CID_CONTRAST,
                (settings.contrast = newset.contrast));
    }
    if (newset.gain != settings.gain)
    {
        setControlSetting(V4L2_CID_GAIN, (settings.gain = newset.gain));
    }
    if (newset.blue_chroma != settings.blue_chroma)
    {
        setControlSetting(V4L2_CID_BLUE_BALANCE,
                          (settings.blue_chroma = newset.blue_chroma));
    }
    if (newset.red_chroma != settings.red_chroma)
    {
        setControlSetting(V4L2_CID_RED_BALANCE,
                          (settings.red_chroma = newset.red_chroma));
    }
}

bool V4L2ImageTranscriber::waitForImage() {
    PROF_ENTER(P_DQBUF);
    this->captureNew();
    PROF_EXIT(P_DQBUF);
    uint8_t* current_image = static_cast<uint8_t*>(mem[currentBuf->index]);
    if (current_image) {
        sensors->updateVisionAngles();
        PROF_ENTER(P_ACQUIRE_IMAGE);
#ifdef CAN_SAVE_FRAMES
       if (sensors->getWriteableNaoImage(cameraType) != NULL) {
           _copy_image(current_image,
                       sensors->getWriteableNaoImage(cameraType));
           sensors->notifyNewNaoImage();
           ImageAcquisition::acquire_image_fast(table, params,
                                               sensors->getNaoImage(cameraType),
                                               image);
       } else {
           ImageAcquisition::acquire_image_fast(table, params,
                                                current_image, image);
       }
#else
        ImageAcquisition::acquire_image_fast(table, params,
                current_image, image);
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
    if(buf->bytesused != SIZE)
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

    // Have to make sure the setting "sticks"
    while(getControlSetting(id) != value)
    {
        if (ioctl(fd, VIDIOC_S_CTRL, &control_s) < 0)
        {
            printf("CAMERA::Warning::Control setting failed.\n");
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
    int exposure = getControlSetting(V4L2_CID_EXPOSURE);
    int brightness = getControlSetting(V4L2_CID_BRIGHTNESS);
    int contrast = getControlSetting(V4L2_CID_CONTRAST);
    int red = getControlSetting(V4L2_CID_RED_BALANCE);
    int blue = getControlSetting(V4L2_CID_BLUE_BALANCE);
    int gain = getControlSetting(V4L2_CID_GAIN);

    if (settings.exposure != Camera:: KEEP_DEFAULT &&
        exposure != settings.exposure) {
        printf("CAMERA::WARNING::Exposure setting is wrong:");
        printf(" is %d, not %d.\n", exposure, settings.exposure);
        allFine = false;
    }
    if (settings.brightness != Camera:: KEEP_DEFAULT &&
        brightness != settings.brightness) {
        printf("CAMERA::WARNING::Brightness setting is wrong:");
        printf(" is %d, not %d.\n", brightness, settings.brightness);
        allFine = false;
    }
    if (settings.contrast != Camera:: KEEP_DEFAULT &&
        contrast != settings.contrast) {
        printf("CAMERA::WARNING::Contrast setting is wrong:");
        printf(" is %d, not %d.\n", contrast, settings.contrast);
        allFine = false;
    }
    if (settings.red_chroma != Camera:: KEEP_DEFAULT &&
        red != settings.red_chroma) {
        printf("CAMERA::WARNING::Red chroma setting is wrong:");
        printf(" is %d, not %d.\n", red, settings.red_chroma);
        allFine = false;
    }
    if (settings.blue_chroma != Camera:: KEEP_DEFAULT &&
        blue != settings.blue_chroma) {
        printf("CAMERA::WARNING::Blue chroma setting is wrong:");
        printf(" is %d, not %d.\n", blue, settings.blue_chroma);
        allFine = false;
    }
    if (settings.gain != Camera:: KEEP_DEFAULT &&
        gain != settings.gain) {
        printf("CAMERA::WARNING::Gain setting is wrong:");
        printf(" is %d, not %d.\n", gain, settings.gain);
        allFine = false;
    }

    if (allFine) {
        printf("CAMERA::");
        if(cameraType == Camera::BOTTOM)
            printf("Bottom camera settings were set correctly.\n");
        else printf("Top camera settings were set correctly.\n");
    }
}

void V4L2ImageTranscriber::writeCameraSettings() {
    setControlSetting(V4L2_CID_EXPOSURE, settings.exposure);
    setControlSetting(V4L2_CID_BRIGHTNESS, settings.brightness);
    setControlSetting(V4L2_CID_CONTRAST, settings.contrast);
    setControlSetting(V4L2_CID_GAIN, settings.gain);
    setControlSetting(V4L2_CID_BLUE_BALANCE, settings.blue_chroma);
    setControlSetting(V4L2_CID_RED_BALANCE, settings.red_chroma);
}

} /* namespace corpus */
} /* namespace man */
