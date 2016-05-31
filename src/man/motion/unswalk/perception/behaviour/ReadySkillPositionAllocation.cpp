/*
 * ReadySkillPositionAllocation.cpp
 *
 *  Created on: 11/07/2014
 *      Author: osushkov
 */

#include "ReadySkillPositionAllocation.hpp"

#include <iostream>

ReadySkillPositionAllocation::ReadySkillPositionAllocation() :fromPlayerNum(0) {
   readyPositionAllocation0 = -1;
   readyPositionAllocation1 = -1;
   readyPositionAllocation2 = -1;
   readyPositionAllocation3 = -1;
   readyPositionAllocation4 = -1;
}

ReadySkillPositionAllocation::ReadySkillPositionAllocation(int fromPlayerNum, std::vector<int> positionAllocations) :
      fromPlayerNum(fromPlayerNum) {
   readyPositionAllocation0 = positionAllocations[0];
   readyPositionAllocation1 = positionAllocations[1];
   readyPositionAllocation2 = positionAllocations[2];
   readyPositionAllocation3 = positionAllocations[3];
   readyPositionAllocation4 = positionAllocations[4];
}

bool ReadySkillPositionAllocation::isValid(void) const {
   return fromPlayerNum > 0; // positions are only valid when originating from a player num 1 or above.
}

bool ReadySkillPositionAllocation::canOverride(const ReadySkillPositionAllocation &other) const {
   // Lower numbered players win out, unless the player number is not valid.
   return isValid() && (!other.isValid() || fromPlayerNum < other.fromPlayerNum);
}
