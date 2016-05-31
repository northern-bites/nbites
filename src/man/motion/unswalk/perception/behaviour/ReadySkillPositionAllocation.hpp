/*
 * ReadySkillPositionAllocation.hpp
 *
 *  Created on: 11/07/2014
 *      Author: osushkov
 */

#pragma once

#include <vector>

class ReadySkillPositionAllocation {
public:
   ReadySkillPositionAllocation();
   ReadySkillPositionAllocation(int fromPlayerNum, std::vector<int> positionAllocations);
   
   bool isValid(void) const;
   bool canOverride(const ReadySkillPositionAllocation &other) const;
   
   int fromPlayerNum;
   // TODO: this is horrible, fix this by serializing BroadcastData properly.
   int readyPositionAllocation0;
   int readyPositionAllocation1;
   int readyPositionAllocation2;
   int readyPositionAllocation3;
   int readyPositionAllocation4;
   
   template<class Archive>
   void serialize(Archive &ar, const unsigned int file_version) {
      ar & fromPlayerNum;
      ar & readyPositionAllocation0;
      ar & readyPositionAllocation1;
      ar & readyPositionAllocation2;
      ar & readyPositionAllocation3;
      ar & readyPositionAllocation4;
   }
};
