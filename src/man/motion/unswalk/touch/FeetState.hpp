#pragma once
#include "types/SensorValues.hpp"
#include "perception/kinematics/UNSWKinematics.hpp"

class FeetState {
   public:
      FeetState();
      void update(const SensorValues &sensors, UNSWKinematics &kinematics);
      bool groundContact[2];  //for each foot
      float CoP[2][2];        //left and right foot for x and y axis
      float footPos[2][3];
      float ZMP[2];
};
