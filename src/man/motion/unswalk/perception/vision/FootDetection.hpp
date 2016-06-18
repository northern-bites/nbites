#pragma once

//#include <utility>
#include <vector>
#include "perception/vision/VisionConstants.hpp"
#include "perception/vision/VisionDefs.hpp"
#include "perception/vision/CameraToRR.hpp"
#include "types/FootInfo.hpp"

class FootDetection {
   public:
      float sonarL;
      float sonarR;
      float headPitch;

      std::vector<FootInfo> feet;
      void findFeet(Vision *vision, CameraToRR *convRR);
   private:
      int sonarCounter;
      int sonarLostCounter;
      bool detectEdge(uint16_t row, uint16_t col, Vision *vision);
      bool areCentresClose(int x1, int y1, int x2, int y2);
};
