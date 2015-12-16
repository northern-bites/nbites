#include "perception/vision/Camera.hpp"
#include "utils/Timer.hpp"
#include "utils/Logger.hpp"

using namespace std;

/**
 * Controls:


***** NOTE THAT THESE CONTROLS ARE NO LONGER USED        *****
***** WE NOW SET THEM ONE AT A TIME IN setCameraSettings ****
***** DUE TO CAMERA DRIVERS BEING RETARDEDLY BUGGY       *****
 
__u32 controlIds[NUM_CONTROLS] =
{V4L2_CID_VFLIP, V4L2_CID_HFLIP,
 V4L2_CID_DO_WHITE_BALANCE, V4L2_CID_GAIN, V4L2_CID_EXPOSURE_AUTO,
 V4L2_CID_EXPOSURE, V4L2_CID_BACKLIGHT_COMPENSATION, V4L2_CID_AUTO_WHITE_BALANCE,
 V4L2_CID_BLUE_BALANCE, V4L2_CID_RED_BALANCE, V4L2_CID_BRIGHTNESS,
 V4L2_CID_CONTRAST, V4L2_CID_SATURATION, V4L2_CID_HUE, V4L2_CID_HCENTER,
 V4L2_CID_VCENTER, V4L2_CID_SHARPNESS};
*/


__u32 controlIds[NUM_CONTROLS] =
/*
{V4L2_CID_BRIGHTNESS, V4L2_CID_CONTRAST,
 V4L2_CID_SATURATION, V4L2_CID_SHARPNESS,
 V4L2_CID_HFLIP, V4L2_CID_VFLIP,
 V4L2_CID_BACKLIGHT_COMPENSATION, V4L2_CID_AUTO_WHITE_BALANCE,
 V4L2_CID_DO_WHITE_BALANCE, V4L2_CID_EXPOSURE_AUTO,
 V4L2_CID_HUE, V4L2_CID_AUTO_EXPOSURE_CORRECTION,
 V4L2_CID_EXPOSURE_CORRECTION, V4L2_CID_HUE_AUTO,
 V4L2_CID_AUTOGAIN, V4L2_CID_AUTOGAIN,
 V4L2_CID_EXPOSURE};
 */

{V4L2_CID_HFLIP, V4L2_CID_VFLIP,
 V4L2_CID_EXPOSURE_AUTO,
 V4L2_CID_BRIGHTNESS, V4L2_CID_CONTRAST,
 V4L2_CID_SATURATION, V4L2_CID_HUE,
 V4L2_CID_SHARPNESS,
 V4L2_CID_AUTO_WHITE_BALANCE, V4L2_CID_BACKLIGHT_COMPENSATION, 
 V4L2_CID_EXPOSURE_AUTO,
 V4L2_CID_EXPOSURE, V4L2_CID_GAIN,
 V4L2_CID_DO_WHITE_BALANCE};

__s32 controlValues_lights[NUM_CAMERAS][NUM_CONTROLS] =
{ {1, 1,
   1,
   220, 64,
   180, 0,
   2,
   0, 0x00,
   0,
   60, 100
   -100},
  {0, 0,
   1,
   220, 64,
   180, 0,
   2,
   0, 0x00,
   0,
   60, 120
   -100}
};


/*
{  { 220, 64,
     180, 2,
     1, 1,
     0, 0,
     -100, 0,
     0, 0,
     0, 0,
     0, 0,
     80},
   { 220, 64,
     220, 2,
     0, 0,
     0, 0,
     -120, 0,
     0, 0,
     0, 0,
     0, 0,
     80}
};
*/
/*
__s32 controlValues_lights[NUM_CAMERAS][NUM_CONTROLS] =
{  { true, true,
     -40, 131, false,
     0, false, false,
     128, 75, 128,
     64, 225, -43, 0,
     0, 2},
   { false, false,
     -40, 131, false,
     100, false, false,
     128, 75, 128,
     64, 225, -43, 0,
     0, 2}
};
*/
__s32 (*controlValues)[NUM_CONTROLS] = controlValues_lights;

Camera::Camera() : dumpFile(0) {
   imageSize = IMAGE_WIDTH * IMAGE_HEIGHT * 2;
}

bool Camera::startRecording(const char *filename, uint32_t frequency_ms) {
   this->frequency_ms = frequency_ms;
   if (dumpFile != NULL) {
      fclose(dumpFile);
   }
   dumpFile = fopen(filename, "w");
   llog(INFO) << "Starting camera dump to file: " << filename << endl;
   return dumpFile != NULL;
}

void Camera::stopRecording() {
   if (dumpFile != NULL) {
      fclose(dumpFile);
      dumpFile = NULL;
   }
   llog(INFO) << "Finishing camera dump to file" << endl;
}

void Camera::writeFrame(const uint8_t*& image) {
   static Timer t;
   if (dumpFile != NULL && image != NULL) {
      if (t.elapsed_ms() >= frequency_ms) {
         t.restart();
         llog(DEBUG3) << "Writing frame to dumpFile" << endl;
         int written = fwrite(image, imageSize, 1, dumpFile);
         llog(DEBUG3) << "wrote " << written << " frames" << endl;
         fflush(dumpFile);
      }
   }
}
