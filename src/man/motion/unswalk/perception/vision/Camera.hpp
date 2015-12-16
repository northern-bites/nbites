#pragma once

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <alvision/alvisiondefinitions.h>
#include <linux/videodev2.h>
#include "WhichCamera.hpp"

#define IMAGE_WIDTH 640
#define IMAGE_HEIGHT 480

// these are undocumented controls available in the Nao V3+
#define V4L2_CID_AUTO_EXPOSURE_CORRECTION           (V4L2_CID_BASE+32)
#define V4L2_CID_SET_DEFAULT_PARAMETERS             (V4L2_CID_BASE+33)
#define V4L2_CID_EXPOSURE_CORRECTION                (V4L2_CID_BASE+34)
#define V4L2_CID_AUTO_EXPOSURE_CORRECTION_ALGORITHM (V4L2_CID_BASE+35)


// there are 17 controls on the Nao V3+ that we care about setting
#define NUM_CONTROLS 14

/**
 * The IDs of the controls we set on the Nao
 */
extern __u32 controlIds[NUM_CONTROLS];

/**
 * The initial values for each camera for the controls we set on the Nao
 */
extern __s32 (*controlValues)[NUM_CONTROLS];

class Camera {
   public:
      Camera();
      virtual ~Camera() {}

      /**
       * Get the image and convert it to the requested colourSpace (currently only
       * kYUVColorSpace (YUV422) is supported)
       *
       * @return a pointer to the image if the image was fetched successfully.
       * NULL, otherwise.
       */
      virtual const uint8_t *get(const int colourSpace = AL::kYUVColorSpace) =
         0;

      /**
       * If applicable, changes the current camera being used.  If necesary,
       * disables the video stream momentarily so the stream can switch inputs.
       *
       * @param whichCamera specifies the top or bottom camera to switch to
       * @return whether the set succeeds
       * @sideeffect stops capturing, switches the camera, then starts again
       */
      virtual bool setCamera(WhichCamera whichCamera) = 0;

      /**
       * Gets the current camera
       *
       * @return either the top or bottom camera
       */
      virtual WhichCamera getCamera() = 0;

      /**
       * Starts recording to a file.  If there is a recording in progress, will
       * stop recording, first.
       *
       * @param filename the name of the file to save to
       * @param frequency_ms record frames at most once every frequency_ms ms
       * @returns 0 if there was no error, or an error code otherwise
       */
      bool startRecording(const char *filename, uint32_t frequency_ms);

      /**
       * Stops recording.  No effect if not recording.
       */
      void stopRecording();

      /**
       * Sets a camera control (eg. hue)
       *
       * @param id the control id to set.
       * @param value the value to set the control to
       * @returns true if success, false otherwise
       */
      virtual bool setControl(const uint32_t id, const int32_t value) = 0;

   protected:
      /* File handle pointing to where we are recording */
      FILE *dumpFile;

      unsigned int imageSize;

      /* How frequently to record frames */
      uint32_t frequency_ms;

      /* Writes a frame to disk if we are currently recording */
      void writeFrame(const uint8_t*& image);
};
