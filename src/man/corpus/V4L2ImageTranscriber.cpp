/*
 * V4L2ImageTranscriber.cpp
 *
 *  Created on: Jun 27, 2011
 *      Author: oneamtu
 *      Credits go to Colin Graf and Thomas Rofer of BHuman from where this
 *      code is mostly inspired from
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
//#define V4L2_CID_WHITE_BALANCE_TEMPERATURE  (V4L2_CID_BASE+26)
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

namespace man {
namespace corpus {

using boost::shared_ptr;

V4L2ImageTranscriber::V4L2ImageTranscriber(shared_ptr<Sensors> s) :
        ThreadedImageTranscriber(s, "V4L2ImageTranscriber"),
        settings(Camera::getDefaultSettings()),
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

    initSettings();

    assertCameraSettings();

    startCapturing();
}

V4L2ImageTranscriber::~V4L2ImageTranscriber() {
    // disable streaming
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(ioctl(fd, VIDIOC_STREAMOFF, &type) == -1)
        printf("Capture stop failed.\n");

    // unmap buffers
    for (int i = 0; i < frameBufferCount; ++i)
        munmap(mem[i], memLength[i]);

    // close the device
    close(cameraAdapterFd);
    close(fd);
    free(buf);
}

/**************************
 *                        *
 *     INIT METHODS       *
 *                        *
 *************************/

void V4L2ImageTranscriber::initTable(const string& filename)
{
    FILE *fp = fopen(filename.c_str(), "r");   //open table for reading

    if (fp == NULL) {
        printf("initTable() FAILED to open filename: %s", filename.c_str());
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
    printf("CAMERA::Loaded colortable %s\n",filename.c_str());
#endif

    fclose(fp);
}

void V4L2ImageTranscriber::initOpenI2CAdapter() {
    cameraAdapterFd = open("/dev/i2c-0", O_RDWR);

    if(cameraAdapterFd == -1)
        printf("Camera adapter FD is WRONG.\n");
    if(ioctl(cameraAdapterFd, 0x703, 8) == -1)
        printf("Opening I2C adapter failed.\n");
}

void V4L2ImageTranscriber::initSelectCamera() {
    unsigned char cmd[2] = { Camera::BOTTOM, 0 };
    i2c_smbus_write_block_data(cameraAdapterFd, 220, 1, cmd);
}

void V4L2ImageTranscriber::initOpenVideoDevice() {
    // open device
#if ROBOT_TYPE == NAO_NEXTGEN
    fd = open("/dev/video1", O_RDWR);
#else
    fd = open("/dev/video0", O_RDWR);
#endif

    if(fd == -1)
        printf("Video Device FD is WRONG.\n");
}

void V4L2ImageTranscriber::initSetCameraDefaults() {
    // set default parameters
#if ROBOT_TYPE == NAO_RL_33
    struct v4l2_control control;
    memset(&control, 0, sizeof(control));
    control.id = V4L2_CID_CAM_INIT;
    control.value = 0;
    if(ioctl(fd, VIDIOC_S_CTRL, &control) == -1)
        perror("Setting default parameters failed:\n");
#endif

    v4l2_std_id esid0 = WIDTH == 320 ? 0x04000000UL : 0x08000000UL;
    if(ioctl(fd, VIDIOC_S_STD, &esid0) == -1)
        printf("Setting default parameters failed. 2\n");
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
    if(ioctl(fd, VIDIOC_S_FMT, &fmt) == -1)
    {
        printf("Setting image format failed");
    }

    if(fmt.fmt.pix.sizeimage != SIZE)
        printf("Size setting is WRONG.\n");
}

void V4L2ImageTranscriber::initSetFrameRate() {
    // set frame rate
    struct v4l2_streamparm fps;
    memset(&fps, 0, sizeof(struct v4l2_streamparm));
    fps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(ioctl(fd, VIDIOC_G_PARM, &fps) == -1)
        printf("Getting FPS failed.\n");
    fps.parm.capture.timeperframe.numerator = 1;
    fps.parm.capture.timeperframe.denominator = 30;
    if(ioctl(fd, VIDIOC_S_PARM, &fps) == -1)
        printf("Setting FPS failed.\n");;
}

void V4L2ImageTranscriber::initRequestAndMapBuffers() {
    // request buffers
    struct v4l2_requestbuffers rb;
    memset(&rb, 0, sizeof(struct v4l2_requestbuffers));
    rb.count = frameBufferCount;
    rb.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    rb.memory = V4L2_MEMORY_MMAP;
    if(ioctl(fd, VIDIOC_REQBUFS, &rb) == -1)
        printf("Requesting buffers failed.\n");
    if(rb.count != frameBufferCount)
        printf("Buffer count is WRONG.\n");

    // map or prepare the buffers
    buf = static_cast<struct v4l2_buffer*>(calloc(1,
            sizeof(struct v4l2_buffer)));
    for(    int i = 0; i < frameBufferCount; ++i)
    {
        buf->index = i;
        buf->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf->memory = V4L2_MEMORY_MMAP;
        if(ioctl(fd, VIDIOC_QUERYBUF, buf) == -1)
            printf("Querying buffer failed.\n");
        memLength[i] = buf->length;
        mem[i] = mmap(0, buf->length, PROT_READ | PROT_WRITE,
                      MAP_SHARED, fd, buf->m.offset);
        if(mem[i] == MAP_FAILED)
            printf("Map failed.\n");
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

void V4L2ImageTranscriber::enumerate_menu ()
{
    {
        printf ("  Menu items:\n");

        memset (&querymenu, 0, sizeof (querymenu));
        querymenu.id = queryctrl.id;

        for (querymenu.index = queryctrl.minimum;
             querymenu.index <= queryctrl.maximum;
             querymenu.index++) {
            if (0 == ioctl (fd, VIDIOC_QUERYMENU, &querymenu)) {
                printf ("  %s\n", querymenu.name);
            }
        }
    }

    memset (&queryctrl, 0, sizeof (queryctrl));

    for (queryctrl.id = V4L2_CID_BASE;
         queryctrl.id < V4L2_CID_LASTP1;
         queryctrl.id++) {
        if (0 == ioctl (fd, VIDIOC_QUERYCTRL, &queryctrl)) {
            if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
                continue;

            printf ("Control %s\n", queryctrl.name);

            if (queryctrl.type == V4L2_CTRL_TYPE_MENU)
                enumerate_menu ();
        } else {
            if (errno == EINVAL)
                continue;

            perror ("VIDIOC_QUERYCTRL");
            exit (EXIT_FAILURE);
        }
    }

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
}

void V4L2ImageTranscriber::initSettings()
{
    // The following should be 1 if we're using the top camera (?)
    setControlSetting(V4L2_CID_HFLIP, 0);
    setControlSetting(V4L2_CID_VFLIP, 0);

    setControlSetting(V4L2_CID_EXPOSURE_AUTO, 1);

    setControlSetting(V4L2_CID_BRIGHTNESS, settings.brightness);

    setControlSetting(V4L2_CID_CONTRAST, settings.brightness);
    setControlSetting(V4L2_CID_SATURATION, 150);

    setControlSetting(V4L2_CID_AUTO_WHITE_BALANCE, 0);
    setControlSetting(V4L2_CID_SHARPNESS, 3);

    setControlSetting(V4L2_CID_BACKLIGHT_COMPENSATION, 0);

    setControlSetting(V4L2_CID_EXPOSURE_AUTO, 0);

    setControlSetting(V4L2_CID_EXPOSURE, settings.exposure);

    if(settings.gain != Camera::KEEP_DEFAULT)
        setControlSetting(V4L2_CID_GAIN, settings.gain);
    if(settings.blue_chroma != Camera::KEEP_DEFAULT)
        setControlSetting(V4L2_CID_BLUE_BALANCE, settings.blue_chroma);
    if(settings.red_chroma != Camera::KEEP_DEFAULT)
        setControlSetting(V4L2_CID_BLUE_BALANCE, settings.red_chroma);

}

void V4L2ImageTranscriber::startCapturing() {
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(ioctl(fd, VIDIOC_STREAMON, &type) == -1)
        printf("Start capture failed.\n");
}


/**************************
 *                        *
 *    THREAD METHODS      *
 *                        *
 *************************/

int V4L2ImageTranscriber::start()
{
    return Thread::start();
}

void V4L2ImageTranscriber::run()
{
    Thread::running = true;
    Thread::trigger->on();

    long long lastProcessTimeAvg = VISION_FRAME_LENGTH_uS;

    struct timespec interval, remainder;
    while (Thread::running) {
        PROF_ENTER(P_MAIN);
        PROF_ENTER(P_GETIMAGE);
        //start timer
        const long long startTime = monotonic_micro_time();

        if (waitForImage()) {
            PROF_EXIT(P_GETIMAGE);
            subscriber->notifyNextVisionImage();
        }

        //stop timer
        const long long processTime = monotonic_micro_time() - startTime;
        //sleep until next frame

        lastProcessTimeAvg = lastProcessTimeAvg/2 + processTime/2;

        if (processTime > VISION_FRAME_LENGTH_uS) {
            if (processTime > VISION_FRAME_LENGTH_PRINT_THRESH_uS) {
#ifdef DEBUG_ALIMAGE_LOOP
                cout << "Time spent in ALImageTranscriber loop longer than"
                          << " frame length: " << processTime <<endl;
#endif
            }
            //Don't sleep at all
        } else{
            const long int microSleepTime =
                static_cast<long int>(VISION_FRAME_LENGTH_uS - processTime);
            const long int nanoSleepTime =
                static_cast<long int>((microSleepTime %(1000 * 1000)) * 1000);

            const long int secSleepTime =
                static_cast<long int>(microSleepTime / (1000*1000));

            // cout << "Sleeping for nano: " << nanoSleepTime
            //      << " and sec:" << secSleepTime << endl;

            interval.tv_sec = static_cast<time_t>(secSleepTime);
            interval.tv_nsec = nanoSleepTime;

            nanosleep(&interval, &remainder);
        }
        PROF_EXIT(P_MAIN);
        PROF_NFRAME();
    }
    Thread::trigger->off();
}

void V4L2ImageTranscriber::stop()
{
//    cout << "Stopping ALImageTranscriber" << endl;
    running = false;
    Thread::stop();
}

/**************************
 *                        *
 *    EXTRA METHODS       *
 *                        *
 *************************/

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
       if (sensors->getWriteableNaoImage() != NULL) {
           _copy_image(current_image, sensors->getWriteableNaoImage());
           sensors->notifyNewNaoImage();
           ImageAcquisition::acquire_image_fast(table, params,
                                                sensors->getNaoImage(), image);
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
        sensors->setImage(image);
        return true;
    } else {
        printf("Warning - the buffer we dequeued was NULL\n");
    }
    return false;
}

bool V4L2ImageTranscriber::captureNew() {
    // dequeue a frame buffer (this call blocks when there is
    // no new image available)
    assertCameraSettings();

    if(ioctl(fd, VIDIOC_DQBUF, buf) == -1)
        printf("Dequeueing the frame buffer failed.\n");
    if(buf->bytesused != SIZE) printf("Wrong size!.\n");
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
        if(ioctl(fd, VIDIOC_QBUF, currentBuf) == -1)
            printf("Releasing buffer failed.\n");
    }
    return true;
}

int V4L2ImageTranscriber::getControlSetting(unsigned int id) {
    struct v4l2_queryctrl queryctrl;
    queryctrl.id = id;
    if (ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl) < 0)
    {
        printf("Control DNE.\n");
        return -1;
    }
    if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED
        )
    {
        printf("Control not available.\n");
        return -1; // not available
    }
    if (queryctrl.type != V4L2_CTRL_TYPE_BOOLEAN
            && queryctrl.type != V4L2_CTRL_TYPE_INTEGER
            && queryctrl.type != V4L2_CTRL_TYPE_MENU)
    {
        printf("Control not supported.\n");
        return -1; // not supported
    }

    struct v4l2_control control_s;
    control_s.id = id;
    if (ioctl(fd, VIDIOC_G_CTRL, &control_s) < 0)
    {
        printf("Getting control id %d failed.\n", id);
        return -1;
    }
    if (control_s.value == queryctrl.default_value)
    {
        printf("Warning: Control id %d is still default.\n", id);
    }
    return control_s.value;
}

bool V4L2ImageTranscriber::setControlSetting(unsigned int id, int value) {
    struct v4l2_queryctrl queryctrl;
    bool ok = true;
    queryctrl.id = id;
    printf("Control number %d.\n", id);
    if (ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl) < 0)
    {
        printf("  Control DNE.\n");
        ok = false;
    }
    if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
    {
        printf("  Control disabled.\n");
        ok = false; // not available
    }
    if (queryctrl.type != V4L2_CTRL_TYPE_BOOLEAN
            && queryctrl.type != V4L2_CTRL_TYPE_INTEGER
            && queryctrl.type != V4L2_CTRL_TYPE_MENU)
    {
        printf("  Control not supported.\n");
        ok = false;
    }

    struct v4l2_control control_s;
    control_s.id = id;
    control_s.value = value;
    while(getControlSetting(id) != value)
    {
        printf("trying %d.\n", id);
        if (ioctl(fd, VIDIOC_S_CTRL, &control_s) < 0)
        {
            printf("  Control setting failed anyway.\n");
            ok = false;
        }
    }
    return ok;
}

void V4L2ImageTranscriber::assertCameraSettings() {
    bool allFine = true;
    // check frame rate
    struct v4l2_streamparm fps;
    memset(&fps, 0, sizeof(fps));
    fps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(ioctl(fd, VIDIOC_G_PARM, &fps) == -1)
        printf("Camera::assertCameraSettings::Getting settings failed\n");

    if (fps.parm.capture.timeperframe.numerator != 1) {
        printf("CAMERA::fps.parm.capture.timeperframe.numerator is wrong.\n");
        allFine = false;
    }
    if (fps.parm.capture.timeperframe.denominator != 30) {
        printf("CAMERA::fps.parm.capture.timeperframe.denominator is wrong.\n");
        allFine = false;
    }

    // check camera settings
    int exposure = getControlSetting(V4L2_CID_EXPOSURE);
    int brightness = getControlSetting(V4L2_CID_BRIGHTNESS);
    int contrast = getControlSetting(V4L2_CID_CONTRAST);
    int red = getControlSetting(V4L2_CID_RED_BALANCE);
    int blue = getControlSetting(V4L2_CID_BLUE_BALANCE);
    int gain = getControlSetting(V4L2_CID_GAIN);

    if (exposure != settings.exposure) {
        printf("CAMERA::WARNING::Exposure setting is wrong:");
        printf(" is %d, not %d.\n", exposure, settings.exposure);
        allFine = false;
    }
    if (brightness != settings.brightness) {
        printf("CAMERA::WARNING::Brightness setting is wrong:");
        printf(" is %d, not %d.\n", brightness, settings.brightness);
        allFine = false;
    }
    if (contrast != settings.contrast) {
        printf("CAMERA::WARNING::Contrast setting is wrong:");
        printf(" is %d, not %d.\n", contrast, settings.contrast);
        allFine = false;
    }
    if (red != settings.red_chroma) {
        printf("CAMERA::WARNING::Red chroma setting is wrong:");
        printf(" is %d, not %d.\n", red, settings.red_chroma);
        allFine = false;
    }
    if (blue != settings.blue_chroma) {
        printf("CAMERA::WARNING::Blue chroma setting is wrong:");
        printf(" is %d, not %d.\n", blue, settings.blue_chroma);
        allFine = false;
    }
    if (gain != settings.gain) {
        printf("CAMERA::WARNING::Gain setting is wrong:");
        printf(" is %d, not %d.\n", gain, settings.gain);
        allFine = false;
    }

    if (allFine) {
        printf("CAMERA::Settings match settings stored in hardware/driver.\n");
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
