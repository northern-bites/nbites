#pragma once

#include "effector/Effector.hpp"

class SimEffector : Effector {
   public:
      explicit SimEffector();
      ~SimEffector();

      void actuate(JointValues joints, ActionCommand::LED leds, float sonar);
};
