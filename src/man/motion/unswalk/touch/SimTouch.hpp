#pragma once

#include "touch/Touch.hpp"

class SimTouch : Touch {
   public:
      explicit SimTouch();
      ~SimTouch();

      UNSWSensorValues getSensors(UNSWKinematics &kinematics);
      bool getStanding();
      ButtonPresses getButtons();

   private:
      bool isFirstTime;
};
