#pragma once

#include "generator/Generator.hpp"

class StandGenerator : Generator {
   public:
      explicit StandGenerator();
      ~StandGenerator();
      virtual JointValues makeJoints(ActionCommand::All* request,
                                     Odometry* odometry,
                                     const UNSWSensorValues &sensors,
                                     BodyModel &bodyModel,
                                     float ballX,
                                     float ballY);
      virtual bool isActive();
      void reset();
      void stop();
      void readOptions(const boost::program_options::variables_map &config);

   private:
      float phi;
      Generator *posGen;
};
