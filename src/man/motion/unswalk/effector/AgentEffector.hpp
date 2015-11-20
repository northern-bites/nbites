#pragma once

#include "motion/effector/Effector.hpp"
#include "libagent/AgentData.hpp"

class AgentEffector : Effector {
   public:
      explicit AgentEffector();
      ~AgentEffector();
      void actuate(JointValues joints, ActionCommand::LED leds, float sonar);

   private:
      int shared_fd;
      AgentData* shared_data;
};
