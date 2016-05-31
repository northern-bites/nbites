#pragma once

#include "Touch.hpp"

class NullTouch : Touch {
   public:
      UNSWSensorValues getSensors(UNSWKinematics &kinematics);
      bool getStanding();
      ButtonPresses getButtons();
   private:
      UNSWSensorValues nullSensors;
};

