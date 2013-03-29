#include "TranscriberModule.h"

#include <cstring>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <cerrno>
#include <iostream>
#include <linux/version.h>
#include <bn/i2c/i2c-dev.h>

namespace man {
namespace image {

static void verify(int x, const char* msg)
{
    if(x != 0)
    {
        std::cerr << "CAMERA ERROR::" << msg <<
            "\nSystem Error Message: " <<
            strerror(errno) << std::endl;
    }
}

TranscriberBuffer::TranscriberBuffer(void* pixels,
                                    int fd,
                                    const struct v4l2_buffer& buf)
    : VideoPixelBuffer(pixels),
      fd(fd)
{
    releaseBuff = buf;
}

TranscriberBuffer::~TranscriberBuffer()
{
    verify(ioctl(fd, VIDIOC_QBUF, &releaseBuff),
           "Releasing buffer failed.");
}

ImageTranscriber::ImageTranscriber(Camera::Type which) :
    settings(Camera::getSettings(which)),
    cameraType(which),
    timeStamp(0)
{
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

ImageTranscriber::~ImageTranscriber()
{
    // disable streaming
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    verify(ioctl(fd, VIDIOC_STREAMOFF, &type),
           "Capture stop failed.");

    // unmap buffers
    for (int i = 0; i < NUM_BUFFERS; ++i)
        munmap(mem[i], memLength[i]);

    // close the device
    close(cameraAdapterFd);
    close(fd);
}

void ImageTranscriber::initOpenI2CAdapter() {
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

    verify(ioctl(cameraAdapterFd, 0x703, 8),
           "Opening I2C adapter failed.");
}

void ImageTranscriber::initSelectCamera() {
    unsigned char cmd[2] = { (unsigned char) cameraType, 0 };
    i2c_smbus_write_block_data(cameraAdapterFd, 220, 1, cmd);
}

void ImageTranscriber::initOpenVideoDevice() {
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

void ImageTranscriber::initSetCameraDefaults() {
    v4l2_std_id esid0 = WIDTH == 320 ? 0x04000000UL : 0x08000000UL;
    verify(ioctl(fd, VIDIOC_S_STD, &esid0),
           "Setting default parameters failed.");
    }

void ImageTranscriber::initSetImageFormat() {
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
    verify(ioctl(fd, VIDIOC_S_FMT, &fmt),
           "Setting image format failed.");

    if(fmt.fmt.pix.sizeimage != (unsigned int)SIZE)
        std::cerr << "CAMERA ERROR::Size setting is WRONG." << std::endl;
}

void ImageTranscriber::initSetFrameRate() {
    // We want frame rate to be 30 fps
    struct v4l2_streamparm fps;
    memset(&fps, 0, sizeof(struct v4l2_streamparm));
    fps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    verify(ioctl(fd, VIDIOC_G_PARM, &fps),
           "Getting FPS failed.");
    fps.parm.capture.timeperframe.numerator = 1;
    fps.parm.capture.timeperframe.denominator = 30;
    verify(ioctl(fd, VIDIOC_S_PARM, &fps),
           "Setting FPS failed.");
}

void ImageTranscriber::initRequestAndMapBuffers() {
    // request buffers
    struct v4l2_requestbuffers rb;
    memset(&rb, 0, sizeof(struct v4l2_requestbuffers));
    rb.count = NUM_BUFFERS;
    rb.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    rb.memory = V4L2_MEMORY_MMAP;
    verify(ioctl(fd, VIDIOC_REQBUFS, &rb),
           "Requesting buffers failed.");

    if(rb.count != (unsigned int)NUM_BUFFERS)
    {
        std::cerr << "CAMERA ERROR::Buffer count is WRONG." << std::endl;
    }

    // map or prepare the buffers
    for(int i = 0; i < NUM_BUFFERS; ++i)
    {
        requestBuff.index = i;
        requestBuff.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        requestBuff.memory = V4L2_MEMORY_MMAP;
        verify(ioctl(fd, VIDIOC_QUERYBUF, &requestBuff),
               "Querying buffer failed.");
        memLength[i] = requestBuff.length;
        mem[i] = mmap(0, requestBuff.length, PROT_READ | PROT_WRITE,
                      MAP_SHARED, fd, requestBuff.m.offset);
        if(mem[i] == MAP_FAILED)
            std::cerr << "CAMERA ERROR::Map failed." << std::endl;
    }
}

void ImageTranscriber::initQueueAllBuffers() {
    // queue the buffers
    for (int i = 0; i < NUM_BUFFERS; ++i) {
        requestBuff.index = i;
        requestBuff.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        requestBuff.memory = V4L2_MEMORY_MMAP;
        if(ioctl(fd, VIDIOC_QBUF, &requestBuff) == -1)
            std::cerr << "Queueing a buffer failed." << std::endl;
    }
}
void ImageTranscriber::initSettings()
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

int ImageTranscriber::getControlSetting(unsigned int id) {
    struct v4l2_control control_s;
    control_s.id = id;
    if (ioctl(fd, VIDIOC_G_CTRL, &control_s) < 0)
    {
        std::cerr << "CAMERA::Warning::Getting control failed." << std::endl;
        return -1;
    }
    return control_s.value;
}

bool ImageTranscriber::setControlSetting(unsigned int id, int value) {
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

void ImageTranscriber::assertCameraSettings() {
    bool allFine = true;
    // check frame rate
    struct v4l2_streamparm fps;
    memset(&fps, 0, sizeof(fps));
    fps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    verify(ioctl(fd, VIDIOC_G_PARM, &fps),
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

void ImageTranscriber::startCapturing() {
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    verify(ioctl(fd, VIDIOC_STREAMON, &type),
           "Start capture failed.");
}

messages::YUVImage ImageTranscriber::getNextImage()
{
    // dequeue a frame buffer (this call blocks when there is
    // no new image available)
    verify(ioctl(fd, VIDIOC_DQBUF, &requestBuff),
           "Dequeueing the frame buffer failed.");
    if(requestBuff.bytesused != (unsigned int)SIZE)
        std::cerr << "CAMERA::ERROR::Wrong buffer size!" << std::endl;

    static bool shout = true;
    if (shout) {
        shout = false;
        std::cerr << "CAMERA::Camera is working." << std::endl;
    }

    return messages::YUVImage(new TranscriberBuffer(mem[requestBuff.index],
                                                    fd,
                                                    requestBuff),
                              2*WIDTH, HEIGHT, 2*WIDTH);
}

TranscriberModule::TranscriberModule(ImageTranscriber& trans)
    : imageOut(base()), it(trans)
{
}

void TranscriberModule::run_()
{
    messages::YUVImage image = it.getNextImage();
    portals::Message<messages::YUVImage> imageOutMessage(&image);
    imageOut.setMessage(imageOutMessage);
}

}
}
