#include "TranscriberModule.h"

#include <cstring>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <cerrno>
#include <iostream>
#include <fstream>
#include <string>
#include <streambuf>
#include <linux/version.h>
#include <bn/i2c/i2c-dev.h>
#include <vector>

#include "Profiler.h"
#include "DebugConfig.h"

#include "../log/logging.h"
#include "../control/control.h"
#include "nbdebug.h"
#include "thumbnail.h"
#include "../../share/logshare/SExpr.h"

using nblog::SExpr;
using nblog::NBLog;

#define V4L2_MT9M114_FADE_TO_BLACK (V4L2_CID_PRIVATE_BASE)

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
    timeStamp(0),
    exiting(false)
{
    // Bottom camera takes video at half the resolution of the top camera
    if (which == Camera::TOP) {
        width = WIDTH_TOP_CAMERA;
        height = HEIGHT_TOP_CAMERA;
    } else {
        width = WIDTH_TOP_CAMERA / 2;
        height = HEIGHT_TOP_CAMERA / 2;
    }
    size = 2*width*height;

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
    // If this is true then we've (hopefully) already closed up shop
    if (exiting) return;

    // disable streaming
    std::cout << "STOPPING THE CAMERA" << std::endl;
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    verify(ioctl(fd, VIDIOC_STREAMOFF, &type),
           "Capture stop failed.");

    // unmap buffers
    for (int i = 0; i < NUM_BUFFERS; ++i)
        munmap(mem[i], memLength[i]);
    // close the device
    close(cameraAdapterFd);
    close(fd);
    std::cout << "Done closing Camera" << std::endl;
}

void ImageTranscriber::closeDriver()
{
    exiting = true;
    std::cout << "STOPPING THE CAMERA" << std::endl;
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    verify(ioctl(fd, VIDIOC_STREAMOFF, &type),
           "Capture stop failed.");

    // unmap buffers
    for (int i = 0; i < NUM_BUFFERS; ++i)
        munmap(mem[i], memLength[i]);
    // close the device
    close(cameraAdapterFd);
    close(fd);
    std::cout << "Done closing Camera" << std::endl;
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
    v4l2_std_id esid0 = width == 320 ? 0x04000000UL : 0x08000000UL;
    verify(ioctl(fd, VIDIOC_S_STD, &esid0),
           "Setting default parameters failed.");
    }

void ImageTranscriber::initSetImageFormat() {
    // set format
    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(struct v4l2_format));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    // We ask for a 640 by 480 image
    fmt.fmt.pix.width = width;
    fmt.fmt.pix.height = height;
    //"In this format each four bytes is two pixels. Each four bytes is two
    //Y's, a Cb and a Cr. Each Y goes to one of the pixels, and the Cb and
    //Cr belong to both pixels. As you can see, the Cr and Cb components have
    //half the horizontal resolution of the Y component."
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    verify(ioctl(fd, VIDIOC_S_FMT, &fmt),
           "Setting image format failed.");

    if(fmt.fmt.pix.sizeimage != (unsigned int)size)
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
    std::string filepath;
#ifdef NAOQI_2 //for NAOQI 2.x
    if(cameraType == Camera::TOP) {
        filepath = "/home/nao/nbites/Config/V5topCameraParams.txt";
        std::cout<<"[INFO] Camera::TOP"<<std::endl;
    } else {
        filepath = "/home/nao/nbites/Config/V5bottomCameraParams.txt";
        std::cout<<"[INFO] Camera::BOTTOM"<<std::endl;
    }
#else //for NAOQI 1.14
    if(cameraType == Camera::TOP) {
        filepath = "/home/nao/nbites/Config/V4topCameraParams.txt";
        std::cout<<"[INFO] Camera::TOP"<<std::endl;
    } else {
        filepath = "/home/nao/nbites/Config/V4bottomCameraParams.txt";
        std::cout<<"[INFO] Camera::BOTTOM"<<std::endl;
    }
#endif
    
    if(FILE *file = fopen(filepath.c_str(),"r")) {
        fclose(file);
        
        std::ifstream inputFile(filepath);
        std::string readInFile((std::istreambuf_iterator<char>(inputFile)),
                                std::istreambuf_iterator<char>());

        int i=0;
        SExpr params = *SExpr::read(readInFile,i);

        if(params.count() >= 2) {
            std::cout << "[INFO] Reading from SExpr path: ";
            std::cout << filepath << std::endl;

            updated_settings.hflip = params.find("hflip")->get(1)->valueAsInt();
            updated_settings.vflip = params.find("vflip")->get(1)->valueAsInt();
            updated_settings.auto_exposure = params.find("autoexposure")->get(1)->valueAsInt();
            updated_settings.brightness = params.find("brightness")->get(1)->valueAsInt();
            updated_settings.contrast = params.find("contrast")->get(1)->valueAsInt();
            updated_settings.saturation = params.find("saturation")->get(1)->valueAsInt();
            updated_settings.hue = params.find("hue")->get(1)->valueAsInt();
            updated_settings.sharpness = params.find("sharpness")->get(1)->valueAsInt();
            updated_settings.gamma = params.find("gamma")->get(1)->valueAsInt();
            updated_settings.auto_whitebalance = params.find("auto_whitebalance")->get(1)->valueAsInt();
            updated_settings.exposure = params.find("exposure")->get(1)->valueAsInt();
            updated_settings.gain = params.find("gain")->get(1)->valueAsInt();
            updated_settings.white_balance = params.find("white_balance")->get(1)->valueAsInt();
            updated_settings.fade_to_black = params.find("fade_to_black")->get(1)->valueAsInt();
        } else {
            std::cout<<"[ERR] Invalid SExpr"<<std::endl;   
            std::cout<<"[ERR] Check /nbites/Config/ for them"<<std::endl;  
        }
    } else {
        std::cout<<"[ERR] Config files not found."<<std::endl;
    }

    // DO NOT SCREW UP THE ORDER BELOW
    setControlSetting(V4L2_CID_HFLIP, updated_settings.hflip);
    setControlSetting(V4L2_CID_VFLIP, updated_settings.vflip);

    // Still need to turn this on to change brightness, grumble grumble
    setControlSetting(V4L2_CID_EXPOSURE_AUTO, 1);

    setControlSetting(V4L2_CID_BRIGHTNESS, updated_settings.brightness);
    setControlSetting(V4L2_CID_CONTRAST, updated_settings.contrast);
    setControlSetting(V4L2_CID_SATURATION, updated_settings.saturation);
    setControlSetting(V4L2_CID_HUE, updated_settings.hue);
    setControlSetting(V4L2_CID_SHARPNESS, updated_settings.sharpness);

#ifdef NAOQI_2
    setControlSetting(V4L2_CID_GAMMA, updated_settings.gamma);
#endif
    // Auto white balance, exposure,  and backlight comp off!
    // The first two are both for white balance. The docs don't make
    // it clear what the difference is...
    setControlSetting(V4L2_CID_AUTO_WHITE_BALANCE,
                      updated_settings.auto_whitebalance);
    setControlSetting(V4L2_CID_BACKLIGHT_COMPENSATION,
                      updated_settings.backlight_compensation);
    setControlSetting(V4L2_CID_EXPOSURE_AUTO, updated_settings.auto_exposure);
    setControlSetting(V4L2_CID_EXPOSURE, updated_settings.exposure);
    setControlSetting(V4L2_CID_GAIN, updated_settings.gain);

#ifdef NAOQI_2
    setControlSetting(V4L2_CID_DO_WHITE_BALANCE, 0);
    setControlSetting(V4L2_CID_WHITE_BALANCE_TEMPERATURE, updated_settings.white_balance);
#else
    setControlSetting(V4L2_CID_DO_WHITE_BALANCE, updated_settings.white_balance);
#endif
    setControlSetting(V4L2_MT9M114_FADE_TO_BLACK, updated_settings.fade_to_black);

    //testControlSettings();
}

void ImageTranscriber::testControlSettings() {
    int hflip = getControlSetting(V4L2_CID_HFLIP);
    int vflip = getControlSetting(V4L2_CID_VFLIP);
    int brightness = getControlSetting(V4L2_CID_BRIGHTNESS);
    int contrast = getControlSetting(V4L2_CID_CONTRAST);
    int saturation = getControlSetting(V4L2_CID_SATURATION);
    int hue = getControlSetting(V4L2_CID_HUE);
    int sharpness = getControlSetting(V4L2_CID_SHARPNESS);
    int gain = getControlSetting(V4L2_CID_GAIN);
    int exposure = getControlSetting(V4L2_CID_EXPOSURE);

#ifdef NAOQI_2
    int whitebalance = getControlSetting(V4L2_CID_WHITE_BALANCE_TEMPERATURE);
#else
    int whitebalance = getControlSetting(V4L2_CID_DO_WHITE_BALANCE);
#endif
    int fade = getControlSetting(V4L2_MT9M114_FADE_TO_BLACK);


    std::cout<<"***Settings from Driver***"<<std::endl;
    std::cout<<"Camera: "<<cameraType<<std::endl;
    std::cout<<"HFLIP: "<<hflip<<"\n"
             <<"VFLIP: "<<vflip<<"\n"
             <<"Brightness: "<<brightness<<"\n"
             <<"Contrast: "<<contrast<<"\n"
             <<"Saturation: "<<saturation<<"\n"
             <<"Hue: "<<hue<<"\n"
             <<"Sharpness: "<<sharpness<<"\n"
             <<"Gain: "<<gain<<"\n"
             <<"Exposure: "<<exposure<<"\n"
             <<"Whitebalance: "<<whitebalance<<"\n"
             <<"Fade: "<<fade<<"\n"<<std::endl;
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

    //std::cout<<"VALUE ON DRIVER: "<<getControlSetting(id)<<std::endl;
    //std::cout<<"VALUE PASSED IN: "<<value<<std::endl;

    // Have to make sure the setting "sticks"
    while(getControlSetting(id) != value)
    {
        errno = 0;
        if (ioctl(fd, VIDIOC_S_CTRL, &control_s) < 0)
        {
            int err = errno;
            std::cerr << "CAMERA::Warning::Control setting failed with errno: " <<
                strerror(err) << std::endl;
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
#ifdef NAOQI_2
    int whitebalance = getControlSetting(V4L2_CID_WHITE_BALANCE_TEMPERATURE);
#else
    int whitebalance = getControlSetting(V4L2_CID_DO_WHITE_BALANCE);
#endif
    int fade = getControlSetting(V4L2_MT9M114_FADE_TO_BLACK);

    //std::cerr << "Done checking driver settings" << std::endl;

    if (hflip != updated_settings.hflip)
    {
        std::cerr << "CAMERA::WARNING::Horizontal flip setting is wrong:"
                  << std::endl;
        std::cerr << " is " << hflip << " not " << updated_settings.hflip <<
            std::endl;
        allFine = false;
    }
    if (vflip != updated_settings.vflip)
    {
        std::cerr << "CAMERA::WARNING::Vertical flip setting is wrong:"
                  << std::endl;
        std::cerr << " is " << vflip  << " not " << updated_settings.vflip <<
            std::endl;
        allFine = false;
    }
    if (brightness != updated_settings.brightness)
    {
        std::cerr << "CAMERA::WARNING::Brightness setting is wrong:"
                  << std::endl;
        std::cerr << " is " <<  brightness << " not " << updated_settings.brightness
                  << std::endl;
        allFine = false;
    }
    if (contrast != updated_settings.contrast)
    {
        std::cerr << "CAMERA::WARNING::Contrast setting is wrong:"
                  << std::endl;
        std::cerr << " is " <<  contrast << " not " << updated_settings.contrast
                  << std::endl;
        allFine = false;
    }
    if (saturation != updated_settings.saturation)
    {
        std::cerr << "CAMERA::WARNING::Saturation setting is wrong:"
                  << std::endl;
        std::cerr << " is " <<  saturation << " not " << updated_settings.saturation
                  << std::endl;
        allFine = false;
    }
    if (hue != updated_settings.hue)
    {
        std::cerr << "CAMERA::WARNING::Hue setting is wrong:"
                  << std::endl;
        std::cerr << " is " <<  hue << " not " << updated_settings.hue
                  << std::endl;
        allFine = false;
    }
   if (sharpness != updated_settings.sharpness)
    {
        std::cerr << "CAMERA::WARNING::Sharpness setting is wrong:"
                  << std::endl;
        std::cerr << " is " <<  sharpness << " not " << updated_settings.sharpness
                  << std::endl;
        allFine = false;
    }
   if (gain != updated_settings.gain)
    {
        std::cerr << "CAMERA::WARNING::Gain setting is wrong:"
                  << std::endl;
        std::cerr << " is " <<  gain << " not " << updated_settings.gain
                  << std::endl;
        allFine = false;
    }
   if (exposure != updated_settings.exposure)
    {
        std::cerr << "CAMERA::WARNING::Exposure setting is wrong:"
                  << std::endl;
        std::cerr << " is " <<  exposure << " not " << updated_settings.exposure
                  << std::endl;
        allFine = false;
    }
   if (whitebalance != updated_settings.white_balance)
    {
        std::cerr << "CAMERA::WARNING::Whitebalance setting is wrong:"
                  << std::endl;
        std::cerr << " is " <<  whitebalance << " not " << updated_settings.white_balance
                  << std::endl;
        allFine = false;
    }
   if (fade != updated_settings.fade_to_black)
   {
        std::cerr << "CAMERA::WARNING::Fade to black setting is wrong:"
                  << std::endl;
        std::cerr << " is " <<  fade << " not " << updated_settings.fade_to_black
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

// The heart of the transcriber, returns an image full of pixels from video mem
messages::YUVImage ImageTranscriber::getNextImage()
{
    if (exiting) {
        // Make sure we aren't trying to read from the camera if
        // man is in the process of exiting
        return messages::YUVImage(width*2, height);
    }
    // dequeue a frame buffer (this call blocks when there is
    // no new image available)
    if(cameraType == Camera::TOP)
    {
        PROF_ENTER(P_TOP_DQBUF);
    }
    else
    {
        PROF_ENTER(P_BOT_DQBUF);
    }

    verify(ioctl(fd, VIDIOC_DQBUF, &requestBuff),
           "Dequeueing the frame buffer failed.");

    if(cameraType == Camera::TOP)
    {
        PROF_EXIT(P_TOP_DQBUF);
    }
    else
    {
        PROF_EXIT(P_BOT_DQBUF);
    }

    if(requestBuff.bytesused != (unsigned int)size)
        std::cerr << "CAMERA::ERROR::Wrong buffer size!" << std::endl;

    static bool shout = true;
    if (shout) {
        shout = false;
        std::cerr << "CAMERA::Camera is working." << std::endl;
    }

    // Pixels -> TranscriberBuffer -> YUVImage
    return messages::YUVImage(new TranscriberBuffer(mem[requestBuff.index],
                                                    fd,
                                                    requestBuff),
                              2*width, height, 2*width);

}

TranscriberModule::TranscriberModule(ImageTranscriber& trans)
    : imageOut(base()),
      jointsOut(base()),
      inertsOut(base()),
      it(trans),
      image_index(0),
      file_mod_time(),
      first_time(1)
{
}

TranscriberModule::~TranscriberModule()
{
    std::cout << "Transcriber destructor" << std::endl;
    delete &it;
}

void TranscriberModule::closeTranscriber()
{
    it.closeDriver();
}

void logThumbnail(messages::YUVImage& image, std::string& ifrom, size_t iindex);

// Get image from Transcriber and outportal it
void TranscriberModule::run_()
{
    struct stat file_stats;
    std::string filepath;
    #ifdef NAOQI_2
        if(it.type() == Camera::TOP) {
            filepath = "/home/nao/nbites/Config/V5topCameraParams.txt";
        } else {
            filepath = "/home/nao/nbites/Config/V5bottomCameraParams.txt";
        }
    #else
        if(it.type() == Camera::TOP) {
            filepath = "/home/nao/nbites/Config/V4topCameraParams.txt";
        } else {
            filepath = "/home/nao/nbites/Config/V4bottomCameraParams.txt";
        }
    #endif

    if(FILE *file = fopen(filepath.c_str(),"r")) { //existence check
        fclose(file);
        int err = stat(filepath.c_str(),&file_stats);
        if(first_time == 1) { //prevent settings from initting twice on startup
            std::cout<<"[INFO] First Time Running"<<std::endl;
            file_mod_time = file_stats.st_mtime;
            first_time = 0;
        }
        int time_diff = std::difftime(file_stats.st_mtime, file_mod_time);
        if(time_diff > 0.0) { //check if the file has been modified
            file_mod_time = file_stats.st_mtime;
            std::cout<<"[INFO] New Mod. Time: "<<file_mod_time<<std::endl;
            std::cout<<"[INFO] Calling initSettings() now"<<std::endl;
            it.initSettings();
        }
    } else {
        std::cout<<"[ERR] File Does Not Exist"<<std::endl;
    }

    jointsIn.latch();
    inertsIn.latch();

    /* Pass the most recent joints and inerts thru transcriber and outportal,
     * so that vision has synced images, joints, and inerts to process. */
    jointsOut.setMessage(portals::Message<messages::JointAngles>(
                         &jointsIn.message()));
    inertsOut.setMessage(portals::Message<messages::InertialState>(
                         &inertsIn.message()));

    messages::YUVImage image = it.getNextImage();
    portals::Message<messages::YUVImage> imageOutMessage(&image);
    imageOut.setMessage(imageOutMessage);

#ifdef USE_LOGGING
    if (control::flags[control::thumbnail]) {
        std::string image_from;
        
        if (it.type() == Camera::TOP) {
            image_from = "camera_TOP";
        } else {
            image_from = "camera_BOT";
        }

        logThumbnail(image, image_from, ++image_index);
    }
#endif
}
    
void logThumbnail(messages::YUVImage& image, std::string& ifrom, size_t iindex)
{
    int from_width = image.width() / 2;
    
    if (from_width == 640) {
        resconvert::ImageResolution rFrom = resconvert::R640_480;
        resconvert::ImageResolution rTo = resconvert::R080_060;
        
        char buffer[resconvert::ImageBufferSize[rTo]];
        resconvert::resDownPck(rFrom, (const resconvert::YUVSubPixel *) image.pixelAddress(0,0), rTo, (resconvert::YUVSubPixel *) buffer);
        //std::string im_buf(buffer, resconvert::ImageBufferSize[rTo]);
        
        SExpr ci1("YUVImage", ifrom, clock(), iindex, resconvert::ImageBufferSize[rTo]);
        ci1.append(SExpr::keyValue("width", resconvert::ImageWidth[rTo]));
        ci1.append(SExpr::keyValue("height", resconvert::ImageHeight[rTo]));
        ci1.append(SExpr::keyValue("encoding", "[Y8(U8/V8)]"));
        
        std::vector<SExpr> contents = {ci1};
        NBLog(NBL_IMAGE_BUFFER, "thumbnail",
              contents, (const void *) buffer, resconvert::ImageBufferSize[rTo]);
        
    } else if (from_width == 320) {
        resconvert::ImageResolution rFrom = resconvert::R320_240;
        resconvert::ImageResolution rTo = resconvert::R080_060;
        
        char buffer[resconvert::ImageBufferSize[rTo]];
        resconvert::resDownPck(rFrom, (const resconvert::YUVSubPixel *) image.pixelAddress(0,0), rTo, (resconvert::YUVSubPixel *) buffer);
        //std::string im_buf(buffer, ImageBufferSize[rTo]);
        
        SExpr ci1("YUVImage", ifrom, clock(), iindex, resconvert::ImageBufferSize[rTo]);
        ci1.append(SExpr::keyValue("width", resconvert::ImageWidth[rTo]));
        ci1.append(SExpr::keyValue("height", resconvert::ImageHeight[rTo]));
        ci1.append(SExpr::keyValue("encoding", "[Y8(U8/V8)]"));
        
        std::vector<SExpr> contents = {ci1};
        NBLog(NBL_IMAGE_BUFFER, "thumbnail",
              contents, (void *) buffer, resconvert::ImageBufferSize[rTo]);
    } else {
        printf("WARNING: thumbnail sees size it's not prepared for!\n");
    }
}

}
}
