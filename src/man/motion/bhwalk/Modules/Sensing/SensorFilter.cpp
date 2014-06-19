/**
* @file SensorFilter.cpp
* Implementation of module SensorFilter.
* @author Colin Graf
*/

#include "SensorFilter.h"
#include "Tools/Debugging/DebugDrawings.h"

MAKE_MODULE(SensorFilter, Sensing)

PROCESS_WIDE_STORAGE(SensorFilter) SensorFilter::theInstance = 0;

void SensorFilter::update(FilteredSensorDataBH& filteredSensorData)
{
  // copy sensor data from representation SensorDataBH to representation FilteredSensorDataBH,
  // but keep values from gyro and acc
  Vector2BH<> gyro(filteredSensorData.data[SensorDataBH::gyroX], filteredSensorData.data[SensorDataBH::gyroY]);
  Vector3BH<> acc(filteredSensorData.data[SensorDataBH::accX], filteredSensorData.data[SensorDataBH::accY], filteredSensorData.data[SensorDataBH::accZ]);
  (SensorDataBH&)filteredSensorData = theSensorDataBH;
  filteredSensorData.data[SensorDataBH::gyroX] = gyro.x;
  filteredSensorData.data[SensorDataBH::gyroY] = gyro.y;
  filteredSensorData.data[SensorDataBH::accX] = acc.x;
  filteredSensorData.data[SensorDataBH::accY] = acc.y;
  filteredSensorData.data[SensorDataBH::accZ] = acc.z;

  if(theDamageConfigurationBH.usLDefect)
  {
    for(int i = SensorDataBH::usL; i < SensorDataBH::usLEnd; ++i)
      filteredSensorData.data[i] = 2550.0f;
  }

  if(theDamageConfigurationBH.usRDefect)
  {
    for(int i = SensorDataBH::usR; i < SensorDataBH::usREnd; ++i)
      filteredSensorData.data[i] = 2550.0f;
  }

  // take calibrated inertia sensor data
  for(int i = 0; i < 2; ++i)
  {
    if(theInertiaSensorDataBH.gyro[i] != InertiaSensorDataBH::off)
      filteredSensorData.data[SensorDataBH::gyroX + i] = theInertiaSensorDataBH.gyro[i];
    else if(filteredSensorData.data[SensorDataBH::gyroX + i] == SensorDataBH::off)
      filteredSensorData.data[SensorDataBH::gyroX + i] = 0.f;
  }
  filteredSensorData.data[SensorDataBH::gyroZ] = 0.f;
  for(int i = 0; i < 3; ++i)
  {
    if(theInertiaSensorDataBH.acc[i] != InertiaSensorDataBH::off)
      filteredSensorData.data[SensorDataBH::accX + i] = theInertiaSensorDataBH.acc[i];
    else if(filteredSensorData.data[SensorDataBH::accX + i] == SensorDataBH::off)
      filteredSensorData.data[SensorDataBH::accX + i] = 0.f;
  }

  // take orientation data
  filteredSensorData.data[SensorDataBH::angleX] = std::atan2(theOrientationDataBH.rotation.c1.z, theOrientationDataBH.rotation.c2.z);
  filteredSensorData.data[SensorDataBH::angleY] = std::atan2(-theOrientationDataBH.rotation.c0.z, theOrientationDataBH.rotation.c2.z);

  // some code for calibrating the gain of the gyro sensors:
#ifndef RELEASE
  if(filteredSensorData.data[SensorDataBH::gyroX] != SensorDataBH::off)
  {
    gyroAngleXSum += filteredSensorData.data[SensorDataBH::gyroX] * (theSensorDataBH.timeStamp - lastIteration) * 0.001f;
    gyroAngleXSum = normalizeBH(gyroAngleXSum);
    lastIteration = theSensorDataBH.timeStamp;
  }
  PLOT("module:SensorFilter:gyroAngleXSum", gyroAngleXSum);
  DEBUG_RESPONSE_ONCE("module:SensorFilter:gyroAngleXSum:reset", gyroAngleXSum = 0.f;);
#endif

  PLOT("module:SensorFilter:rawAngleX", theSensorDataBH.data[SensorDataBH::angleX]);
  PLOT("module:SensorFilter:rawAngleY", theSensorDataBH.data[SensorDataBH::angleY]);

  PLOT("module:SensorFilter:rawAccX", theSensorDataBH.data[SensorDataBH::accX]);
  PLOT("module:SensorFilter:rawAccY", theSensorDataBH.data[SensorDataBH::accY]);
  PLOT("module:SensorFilter:rawAccZ", theSensorDataBH.data[SensorDataBH::accZ]);

  PLOT("module:SensorFilter:rawGyroX", theSensorDataBH.data[SensorDataBH::gyroX]);
  PLOT("module:SensorFilter:rawGyroY", theSensorDataBH.data[SensorDataBH::gyroY]);
  PLOT("module:SensorFilter:rawGyroZ", theSensorDataBH.data[SensorDataBH::gyroZ]);

  PLOT("module:SensorFilter:angleX", filteredSensorData.data[SensorDataBH::angleX]);
  PLOT("module:SensorFilter:angleY", filteredSensorData.data[SensorDataBH::angleY]);

  PLOT("module:SensorFilter:accX", filteredSensorData.data[SensorDataBH::accX]);
  PLOT("module:SensorFilter:accY", filteredSensorData.data[SensorDataBH::accY]);
  PLOT("module:SensorFilter:accZ", filteredSensorData.data[SensorDataBH::accZ]);

  PLOT("module:SensorFilter:gyroX", filteredSensorData.data[SensorDataBH::gyroX] != float(SensorDataBH::off) ? filteredSensorData.data[SensorDataBH::gyroX] : 0);
  PLOT("module:SensorFilter:gyroY", filteredSensorData.data[SensorDataBH::gyroY] != float(SensorDataBH::off) ? filteredSensorData.data[SensorDataBH::gyroY] : 0);
  PLOT("module:SensorFilter:gyroZ", filteredSensorData.data[SensorDataBH::gyroZ] != float(SensorDataBH::off) ? filteredSensorData.data[SensorDataBH::gyroZ] : 0);
}

