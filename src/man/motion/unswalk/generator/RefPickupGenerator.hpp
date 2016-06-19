#pragma once

#include "generator/Generator.hpp"

class RefPickupGenerator : Generator {
   public:
      explicit RefPickupGenerator();
      ~RefPickupGenerator();
      virtual JointValues makeJoints(ActionCommand::All* request,
                                     Odometry* odometry,
                                     const UNSWSensorValues &sensors,
                                     BodyModel &bodyModel,
                                     float ballX,
                                     float ballY);
      virtual bool isActive();
      void reset();
      void stop();
      void readOptions(std::string path);

   private:
      float phi;
      int t;
      bool stopping;
      bool stopped;
      Generator *standGen;
};
