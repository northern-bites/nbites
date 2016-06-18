#pragma once

#include <string>
#include <fstream>
#include "types/ActionCommand.hpp"
#include "Planner.hpp"
#include "BodyModel.hpp"

class RLPlanner : Planner {
   public:
      RLPlanner(std::string plannerFile);
      Action getAction(BodyModel b, ActionCommand::All a);

   private:
      bool loadFile(std::string plannerFile);
      std::string getLineNoComments(std::ifstream &f);
      int table[3][21][21][25];
};

