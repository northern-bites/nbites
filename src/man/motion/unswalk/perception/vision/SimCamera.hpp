#pragma once

#include "perception/vision/UNSWCamera.hpp"

class SimCamera : public UNSWCamera {
   public:
      explicit SimCamera();
      ~SimCamera();

      const uint8_t *get(const int colourSpace);
      bool setCamera(WhichCamera whichCamera);
      WhichCamera getCamera();
      bool setControl(const uint32_t id, const int32_t value);
};
