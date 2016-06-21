#include <malloc.h>
#include <limits.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cassert>
#include <cerrno>
#include <cstring>
#include <ctime>
#include <iostream>
#include <stdexcept>
#include "perception/vision/NaoCamera.hpp"
#include "utils/i2c-dev.h"
#include "utils/Logger.hpp"
#include "utils/speech.hpp"
#include "utils/Timer.hpp"

using namespace std;

/**
 * reads the system error and writes it to the log, then throws an exception
 * @param s an additional string, decribing the failed action
 */
static inline void errno_throw(const char *s) {
   llog(ERROR) << s << " error "<< errno << ", " << strerror(errno) << endl;
   throw runtime_error(strerror(errno));
}

/**
 * sets all bytes of x to 0
 * @param x the variable to clear
 */
#define CLEAR(x) memset(&(x), 0, sizeof(x))

bool NaoCamera::setControl(const uint32_t controlId,
                           const int32_t controlValue) {
   if (v4lDeviceP) {
      struct v4l2_control control;
      CLEAR(control);
      if (controlId < V4L2_CID_BASE)
         control.id = V4L2_CID_BASE + controlId;
      else
         control.id    = controlId;
      control.value = controlValue;

      struct v4l2_control current;
      CLEAR(current);
      current.id = control.id;
      ioctl (fd, VIDIOC_G_CTRL, &current);
      cout << "1setting id " << control.id << "to " << controlValue << std::endl;
      while (current.value != controlValue) {
//         CLEAR(current);
//         current.id = control.id;
         ioctl(fd, VIDIOC_S_CTRL, &control);
         ioctl (fd, VIDIOC_G_CTRL, &current);
         //cout << "setting id " << control.id << "to " << controlValue << std::endl;
      }
/*
      if (-1 == ioctl(fd, VIDIOC_S_CTRL, &control) && errno != ERANGE) {
         llog(ERROR) << "VIDIOC_S_CTRL error " << errno << ": " <<
               strerror(errno) << endl << "control.id: " << control.id << endl
               << "control.value: " << control.value << endl;
         return false;
      }
*/
      return true;
   } else {
      llog(WARNING) << "tried to change settings on a file, not a v4l device";
      return false;
   }
}

/**
 * These adresses, commands and flags are given to us by aldeberan. They enable
 * talking to and controlling the camera controller
 * I2C_DEVICE       - the i2c bus that the camera controller resides on
 * I2C_SLAVE        - the flag to indicate slave mode on the i2c bus. allows
 *                    writing to the camera controller
 * DSPIC_I2C_ADDR   - the address of the camera controller on the i2c bus
 * DSPIC_SWITCH_REG - register of the active camera. can be read to determine
 *                    camera or set to change camera.
 *                    currently 0x01 is top camera and 0x02 is bottom.
 */
#define I2C_DEVICE "/dev/i2c-0"
#define I2C_SLAVE 0x0703
#define DSPIC_I2C_ADDR 0x8
#define DSPIC_SWITCH_REG 220

#define CAMERA_FRAMERATE 30

using namespace AL;

/**
 * Set the initial settings for the camera(format, etc.)
 */
NaoCamera::NaoCamera(Blackboard *blackboard, const char *filename, const IOMethod method,
                     const int format, const WhichCamera initialCamera):
      filename(filename), io(method), format(format),
               currentCamera(WHICH_CAMERA_ERROR) {

    readCameraSettings(blackboard);
   // open your camera
   open_device();
   // decide if its a file or a device
   struct stat buf;
   if (fstat(fd, &buf) < 0) {
      llog(ERROR) << "failed to stat " << filename << endl;
      throw runtime_error("inside NaoCamera.cpp, failed to stat camera");
   }
   v4lDeviceP = S_ISCHR(buf.st_mode);

   // fix green camera bug
   immediateSetCamera(TOP_CAMERA);
   setControl(V4L2_CID_SET_DEFAULT_PARAMETERS, 0);
   immediateSetCamera(BOTTOM_CAMERA);
   setControl(V4L2_CID_SET_DEFAULT_PARAMETERS, 0);
   // reopen your camera (SET_DEFAULT_PARAMETERS causes hang on DQBUF)
   close_device();
   open_device();

   init_buffers();

   // init() the first camera
   if (!init_camera()) {
      llog(FATAL) << "Error initializing camera!\n";
      throw runtime_error("Error initializing camera");
   }

   // force getCamera to check the hardware
   currentCamera = getCamera();

   // because set camera stops capturing
   start_capturing();
   // swap to and init() the second camera
   // note that there is no swap back
   setCamera((currentCamera == TOP_CAMERA)?BOTTOM_CAMERA:TOP_CAMERA);

   // because set camera starts capturing, and because we don't want to stream
   // while init'ing (although alvideo input inits while streaming)
   stop_capturing();

   if (!init_camera()) {
      llog(FATAL) << "Error initializing second camera!\n";
      throw runtime_error("Error initializing second camera");
   }
   start_capturing();
   setCamera(initialCamera);
   dumpFile = NULL;
}


/**
 * Initialise parameters without initialising the camera.
 */
NaoCamera::NaoCamera(Blackboard *blackboard, const char *filename, const IOMethod method,
                     const int format, const WhichCamera initialCamera,
                     int dummy):
      filename(filename), io(method), format(format),
               currentCamera(WHICH_CAMERA_ERROR)
{
    readCameraSettings(blackboard);
}


void NaoCamera::readCameraSettings(Blackboard *blackboard) {
    readCameraSettings(blackboard, topCameraSettings, std::string("camera.top"));
    readCameraSettings(blackboard, botCameraSettings, std::string("camera.bot"));
}

void NaoCamera::readCameraSettings(Blackboard *blackboard,
        CameraSettings &settings, std::string cameraName) {

    settings.hflip = blackboard->config[(cameraName + ".hflip").c_str()].as<int>();
    settings.vflip = blackboard->config[(cameraName + ".vflip").c_str()].as<int>();
    settings.brightness = blackboard->config[(cameraName + ".brightness").c_str()].as<int>();
    settings.contrast = blackboard->config[(cameraName + ".contrast").c_str()].as<int>();
    settings.saturation = blackboard->config[(cameraName + ".saturation").c_str()].as<int>();
    settings.hue = blackboard->config[(cameraName + ".hue").c_str()].as<int>();
    settings.sharpness = blackboard->config[(cameraName + ".sharpness").c_str()].as<int>();
    settings.backlightCompensation = blackboard->config[(cameraName + ".backlightcompensation").c_str()].as<int>();
    settings.exposure = blackboard->config[(cameraName + ".exposure").c_str()].as<int>();
    settings.gain = blackboard->config[(cameraName + ".gain").c_str()].as<int>();
    settings.whiteBalance = blackboard->config[(cameraName + ".whitebalance").c_str()].as<int>();
}

void NaoCamera::setCameraSettings(const WhichCamera camera) {

    CameraSettings &settings = topCameraSettings;
    if (camera == TOP_CAMERA) {
        settings = topCameraSettings;
    } else if (camera == BOTTOM_CAMERA) {
        settings = botCameraSettings;
    } else {
        return;
    }
    setControl(V4L2_CID_HFLIP, settings.hflip);
    setControl(V4L2_CID_VFLIP, settings.vflip);
    setControl(V4L2_CID_AUTO_WHITE_BALANCE, 1);
    setControl(V4L2_CID_EXPOSURE_AUTO, 1);
    setControl(V4L2_CID_BRIGHTNESS, settings.brightness);
    setControl(V4L2_CID_CONTRAST, settings.contrast);
    setControl(V4L2_CID_SATURATION, settings.saturation);
    setControl(V4L2_CID_HUE, settings.hue);
    setControl(V4L2_CID_SHARPNESS, settings.sharpness);
    setControl(V4L2_CID_AUTO_WHITE_BALANCE, 0);
    setControl(V4L2_CID_BACKLIGHT_COMPENSATION, settings.backlightCompensation);
    setControl(V4L2_CID_EXPOSURE_AUTO, 0);
    setControl(V4L2_CID_EXPOSURE, settings.exposure);
    setControl(V4L2_CID_GAIN, settings.gain);
    setControl(V4L2_CID_DO_WHITE_BALANCE, settings.whiteBalance);

/*

   for (unsignedk int controlIndex = 0; controlIndex < NUM_CONTROLS;
        ++controlIndex) {
      setControl(controlIds[controlIndex],
                       controlValues[camera][controlIndex]);
   }
*/
}

NaoCamera::~NaoCamera() {
   stop_capturing();
   uninit_buffers();
   close_device();
}

bool NaoCamera::imgDimensions(int *const width, int *const height) {
   switch (format) {
      case k960p:
         *width  = 1280;
         *height = 960;
         break;
      case kVGA:
         *width  = 640;
         *height = 480;
         break;
      case kQVGA:
         *width  = 320;
         *height = 240;
         break;
      case kQQVGA:
         *width  = 160;
         *height = 120;
         break;
      default:
         return false;
   }
   return true;
}

const uint8_t *NaoCamera::read_frame(void) {
   unsigned int i;
   const uint8_t *image = NULL;
   switch (io) {
      /* reading from file and video device are exactly the same */
      case IO_METHOD_READ:
         if (-1 == read(fd, buffers[0].start, buffers[0].length)) {
            switch (errno) {
               case EAGAIN:
                  return false;

               case EIO:
                  /* Could ignore EIO, see spec. */

                  /* fall through */

               default:
                  errno_throw("read");
            }
         }

         image = buffers[0].start;

         break;

      case IO_METHOD_MMAP:
         if (lastDequeued.index != UINT_MAX) {
            if (-1 == ioctl(fd, VIDIOC_QBUF, &lastDequeued)) {
               errno_throw("VIDEOC_BUF");
            }
         }

         CLEAR(lastDequeued);

         lastDequeued.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
         lastDequeued.memory = V4L2_MEMORY_MMAP;

         if (-1 == ioctl(fd, VIDIOC_DQBUF, &lastDequeued)) {
            switch (errno) {
               case EAGAIN:
                  return false;

               case EIO:
                  /* Could ignore EIO, see spec. */
                  std::cout << "eio" << std::endl;

                  /* fall through */
               case EFAULT:
                  std::cout << "efault" << std::endl;

               case EINVAL:
                  std::cout << "einval" << std::endl;

               default:
                  std::cout << "lol mmap" << std::endl;
                  errno_throw("VIDIOC_DQBUF");
            }
         }

         assert(lastDequeued.index < n_buffers);

         image = buffers[lastDequeued.index].start;

         break;

      case IO_METHOD_USERPTR:
         // TODO(jayen): verify that lastDequeued fix works for user pointers
         if (lastDequeued.index != UINT_MAX) {
            if (-1 == ioctl(fd, VIDIOC_QBUF, &lastDequeued)) {
               errno_throw("VIDEOC_BUF");
            }
         }

         CLEAR(lastDequeued);

         lastDequeued.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
         lastDequeued.memory = V4L2_MEMORY_USERPTR;

         if (-1 == ioctl(fd, VIDIOC_DQBUF, &lastDequeued)) {
            switch (errno) {
               case EAGAIN:
                  return false;

               case EIO:
                  /* Could ignore EIO, see spec. */

                  /* fall through */

               default:
                  std::cout << "lol user?" << std::endl;
                  errno_throw("VIDIOC_DQBUF");
            }
         }

         for (i = 0; i < n_buffers; ++i)
            if (lastDequeued.m.userptr == (uint32_t) buffers[i].start
                && lastDequeued.length == buffers[i].length)
               break;

         assert(i < n_buffers);

         image =(const uint8_t*) lastDequeued.m.userptr;

         break;

      default:
         throw runtime_error("what kind of IO method is that?!?!?");
   }

   return image;
}

const uint8_t *NaoCamera::get(const int colourSpace) {
   if (colourSpace != kYUVColorSpace)
      throw runtime_error("only yuv422 is supported!");
   fd_set fds;
   struct timeval tv;
   int r;
   FD_ZERO(&fds);
   FD_SET(fd, &fds);

   /* Timeout. */
   tv.tv_sec = 0;
   tv.tv_usec = 100000;

   if (io == IO_METHOD_MMAP) {
      select:
      r = select(fd + 1, &fds, NULL, NULL, &tv);

      if (-1 == r) {
         if (EINTR == errno)
            goto select;  // Got interrupted (by profiler?)

         errno_throw("select");
      }

      if (0 == r)
         llog(ERROR) << "select timeout\n";
   }

   const uint8_t *image = read_frame();
   writeFrame(image);
   llog(DEBUG2) << "image returning from NaoCamera: " << (void *)image << endl;
   return image;
}

bool NaoCamera::init_camera() {
   if (v4lDeviceP) {
      // GET video device information
      v4l2_std_id esid0;
      CLEAR (esid0);
      int test;
      test = ioctl(fd, VIDIOC_G_STD, &esid0);
      if (test != 0) {
         perror("ioctl 1");
         llog(ERROR) << "failed ioctl with error code " << test << endl;
         throw runtime_error("inside NaoCamera.cpp, failed ioctl");
      }

      // set video device standard
      switch (format) {
         // aldebaran is invalidly using this field
         // http:// v4l2spec.bytesex.org/spec-single/v4l2.html#V4L2-STD-ID
         // The 32 most significant bits are reserved for custom(driver defined)
         // video standards.
         // There isn't supposed to be a way to specify QQVGA with this field
         // REVIEW: can these not be defined earlier?
         case k960p:
            esid0 =(v4l2_std_id)0x08000000; /*VGA*/
            break;
         case kVGA:
            esid0 =(v4l2_std_id)0x08000000; /*VGA*/
            break;
         case kQVGA:
            esid0 =(v4l2_std_id)0x04000000; /*QVGA*/
            break;
         default:
            throw runtime_error("inside NaoCamera.cpp, unknown format");
      }
      test = ioctl(fd, VIDIOC_S_STD, &esid0);
      if (test != 0) {
         perror("ioctl 2");
         llog(ERROR) << "failed ioctl 2 error " << test << endl;
      }
      // set video device format
      struct v4l2_format fmt0;
      memset(&fmt0, 0, sizeof(fmt0));
      fmt0.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      fmt0.fmt.pix.field = V4L2_FIELD_NONE;

      switch (format) {
         case k960p:
            fmt0.fmt.pix.width       = 1280;
            fmt0.fmt.pix.height      = 960;
            break;
         case kVGA:
            fmt0.fmt.pix.width       = 640;
            fmt0.fmt.pix.height      = 480;
            break;
         case kQVGA:
            fmt0.fmt.pix.width       = 320;
            fmt0.fmt.pix.height      = 240;
            break;
         default:
            throw runtime_error("inside NaoCamera.cpp, unknown format");
      }

      // fmt0.fmt.pix.pixelformat = 0x56595559;

      fmt0.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
      test = ioctl(fd, VIDIOC_S_FMT, &fmt0);
      if (test != 0) {
         perror("ioctl 3");
         llog(ERROR) << "failed ioctl 3 error" << test << endl;
         throw runtime_error("NaoCamera.cpp, failed ioctl 3");
      }

      // set fps
      struct v4l2_streamparm parm;
      CLEAR(parm);

      parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      test = ioctl(fd, VIDIOC_G_PARM, &parm);
      if (test != 0) {
         perror("ioctl 4");
         llog(ERROR) << "failed ioctl 4" << endl;
         throw runtime_error("NaoCamera.cpp, failed ioctl 4");
      }
      parm.parm.capture.timeperframe.numerator = 1;
      parm.parm.capture.timeperframe.denominator = CAMERA_FRAMERATE;
      parm.parm.capture.capability = V4L2_CAP_TIMEPERFRAME;
      ioctl(fd, VIDIOC_S_PARM, &parm);

      imageSize = fmt0.fmt.pix.sizeimage;

      setCameraSettings(getCamera());
   }

   return true;
}

void NaoCamera::stop_capturing(void) {
   enum v4l2_buf_type type;

   switch (io) {
      case IO_METHOD_READ:
         /* Nothing to do. */
         break;

      case IO_METHOD_MMAP:
      case IO_METHOD_USERPTR:
         type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

         if (-1 == ioctl(fd, VIDIOC_STREAMOFF, &type))
            errno_throw("VIDIOC_STREAMOFF");

         break;
      default:
         throw runtime_error("unknown io method");
         break;
   }
}

void NaoCamera::uninit_buffers(void) {
   unsigned int i;

   switch (io) {
      case IO_METHOD_READ:
         free(buffers[0].start);
         break;

      case IO_METHOD_MMAP:
         for (i = 0; i < n_buffers; ++i)
            if (-1 == munmap(buffers[i].start, buffers[i].length))
               errno_throw("munmap");
         break;

      case IO_METHOD_USERPTR:
         for (i = 0; i < n_buffers; ++i)
            free(buffers[i].start);
         break;
      default:
         throw runtime_error("unknown io method");
         break;
   }
}

void NaoCamera::open_device(void) {
   fd = open(filename.c_str(), O_CLOEXEC | O_RDWR);

   if (fd < 0) {
      llog(ERROR) << "failed to open " << filename << endl;
      sleep(5);
      SAY(string("dev video ") + strerror(errno), true);
      throw runtime_error("inside NaoCamera.cpp, failed to open camera");
   }
}

void NaoCamera::close_device(void) {
   if (-1 == close(fd))
      errno_throw("close");

   fd = -1;
}

void NaoCamera::open_i2cbus(void) {
   i2cBus = open(I2C_DEVICE, O_CLOEXEC | O_RDWR);
   if (i2cBus < 0) {
      llog(ERROR) << " : error opening I2C for connection to dsPIC" << endl;
      throw runtime_error("error opening I2C for connection to dsPIC");
   }
}

void NaoCamera::close_i2cbus(void) {
   if (close(i2cBus) < 0) {
      llog(ERROR) << __PRETTY_FUNCTION__
                  << " : error closing I2C connection to dsPIC" << endl;
   }
}

void NaoCamera::init_mmap(void) {
   struct v4l2_requestbuffers req;

   CLEAR(req);

   req.count  = NUM_FRAME_BUFFERS;
   req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
   req.memory = V4L2_MEMORY_MMAP;

   if (-1 == ioctl(fd, VIDIOC_REQBUFS, &req)) {
      if (EINVAL == errno) {
         llog(ERROR) << "memory mapping not supported on this device!" << endl;
         exit(EXIT_FAILURE);
      } else {
         errno_throw("VIDIOC_REQBUFS");
      }
   }

   if (req.count < 1) {
      llog(ERROR) << "Insufficient buffer memory" << endl;
      throw runtime_error("Insufficient buffer memory");
   }

   CLEAR(buffers);

   if (!buffers) {
      llog(ERROR) << "Out of memory\n";
      exit(EXIT_FAILURE);
   }

   for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
      struct v4l2_buffer buf;

      CLEAR(buf);

      buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      buf.memory = V4L2_MEMORY_MMAP;
      buf.index  = n_buffers;

      if (-1 == ioctl(fd, VIDIOC_QUERYBUF, &buf))
         errno_throw("VIDIOC_QUERYBUF");

      buffers[n_buffers].length = buf.length;
      buffers[n_buffers].start = reinterpret_cast<uint8_t*>
            (mmap(NULL /* start anywhere */,
                  buf.length,
                  PROT_READ | PROT_WRITE /* required */,
                  MAP_SHARED /* recommended */,
                  fd, buf.m.offset));

      if (MAP_FAILED == buffers[n_buffers].start)
         errno_throw("mmap");
   }
}

void NaoCamera::init_buffers(void) {
   if (v4lDeviceP) {
      struct v4l2_capability cap;
      struct v4l2_cropcap cropcap;
      struct v4l2_crop crop;
      struct v4l2_format fmt;
      v4l2_std_id esid0;
      unsigned int min;

      CLEAR(cap);
      CLEAR(cropcap);
      CLEAR(crop);
      CLEAR(fmt);
      if (-1 == ioctl(fd, VIDIOC_QUERYCAP, &cap)) {
         if (EINVAL == errno) {
            llog(ERROR) << "not an V4L2 device" << endl;
            throw runtime_error("not a v4l2 device");
         } else {
            errno_throw("VIDIOC_QUERYCAP");
         }
      }

      if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
         llog(ERROR) << "not a video capture device" << endl;
         throw runtime_error("not a video capture device");
      }

      switch (io) {
         case IO_METHOD_READ:
            if (!(cap.capabilities & V4L2_CAP_READWRITE)) {
               llog(ERROR) << "does not support read i/o" << endl;
               throw runtime_error("does not support read i/o");
            }

            break;

         case IO_METHOD_MMAP:
         case IO_METHOD_USERPTR:
            if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
               llog(ERROR) << "does not support streaming i/o" << endl;
               throw runtime_error("does not support streaming i/o");
            }

            break;

         default:
            throw runtime_error("what kind of IO method is that?!?!?");
      }


      /* Select video input, video standard and tune here. */


      CLEAR(cropcap);

      cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

      if (0 == ioctl(fd, VIDIOC_CROPCAP, &cropcap)) {
         crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
         crop.c = cropcap.defrect; /* reset to default */

         if (-1 == ioctl(fd, VIDIOC_S_CROP, &crop)) {
            switch (errno) {
               case EINVAL:
                  /* Cropping not supported. */
                  break;
               default:
                  /* Errors ignored. */
                  break;
            }
         }
      } else {
         /* Errors ignored. */
      }

      switch (format) {
         // REVIEW: this is done somewhere else - maybe we can refactor it out
         // aldebaran is invalidly using this field
         // http:// v4l2spec.bytesex.org/spec-single/v4l2.html#V4L2-STD-ID
         // The 32 most significant bits are reserved for custom(driver defined)
         // video standards.
         // There isn't supposed to be a way to specify QQVGA with this field
         case k960p:
            esid0 = 0x08000000UL; /*VGA*/
            break;
         case kVGA:
            esid0 = 0x08000000UL; /*VGA*/
            break;
         case kQVGA:
            esid0 = 0x04000000UL; /*QVGA*/
            break;
      }
      if (-1 == ioctl(fd, VIDIOC_S_STD, &esid0))
         errno_throw("VIDIOC_S_STD");

      CLEAR(fmt);

      fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      switch (format) {
         case k960p:
            fmt.fmt.pix.width     = 1280;
            fmt.fmt.pix.height    = 960;
            break;
         case kVGA:
            fmt.fmt.pix.width     = 640;
            fmt.fmt.pix.height    = 480;
            break;
         case kQVGA:
            fmt.fmt.pix.width     = 320;
            fmt.fmt.pix.height    = 240;
            break;
      }
      fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
      fmt.fmt.pix.field       = V4L2_FIELD_NONE;

      if (-1 == ioctl(fd, VIDIOC_G_FMT, &fmt)) 
         errno_throw("VIDIOC_G_FMT");

      if (-1 == ioctl(fd, VIDIOC_S_FMT, &fmt))
         errno_throw("VIDIOC_S_FMT");

      /* Note VIDIOC_S_FMT may change width and height. */

      /* Buggy driver paranoia. */
      min = fmt.fmt.pix.width * 2;
      if (fmt.fmt.pix.bytesperline < min)
         fmt.fmt.pix.bytesperline = min;
      min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
      if (fmt.fmt.pix.sizeimage < min)
         fmt.fmt.pix.sizeimage = min;

      imageSize = fmt.fmt.pix.sizeimage;
   } else {
      llog(INFO) << "no need to init buffers for files" << endl;
   }

   switch (io) {
      case IO_METHOD_READ:
         init_read();
         break;

      case IO_METHOD_MMAP:
         init_mmap();
         break;

      case IO_METHOD_USERPTR:
         init_userp();
         break;

      default:
         throw runtime_error("what kind of IO method is that?!?!?");
   }
}

void NaoCamera::init_userp() {
   struct v4l2_requestbuffers req;
   unsigned int page_size;

   page_size = getpagesize();
   imageSize =(imageSize + page_size - 1) & ~(page_size - 1);

   CLEAR(req);

   req.count  = NUM_FRAME_BUFFERS;
   req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
   req.memory = V4L2_MEMORY_USERPTR;

   if (-1 == ioctl(fd, VIDIOC_REQBUFS, &req)) {
      if (EINVAL == errno) {
         llog(ERROR) << "does not support user pointer i/o" << endl;
         throw runtime_error("does not support user pointer i/o");
      } else {
         errno_throw("VIDIOC_REQBUFS");
      }
   }

   CLEAR(buffers);

   if (!buffers) {
      llog(ERROR) << "Out of memory\n";
      exit(EXIT_FAILURE);
   }

   for (n_buffers = 0; n_buffers < NUM_FRAME_BUFFERS; ++n_buffers) {
      buffers[n_buffers].length = imageSize;
      buffers[n_buffers].start =
            reinterpret_cast<uint8_t*>(memalign(/* boundary */ page_size,
                                                imageSize));

      if (!buffers[n_buffers].start) {
         llog(ERROR) << "Out of memory\n";
         exit(EXIT_FAILURE);
      }
   }
}

void NaoCamera::start_capturing(void) {
   // assign an impossible index to lastDequeued so we know not to
   // re-enqueue if there have been no previous buffers
   lastDequeued.index = UINT_MAX;

   unsigned int i;
   enum v4l2_buf_type type;

   switch (io) {
      case IO_METHOD_READ:
         /* Nothing to do. */
         break;

      case IO_METHOD_MMAP:
         for (i = 0; i < n_buffers; ++i) {
            struct v4l2_buffer buf;

            CLEAR(buf);

            buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory      = V4L2_MEMORY_MMAP;
            buf.index       = i;

            if (-1 == ioctl(fd, VIDIOC_QBUF, &buf))
               errno_throw("VIDIOC_QBUF");
         }

         type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

         if (-1 == ioctl(fd, VIDIOC_STREAMON, &type))
            errno_throw("VIDIOC_STREAMON");

         break;

      case IO_METHOD_USERPTR:
         for (i = 0; i < n_buffers; ++i) {
            struct v4l2_buffer buf;

            CLEAR(buf);

            buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory      = V4L2_MEMORY_USERPTR;
            buf.index       = i;
            buf.m.userptr   = (uint32_t) buffers[i].start;
            buf.length      = buffers[i].length;

            if (-1 == ioctl(fd, VIDIOC_QBUF, &buf))
               errno_throw("VIDIOC_QBUF");
         }

         type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

         if (-1 == ioctl(fd, VIDIOC_STREAMON, &type))
            errno_throw("VIDIOC_STREAMON");

         break;

      default:
         throw runtime_error("what kind of IO method is that?!?!?");
   }
}

void NaoCamera::init_read(void) {
   CLEAR(buffers);

   if (!buffers) {
      llog(ERROR) << "Out of memory\n";
      exit(EXIT_FAILURE);
   }

   buffers[0].length = imageSize;
   buffers[0].start  = reinterpret_cast<uint8_t*>(malloc(imageSize));

   if (!buffers[0].start) {
      llog(ERROR) << "Out of memory\n";
      exit(EXIT_FAILURE);
   }
}

bool NaoCamera::immediateSetCamera(WhichCamera whichCamera) {
   if (v4lDeviceP) {
      // cmd will be written to the camera controllers 'switch camera' register
      // we set it to the appropriate value.
      // REVIEW: perhaps we should define 0x01 and 0x02 earlier in case it
      // changes...
      // padded byte is for "PEC" (see i2c-dev.h)
      static unsigned char cmd[2] = {0, 0};
      switch (whichCamera) {
         case TOP_CAMERA:
            cmd[0] = 0x01;
            break;
         case BOTTOM_CAMERA:
            cmd[0] = 0x02;
            break;
         default:
            return false;
      }

      // Connect to dsPIC through I2C, then set cameras
      open_i2cbus();
      if (ioctl(i2cBus, I2C_SLAVE, DSPIC_I2C_ADDR)) {
         llog(ERROR) << " : Can't connect I2C to dsPIC" << endl;
         close_i2cbus();
         return false;
      }
      // even though length is 1, cmd includes the PEC
      int size = i2c_smbus_write_block_data(i2cBus, DSPIC_SWITCH_REG, 1, cmd);
      close_i2cbus();
      if (size == -1) {
         llog(ERROR) << __PRETTY_FUNCTION__
               << " : error switching to bottom camera" << endl;
         return false;
      }
   }
   currentCamera = whichCamera;
   return true;
}

bool NaoCamera::setCamera(WhichCamera whichCamera) {
   
   // TODO(jayen): this shouldn't be dependent on a v4l device, but rather
   // if this is a nao
   if (v4lDeviceP) {
      if (whichCamera == getCamera())
         return true;

      Timer timer;
      // cmd will be written to the camera controllers 'switch camera' register
      // we set it to the appropriate value.
      // REVIEW: perhaps we should define 0x01 and 0x02 earlier in case it
      // changes...
      // padded byte is for "PEC" (see i2c-dev.h)
      static unsigned char cmd[2] = {0, 0};
      switch (whichCamera) {
         case TOP_CAMERA:
            cmd[0] = 0x01;
            break;
         case BOTTOM_CAMERA:
            cmd[0] = 0x02;
            break;
         default:
            return false;
      }

      // need to stop capture at switch time, so as not to switch in mid-frame
      stop_capturing();
      // close_device(); // closing and reopening takes 300ms

      // Connect to dsPIC through I2C, then set cameras
      open_i2cbus();
      if (ioctl(i2cBus, I2C_SLAVE, DSPIC_I2C_ADDR)) {
         llog(ERROR) << " : Can't connect I2C to dsPIC" << endl;
         close_i2cbus();
         return false;
      }
      // even though length is 1, cmd includes the PEC
      int size = i2c_smbus_write_block_data(i2cBus, DSPIC_SWITCH_REG, 1, cmd);
      close_i2cbus();

      // open_device(); // closing and reopening takes 300ms
      // init_buffers(); // closing and reopening takes 300ms
      start_capturing();

      if (size == -1) {
         llog(ERROR) << __PRETTY_FUNCTION__
                     << " : error switching to bottom camera" << endl;
         return false;
      }
      llog(VERBOSE) << "setCamera took " << timer.elapsed_us() << " useconds\n";
   }

   currentCamera = whichCamera;

   return true;
}

WhichCamera NaoCamera::getCamera() {
   if (currentCamera != WHICH_CAMERA_ERROR)
      return currentCamera;

   // TODO(jayen): this shouldn't be dependent on a v4l device, but rather
   // if this is a nao
   if (v4lDeviceP) {
      // Connect to dsPIC through I2C
      open_i2cbus();
      if (ioctl(i2cBus, I2C_SLAVE, DSPIC_I2C_ADDR)) {
         llog(ERROR) << " : Can't connect I2C to dsPIC" << endl;
         close_i2cbus();
         return WHICH_CAMERA_ERROR;
      }
      // read the camera controller register which contains the current camera
      int val = i2c_smbus_read_byte_data(i2cBus, DSPIC_SWITCH_REG);
      close_i2cbus();
      if (val == -1) {
         llog(ERROR) << __PRETTY_FUNCTION__
                     << " : error asking which camera is active to dsPIC"
                     << endl;
         return WHICH_CAMERA_ERROR;
      }
      llog(INFO) <<((val == 0x1)?"Top Camera\n":"Bottom Camera\n");

      currentCamera =((val == 0x1)?TOP_CAMERA:BOTTOM_CAMERA);
   }
   return currentCamera;
}
