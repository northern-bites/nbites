#pragma once

#include "types/ActionCommand.hpp"
#include "types/JointValues.hpp"

/**
 * Effector - actuates joints and LEDs as ordered by MotionAdapter
 */
class Effector {
   public:
      virtual ~Effector() {}
      /**
       * actuate - set the values that Effector will use for the next cycle
       * @param joints Angles and Stiffnesses to set each joint to
       * @param leds LED Command to follow
       * @param sonar Sonar command
       * @see Sonar::Mode
       */
      virtual void actuate(JointValues joints, ActionCommand::LED leds,
                           float sonar) = 0;
};
