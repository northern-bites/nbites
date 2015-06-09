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
    timeStamp(0)
    //param(std::string("/home/nao/nbites/Config/") + (which == Camera::TOP ? "top" : "bottom") + "CameraParams.txt")
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
    std::string filepath;
    if(cameraType == Camera::TOP) {
        filepath = "/home/nao/nbites/Config/topCameraParams.txt";
        std::cout<<"[INFO] Camera::TOP"<<std::endl;
    } else {
        filepath = "/home/nao/nbites/Config/bottomCameraParams.txt";
        std::cout<<"[INFO] Camera::BOTTOM"<<std::endl;
    }
    
    if(FILE *file = fopen(filepath.c_str(),"r")) {
        fclose(file);
        
        std::ifstream inputFile(filepath);
        std::string readInFile((std::istreambuf_iterator<char>(inputFile)),
                                std::istreambuf_iterator<char>());

        int i=0;
        SExpr params = *SExpr::read(readInFile,i);

        if(params.count() >= 2) {
            std::cout<<"[INFO] Reading from SExpr"<<std::endl;

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
        }
    } else { //if file does not exist obtain settings specified in Camera.h 
             //(this will be deprecated soon)
        std::cout<<"[INFO] Setting Params from Camera.h"<<std::endl;

        updated_settings.hflip = settings.hflip;
        updated_settings.vflip = settings.vflip;
        updated_settings.auto_exposure = settings.auto_exposure;
        updated_settings.brightness = settings.brightness;
        updated_settings.contrast = settings.contrast;
        updated_settings.saturation = settings.saturation;
        updated_settings.hue = settings.hue;
        updated_settings.sharpness = settings.sharpness;
        updated_settings.gamma = settings.gamma;
        updated_settings.auto_whitebalance = settings.auto_whitebalance;
        updated_settings.backlight_compensation = settings.backlight_compensation;
        updated_settings.exposure = settings.exposure;
        updated_settings.gain = settings.gain;
        updated_settings.white_balance = settings.white_balance;
        updated_settings.fade_to_black = settings.fade_to_black;
    }

    // DO NOT SCREW UP THE ORDER BELOW
    setControlSetting(V4L2_CID_HFLIP, updated_settings.hflip);
    setControlSetting(V4L2_CID_VFLIP, updated_settings.vflip);

    // Still need to turn this on to change brightness, grumble grumble
    setControlSetting(V4L2_CID_EXPOSURE_AUTO, updated_settings.auto_exposure);

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

    // This is actually just the white balance setting!
    setControlSetting(V4L2_CID_DO_WHITE_BALANCE, updated_settings.white_balance);
    setControlSetting(V4L2_MT9M114_FADE_TO_BLACK, updated_settings.fade_to_black);

#ifdef NAOQI_2
    setControlSetting(V4L2_CID_DO_WHITE_BALANCE, 0);
#endif
    // This is actually just the white balance setting!
#ifdef NAOQI_2
    setControlSetting(V4L2_CID_WHITE_BALANCE_TEMPERATURE, updated_settings.white_balance);
#else
    setControlSetting(V4L2_CID_DO_WHITE_BALANCE, updated_settings.white_balance);
#endif
    setControlSetting(V4L2_MT9M114_FADE_TO_BLACK, updated_settings.fade_to_black);

    testControlSettings();
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

    if(requestBuff.bytesused != (unsigned int)SIZE)
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
                                                    2*WIDTH, HEIGHT, 2*WIDTH);
}

TranscriberModule::TranscriberModule(ImageTranscriber& trans)
    : imageOut(base()),
      jointsOut(base()),
      inertsOut(base()),
      it(trans),
      image_index(0),
      file_mod_time()
{
}

// Get image from Transcriber and outportal it
void TranscriberModule::run_()
{
    struct stat file_stats;
    std::string filepath;
    if(it.type() == Camera::TOP) { //set path according to camera
        filepath = "/home/nao/nbites/Config/topCameraParams.txt";
    } else {
        filepath = "/home/nao/nbites/Config/bottomCameraParams.txt";
    }
    if(FILE *file = fopen(filepath.c_str(),"r")) { //existence check
        fclose(file);
        int err = stat(filepath.c_str(),&file_stats);
        if(err != 0) {
            std::cout<<"[INFO] FILE HAS BEEN MODIFIED"<<std::endl;
        }
        int time_diff = std::difftime(file_stats.st_mtime, file_mod_time);
        if(time_diff > 0.0) { //check if the file has been modified
            file_mod_time = file_stats.st_mtime;
            std::cout<<"[INFO] New Mod. Time: "<<file_mod_time<<std::endl;
            std::cout<<"[INFO] Calling initSettings() now"<<std::endl;
            it.initSettings();
        }
    } else {
        //std::cout<<"[ERR] File Does Not Exist"<<std::endl;
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
    if (control::flags[control::tripoint]) {
        ++image_index;
        
        std::string image_from;
        if (it.type() == Camera::TOP) {
            image_from = "camera_TOP";
        } else {
            image_from = "camera_BOT";
        }
        
        long im_size = (image.width() * image.height() * 1);
        int im_width = image.width() / 2;
        int im_height= image.height();
        
        messages::JointAngles ja_pb = jointsIn.message();
        messages::InertialState is_pb = inertsIn.message();
        
        std::string ja_buf;
        std::string is_buf;
        std::string im_buf((char *) image.pixelAddress(0, 0), im_size);
        ja_pb.SerializeToString(&ja_buf);
        is_pb.SerializeToString(&is_buf);
        
        im_buf.append(is_buf);
        im_buf.append(ja_buf);
        
        std::vector<SExpr> contents;
        
        SExpr imageinfo("YUVImage", image_from, clock(), image_index, im_size);
        imageinfo.append(SExpr("width", im_width)   );
        imageinfo.append(SExpr("height", im_height) );
        imageinfo.append(SExpr("encoding", "[Y8(U8/V8)]"));
        contents.push_back(imageinfo);
        
        /*
         // Raw accelerometer data.
         optional float acc_x = 1;
         optional float acc_y = 2;
         optional float acc_z = 3;
         
         // Raw gyrometer data.
         optional float gyr_x = 4;
         optional float gyr_y = 5;
         
         // Filtered angle data.
         optional float angle_x = 6;
         optional float angle_y = 7;
         */
        
        SExpr inerts("InertialState", "tripoint", clock(), image_index, is_buf.length());
        inerts.append(SExpr("acc_x", is_pb.acc_x()));
        inerts.append(SExpr("acc_y", is_pb.acc_y()));
        inerts.append(SExpr("acc_z", is_pb.acc_z()));
        
        inerts.append(SExpr("gyr_x", is_pb.gyr_x()));
        inerts.append(SExpr("gyr_y", is_pb.gyr_y()));
        
        inerts.append(SExpr("angle_x", is_pb.angle_x()));
        inerts.append(SExpr("angle_y", is_pb.angle_y()));
        contents.push_back(inerts);
        
        /*
         // Head angles.
         optional float head_yaw = 1;
         optional float head_pitch = 2;
         
         // Left arm angles.
         optional float l_shoulder_pitch = 3;
         optional float l_shoulder_roll = 4;
         optional float l_elbow_yaw = 5;
         optional float l_elbow_roll = 6;
         optional float l_wrist_yaw = 7;
         optional float l_hand = 8;
         
         // Right arm angles.
         optional float r_shoulder_pitch = 9;
         optional float r_shoulder_roll = 10;
         optional float r_elbow_yaw = 11;
         optional float r_elbow_roll = 12;
         optional float r_wrist_yaw = 13;
         optional float r_hand = 14;
         
         // Pelvis angles.
         optional float l_hip_yaw_pitch = 15;
         optional float r_hip_yaw_pitch = 16;
         
         // Left leg angles.
         optional float l_hip_roll = 17;
         optional float l_hip_pitch = 18;
         optional float l_knee_pitch = 19;
         optional float l_ankle_pitch = 20;
         optional float l_ankle_roll = 21;
         
         // Right leg angles.
         optional float r_hip_roll = 22;
         optional float r_hip_pitch = 23;
         optional float r_knee_pitch = 24;
         optional float r_ankle_pitch = 25;
         optional float r_ankle_roll = 26;
         */
        
        SExpr joints("JointAngles", "tripoint", clock(), image_index, ja_buf.length());
        joints.append(SExpr("head_yaw", ja_pb.head_yaw()));
        joints.append(SExpr("head_pitch", ja_pb.head_pitch()));

        joints.append(SExpr("l_shoulder_pitch", ja_pb.l_shoulder_pitch()));
        joints.append(SExpr("l_shoulder_roll", ja_pb.l_shoulder_roll()));
        joints.append(SExpr("l_elbow_yaw", ja_pb.l_elbow_yaw()));
        joints.append(SExpr("l_elbow_roll", ja_pb.l_elbow_roll()));
        joints.append(SExpr("l_wrist_yaw", ja_pb.l_wrist_yaw()));
        joints.append(SExpr("l_hand", ja_pb.l_hand()));

        joints.append(SExpr("r_shoulder_pitch", ja_pb.r_shoulder_pitch()));
        joints.append(SExpr("r_shoulder_roll", ja_pb.r_shoulder_roll()));
        joints.append(SExpr("r_elbow_yaw", ja_pb.r_elbow_yaw()));
        joints.append(SExpr("r_elbow_roll", ja_pb.r_elbow_roll()));
        joints.append(SExpr("r_wrist_yaw", ja_pb.r_wrist_yaw()));
        joints.append(SExpr("r_hand", ja_pb.r_hand()));

        joints.append(SExpr("l_hip_yaw_pitch", ja_pb.l_hip_yaw_pitch()));
        joints.append(SExpr("r_hip_yaw_pitch", ja_pb.r_hip_yaw_pitch()));

        joints.append(SExpr("l_hip_roll", ja_pb.l_hip_roll()));
        joints.append(SExpr("l_hip_pitch", ja_pb.l_hip_pitch()));
        joints.append(SExpr("l_knee_pitch", ja_pb.l_knee_pitch()));
        joints.append(SExpr("l_ankle_pitch", ja_pb.l_ankle_pitch()));
        joints.append(SExpr("l_ankle_roll", ja_pb.l_ankle_roll()));

        joints.append(SExpr("r_hip_roll", ja_pb.r_hip_roll() ));
        joints.append(SExpr("r_hip_pitch", ja_pb.r_hip_pitch() ));
        joints.append(SExpr("r_knee_pitch", ja_pb.r_knee_pitch() ));
        joints.append(SExpr("r_ankle_pitch", ja_pb.r_ankle_pitch() ));
        joints.append(SExpr("r_ankle_roll", ja_pb.r_ankle_roll() ));
        contents.push_back(joints);
        
        NBLog(NBL_IMAGE_BUFFER, "tripoint",
                   contents, im_buf);
    }
#endif
}

}
}


            /*
            updated_settings.hflip = param.getParam<bool>("hflip");
            updated_settings.vflip = param.getParam<bool>("vflip");
            updated_settings.auto_exposure = param.getParam<bool>("autoexposure");
            updated_settings.brightness = param.getParam<int>("brightness");
            updated_settings.contrast = param.getParam<int>("contrast");
            updated_settings.saturation = param.getParam<int>("saturation");
            updated_settings.hue = param.getParam<int>("hue");
            updated_settings.sharpness = param.getParam<int>("sharpness");
            updated_settings.gamma = param.getParam<int>("gamma");
            updated_settings.auto_whitebalance = param.getParam<int>("auto_whitebalance");
            updated_settings.backlight_compensation = 0x00;
            updated_settings.exposure = param.getParam<int>("exposure");
            updated_settings.gain = param.getParam<int>("gain");
            updated_settings.white_balance = param.getParam<int>("white_balance");
            updated_settings.fade_to_black = param.getParam<int>("fade_to_black");
            */