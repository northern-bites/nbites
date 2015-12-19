#pragma once

#include "Effector.hpp"

class NullEffector : Effector {
   public:
      void actuate(JointValues joints, ActionCommand::LED leds, float sonar) {}
};

