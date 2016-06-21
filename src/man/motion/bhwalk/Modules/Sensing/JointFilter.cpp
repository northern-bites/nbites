/**
* @file JointFilter.cpp
* Implementation of module JointFilter.
* @author Colin Graf
*/

#include "JointFilter.h"

MAKE_MODULE(JointFilter, Sensing)

PROCESS_WIDE_STORAGE(JointFilter) JointFilter::theInstance = 0;

void JointFilter::update(FilteredJointDataBH& filteredJointData)
{
  for(int i = 0; i < JointDataBH::numOfJoints; ++i)
    if(theJointDataBH.angles[i] != JointDataBH::off)
      filteredJointData.angles[i] = theJointDataBH.angles[i];
    else if(filteredJointData.angles[i] == JointDataBH::off)
      filteredJointData.angles[i] = 0;
  filteredJointData.timeStamp = theJointDataBH.timeStamp;
}
