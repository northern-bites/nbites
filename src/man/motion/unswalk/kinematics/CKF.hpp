

#pragma once


#include <vector>
#include <Eigen/Eigen>
#include "types/SensorValues.hpp"


/* Complementary Kalman Filter (CKF), estimates robot torso pitch and roll 
   (forward lean and right side lean) using gyro and accelerometer */

class CKF {
   public:

      CKF();

      void update(const SensorValues &sensorValues);

      void resetFilter();

      float getSideLean();

      float getForwardLean();
      
   private:

      Eigen::Vector4f state; // roll, pitch, roll_velocity, pitch_velocity
      Eigen::Matrix<float, 4, 4> var;
      Eigen::Matrix<float, 4, 4> Q; // Process noise
      Eigen::Matrix<float, 4, 4> R; // Observation noise

};
