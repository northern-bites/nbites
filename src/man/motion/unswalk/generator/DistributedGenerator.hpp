#pragma once

#include <map>
#include <string>
#include <vector>
#include "generator/Generator.hpp"

#define MAX_STIFF 1.0

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

      // An attempt to call a stand position without swapping to the stand generator
      int current_time;
      int max_iter;
      std::string file_name;
      std::vector<JointValues> joints;
      ActionCommand::Body active;
      void interpolate(JointValues newJoint, int duration = 0);
      void constructPose(std::string path);
};
