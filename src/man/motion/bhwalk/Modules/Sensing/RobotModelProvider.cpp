/**
* @file RobotModelProvider.cpp
*
* This file implements a module that provides information about the current state of the robot's limbs.
*
* @author <a href="mailto:allli@informatik.uni-bremen.de">Alexander Härtl</a>
*/

#include "RobotModelProvider.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/Debugging/DebugDrawings3D.h"

PROCESS_WIDE_STORAGE(RobotModelProvider) RobotModelProvider::theInstance = 0;

void RobotModelProvider::update(RobotModelBH& robotModel)
{
  robotModel.setJointData(theFilteredJointDataBH, theRobotDimensionsBH, theMassCalibrationBH);

  DECLARE_DEBUG_DRAWING3D("module:RobotModelProvider:massOffsets", "origin",
  {
    for(int i = 0; i < MassCalibrationBH::numOfLimbs; ++i)
    {
      const Vector3BH<>& v(robotModel.limbs[i] * theMassCalibrationBH.masses[i].offset);
      SPHERE3D("module:RobotModelProvider:massOffsets", v.x, v.y, v.z, 3, ColorRGBA(0, 200, 0));
    }
  });

  DECLARE_DEBUG_DRAWING3D("module:RobotModelProvider:joints", "origin",
  {
    const float axisLineWidth = 1.f;

    for(int i = 0; i < 2; ++i)
    {
      int firstJoint = i == 0 ? MassCalibrationBH::pelvisLeft : MassCalibrationBH::pelvisRight;
      for(int i = 0; i < (MassCalibrationBH::footLeft + 1) - MassCalibrationBH::pelvisLeft; ++i)
      {
        Pose3DBH& next = robotModel.limbs[firstJoint + i];
        SPHERE3D("module:RobotModelProvider:joints", next.translation.x, next.translation.y, next.translation.z, 3, ColorRGBA(0, 0, 0));

        Vector3BH<> axis  = i == 0 ? Vector3BH<>(0.f, firstJoint == MassCalibrationBH::pelvisLeft ? -50.f : 50.f, 50.f).normalizeBH(50.f) : i == 1 || i == 5 ? Vector3BH<>(50.f, 0.f, 0.f) : Vector3BH<>(0.f, 50.f, 0.f);
        ColorRGBA color = i == 0 ? ColorRGBA(0, 0, 255)      : i == 1 || i == 5 ? ColorRGBA(255, 0, 0)      : ColorRGBA(0, 255, 0);
        Vector3BH<> p = next * axis;
        LINE3D("module:RobotModelProvider:joints", next.translation.x, next.translation.y, next.translation.z, p.x, p.y, p.z, axisLineWidth, color);
      }
    }
    for(int i = 0; i < 2; ++i)
    {
      int firstJoint = i == 0 ? MassCalibrationBH::shoulderLeft : MassCalibrationBH::shoulderRight;
      for(int i = 0; i < (MassCalibrationBH::foreArmLeft + 1) - MassCalibrationBH::shoulderLeft; ++i)
      {
        Pose3DBH& next = robotModel.limbs[firstJoint + i];
        SPHERE3D("module:RobotModelProvider:joints", next.translation.x, next.translation.y, next.translation.z, 3, ColorRGBA(0, 0, 0));

        Vector3BH<> axis  = i == 1 || i == 3 ? Vector3BH<>(0.f, 0.f, 50.f) : i == 2 ? Vector3BH<>(50.f, 0.f, 0.f) : Vector3BH<>(0.f, 50.f, 0.f);
        ColorRGBA color = i == 1 || i == 3 ? ColorRGBA(0, 0, 255)      : i == 2 ? ColorRGBA(255, 0, 0)      : ColorRGBA(0, 255, 0);
        Vector3BH<> p = next * axis;
        LINE3D("module:RobotModelProvider:joints", next.translation.x, next.translation.y, next.translation.z, p.x, p.y, p.z, axisLineWidth, color);
      }
    }
    for(int i = 0; i < (MassCalibrationBH::head + 1) - MassCalibrationBH::neck; ++i)
    {
      Pose3DBH& next = robotModel.limbs[MassCalibrationBH::neck + i];
      SPHERE3D("module:RobotModelProvider:joints", next.translation.x, next.translation.y, next.translation.z, 3, ColorRGBA(0, 0, 0));

      Vector3BH<> axis  = i == 0 ? Vector3BH<>(0.f, 0.f, 50.f) : Vector3BH<>(0.f, 50.f, 0.f);
      ColorRGBA color = i == 0 ? ColorRGBA(0, 0, 255)      : ColorRGBA(0, 255, 0);
      Vector3BH<> p = next * axis;
      LINE3D("module:RobotModelProvider:joints", next.translation.x, next.translation.y, next.translation.z, p.x, p.y, p.z, axisLineWidth, color);
    }
  });
}


MAKE_MODULE(RobotModelProvider, Sensing)
