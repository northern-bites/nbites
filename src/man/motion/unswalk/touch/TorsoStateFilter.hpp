/**
 * for body lean and angular velocity,
 * using kalman filter
 */
#pragma once
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <iostream>
#include "types/SensorValues.hpp"
#include "perception/kinematics/Kinematics.hpp"
#include "motion/touch/FeetState.hpp"

using namespace boost::numeric::ublas;
using namespace std;

class TorsoStateFilter {
   public:
      explicit TorsoStateFilter();
      matrix<float> update(matrix<float> obs, FeetState feetState);
      void init(float dt, float obsAngleSD, float obsVelSD, bool frontal);
      ~TorsoStateFilter();

   private:
      matrix<float> est;
      matrix<float> estBar;
      matrix<float> covEst;
      matrix<float> covEstBar;
      matrix<float> covR;
      matrix<float> covQ;

      float dt;
      bool frontal;

      float getFulcrum(matrix<float> obs, FeetState feetState);
};
