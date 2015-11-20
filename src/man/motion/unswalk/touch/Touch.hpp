#pragma once

#include "types/ButtonPresses.hpp"
#include "types/SensorValues.hpp"
#include "perception/kinematics/Kinematics.hpp"
#include <boost/program_options/variables_map.hpp>

/**
 * Touch - repsonsible for reporting on the state of all non-vision senses
 */
class Touch {
   public:
      virtual ~Touch() {}
      /**
       * getSensors - read robot sensors
       * @return current values of sensors
       */
      virtual void readOptions(const boost::program_options::variables_map& config){}
      virtual SensorValues getSensors(Kinematics &kinematics) = 0;

      virtual bool getStanding() = 0;
      virtual ButtonPresses getButtons() = 0;
};
