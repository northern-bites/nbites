/**
* @file SensorFilter.h
* Declaration of module SensorFilter.
* @author Colin Graf
*/

#pragma once

#include "Tools/Module/Module.h"
#include "Representations/Infrastructure/SensorData.h"
#include "Representations/Sensing/InertiaSensorData.h"
#include "Representations/Sensing/OrientationData.h"
#include "Representations/Configuration/DamageConfiguration.h"

MODULE(SensorFilter)
  REQUIRES(SensorDataBH)
  REQUIRES(InertiaSensorDataBH)
  REQUIRES(OrientationDataBH)
  REQUIRES(DamageConfigurationBH)
  PROVIDES_WITH_MODIFY_AND_OUTPUT(FilteredSensorDataBH)
END_MODULE

/**
* A module for sensor data filtering.
*/
class SensorFilter : public SensorFilterBase
{
public:
  static PROCESS_WIDE_STORAGE(SensorFilter) theInstance;
  /**
  * Updates the FilteredSensorDataBH representation.
  * @param filteredSensorData The sensor data representation which is updated by this module.
  */
  void update(FilteredSensorDataBH& filteredSensorData);

private:
  float gyroAngleXSum;
  unsigned lastIteration;
};
