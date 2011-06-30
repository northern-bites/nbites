/*
 * V4L2ImageTranscriber.cpp
 *
 *  Created on: Jun 27, 2011
 *      Author: oneamtu
 *      Credits go to Colin Graf and Thomas Rofer of BHuman from where this
 *      code is mostly inspired from
 */

#include "V4L2ImageTranscriber.h"

#include <cstring>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#define USE_USERPTR
#ifdef USE_USERPTR
#include <malloc.h> // memalign
#endif

#undef __STRICT_ANSI__
#include <linux/version.h>
#include <bn/i2c/i2c-dev.h>
//#include <linux/i2c.h>
#define __STRICT_ANSI__

#include "ImageAcquisition.h"

#ifndef V4L2_CID_AUTOEXPOSURE
#  define V4L2_CID_AUTOEXPOSURE     (V4L2_CID_BASE+32)
#endif

#ifndef V4L2_CID_CAM_INIT
#  define V4L2_CID_CAM_INIT         (V4L2_CID_BASE+33)
#endif

#ifndef V4L2_CID_AUDIO_MUTE
#  define V4L2_CID_AUDIO_MUTE       (V4L2_CID_BASE+9)
#endif

#ifndef V4L2_CID_POWER_LINE_FREQUENCY
#  define V4L2_CID_POWER_LINE_FREQUENCY  (V4L2_CID_BASE+24)
enum v4l2_power_line_frequency {
    V4L2_CID_POWER_LINE_FREQUENCY_DISABLED = 0,
    V4L2_CID_POWER_LINE_FREQUENCY_50HZ = 1,
    V4L2_CID_POWER_LINE_FREQUENCY_60HZ = 2,
};

#define V4L2_CID_HUE_AUTO      (V4L2_CID_BASE+25)
#define V4L2_CID_WHITE_BALANCE_TEMPERATURE  (V4L2_CID_BASE+26)
#define V4L2_CID_SHARPNESS      (V4L2_CID_BASE+27)
#define V4L2_CID_BACKLIGHT_COMPENSATION   (V4L2_CID_BASE+28)

#define V4L2_CID_CAMERA_CLASS_BASE     (V4L2_CTRL_CLASS_CAMERA | 0x900)
#define V4L2_CID_CAMERA_CLASS       (V4L2_CTRL_CLASS_CAMERA | 1)

#define V4L2_CID_EXPOSURE_AUTO      (V4L2_CID_CAMERA_CLASS_BASE+1)
enum v4l2_exposure_auto_type {
    V4L2_EXPOSURE_MANUAL = 0,
    V4L2_EXPOSURE_AUTO = 1,
    V4L2_EXPOSURE_SHUTTER_PRIORITY = 2,
    V4L2_EXPOSURE_APERTURE_PRIORITY = 3
};
#define V4L2_CID_EXPOSURE_ABSOLUTE    (V4L2_CID_CAMERA_CLASS_BASE+2)
#define V4L2_CID_EXPOSURE_AUTO_PRIORITY    (V4L2_CID_CAMERA_CLASS_BASE+3)

#define V4L2_CID_PAN_RELATIVE      (V4L2_CID_CAMERA_CLASS_BASE+4)
#define V4L2_CID_TILT_RELATIVE      (V4L2_CID_CAMERA_CLASS_BASE+5)
#define V4L2_CID_PAN_RESET      (V4L2_CID_CAMERA_CLASS_BASE+6)
#define V4L2_CID_TILT_RESET      (V4L2_CID_CAMERA_CLASS_BASE+7)

#define V4L2_CID_PAN_ABSOLUTE      (V4L2_CID_CAMERA_CLASS_BASE+8)
#define V4L2_CID_TILT_ABSOLUTE      (V4L2_CID_CAMERA_CLASS_BASE+9)

#define V4L2_CID_FOCUS_ABSOLUTE      (V4L2_CID_CAMERA_CLASS_BASE+10)
#define V4L2_CID_FOCUS_RELATIVE      (V4L2_CID_CAMERA_CLASS_BASE+11)
#define V4L2_CID_FOCUS_AUTO      (V4L2_CID_CAMERA_CLASS_BASE+12)

#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26) */

//V4L2ImageTranscriber* V4L2ImageTranscriber::theInstance = 0;
namespace man {
namespace corpus {

using boost::shared_ptr;

V4L2ImageTranscriber::V4L2ImageTranscriber(shared_ptr<Synchro> synchro,
        shared_ptr<Sensors> s) :
        ThreadedImageTranscriber(s, synchro, "V4L2ImageTranscriber"),
        settings(Camera::getDefaultSettings()), currentBuf(0), timeStamp(0),
        image(reinterpret_cast<uint16_t*>(new uint8_t[IMAGE_BYTE_SIZE])),
        table(new unsigned char[yLimit * uLimit * vLimit]),
        params(y0, u0, v0, y1, u1, v1, yLimit, uLimit, vLimit){

    initTable("/home/nao/naoqi/lib/naoqi/table.mtb");

//    Camera::Type current = settings.type;
    //first we set up the other camera
//    settings.type = Camera::getOtherCameraType(settings.type);
    initOpenI2CAdapter();
    initSelectCamera();
    initOpenVideoDevice();
    initSetCameraDefaults();
    initSetImageFormat();
    initSetFrameRate();
    initRequestAndMapBuffers();
    initQueueAllBuffers();
//    initDefaultControlSettings();
//
//    //and then the camera we want to use
//    settings.type = current;
//    initSelectCamera();
//    initSetCameraDefaults();
//    initSetImageFormat();
//    initSetFrameRate();
//    initRequestAndMapBuffers();
//    initQueueAllBuffers();
//    initDefaultControlSettings();

    setSettings(settings);

    startCapturing();
}

V4L2ImageTranscriber::~V4L2ImageTranscriber() {
    // disable streaming
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    CHECK_SUCCESS(ioctl(fd, VIDIOC_STREAMOFF, &type));

    // unmap buffers
    for (int i = 0; i < frameBufferCount; ++i)
#ifdef USE_USERPTR
        free(mem[i]);
#else
        munmap(mem[i], memLength[i]);
#endif

    // close the device
#ifndef NO_NAO_EXTENSIONS
    close(cameraAdapterFd);
#endif
    close(fd);
    free(buf);
}

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
        //start timer
        const long long startTime = monotonic_micro_time();

        if (waitForImage()) {
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
    }
    Thread::trigger->off();
}

void V4L2ImageTranscriber::stop()
{
//    cout << "Stopping ALImageTranscriber" << endl;
    running = false;
    Thread::stop();
}

bool V4L2ImageTranscriber::waitForImage() {
    this->captureNew();
    //uint8_t* current_image = static_cast<uint8_t*>(mem[currentBuf->index]);
    uint8_t* current_image = (uint8_t*) (currentBuf->m.userptr);
    printf("currentBuf %lu \n", currentBuf->m.userptr);
    if (current_image) {
#ifdef CAN_SAVE_FRAMES
    _copy_image(image, naoImage);
    ImageAcquisition::acquire_image_fast(table, params,
            naoImage, image);
#else
    unsigned long long startTime = monotonic_micro_time();
    ImageAcquisition::acquire_image_fast(table, params,
            current_image, image);
    printf("Acquisition time %llu\n", monotonic_micro_time() - startTime);
#endif
    sensors->setImage(image);
    return true;
    }
    return false;
}

void V4L2ImageTranscriber::initTable(string filename)
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
    printf("Loaded colortable %s\n",filename.c_str());
#endif

    fclose(fp);
}

void V4L2ImageTranscriber::initTable(unsigned char* buffer)
{
    memcpy(table, buffer, yLimit * uLimit * vLimit);
}


bool V4L2ImageTranscriber::captureNew() {
    // requeue the buffer of the last captured image which is obsolete now
    if (currentBuf) {
        unsigned long long startTime = monotonic_micro_time();
        CHECK_SUCCESS(ioctl(fd, VIDIOC_QBUF, currentBuf));
        printf("QBUF time %llu\n", monotonic_micro_time() - startTime);
    }

    // dequeue a frame buffer (this call blocks when there is no new image available) */
    unsigned long long startTime = monotonic_micro_time();
    CHECK_SUCCESS(ioctl(fd, VIDIOC_DQBUF, buf));
    printf("DQBUF time %llu\n", monotonic_micro_time() - startTime);
    //timeStamp = SystemCall::getCurrentSystemTime();
    //ASSERT(buf->bytesused == SIZE);
    currentBuf = buf;

    static bool shout = true;
    if (shout) {
        shout = false;
        printf("Camera is working\n");
    }

    return true;
}

const unsigned char* V4L2ImageTranscriber::getImage() const {
#ifdef USE_USERPTR
    return currentBuf ? (unsigned char*)currentBuf->m.userptr : 0;
#else
    return currentBuf ? static_cast<unsigned char*>(mem[currentBuf->index]) : 0;
#endif
}

unsigned long long V4L2ImageTranscriber::getTimeStamp() const {
    //ASSERT(currentBuf);
    return timeStamp;
}

int V4L2ImageTranscriber::getControlSetting(unsigned int id) {
    struct v4l2_queryctrl queryctrl;
    queryctrl.id = id;
    if (ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl) < 0)
        return -1;
    if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED
        )
        return -1; // not available
    if (queryctrl.type != V4L2_CTRL_TYPE_BOOLEAN
            && queryctrl.type != V4L2_CTRL_TYPE_INTEGER
            && queryctrl.type != V4L2_CTRL_TYPE_MENU)
        return -1; // not supported

    struct v4l2_control control_s;
    control_s.id = id;
    if (ioctl(fd, VIDIOC_G_CTRL, &control_s) < 0)
        return -1;
    if (control_s.value == queryctrl.default_value)
        return -1;
    return control_s.value;
}

bool V4L2ImageTranscriber::setControlSetting(unsigned int id, int value) {
    struct v4l2_queryctrl queryctrl;
    queryctrl.id = id;
    if (ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl) < 0)
        return false;
    if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED
        )
        return false; // not available
    if (queryctrl.type != V4L2_CTRL_TYPE_BOOLEAN
            && queryctrl.type != V4L2_CTRL_TYPE_INTEGER
            && queryctrl.type != V4L2_CTRL_TYPE_MENU)
        return false; // not supported

    // clip value
    if (value < queryctrl.minimum)
        value = queryctrl.minimum;
    if (value > queryctrl.maximum)
        value = queryctrl.maximum;
    if (value < 0)
        value = queryctrl.default_value;

    struct v4l2_control control_s;
    control_s.id = id;
    control_s.value = value;
    if (ioctl(fd, VIDIOC_S_CTRL, &control_s) < 0)
        return false;
    return true;
}

void V4L2ImageTranscriber::setSettings(const Camera::Settings& newset) {
    if (newset.exposure != settings.exposure)
        setControlSetting(V4L2_CID_EXPOSURE,
                (settings.exposure = newset.exposure));
    if (newset.brightness != settings.brightness)
        setControlSetting(V4L2_CID_BRIGHTNESS,
                (settings.brightness = newset.brightness));
    if (newset.contrast != settings.contrast)
        setControlSetting(V4L2_CID_CONTRAST,
                (settings.contrast = newset.contrast));
    if (newset.gain != settings.gain)
        setControlSetting(V4L2_CID_GAIN, (settings.gain = newset.gain));
    if (newset.blue_chroma != settings.blue_chroma)
        setControlSetting(V4L2_CID_BLUE_BALANCE, (settings.blue_chroma = newset.blue_chroma));
    if (newset.red_chroma != settings.red_chroma)
        setControlSetting(V4L2_CID_RED_BALANCE, (settings.red_chroma = newset.red_chroma));
}

Camera V4L2ImageTranscriber::switchCamera(Camera camera) {
//#ifndef NO_NAO_EXTENSIONS
//    unsigned char cmd[2] = { camera, 0 };
//    int flip = camera == UPPER_CAMERA ? 1 : 0;
//    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//
//    // disable streaming
//    VERIFY(ioctl(fd, VIDIOC_STREAMOFF, &type) != -1);
//
//    // switch camera
//    VERIFY(i2c_smbus_write_block_data(cameraAdapterFd, 220, 1, cmd) != -1);
//    VERIFY(setControlSetting(V4L2_CID_VFLIP, flip));
//    VERIFY(setControlSetting(V4L2_CID_HFLIP, flip));
//
//    // enable streaming
//    VERIFY(ioctl(fd, VIDIOC_STREAMON, &type) != -1);
//
//    currentCamera = camera;
//    return camera;
//#else
//    return LOWER_CAMERA;
//#endif
}

Camera V4L2ImageTranscriber::switchToUpper() {
//    if (currentCamera == UPPER_CAMERA)
//        return UPPER_CAMERA;
//
//    return switchCamera(UPPER_CAMERA);
}

Camera V4L2ImageTranscriber::switchToLower() {
//    if (currentCamera == LOWER_CAMERA)
//        return LOWER_CAMERA;
//
//    return switchCamera(LOWER_CAMERA);
}

void V4L2ImageTranscriber::initOpenI2CAdapter() {
#ifndef NO_NAO_EXTENSIONS
    cameraAdapterFd = open("/dev/i2c-0", O_RDWR);
//    ASSERT(cameraAdapterFd != -1);
    CHECK_SUCCESS(ioctl(cameraAdapterFd, 0x703, 8));
    CHECK_SUCCESS(i2c_smbus_read_byte_data(cameraAdapterFd, 170) >= 2); // at least Nao V3
#endif
}

void V4L2ImageTranscriber::initSelectCamera() {
#ifndef NO_NAO_EXTENSIONS
    unsigned char cmd[2] = { static_cast<int>(settings.type), 0 };
    i2c_smbus_write_block_data(cameraAdapterFd, 220, 1, cmd); // select camera
#endif
}

void V4L2ImageTranscriber::initOpenVideoDevice() {
    // open device
    CHECK_SUCCESS(fd = open("/dev/video0", O_RDWR));
}

void V4L2ImageTranscriber::initSetCameraDefaults() {
    // set default parameters
#ifndef NO_NAO_EXTENSIONS
    struct v4l2_control control;
    memset(&control, 0, sizeof(control));
    control.id = V4L2_CID_CAM_INIT;
    control.value = 0;
    CHECK_SUCCESS(ioctl(fd, VIDIOC_S_CTRL, &control));

    v4l2_std_id esid0 = WIDTH == 320 ? 0x04000000UL : 0x08000000UL;
    CHECK_SUCCESS(ioctl(fd, VIDIOC_S_STD, &esid0));
#endif
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
    CHECK_SUCCESS(ioctl(fd, VIDIOC_S_FMT, &fmt));

//    ASSERT(fmt.fmt.pix.sizeimage == SIZE);
}

void V4L2ImageTranscriber::initSetFrameRate() {
    // set frame rate
    struct v4l2_streamparm fps;
    memset(&fps, 0, sizeof(struct v4l2_streamparm));
    fps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    CHECK_SUCCESS(ioctl(fd, VIDIOC_G_PARM, &fps));
    fps.parm.capture.timeperframe.numerator = 1;
    fps.parm.capture.timeperframe.denominator = 30;
    CHECK_SUCCESS(ioctl(fd, VIDIOC_S_PARM, &fps));
}

void V4L2ImageTranscriber::initRequestAndMapBuffers() {
    // request buffers
    struct v4l2_requestbuffers rb;
    memset(&rb, 0, sizeof(struct v4l2_requestbuffers));
    rb.count = frameBufferCount;
    rb.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
#ifdef USE_USERPTR
    rb.memory = V4L2_MEMORY_USERPTR;
#else
    rb.memory = V4L2_MEMORY_MMAP;
#endif
    CHECK_SUCCESS(ioctl(fd, VIDIOC_REQBUFS, &rb) != -1);
    //ASSERT(rb.count == frameBufferCount);

    // map or prepare the buffers
    buf = static_cast<struct v4l2_buffer*>(calloc(1,
            sizeof(struct v4l2_buffer)));
//#ifdef USE_USERPTR
//            unsigned int bufferSize = SIZE;
//            unsigned int pageSize = getpagesize();
//            bufferSize = (bufferSize + pageSize - 1) & ~(pageSize - 1);
//#endif
for(    int i = 0; i < frameBufferCount; ++i)
    {
#ifdef USE_USERPTR
        memLength[i] = SIZE;
        mem[i] = malloc(SIZE);
        printf("malloc : %lu\n", (long unsigned) mem[i]);
#else
        buf->index = i;
        buf->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf->memory = V4L2_MEMORY_MMAP;
        CHECK_SUCCESS(ioctl(fd, VIDIOC_QUERYBUF, buf));
        memLength[i] = buf->length;
        mem[i] = mmap(0, buf->length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf->m.offset);
        //ASSERT(mem[i] != MAP_FAILED);
#endif
    }
}

void V4L2ImageTranscriber::initQueueAllBuffers() {
    // queue the buffers
    for (int i = 0; i < frameBufferCount; ++i) {
        buf->index = i;
        buf->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
#ifdef USE_USERPTR
        buf->memory = V4L2_MEMORY_USERPTR;
        buf->m.userptr = (unsigned long) mem[i];
        buf->length = memLength[i];
#else
        buf->memory = V4L2_MEMORY_MMAP;
#endif
        CHECK_SUCCESS(ioctl(fd, VIDIOC_QBUF, buf));
        CHECK_SUCCESS(ioctl(fd, VIDIOC_QUERYBUF, buf));
        printf("querybuf : %lu\n", (long unsigned) buf->m.userptr);
    }
}

void V4L2ImageTranscriber::initDefaultControlSettings() {
    // request camera's default control settings
    settings.exposure = getControlSetting(V4L2_CID_EXPOSURE);
    settings.brightness = getControlSetting(V4L2_CID_BRIGHTNESS);
    settings.contrast = getControlSetting(V4L2_CID_CONTRAST);
    settings.red_chroma = getControlSetting(V4L2_CID_RED_BALANCE);
    settings.blue_chroma = getControlSetting(V4L2_CID_BLUE_BALANCE);
    settings.gain = getControlSetting(V4L2_CID_GAIN);
    // make sure automatic stuff is off
#ifndef NO_NAO_EXTENSIONS
    CHECK_SUCCESS(setControlSetting(V4L2_CID_AUTOEXPOSURE, 0));
    CHECK_SUCCESS(setControlSetting(V4L2_CID_AUTO_WHITE_BALANCE, 0));
    CHECK_SUCCESS(setControlSetting(V4L2_CID_AUTOGAIN, 0));
    int flip = 0;//currentCamera == UPPER_CAMERA ? 1 : 0;
    CHECK_SUCCESS(setControlSetting(V4L2_CID_HFLIP, flip));
    CHECK_SUCCESS(setControlSetting(V4L2_CID_VFLIP, flip));
#else
    setControlSetting(V4L2_CID_AUTOEXPOSURE , 0);
    setControlSetting(V4L2_CID_AUTO_WHITE_BALANCE, 0);
    setControlSetting(V4L2_CID_AUTOGAIN, 0);
    setControlSetting(V4L2_CID_HUE_AUTO, 0);
    setControlSetting(V4L2_CID_EXPOSURE_AUTO, V4L2_EXPOSURE_MANUAL);
    setControlSetting(V4L2_CID_HFLIP, 0);
    setControlSetting(V4L2_CID_VFLIP, 0);
#endif
}

void V4L2ImageTranscriber::startCapturing() {
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    CHECK_SUCCESS(ioctl(fd, VIDIOC_STREAMON, &type) != -1);
}

void V4L2ImageTranscriber::assertCameraSettings() {
    bool allFine = true;
    // check frame rate
    struct v4l2_streamparm fps;
    memset(&fps, 0, sizeof(fps));
    fps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    CHECK_SUCCESS(ioctl(fd, VIDIOC_G_PARM, &fps));
    if (fps.parm.capture.timeperframe.numerator != 1) {
//        //OUTPUT(idText, text,
//                "fps.parm.capture.timeperframe.numerator is wrong.");
        allFine = false;
    }
    if (fps.parm.capture.timeperframe.denominator != 30) {
//        //OUTPUT(idText, text,
//                "fps.parm.capture.timeperframe.denominator is wrong.");
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
        //OUTPUT(idText, text, "Exposure setting is wrong.");
        allFine = false;
    }
    if (brightness != settings.brightness) {
        //OUTPUT(idText, text, "Brightnes setting is wrong.");
        allFine = false;
    }
    if (contrast != settings.contrast) {
        //OUTPUT(idText, text, "Contrast setting is wrong.");
        allFine = false;
    }
    if (red != settings.red_chroma) {
        //OUTPUT(idText, text, "Red gain setting is wrong.");
        allFine = false;
    }
    if (blue != settings.blue_chroma) {
        //OUTPUT(idText, text, "Blue gain setting is wrong.");
        allFine = false;
    }
    if (gain != settings.gain) {
        //OUTPUT(idText, text, "Gain setting is wrong.");
        allFine = false;
    }

    if (allFine) {
        //OUTPUT(idText, text,
//                "Camera settings match settings stored in hardware/driver.");
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
