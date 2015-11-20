#pragma once

#include "utils/body.hpp"
#include "types/JointValues.hpp"

/**
 * A container for joint values, IMU values, FSR values, buttons and sonar
 * readings obtained from the Motion subsystem.
 */
struct SensorValues {
   SensorValues() {
      for (int i = 0; i < Sensors::NUMBER_OF_SENSORS; ++i) {
         sensors[i] = NAN;
      }
      for (int i = 0; i < Sonar::NUMBER_OF_READINGS; ++i) {
         sonar[i] = NAN;
      }
   }

   SensorValues(bool zero) {
      joints = JointValues(true);
      for (int i = 0; i < Sensors::NUMBER_OF_SENSORS; ++i) {
         sensors[i] = 0;
      }
      for (int i = 0; i < Sonar::NUMBER_OF_READINGS; ++i) {
         sonar[i] = 0;
      }
   }

   JointValues joints;
   float sensors[Sensors::NUMBER_OF_SENSORS];
   float sonar[Sonar::NUMBER_OF_READINGS];

   template<class Archive>
   void serialize(Archive &ar, const unsigned int file_version) {
      ar & joints;
      ar & sensors;
      ar & sonar;
   }
};
