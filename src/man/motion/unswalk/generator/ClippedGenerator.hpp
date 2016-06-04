#pragma once

#include "generator/Generator.hpp"
#include "generator/DistributedGenerator.hpp"

class ClippedGenerator : Generator {
   public:
      explicit ClippedGenerator(Generator* g);
      ~ClippedGenerator();
      virtual JointValues makeJoints(ActionCommand::All* request,
                                     Odometry* odometry,
                                     const UNSWSensorValues &sensors,
                                     BodyModel &bodyModel,
                                     float ballX,
                                     float ballY);
      virtual bool isActive();
      void reset();
      void readOptions(const boost::program_options::variables_map &config);
      bool isStanding() { return ((DistributedGenerator *)generator)->isStanding(); }

   private:
      Generator* generator;
      JointValues old_j;
      bool old_exists;
};
