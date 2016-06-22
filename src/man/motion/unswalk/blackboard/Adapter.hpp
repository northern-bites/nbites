#pragma once

#include "utils/Logger.hpp"

class Blackboard;

class Adapter {
   protected:
      Adapter(Blackboard *blackboard) {
         this->blackboard = blackboard;
         llog(INFO) << "Adapter loaded" << std::endl; 
      }
      Blackboard *blackboard;
};

