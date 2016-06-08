/**
* @file RobotModelBH.cpp
* Implementation of class RobotModelBH.
* @author Alexander Härtl
*/

#include "RobotModel.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/Debugging/DebugDrawings3D.h"
#include "Tools/ForwardKinematic.h"

RobotModelBH::RobotModelBH(const JointDataBH& joints, const RobotDimensionsBH& robotDimensions, const MassCalibrationBH& massCalibration)
{
  setJointData(joints, robotDimensions, massCalibration);
}

void RobotModelBH::setJointData(const JointDataBH& joints, const RobotDimensionsBH& robotDimensions, const MassCalibrationBH& massCalibration)
{
  ForwardKinematic::calculateHeadChain(joints, robotDimensions, massCalibration, limbs);

  for(int side = 0; side < 2; side++)
  {
    const bool left = side == 0;
    ForwardKinematic::calculateArmChain(left, joints, robotDimensions, massCalibration, limbs);
    ForwardKinematic::calculateLegChain(left, joints, robotDimensions, massCalibration, limbs);
  }

  // calculate center of mass
  centerOfMass = Vector3BH<>();
  totalMass = 0.0;
  for(int i = 0; i < MassCalibrationBH::numOfLimbs; i++)
  {
    const MassCalibrationBH::MassInfo& limb(massCalibration.masses[i]);
    totalMass += limb.mass;
    centerOfMass += (limbs[i] * limb.offset) * limb.mass;
  }
  centerOfMass /= totalMass;
}

void RobotModelBH::draw() const
{
  DECLARE_DEBUG_DRAWING3D("representation:RobotModelBH", "origin");
  COMPLEX_DRAWING3D("representation:RobotModelBH",
  {
    for(int i = 0; i < MassCalibrationBH::numOfLimbs; ++i)
    {
      const Pose3DBH& p = limbs[i];
      const Vector3BH<>& v = p.translation;
      const Vector3BH<> v1 = p * Vector3BH<>(50, 0, 0);
      const Vector3BH<> v2 = p * Vector3BH<>(0, 50, 0);
      const Vector3BH<> v3 = p * Vector3BH<>(0, 0, 50);
      LINE3D("representation:RobotModelBH", v.x, v.y, v.z, v1.x, v1.y, v1.z, 1, ColorRGBA(255, 0, 0));
      LINE3D("representation:RobotModelBH", v.x, v.y, v.z, v2.x, v2.y, v2.z, 1, ColorRGBA(0, 255, 0));
      LINE3D("representation:RobotModelBH", v.x, v.y, v.z, v3.x, v3.y, v3.z, 1, ColorRGBA(0, 0, 255));
    }
  });
  DECLARE_DEBUG_DRAWING3D("representation:RobotModelBH:centerOfMass", "origin");
  SPHERE3D("representation:RobotModelBH:centerOfMass", centerOfMass.x, centerOfMass.y, centerOfMass.z, 25, ColorRGBA(255, 0, 0));
}
