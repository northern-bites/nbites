#pragma once

#include <string>

#include "perception/vision/NaoCamera.hpp"

/**
 * NaoCameraV4 provides methods for the vision module to interact with
 * the nao camera
 *
 * Changes from V3
 *    Camera manages only a single device, not two devices over the i2c bus.
 *    setCamera modifies the Vision::camera reference.
 */
class NaoCameraV4 : public NaoCamera {
  public:
   /**
    * Constructor
    * opens the device, calibrates it, and sets it up for streaming
    *
    * @param filename the device or file to get images from
    * @param method the io method to use
    * @see IOMethod
    * @param format the format of the image
    * @see AL::kVGA
    * @param initialCamera which camera to start with (top or bottom)
    * @see WhichCamera
    */
   NaoCameraV4(Blackboard *blackboard, const char *filename = "/dev/video0",
             const IOMethod method = IO_METHOD_MMAP,
             const int format = AL::k960p,
             const WhichCamera initialCamera = TOP_CAMERA);

   /**
    * Destructor
    * closes the device
    */
   virtual ~NaoCameraV4();

   /**
    * Set Vision::camera = Vision::top_camera.
    */
   bool setCamera(WhichCamera whichCamera);

   /**
    * Return which camera is currently in use.
    */
   WhichCamera getCamera();
};
