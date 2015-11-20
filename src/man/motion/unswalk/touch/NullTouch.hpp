#pragma once

#include "Touch.hpp"

class NullTouch : Touch {
   public:
      SensorValues getSensors(Kinematics &kinematics);
      bool getStanding();
      ButtonPresses getButtons();
   private:
      SensorValues nullSensors;
};

