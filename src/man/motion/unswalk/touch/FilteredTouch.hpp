#pragma once

#include "motion/touch/Touch.hpp"
#include "motion/touch/TorsoStateFilter.hpp"

using namespace boost::numeric::ublas;

class FilteredTouch : Touch {
   public:
      explicit FilteredTouch(Touch* t);
      ~FilteredTouch();
      SensorValues getSensors(Kinematics &kinematics);
      void readOptions(const boost::program_options::variables_map& config);
      bool getStanding();
      ButtonPresses getButtons();
   private:
      Touch* touch;
      bool init;
      SensorValues state;     //filtered
      SensorValues update;    //raw
      FeetState feetState;

      //offset calibration variables
      float imuOffset[3][3];
      float targetOffset[3][3];
      float avg[3][3];
      float count[3][3];
      float k[3][3];
      float err[3][3];
      float prevAng[2];

      //kinematics body state
      Kinematics kinematics;
      matrix<float> lastBodyPosition;
      matrix<float> bodyRotation;
      matrix<float> bodyOrientation;
//      float footPos[2][3];

      TorsoStateFilter kf[2];

      float getScaledGyr(int isY);
      void updateIMUOffset();
      void filterOffset(int IMUid, int dir, float obs);
      void filterSensors();
};
