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
#include "perception/vision/NaoCameraV4.hpp"
#include "utils/Logger.hpp"
#include "utils/speech.hpp"
#include "utils/Timer.hpp"

/* Used by setCamera only.
 * Can be removed when v3 backwards compadability is nolonger needed
 */
#include "perception/vision/Vision.hpp"


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

#define CAMERA_FRAMERATE 30

/**
 * Set the initial settings for the camera(format, etc.)
 */
NaoCameraV4::NaoCameraV4(Blackboard *blackboard, const char *filename, const IOMethod method,
                     const int format, const WhichCamera initialCamera)
:
   /* 0 is a dummy value that causes the protected constructor to be called */
   NaoCamera(blackboard, filename, method, format, WHICH_CAMERA_ERROR, 0)
{
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
   setControl(V4L2_CID_SET_DEFAULT_PARAMETERS, 0);

   // reopen your camera (SET_DEFAULT_PARAMETERS causes hang on DQBUF)
   close_device();
   open_device();

   init_buffers();

   if (!init_camera()) {
      llog(FATAL) << "Error initializing camera!\n";
      throw runtime_error("Error initializing camera");
   }

   start_capturing();

   // close everything
   //stop_capturing();
   //uninit_buffers();
//   close_device();

   // re open it again
//   open_device();
   //init_buffers();
/*
   if (!init_camera()) {
      llog(FATAL) << "Error initializing camera!\n";
      throw runtime_error("Error initializing camera");
   }
*/
   //start_capturing();
   
   dumpFile = NULL;
}

NaoCameraV4::~NaoCameraV4()
{
}

bool NaoCameraV4::setCamera(WhichCamera whichCamera) {
   if (whichCamera == TOP_CAMERA) {
      Vision::camera = Vision::top_camera;
      return true;
   } else if (whichCamera == BOTTOM_CAMERA) {
      Vision::camera = Vision::bot_camera;
      //Vision::camera = Vision::top_camera;
      return true;
   } else {
      return false;
   }
}

WhichCamera NaoCameraV4::getCamera() {
   if (Vision::camera == Vision::top_camera) {
      return TOP_CAMERA;
   } else {
      return BOTTOM_CAMERA;
   }
}
