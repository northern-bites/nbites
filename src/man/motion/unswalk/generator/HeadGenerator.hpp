#pragma once

#include "motion/generator/Generator.hpp"

/**
 * HeadGenerator - a Generator implementation that only moves the Head.
 * Optionally uses a PID conroller to smooth the head movements.
 */
class HeadGenerator : Generator {
   public:
      explicit HeadGenerator();
      ~HeadGenerator();
      virtual JointValues makeJoints(ActionCommand::All* request,
                                     Odometry* odometry,
                                     const SensorValues &sensors,
                                     BodyModel &bodyModel,
                                     float ballX,
                                     float ballY);
      virtual bool isActive();
      void reset();

   private:
      float yaw;
      float pitch;
};
