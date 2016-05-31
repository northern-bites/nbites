#pragma once

#include "generator/Generator.hpp"

class NullGenerator : Generator {
   public:
      explicit NullGenerator();
      ~NullGenerator();
      virtual JointValues makeJoints(ActionCommand::All* request,
                                     Odometry* odometry,
                                     const UNSWSensorValues &sensors,
                                     BodyModel &bodyModel,
                                     float ballX,
                                     float ballY);
      virtual bool isActive();
      void reset();
};
