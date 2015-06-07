#include "TranscriberModule.h"

#include <cstring>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <cerrno>
#include <iostream>
#include <linux/version.h>
#include <bn/i2c/i2c-dev.h>
#include <vector>

#include "Profiler.h"
#include "DebugConfig.h"

#include "../log/logging.h"
#include "../control/control.h"
#include "nbdebug.h"

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

    // Still need to turn this on to change brightness, grumble grumble
    setControlSetting(V4L2_CID_EXPOSURE_AUTO, 1);

    setControlSetting(V4L2_CID_BRIGHTNESS, settings.brightness);
    setControlSetting(V4L2_CID_CONTRAST, settings.contrast);
    setControlSetting(V4L2_CID_SATURATION, settings.saturation);
    setControlSetting(V4L2_CID_HUE, settings.hue);
    setControlSetting(V4L2_CID_SHARPNESS, settings.sharpness);
#ifdef NAOQI_2
    setControlSetting(V4L2_CID_GAMMA, settings.gamma);
#endif

    // Auto white balance, exposure,  and backlight comp off!
    // The first two are both for white balance. The docs don't make
    // it clear what the difference is...
    setControlSetting(V4L2_CID_AUTO_WHITE_BALANCE,
                      settings.auto_whitebalance);
    setControlSetting(V4L2_CID_BACKLIGHT_COMPENSATION,
                      settings.backlight_compensation);
    setControlSetting(V4L2_CID_EXPOSURE_AUTO, settings.auto_exposure);
#ifdef NAOQI_2
    setControlSetting(V4L2_CID_DO_WHITE_BALANCE, 0);
#endif
    setControlSetting(V4L2_CID_EXPOSURE, settings.exposure);
    setControlSetting(V4L2_CID_GAIN, settings.gain);

    // This is actually just the white balance setting!
#ifdef NAOQI_2
    setControlSetting(V4L2_CID_WHITE_BALANCE_TEMPERATURE,
                      settings.white_balance);
#else
    setControlSetting(V4L2_CID_DO_WHITE_BALANCE, settings.white_balance);
#endif
    setControlSetting(V4L2_MT9M114_FADE_TO_BLACK, settings.fade_to_black);
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
#ifdef NAOQI_2
    int whitebalance = getControlSetting(V4L2_CID_WHITE_BALANCE_TEMPERATURE);
#else
    int whitebalance = getControlSetting(V4L2_CID_DO_WHITE_BALANCE);
#endif
    int fade = getControlSetting(V4L2_MT9M114_FADE_TO_BLACK);

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
   if (fade != settings.fade_to_black)
   {
        std::cerr << "CAMERA::WARNING::Fade to black setting is wrong:"
                  << std::endl;
        std::cerr << " is " <<  fade << " not " << settings.fade_to_black
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
      image_index(0)
{
}

// Get image from Transcriber and outportal it
void TranscriberModule::run_()
{
    jointsIn.latch();
    inertsIn.latch();

    filteredBallIn.latch();
    naiveBallIn.latch();

    // filteredBallOut.setMessage(portals::Message<messages::FilteredBall>(
    //                      &filteredBallIn.message()));
    // naiveBallOut.setMessage(portals::Message<messages::NaiveBall>(
    //                      &naiveBallIn.message()));

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

    if (control::flags[control::multiball]) {

        messages::NaiveBall nb_pb = naiveBallIn.message();
        messages::FilteredBall fb_pb = filteredBallIn.message();

        std::string nb_buf;
        std::string fb_buf;

        nb_pb.SerializeToString(&nb_buf);
        fb_pb.SerializeToString(&fb_buf);

        int nb_length = nb_buf.length();
        int fb_length = fb_buf.length();

        nb_buf.append(fb_buf);

        std::vector<SExpr> contents;

        SExpr naive("MULTIBALL", "multiball", clock(), -1, nb_buf.length());
        naive.append(SExpr("nb_length", nb_length));
        naive.append(SExpr("fb_length", fb_length));

        contents.push_back(naive);

        SExpr filter("FilteredBall", "multiball", clock(), -1, fb_buf.length());
        // filter.append(SExpr("vis_distance", fb_pb.vis().distance()));
        // filter.append(SExpr("vis_bearing", fb_pb.vis().bearing()));
        // filter.append(SExpr("vis_on", fb_pb.vis().on()));
        // filter.append(SExpr("vis_x", fb_pb.vis().x()));
        // filter.append(SExpr("vis_y", fb_pb.vis().y()));
        // filter.append(SExpr("distance", fb_pb.distance()));
        // filter.append(SExpr("bearing", fb_pb.bearing()));
        // filter.append(SExpr("rel_x", fb_pb.rel_x()));
        // filter.append(SExpr("rel_y", fb_pb.rel_y()));

        // filter.append(SExpr("vel_x", fb_pb.vel_x()));
        // filter.append(SExpr("vel_y", fb_pb.vel_y()));

        // filter.append(SExpr("var_rel_x", fb_pb.var_rel_x()));
        // filter.append(SExpr("var_rel_y", fb_pb.var_rel_y()));
        // filter.append(SExpr("var_vel_x", fb_pb.var_vel_x()));
        // filter.append(SExpr("var_vel_y", fb_pb.var_vel_y()));
        // filter.append(SExpr("is_stationary", fb_pb.is_stationary()));
        // filter.append(SExpr("bearing_deg", fb_pb.bearing_deg()));
        filter.append(SExpr("x", fb_pb.x()));
        filter.append(SExpr("y", fb_pb.y()));

        // filter.append(SExpr("rel_x_dest", fb_pb.var_rel_x()));
        // filter.append(SExpr("rel_y_dest", fb_pb.rel_y_dest()));
        // filter.append(SExpr("speed", fb_pb.speed()));
        // filter.append(SExpr("rel_y_intersect_dest", fb_pb.rel_y_intersect_dest()));
        // filter.append(SExpr("stat_rel_x", fb_pb.stat_rel_x()));
        // filter.append(SExpr("stat_rel_y", fb_pb.stat_rel_y()));

        // filter.append(SExpr("stat_distance", fb_pb.stat_distance()));
        // filter.append(SExpr("stat_bearing", fb_pb.stat_bearing()));
        // filter.append(SExpr("mov_rel_x", fb_pb.mov_rel_x()));
        // filter.append(SExpr("mov_rel_y", fb_pb.mov_rel_y()));
        // filter.append(SExpr("mov_distance", fb_pb.mov_distance()));
        // filter.append(SExpr("mov_bearing", fb_pb.mov_bearing()));
        // filter.append(SExpr("mov_vel_x", fb_pb.mov_vel_x()));
        // filter.append(SExpr("mov_vel_y", fb_pb.mov_vel_y()));
        // filter.append(SExpr("mov_speed", fb_pb.mov_speed()));

        contents.push_back(filter);

        NBLog(NBL_IMAGE_BUFFER, "multiball",
                   contents, nb_buf);
    }
#endif
}

}
}
