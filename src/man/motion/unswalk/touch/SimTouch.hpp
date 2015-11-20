#pragma once

#include "motion/touch/Touch.hpp"

class SimTouch : Touch {
   public:
      explicit SimTouch();
      ~SimTouch();

      SensorValues getSensors(Kinematics &kinematics);
      bool getStanding();
      ButtonPresses getButtons();

   private:
      bool isFirstTime;
};
