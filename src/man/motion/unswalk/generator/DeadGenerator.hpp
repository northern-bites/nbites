#pragma once

#include "generator/Generator.hpp"

#define MIN_TICKS 100

class DeadGenerator : Generator {
   public:
      explicit DeadGenerator();
      ~DeadGenerator();
      virtual JointValues makeJoints(ActionCommand::All* request,
                                     Odometry* odometry,
                                     const UNSWSensorValues &sensors,
                                     BodyModel &bodyModel,
                                     float ballX,
                                     float ballY);
      virtual bool isActive();
      void reset();

   private:
      int ticks;
};
