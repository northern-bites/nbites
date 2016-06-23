#pragma once

#include <stdint.h>
#include <utility>
#include "perception/vision/VisionDefs.hpp"
#include "perception/vision/VisionConstants.hpp"

typedef enum {
   rBALL = 0,
   rROBOT = 1,
   rMAYBE_ROBOT = 2,
   rFIELD_LINE = 3,
   rCONNECTED_ROBOT = 4,
   rDELETED = 5
}
__attribute__((packed)) RegionType;

class ImageRegion {
   public:

      uint16_t numPixels;

      uint16_t numRobotRed;
      uint16_t numRobotBlue;
      uint16_t numWhite;
      uint16_t numOrange;

      uint16_t numTop;

      uint16_t leftMost;
      uint16_t rightMost;
      std::pair<uint16_t, uint16_t> topMost;
      std::pair<uint16_t, uint16_t> bottomMost;

      uint16_t bottomMostRobot;
      uint16_t topMostRobot;
      uint16_t leftMostRobot;
      uint16_t rightMostRobot;

      std::pair<uint16_t, uint16_t> startScans[MAX_NUM_SCAN_POINTS];
      std::pair<uint16_t, uint16_t> endScans[MAX_NUM_SCAN_POINTS];
      uint16_t numScanPoints;

      uint16_t prevStart;
      uint16_t prevEnd;

      uint16_t averageScanLength;
      uint16_t lastNotAllWhite;
      bool deleted;
      bool isRobot;
      RegionType classification;

      void clear() {
         numPixels = 0;
         numRobotRed = 0;
         numRobotBlue = 0;
         numWhite = 0;
         numOrange = 0;
         numTop = 0;
         leftMost = IMAGE_COLS;
         rightMost = 0;
         topMost.second = IMAGE_ROWS;
         bottomMost.second = 0;
         bottomMost.first = 0;
         bottomMostRobot = 0;
         topMostRobot = IMAGE_ROWS;
         leftMostRobot = IMAGE_COLS;
         rightMostRobot = 0;
         deleted = false;
         isRobot = false;
         lastNotAllWhite = 0;
         averageScanLength = 0;
         numScanPoints = 0;
      }
};
