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

#include <cstring>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <cerrno>
#include <iostream>

#include <linux/version.h>
#include <bn/i2c/i2c-dev.h>

#include "ImageAcquisition.h"
//#include "Profiler.h"

// For checking the ioctls; prints error if one occurs
#define VERIFY(x, str) {                               \
        if( (x) != 0) {                                \
            std::cerr << "CAMERA ERROR::" << str <<    \
                "\nSystem Error Message: " <<          \
                strerror(errno) << std::endl;;         \
        }                                              \
    }

using namespace portals;
using namespace messages;

namespace man {
namespace image {

using boost::shared_ptr;

V4L2ImageTranscriber::V4L2ImageTranscriber(Camera::Type which,
                                           OutPortal<ThresholdedImage>* out) :
    outPortal(out),
    settings(Camera::getSettings(which)),
    cameraType(which),
    currentBuf(0),
    timeStamp(0),
    table(new unsigned char[yLimit * uLimit * vLimit]),
    params(y0, u0, v0, y1, u1, v1, yLimit, uLimit, vLimit)
{
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

void V4L2ImageTranscriber::initTable(const std::string& filename)
{
    FILE *fp = fopen(filename.c_str(), "r");   //open table for reading

    if (fp == NULL) {
        std::cerr << "CAMERA::ERROR::initTable() FAILED to open filename:"
                  << filename.c_str() << std::endl;
        return;
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

    std::cerr << "CAMERA::Loaded colortable " << filename.c_str() <<
        std::endl;
    fclose(fp);
}

void V4L2ImageTranscriber::initOpenI2CAdapter() {
    if(cameraType == Camera::TOP)
    {
        cameraAdapterFd = open("/dev/i2c-camera0", O_RDWR);
    }
    else
    {
        cameraAdapterFd = open("/dev/i2c-camera1", O_RDWR);
    }

    if(cameraAdapterFd == -1)
    {
        std::cerr << "CAMERA::ERROR::Camera adapter FD is WRONG." <<
            std::endl;
    }

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
    {
        fd = open("/dev/video0", O_RDWR);
    }
    else
    {
        fd = open("/dev/video1", O_RDWR);
    }

    if(fd == -1)
    {
        std::cerr << "CAMERA::ERROR::Video Device FD is WRONG." << std::endl;
    }
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
    // We ask for a 640 by 480 image
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    //"In this format each four bytes is two pixels. Each four bytes is two
    //Y's, a Cb and a Cr. Each Y goes to one of the pixels, and the Cb and
    //Cr belong to both pixels. As you can see, the Cr and Cb components have
    //half the horizontal resolution of the Y component."
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    VERIFY((ioctl(fd, VIDIOC_S_FMT, &fmt)),
           "Setting image format failed.");

    if(fmt.fmt.pix.sizeimage != (unsigned int)SIZE)
        std::cerr << "CAMERA ERROR::Size setting is WRONG." << std::endl;
}

void V4L2ImageTranscriber::initSetFrameRate() {
    // We want frame rate to be 30 fps
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
    {
        std::cerr << "CAMERA ERROR::Buffer count is WRONG." << std::endl;
    }

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
            std::cerr << "CAMERA ERROR::Map failed." << std::endl;
    }
}

void V4L2ImageTranscriber::initQueueAllBuffers() {
    // queue the buffers
    for (int i = 0; i < frameBufferCount; ++i) {
        buf->index = i;
        buf->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf->memory = V4L2_MEMORY_MMAP;
        if(ioctl(fd, VIDIOC_QBUF, buf) == -1)
            std::cerr << "Queueing a buffer failed." << std::endl;
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

bool V4L2ImageTranscriber::acquireImage() {
    //PROF_ENTER(P_DQBUF);
    this->captureNew();
    //PROF_EXIT(P_DQBUF);
    uint8_t* current_image = static_cast<uint8_t*>(mem[currentBuf->index]);
    if (current_image) {
        Message<ThresholdedImage> image(0);

        //PROF_ENTER(P_ACQUIRE_IMAGE);
        ImageAcquisition::acquire_image_fast(table, params, current_image,
                                             image.get()->get_mutable_image());
        image.get()->set_timestamp(42);
        //PROF_EXIT(P_ACQUIRE_IMAGE);

        outPortal->setMessage(image);

        //PROF_ENTER(P_QBUF);
        this->releaseBuffer();
        //PROF_EXIT(P_QBUF);
        return true;
    }
    else {
        std::cerr << "Warning - the buffer we dequeued was NULL" << std::endl;
    }
    return false;
}

bool V4L2ImageTranscriber::captureNew() {
    // dequeue a frame buffer (this call blocks when there is
    // no new image available)

    VERIFY((ioctl(fd, VIDIOC_DQBUF, buf)),
           "Dequeueing the frame buffer failed.");
    if(buf->bytesused != (unsigned int)SIZE)
        std::cerr << "CAMERA::ERROR::Wrong buffer size!" << std::endl;
    currentBuf = buf;

    static bool shout = true;
    if (shout) {
        shout = false;
        std::cerr << "CAMERA::Camera is working." << std::endl;
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
        std::cerr << "CAMERA::Warning::Getting control failed." << std::endl;
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
            std::cerr << "CAMERA::Warning::Control setting failed." <<
                std::endl;
            return false;
        }
	counter++;
	if(counter > 10)
	  {
          std::cerr << "CAMERA::Warning::Timeout while setting a parameter."
                    << std::endl;
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
        std::cerr << "CAMERA::fps.parm.capture.timeperframe.numerator is"
                  << " wrong." << std::endl;
        allFine = false;
    }
    if (fps.parm.capture.timeperframe.denominator != 30) {
        std::cerr << "CAMERA::fps.parm.capture.timeperframe.denominator" <<
            " is wrong." << std::endl;
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
    //std::cerr << "Done checking driver settings" << std::endl;

    if (hflip != settings.hflip)
    {
        std::cerr << "CAMERA::WARNING::Horizontal flip setting is wrong:"
                  << std::endl;
        std::cerr << " is " << hflip << " not " << settings.hflip <<
            std::endl;
        allFine = false;
    }
    if (vflip != settings.vflip)
    {
        std::cerr << "CAMERA::WARNING::Vertical flip setting is wrong:"
                  << std::endl;
        std::cerr << " is " << vflip  << " not " << settings.vflip <<
            std::endl;
        allFine = false;
    }
    if (brightness != settings.brightness)
    {
        std::cerr << "CAMERA::WARNING::Brightness setting is wrong:"
                  << std::endl;
        std::cerr << " is " <<  brightness << " not " << settings.brightness
                  << std::endl;
        allFine = false;
    }
    if (contrast != settings.contrast)
    {
        std::cerr << "CAMERA::WARNING::Contrast setting is wrong:"
                  << std::endl;
        std::cerr << " is " <<  contrast << " not " << settings.contrast
                  << std::endl;
        allFine = false;
    }
    if (saturation != settings.saturation)
    {
        std::cerr << "CAMERA::WARNING::Saturation setting is wrong:"
                  << std::endl;
        std::cerr << " is " <<  saturation << " not " << settings.saturation
                  << std::endl;
        allFine = false;
    }
    if (hue != settings.hue)
    {
        std::cerr << "CAMERA::WARNING::Hue setting is wrong:"
                  << std::endl;
        std::cerr << " is " <<  hue << " not " << settings.hue
                  << std::endl;
        allFine = false;
    }
   if (sharpness != settings.sharpness)
    {
        std::cerr << "CAMERA::WARNING::Sharpness setting is wrong:"
                  << std::endl;
        std::cerr << " is " <<  sharpness << " not " << settings.sharpness
                  << std::endl;
        allFine = false;
    }
   if (gain != settings.gain)
    {
        std::cerr << "CAMERA::WARNING::Gain setting is wrong:"
                  << std::endl;
        std::cerr << " is " <<  gain << " not " << settings.gain
                  << std::endl;
        allFine = false;
    }
   if (exposure != settings.exposure)
    {
        std::cerr << "CAMERA::WARNING::Exposure setting is wrong:"
                  << std::endl;
        std::cerr << " is " <<  exposure << " not " << settings.exposure
                  << std::endl;
        allFine = false;
    }
   if (whitebalance != settings.white_balance)
    {
        std::cerr << "CAMERA::WARNING::Whitebalance setting is wrong:"
                  << std::endl;
        std::cerr << " is " <<  whitebalance << " not " << settings.white_balance
                  << std::endl;
        allFine = false;
    }

    if (allFine) {
        std::cerr << "CAMERA::";
        if(cameraType == Camera::BOTTOM)
            std::cerr << "Bottom camera settings were set correctly." <<
                std::endl;
        else std::cerr << "Top camera settings were set correctly." <<
                 std::endl;
    }
}


// Taken from V4L2 specs example
// If you need to determine info about driver, use this method
void V4L2ImageTranscriber::enumerate_controls()
{
    memset (&queryctrl, 0, sizeof (queryctrl));

    std::cout << "Public controls:" << std::endl;
    for (queryctrl.id = V4L2_CID_BASE;
         queryctrl.id < V4L2_CID_LASTP1;
         queryctrl.id++) {
        if (0 == ioctl (fd, VIDIOC_QUERYCTRL, &queryctrl)) {
            if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
                continue;

            std::cout << "Control " << queryctrl.name;
            std::cout << " has id " << queryctrl.id;
            std::cout << " steps " << queryctrl.step;
            std::cout << " and min " << queryctrl.minimum << " max " <<
                queryctrl.maximum << std::endl;

            if (queryctrl.type == V4L2_CTRL_TYPE_MENU)
                enumerate_menu ();
        } else {
            if (errno == EINVAL)
                continue;

            perror ("VIDIOC_QUERYCTRL");
            exit (EXIT_FAILURE);
        }
    }

    std::cout << "Private controls:" << std::endl;;
    for (queryctrl.id = V4L2_CID_PRIVATE_BASE;;
         queryctrl.id++) {
        if (0 == ioctl (fd, VIDIOC_QUERYCTRL, &queryctrl)) {
            if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
                continue;

            std::cout << "Control " <<  queryctrl.name << std::endl;

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
                std::cout << "Disabled." << std::endl;
    }

    std::cout << "Control " << queryctrl.name;
    std::cout << " has id " << queryctrl.id;
    std::cout << " and min " << queryctrl.minimum << " max " <<
        queryctrl.maximum << std::endl;

    if (queryctrl.type == V4L2_CTRL_TYPE_MENU)
                enumerate_menu ();
}

void V4L2ImageTranscriber::enumerate_menu ()
{
    std::cout << "  Menu items:" << std::endl;;

    memset (&querymenu, 0, sizeof (querymenu));
    querymenu.id = queryctrl.id;

    for (querymenu.index = queryctrl.minimum;
         querymenu.index <= (unsigned)queryctrl.maximum;
         querymenu.index++) {
        if (0 == ioctl (fd, VIDIOC_QUERYMENU, &querymenu)) {
            std::cout << "  " << querymenu.name << std::endl;
        }
    }
}

}
}
