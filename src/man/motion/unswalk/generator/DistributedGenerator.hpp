#pragma once

#include <map>
#include <string>
#include "generator/Generator.hpp"

class DistributedGenerator : Generator {
   public:
      explicit DistributedGenerator();
      ~DistributedGenerator();
      virtual JointValues makeJoints(ActionCommand::All* request,
                                     Odometry* odometry,
                                     const UNSWSensorValues &sensors,
                                     BodyModel &bodyModel,
                                     float ballX,
                                     float ballY);
      virtual bool isActive();
      void reset();
      void readOptions(std::string path);
      bool isStopping;
      static const unsigned int NUM_NBITE_GENS = 3;
      bool isStanding();

   private:
      Generator* bodyGenerators[ActionCommand::Body::NUM_ACTION_TYPES];
      Generator* headGenerator;
      ActionCommand::Body::ActionType current_generator;
      ActionCommand::Body::ActionType prev_generator;
      ActionCommand::Body::ActionType requestedDive;
};
