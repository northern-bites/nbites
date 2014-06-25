/**
 * @file ForwardKinematic.h
 * @author <A href="mailto:allli@informatik.uni-bremen.de">Alexander Härtl</A>
 * @author <a href="mailto:afabisch@tzi.de">Alexander Fabisch</a>
 */

#pragma once

#include "Tools/Math/Pose3D.h"
#include "Representations/Infrastructure/JointData.h"
#include "Representations/Configuration/RobotDimensions.h"
#include "Representations/Configuration/MassCalibration.h"

class ForwardKinematic
{
public:
  static void calculateArmChain(bool left, const JointDataBH& joints, const RobotDimensionsBH& robotDimensions, const MassCalibrationBH& massCalibration, Pose3DBH limbs[MassCalibrationBH::numOfLimbs])
  {
    int sign = left ? -1 : 1;
    MassCalibrationBH::Limb shoulder = left ? MassCalibrationBH::shoulderLeft : MassCalibrationBH::shoulderRight;
    JointDataBH::Joint arm0 = left ? JointDataBH::LShoulderPitch : JointDataBH::RShoulderPitch;

    limbs[shoulder + 0] = Pose3DBH(robotDimensions.armOffset.x, robotDimensions.armOffset.y * -sign, robotDimensions.armOffset.z)
                          .rotateY(-joints.angles[arm0 + 0]);
    limbs[shoulder + 1] = Pose3DBH(limbs[shoulder + 0])
                          .rotateZ(joints.angles[arm0 + 1] * -sign);
    limbs[shoulder + 2] = Pose3DBH(limbs[shoulder + 1])
                          .translate(robotDimensions.upperArmLength, robotDimensions.yElbowShoulder * -sign, 0)
                          .rotateX(joints.angles[arm0 + 2] * -sign);
    limbs[shoulder + 3] = Pose3DBH(limbs[shoulder + 2])
                          .rotateZ(joints.angles[arm0 + 3] * -sign);
  }

  static void calculateLegChain(bool left, const JointDataBH& joints, const RobotDimensionsBH& robotDimensions, const MassCalibrationBH& massCalibration, Pose3DBH limbs[MassCalibrationBH::numOfLimbs])
  {
    int sign = left ? -1 : 1;
    MassCalibrationBH::Limb pelvis = left ? MassCalibrationBH::pelvisLeft : MassCalibrationBH::pelvisRight;
    JointDataBH::Joint leg0 = left ? JointDataBH::LHipYawPitch : JointDataBH::RHipYawPitch;

    limbs[pelvis + 0] =  Pose3DBH(0, robotDimensions.lengthBetweenLegs / 2.0f * -sign, 0)
                         .rotateX(-pi_4 * sign)
                         .rotateZ(joints.angles[leg0 + 0] * sign)
                         .rotateX(pi_4 * sign);
    limbs[pelvis + 1] = Pose3DBH(limbs[pelvis + 0])
                        .rotateX(joints.angles[leg0 + 1] * sign);
    limbs[pelvis + 2] = Pose3DBH(limbs[pelvis + 1])
                        .rotateY(joints.angles[leg0 + 2]);
    limbs[pelvis + 3] = Pose3DBH(limbs[pelvis + 2])
                        .translate(0, 0, -robotDimensions.upperLegLength)
                        .rotateY(joints.angles[leg0 + 3]);
    limbs[pelvis + 4] = Pose3DBH(limbs[pelvis + 3])
                        .translate(0, 0, -robotDimensions.lowerLegLength)
                        .rotateY(joints.angles[leg0 + 4]);
    limbs[pelvis + 5] = Pose3DBH(limbs[pelvis + 4])
                        .rotateX(joints.angles[leg0 + 5] * sign);
  }

  static void calculateHeadChain(const JointDataBH& joints, const RobotDimensionsBH& robotDimensions, const MassCalibrationBH& massCalibration, Pose3DBH limbs[MassCalibrationBH::numOfLimbs])
  {
    limbs[MassCalibrationBH::neck] = Pose3DBH(0, 0, robotDimensions.zLegJoint1ToHeadPan)
                                   .rotateZ(joints.angles[JointDataBH::HeadYaw]);
    limbs[MassCalibrationBH::head] = Pose3DBH(limbs[MassCalibrationBH::neck])
                                   .rotateY(-joints.angles[JointDataBH::HeadPitch]);
  }
};
