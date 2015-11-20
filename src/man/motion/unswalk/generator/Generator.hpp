#pragma once

#include <boost/program_options.hpp>
#include "types/ActionCommand.hpp"
#include "types/JointValues.hpp"
#include "types/Odometry.hpp"
#include "types/SensorValues.hpp"
#include "motion/generator/BodyModel.hpp"

/**
 * Generator - responsible for tranforming ActionCommands into joint angles.
 * May consider feedback from sensors.
 */
class Generator {
   public:
      virtual ~Generator() {}
      /**
       * makeJoints - generate joint angles & stiffnesses for next cycle
       * @param request New command for body movement. Generator should replace
       *           with the command is is currently doing if it is not the same
       * @param odometry Running tally of walk distance in f, l, t
       * @param sensors Last-read values of sensors
       * @param model of the robot
       * @param ballX relative x position of the ball
       * @param ballY relative y position of the ball
       * @return Values of joint actuators in next cycle
       */
      virtual JointValues makeJoints(ActionCommand::All* request,
                                     Odometry* odometry,
                                     const SensorValues &sensors,
                                     BodyModel &bodyModel,
                                     float ballX,
                                     float ballY) = 0;
      /**
       * isActive - informs the parent of whether the robot is in a neutral stance
       *            (i.e., whether a different generator can be switched to)
       */
      virtual bool isActive() = 0;
      /**
       * reset - return to neutral stance and become inactive immediately
       */
      virtual void reset() = 0;

      /**
       * stop - like reset, but time is given to transition to the neutral stance
       */
      virtual void stop() { /* defaults to reset() - can be overridden */
      }

      virtual void readOptions(const boost::program_options::variables_map &config) {}
};
