/**
 * @file BodyDynamics.h
 * Implementation of BodyDynamics methods.
 * @author Felix Wenk
 */

#include "BodyDynamics.h"

void BodyDynamics::init(const MassCalibrationBH &massCalibration)
{
  for(int i = 0; i < MassCalibrationBH::numOfLimbs; ++i)
  {
    const MassCalibrationBH::MassInfo& m = massCalibration.masses[i];
    limbs[i].I = SpatialInertia(m.inertiaMatrix, m.offset, m.mass);
  }
  inertiasInitialized = true;
}
