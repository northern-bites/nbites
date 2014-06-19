/**
* @file JointFilter.h
* Declaration of module JointFilter.
* @author Colin Graf
*/

#pragma once

#include "Tools/Module/Module.h"
#include "Representations/Infrastructure/JointData.h"

MODULE(JointFilter)
  REQUIRES(JointDataBH)
  PROVIDES_WITH_MODIFY_AND_OUTPUT(FilteredJointDataBH)
END_MODULE

/**
* @class JointFilter
* A module for sensor data filtering.
*/
class JointFilter : public JointFilterBase
{
  public:
  static PROCESS_WIDE_STORAGE(JointFilter) theInstance;
  /**
  * Updates the FilteredJointDataBH representation .
  * @param filteredJointData The joint data representation which is updated by this module.
  */
  void update(FilteredJointDataBH& filteredJointData);
};
