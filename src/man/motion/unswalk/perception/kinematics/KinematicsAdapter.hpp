#pragma once

#include <string>
#include "Kinematics.hpp"
#include "SonarFilter.hpp"
#include "utils/Timer.hpp"
#include "blackboard/Adapter.hpp"

/* Adapter that allows Localisation to communicate with the Blackboard */
class KinematicsAdapter : Adapter {
   public:
      /* Constructor */
      KinematicsAdapter(Blackboard *bb);
      /* Destructor */
      ~KinematicsAdapter();
      /* One cycle of this thread */
      void tick();
   private:
      /* Localisation module instance */
      Kinematics kinematics;
      SonarFilter sonarFilter;
      Timer t;
};
