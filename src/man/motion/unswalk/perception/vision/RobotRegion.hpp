#pragma once

#include <stdint.h>
#include <utility>

#include "perception/vision/ImageRegion.hpp"

/* types/UNSWRobotInfo.hpp is included to replace the below enum.
 * If you require the enum, please include UNSWRobotInfo.hpp into
 * the required file. This is a compatability hack
 *
typedef enum {
   BLUE_ROBOT = 0,
   RED_ROBOT = 1,
   UNKNOWN_ROBOT = 2
}
__attribute__((packed)) RobotType;
*/

#include "types/UNSWRobotInfo.hpp"

class RobotRegion {
   public:
      uint16_t leftMost;
      uint16_t rightMost;
      uint16_t bottomMost;
      uint16_t topMost;
      uint16_t numTop;

      ImageRegion *regions[MAX_NUM_ROBOT_REGIONS];
      uint16_t numRegions;

      uint16_t numRobotRed;
      uint16_t numRobotBlue;
      uint16_t numWhite;

      uint16_t leftMostRobot;
      uint16_t rightMostRobot;
      uint16_t bottomMostRobot;
      uint16_t topMostRobot;

      bool deleted;

      UNSWRobotInfo::Type type;
};
