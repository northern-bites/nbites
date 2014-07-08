/**
 * @file InverseDynamics.h
 * @author Felix Wenk
 */

#pragma once

#include "Tools/Math/Pose3D.h"
#include "Tools/Math/Vector3.h"
#include "Tools/Math/SpatialVector.h"
#include "Tools/Math/SpatialTransform.h"
#include "Tools/Math/SpatialInertia.h"
#include "Representations/Sensing/JointDynamics.h"
#include "Representations/Sensing/BodyDynamics.h"
#include "Representations/Configuration/RobotDimensions.h"
#include "Representations/Configuration/MassCalibration.h"


/**
 * Methods to calculate the forces which have to act on the limbs of the robot
 * to produce the known joint accelerations (JointDynamicsBH).
 */
class InverseDynamics
{
public:
  static void calculateSpatialArmTransforms(bool left, const JointDynamicsBH& jointDynamics,
                                   const RobotDimensionsBH& robotDimensions,
                                   Body bodies[MassCalibrationBH::numOfLimbs])
  {
    int sign = left ? -1 : 1;
    MassCalibrationBH::Limb shoulder = left ? MassCalibrationBH::shoulderLeft : MassCalibrationBH::shoulderRight;
    JointDataBH::Joint arm0 = left ? JointDataBH::LShoulderPitch : JointDataBH::RShoulderPitch;

    // Calculate spatial transforms
    RotationMatrixBH rot = RotationMatrixBH::fromRotationY(-jointDynamics.joint[arm0 + 0][0]).invert(); // base in shoulder0
    Vector3BH<> pos(robotDimensions.armOffset.x, robotDimensions.armOffset.y * -sign, robotDimensions.armOffset.z); // shoulder0 in base
    bodies[shoulder + 0].X = SpatialTransform(rot, pos, true);
    bodies[shoulder + 0].mode = SpatialVector<>(Vector3BH<>(0, -1.0f, 0), Vector3BH<>());

    rot = RotationMatrixBH::fromRotationZ(jointDynamics.joint[arm0 + 1][0] * -sign).invert(); // shoulder0 in shoulder1
    bodies[shoulder + 1].X = SpatialTransform(rot, Vector3BH<>(), true);
    bodies[shoulder + 1].mode = SpatialVector<>(Vector3BH<>(0.0f, 0.0f, static_cast<float>(-sign)), Vector3BH<>());

    pos = Vector3BH<>(robotDimensions.upperArmLength, robotDimensions.yElbowShoulder * -sign, 0); // shoulder2 in shoulder1
    rot = RotationMatrixBH::fromRotationX(jointDynamics.joint[arm0 + 2][0] * -sign).invert(); // shoulder1 to shoulder2
    bodies[shoulder + 2].X = SpatialTransform(rot, pos, true);
    bodies[shoulder + 2].mode = SpatialVector<>(Vector3BH<>(static_cast<float>(-sign), 0.0f, 0.0f), Vector3BH<>());

    rot = RotationMatrixBH::fromRotationZ(jointDynamics.joint[arm0 + 3][0] * -sign).invert(); // shoulder2 to shoulder3
    bodies[shoulder + 3].X = SpatialTransform(rot, Vector3BH<>(), true);
    bodies[shoulder + 3].mode = SpatialVector<>(Vector3BH<>(0.0f, 0.0f, static_cast<float>(-sign)), Vector3BH<>());

    // Calculate inverse spatial transforms
    bodies[shoulder + 0].Xinv = bodies[shoulder + 0].X.inverse();
    bodies[shoulder + 1].Xinv = bodies[shoulder + 1].X.inverse();
    bodies[shoulder + 2].Xinv = bodies[shoulder + 2].X.inverse();
    bodies[shoulder + 3].Xinv = bodies[shoulder + 3].X.inverse();

    // Calculate spatial transforms to origin.
    bodies[shoulder + 0].XinOrigin = bodies[shoulder + 0].Xinv;
    bodies[shoulder + 1].XinOrigin = bodies[shoulder + 0].XinOrigin * bodies[shoulder + 1].Xinv;
    bodies[shoulder + 2].XinOrigin = bodies[shoulder + 1].XinOrigin * bodies[shoulder + 2].Xinv;
    bodies[shoulder + 3].XinOrigin = bodies[shoulder + 2].XinOrigin * bodies[shoulder + 3].Xinv;
  }

  static void calculateSpatialHeadTransforms(const JointDynamicsBH& jointDynamics,
                                             const RobotDimensionsBH& robotDimensions,
                                             Body bodies[MassCalibrationBH::numOfLimbs])
  {
    Vector3BH<> pos = Vector3BH<>(0, 0, robotDimensions.zLegJoint1ToHeadPan); // head pan in base
    RotationMatrixBH rot = RotationMatrixBH::fromRotationZ(jointDynamics.joint[JointDataBH::HeadYaw][0]).invert(); // base in head pan
    bodies[MassCalibrationBH::neck].X = SpatialTransform(rot, pos, true);
    bodies[MassCalibrationBH::neck].mode = SpatialVector<>(Vector3BH<>(0.0f, 0.0f, 1.0f), Vector3BH<>());

    rot = RotationMatrixBH::fromRotationY(-jointDynamics.joint[JointDataBH::HeadPitch][0]).invert(); // head pan in head tilt
    bodies[MassCalibrationBH::head].X = SpatialTransform(rot, Vector3BH<>(), true);
    bodies[MassCalibrationBH::head].mode = SpatialVector<>(Vector3BH<>(0.0f, -1.0f, 0.0f), Vector3BH<>());

    // Calculate inverse spatial transforms
    bodies[MassCalibrationBH::neck].Xinv = bodies[MassCalibrationBH::neck].X.inverse();
    bodies[MassCalibrationBH::head].Xinv = bodies[MassCalibrationBH::head].X.inverse();

    // Calculate spatial transforms to origin
    bodies[MassCalibrationBH::neck].XinOrigin = bodies[MassCalibrationBH::neck].Xinv;
    bodies[MassCalibrationBH::head].XinOrigin = bodies[MassCalibrationBH::neck].XinOrigin * bodies[MassCalibrationBH::head].Xinv;
  }

  static void calculateSpatialLegTransforms(bool left, const JointDynamicsBH& jointDynamics,
                                             const RobotDimensionsBH& robotDimensions,
                                             Body bodies[MassCalibrationBH::numOfLimbs])
  {
    int sign = left ? -1 : 1;
    MassCalibrationBH::Limb pelvis = left ? MassCalibrationBH::pelvisLeft : MassCalibrationBH::pelvisRight;
    JointDataBH::Joint leg0 = left ? JointDataBH::LHipYawPitch : JointDataBH::RHipYawPitch;

    // First leg joint. This one is a little complicated because the actual joint is rolled by pi/4,
    // which has to be reflected in the joint free mode.
    Vector3BH<> pos(0, robotDimensions.lengthBetweenLegs / 2.0f * -sign, 0); // pelvis' in origin;
    RotationMatrixBH rot = RotationMatrixBH::fromRotationX(-pi_4 * sign).rotateZ(jointDynamics.joint[leg0 + 0][0] * sign); // pelvis' in origin
    rot = rot.invert(); // origin in pelvis'
    SpatialTransform originInPelvisPrime(rot, pos, true);
    rot = RotationMatrixBH::fromRotationX(pi_4 * sign).invert(); // pelvis' in pelvis
    SpatialTransform pelvisPrimeInPelvis(rot, Vector3BH<>(), true);
    bodies[pelvis + 0].X = pelvisPrimeInPelvis * originInPelvisPrime; // origin in pelvis
    bodies[pelvis + 0].mode = pelvisPrimeInPelvis * SpatialVector<>(Vector3BH<>(0.0f, 0.0f, static_cast<float>(sign)), Vector3BH<>());

    rot = RotationMatrixBH::fromRotationX(jointDynamics.joint[leg0 + 1][0] * sign).invert(); // pelvis0 in pelvis1
    bodies[pelvis + 1].X = SpatialTransform(rot, Vector3BH<>(), true);
    bodies[pelvis + 1].mode = SpatialVector<>(Vector3BH<>(static_cast<float>(sign), 0.0f, 0.0f), Vector3BH<>());

    rot = RotationMatrixBH::fromRotationY(jointDynamics.joint[leg0 + 2][0]).invert(); // pelvis1 in pelvis2
    bodies[pelvis + 2].X = SpatialTransform(rot, Vector3BH<>(), true);
    bodies[pelvis + 2].mode = SpatialVector<>(Vector3BH<>(0.0f, 1.0f, 0.0f), Vector3BH<>());

    pos = Vector3BH<>(0, 0, -robotDimensions.upperLegLength); // pelvis3 in pelvis2
    rot = RotationMatrixBH::fromRotationY(jointDynamics.joint[leg0 + 3][0]).invert(); // pelvis2 in pelvis3
    bodies[pelvis + 3].X = SpatialTransform(rot, pos, true);
    bodies[pelvis + 3].mode = SpatialVector<>(Vector3BH<>(0.0f, 1.0f, 0.0f), Vector3BH<>());

    pos = Vector3BH<>(0, 0, -robotDimensions.lowerLegLength); // pelvis4 in pelvis3
    rot = RotationMatrixBH::fromRotationY(jointDynamics.joint[leg0 + 4][0]).invert(); // pelvis3 in pelvis4
    bodies[pelvis + 4].X = SpatialTransform(rot, pos, true);
    bodies[pelvis + 4].mode = SpatialVector<>(Vector3BH<>(0.0f, 1.0f, 0.0f), Vector3BH<>());

    rot = RotationMatrixBH::fromRotationX(jointDynamics.joint[leg0 + 5][0] * sign).invert(); // pelvis4 in pelvis5
    bodies[pelvis + 5].X = SpatialTransform(rot, Vector3BH<>(), true);
    bodies[pelvis + 5].mode = SpatialVector<>(Vector3BH<>(static_cast<float>(sign), 0.0f, 0.0f), Vector3BH<>());

    // Calculate inverse spatial transforms
    for(int i = 0; i <= 5; ++i)
      bodies[pelvis + i].Xinv = bodies[pelvis + i].X.inverse();

    // Calculate spatial transforms to origin
    bodies[pelvis + 0].XinOrigin = bodies[pelvis + 0].Xinv;
    for(int i = 1; i <= 5; ++i)
      bodies[pelvis + i].XinOrigin = bodies[pelvis + i - 1].XinOrigin * bodies[pelvis + i].Xinv;
  }

  /**
   * This is the entry point of the inverse dynamics.
   * Returns SpatialTransform from the torso to the support foot.
   */
  static void calculateBodyForces(bool leftSupportLeg,
                                  const JointDynamicsBH& jointDynamics,
                                  const RobotDimensionsBH& robotDimensions,
                                  Body bodies[MassCalibrationBH::numOfLimbs])
  {
    // Calculate spatial transforms
    calculateSpatialArmTransforms(false, jointDynamics, robotDimensions, bodies);
    calculateSpatialArmTransforms(true, jointDynamics, robotDimensions, bodies);
    calculateSpatialHeadTransforms(jointDynamics, robotDimensions, bodies);
    calculateSpatialLegTransforms(false, jointDynamics, robotDimensions, bodies);
    calculateSpatialLegTransforms(true, jointDynamics, robotDimensions, bodies);

    // Calculate test attributes.
    // First half
    calculateLegUpwards(leftSupportLeg, jointDynamics, bodies);
    calculateLegDownwards(!leftSupportLeg, jointDynamics, bodies);
    calculateHeadDownwards(jointDynamics, bodies);
    calculateArmDownwards(true, jointDynamics, bodies);
    calculateArmDownwards(false, jointDynamics, bodies);

    // Second half
    calculateTotalForceArmUpwards(true, bodies);
    calculateTotalForceArmUpwards(false, bodies);
    calculateTotalForceHeadUpwards(bodies);
    calculateTotalForceLegUpwards(!leftSupportLeg, bodies);
    calculateTotalForceLegDownwards(leftSupportLeg, bodies);

    // Inertia and momentum aggregates WITHOUT TORSO! The torso attributes have to be summed up
    // from the head chain, the arm chains and the not-support-foot-chain.
    // Arm chains:
    // Left arm
    calculateAggregatesUpwards(MassCalibrationBH::shoulderLeft, JointDataBH::LShoulderPitch, 3, jointDynamics, bodies);
    // Right arm
    calculateAggregatesUpwards(MassCalibrationBH::shoulderRight, JointDataBH::RShoulderPitch, 3, jointDynamics, bodies);
    // Non-support leg.
    calculateAggregatesUpwards(leftSupportLeg ? MassCalibrationBH::pelvisRight : MassCalibrationBH::pelvisLeft,
                               leftSupportLeg ? JointDataBH::RHipYawPitch      : JointDataBH::LHipYawPitch, 5,
                               jointDynamics, bodies);
    // Head
    calculateAggregatesUpwards(MassCalibrationBH::neck, JointDataBH::HeadYaw, 1, jointDynamics, bodies);
    // Sum up at torso
    calculateSumOfAggregatesAtTorso(leftSupportLeg, jointDynamics, bodies);
    // Support leg
    calculateAggregatesDownwards(leftSupportLeg, jointDynamics, bodies);
  }

  /*
   * Methods to calculate velocities, accelerations and forces of the joints
   * of the kinematic arm, leg and head chains.
   * All of these methods assume that that the joint transforms (tested elsewhere)
   * and joint modes have been calculated already.
   */

  static void calculateLegUpwards(bool left, const JointDynamicsBH& jointDynamics, Body bodies[MassCalibrationBH::numOfLimbs])
  {
    MassCalibrationBH::Limb pelvis = left ? MassCalibrationBH::pelvisLeft : MassCalibrationBH::pelvisRight;
    JointDataBH::Joint leg0 = left ? JointDataBH::LHipYawPitch : JointDataBH::RHipYawPitch;
    MassCalibrationBH::Limb torso = MassCalibrationBH::torso;

    // Initialize the base attributes.
    bodies[pelvis + 5].a = SpatialVector<>(Vector3BH<>(), Vector3BH<>(0.0f, 0.0f, 9.81f * 1e-3f)); // a_5 = -ag
    bodies[pelvis + 5].v = SpatialVector<>(); // support foot does not move => v = 0
    bodies[pelvis + 5].f = bodies[pelvis + 5].I * bodies[pelvis + 5].a;
    // Calculate the chain upwards
    for(int i = 4; i >= 0; --i)
    {
      const SpatialTransform& X = bodies[pelvis + i + 1].Xinv; // Transform from i+1 to i
      const SpatialVector<> vj = bodies[pelvis + i + 1].mode * jointDynamics.joint[leg0 + i + 1][1];
      bodies[pelvis + i].v = X * (bodies[pelvis + i + 1].v - vj);
      bodies[pelvis + i].a = X * (bodies[pelvis + i + 1].a
                                      - bodies[pelvis + i + 1].mode * jointDynamics.joint[leg0 + i + 1][2]
                                      - (bodies[pelvis + i + 1].v ^ vj));
      bodies[pelvis + i].f = bodies[pelvis + i].I * bodies[pelvis + i].a + (bodies[pelvis + i].v ^ (bodies[pelvis + i].I * bodies[pelvis + i].v));
    }
    // Calculate testA, testV, testF for the torso
    const SpatialTransform& X = bodies[pelvis].Xinv; // Transform from pelvis to torso
    const SpatialVector<> vj = bodies[pelvis].mode * jointDynamics.joint[leg0][1];
    bodies[torso].v = X * (bodies[pelvis].v - vj);
    bodies[torso].a = X * (bodies[pelvis].a - bodies[pelvis].mode * jointDynamics.joint[pelvis][2] - (bodies[pelvis].v ^ vj));
    bodies[torso].f = bodies[torso].I * bodies[torso].a + (bodies[torso].v ^ (bodies[torso].I * bodies[torso].v));
  }

  static void calculateLegDownwards(bool left, const JointDynamicsBH& jointDynamics, Body bodies[MassCalibrationBH::numOfLimbs])
  {
    MassCalibrationBH::Limb pelvis = left ? MassCalibrationBH::pelvisLeft : MassCalibrationBH::pelvisRight;
    JointDataBH::Joint leg0 = left ? JointDataBH::LHipYawPitch : JointDataBH::RHipYawPitch;
    MassCalibrationBH::Limb torso = MassCalibrationBH::torso;

    // Caluclate pelvis attributes.
    SpatialVector<> vj = bodies[pelvis].mode * jointDynamics.joint[leg0][1];
    bodies[pelvis].v = bodies[pelvis].X * bodies[torso].v + vj;
    bodies[pelvis].a = bodies[pelvis].X * bodies[torso].a
                          + bodies[pelvis].mode * jointDynamics.joint[leg0][2]
                          + (bodies[pelvis].v ^ vj);
    bodies[pelvis].f = bodies[pelvis].I * bodies[pelvis].a + (bodies[pelvis].v ^ (bodies[pelvis].I * bodies[pelvis].v));
    for(int i = 1; i <= 5; ++i)
    {
      vj = bodies[pelvis + i].mode * jointDynamics.joint[leg0 + i][1];
      bodies[pelvis + i].v = bodies[pelvis + i].X * bodies[pelvis + i - 1].v + vj;
      bodies[pelvis + i].a = bodies[pelvis + i].X * bodies[pelvis + i - 1].a
                                + bodies[pelvis + i].mode * jointDynamics.joint[leg0 + i][2]
                                + (bodies[pelvis + i].v ^ vj);
      bodies[pelvis + i].f = bodies[pelvis + i].I * bodies[pelvis + i].a + (bodies[pelvis + i].v ^ (bodies[pelvis + i].I * bodies[pelvis + i].v));
    }
  }

  static void calculateHeadDownwards(const JointDynamicsBH& jointDynamics, Body bodies[MassCalibrationBH::numOfLimbs])
  {
    const MassCalibrationBH::Limb torso = MassCalibrationBH::torso;
    const MassCalibrationBH::Limb neck = MassCalibrationBH::neck;
    const MassCalibrationBH::Limb head = MassCalibrationBH::head;
    const JointDataBH::Joint yaw = JointDataBH::HeadYaw;
    const JointDataBH::Joint pitch = JointDataBH::HeadPitch;

    // Neck attributes
    SpatialVector<> vj = bodies[neck].mode * jointDynamics.joint[yaw][1];
    bodies[neck].v = bodies[neck].X * bodies[torso].v + vj;
    bodies[neck].a = bodies[neck].X * bodies[torso].a + bodies[neck].mode * jointDynamics.joint[yaw][2] + (bodies[neck].v ^ vj);
    bodies[neck].f = bodies[neck].I * bodies[neck].a + (bodies[neck].v ^ (bodies[neck].I * bodies[neck].v));

    // Head attributes
    vj = bodies[head].mode * jointDynamics.joint[pitch][1];
    bodies[head].v = bodies[head].X * bodies[neck].v + vj;
    bodies[head].a = bodies[head].X * bodies[neck].a + bodies[head].mode * jointDynamics.joint[pitch][2] + (bodies[head].v ^ vj);
    bodies[head].f = bodies[head].I * bodies[head].a + (bodies[head].v ^ (bodies[head].I * bodies[head].v));
  }

  static void calculateArmDownwards(bool left, const JointDynamicsBH& jointDynamics, Body bodies[MassCalibrationBH::numOfLimbs])
  {
    MassCalibrationBH::Limb torso = MassCalibrationBH::torso;
    MassCalibrationBH::Limb shoulder = left ? MassCalibrationBH::shoulderLeft : MassCalibrationBH::shoulderRight;
    JointDataBH::Joint arm0 = left ? JointDataBH::LShoulderPitch : JointDataBH::RShoulderPitch;

    SpatialVector<> vj = bodies[shoulder].mode * jointDynamics.joint[arm0][1];
    bodies[shoulder].v = bodies[shoulder].X * bodies[torso].v + vj;
    bodies[shoulder].a = bodies[shoulder].X * bodies[torso].a + bodies[shoulder].mode * jointDynamics.joint[arm0][2] + (bodies[shoulder].v ^ vj);
    bodies[shoulder].f = bodies[shoulder].I * bodies[shoulder].a + (bodies[shoulder].v ^ (bodies[shoulder].I * bodies[shoulder].v));

    for(int i = 1; i <= 3; ++i)
    {
      vj = bodies[shoulder + i].mode * jointDynamics.joint[arm0 + i][1];
      bodies[shoulder + i].v = bodies[shoulder + i].X * bodies[shoulder + i - 1].v + vj;
      bodies[shoulder + i].a = bodies[shoulder + i].X * bodies[shoulder + i - 1].a
                                  + bodies[shoulder + i].mode * jointDynamics.joint[arm0 + i][2]
                                  + (bodies[shoulder + i].v ^ vj);
      bodies[shoulder + i].f = bodies[shoulder + i].I * bodies[shoulder + i].a + (bodies[shoulder + i].v ^ (bodies[shoulder + i].I * bodies[shoulder + i].v));
    }
  }

  static void calculateTotalForceArmUpwards(bool left, Body bodies[MassCalibrationBH::numOfLimbs])
  {
    MassCalibrationBH::Limb torso = MassCalibrationBH::torso;
    MassCalibrationBH::Limb shoulder = left ? MassCalibrationBH::shoulderLeft : MassCalibrationBH::shoulderRight;

    for(int i = 3; i > 0; --i)
      bodies[shoulder + i - 1].f += bodies[shoulder + i].Xinv * bodies[shoulder + i].f;
    bodies[torso].f += bodies[shoulder].Xinv * bodies[shoulder].f;
  }

  static void calculateTotalForceHeadUpwards(Body bodies[MassCalibrationBH::numOfLimbs])
  {
    const MassCalibrationBH::Limb torso = MassCalibrationBH::torso;
    const MassCalibrationBH::Limb neck = MassCalibrationBH::neck;
    const MassCalibrationBH::Limb head = MassCalibrationBH::head;
    bodies[neck].f += bodies[head].Xinv * bodies[head].f;
    bodies[torso].f += bodies[neck].Xinv * bodies[neck].f;
  }

  static void calculateTotalForceLegUpwards(bool left, Body bodies[MassCalibrationBH::numOfLimbs])
  {
    const MassCalibrationBH::Limb pelvis = left ? MassCalibrationBH::pelvisLeft : MassCalibrationBH::pelvisRight;
    const MassCalibrationBH::Limb torso = MassCalibrationBH::torso;

    for(int i = 5; i > 0; --i)
      bodies[pelvis + i - 1].f += bodies[pelvis + i].Xinv * bodies[pelvis + i].f;
    bodies[torso].f += bodies[pelvis].Xinv * bodies[pelvis].f;
  }

  static void calculateTotalForceLegDownwards(bool left, Body bodies[MassCalibrationBH::numOfLimbs])
  {
    const MassCalibrationBH::Limb pelvis = left ? MassCalibrationBH::pelvisLeft : MassCalibrationBH::pelvisRight;
    const MassCalibrationBH::Limb torso = MassCalibrationBH::torso;

    bodies[pelvis].f += bodies[pelvis].X * bodies[torso].f;
    for(int i = 1; i <= 5; ++i)
      bodies[pelvis + i].f += bodies[pelvis + i].X * bodies[pelvis + i - 1].f;
  }

  static SpatialTransform calculateTransformFromTorsoToFoot(bool left, const Body bodies[MassCalibrationBH::numOfLimbs])
  {
    const MassCalibrationBH::Limb pelvis = left ? MassCalibrationBH::pelvisLeft : MassCalibrationBH::pelvisRight;

    SpatialTransform X = bodies[pelvis + 5].X; // pelvis+4 to pelvis+5
    for(int i = 4; i >= 0; --i)
      X *= bodies[pelvis + i].X;

    return X;
  }

  static void calculateAggregatesUpwards(const MassCalibrationBH::Limb baseBody, const JointDataBH::Joint baseJoint,
                                         const int imax, const JointDynamicsBH& jointDynamics,
                                         Body bodies[MassCalibrationBH::numOfLimbs])
  {
    bodies[baseBody + imax].Ic = bodies[baseBody + imax].I;
    SpatialVector<> vj = bodies[baseBody + imax].mode * jointDynamics.joint[baseJoint + imax][1]; // Joint velocity
    bodies[baseBody + imax].pc = bodies[baseBody + imax].Ic * vj;
    bodies[baseBody + imax].dIc = bodies[baseBody + imax].Ic.derive(vj);
    SpatialVector<> aj = bodies[baseBody + imax].mode * jointDynamics.joint[baseJoint + imax][2]; // Joint acceleration
    bodies[baseBody + imax].fc = (vj ^ bodies[baseBody + imax].pc) + bodies[baseBody + imax].Ic * aj + bodies[baseBody + imax].dIc * vj;
    for(int i = imax - 1; i >= 0; --i)
    {
      bodies[baseBody + i].Ic = bodies[baseBody + i].I
                                + bodies[baseBody + i + 1].X.transform(bodies[baseBody + i + 1].Ic);
      vj = bodies[baseBody + i].mode * jointDynamics.joint[baseJoint + i][1];
      bodies[baseBody + i].pc = bodies[baseBody + i].Ic * vj
                                + bodies[baseBody + i + 1].Xinv * bodies[baseBody + i + 1].pc;
      bodies[baseBody + i].dIc = bodies[baseBody + i + 1].X.transform(bodies[baseBody + i + 1].dIc)
                                 + bodies[baseBody + i].Ic.derive(vj);
      aj = bodies[baseBody + i].mode * jointDynamics.joint[baseJoint + i][2];
      bodies[baseBody + i].fc = bodies[baseBody + i + 1].Xinv * bodies[baseBody + i + 1].fc
                                + (vj ^ bodies[baseBody + i].pc)
                                + bodies[baseBody + i].Ic * aj + bodies[baseBody + i].dIc * vj;
    }
  }

  static void calculateSumOfAggregatesAtTorso(const bool leftSupportLeg,
                                              const JointDynamicsBH& jointDynamics,
                                              Body bodies[MassCalibrationBH::numOfLimbs])
  {
    // Inertia local to the torso
    const MassCalibrationBH::Limb torso = MassCalibrationBH::torso;
    const MassCalibrationBH::Limb neck = MassCalibrationBH::neck;
    const MassCalibrationBH::Limb shoulderLeft = MassCalibrationBH::shoulderLeft;
    const MassCalibrationBH::Limb shoulderRight = MassCalibrationBH::shoulderRight;
    const MassCalibrationBH::Limb pelvis = leftSupportLeg ? MassCalibrationBH::pelvisRight // Non-support pelvis
                                                        : MassCalibrationBH::pelvisLeft;
    const MassCalibrationBH::Limb supportPelvis = leftSupportLeg ? MassCalibrationBH::pelvisLeft // Support pelvis
                                                               : MassCalibrationBH::pelvisRight;
    const JointDataBH::Joint supportLeg0 = leftSupportLeg ? JointDataBH::LHipYawPitch
                                                        : JointDataBH::RHipYawPitch;

    bodies[torso].Ic = bodies[torso].I;
    // Add up arm, non-support-leg and head composite inertias
    bodies[torso].Ic += bodies[shoulderLeft].X.transform(bodies[shoulderLeft].Ic);
    bodies[torso].Ic += bodies[shoulderRight].X.transform(bodies[shoulderRight].Ic);
    bodies[torso].Ic += bodies[neck].X.transform(bodies[neck].Ic);
    bodies[torso].Ic += bodies[pelvis].X.transform(bodies[pelvis].Ic);

    // Momentum local to the torso
    SpatialVector<> vj = bodies[supportPelvis].Xinv
                        * (bodies[supportPelvis].mode * (-jointDynamics.joint[supportLeg0][1]));
    bodies[torso].pc = bodies[torso].Ic * vj;
    // Add up arm, non-support-leg and head composite momenta
    bodies[torso].pc += bodies[shoulderLeft].Xinv * bodies[shoulderLeft].pc;
    bodies[torso].pc += bodies[shoulderRight].Xinv * bodies[shoulderRight].pc;
    bodies[torso].pc += bodies[neck].Xinv * bodies[neck].pc;
    bodies[torso].pc += bodies[pelvis].Xinv * bodies[pelvis].pc;

    // Composite spatial inertia derivative local to the torso:
    bodies[torso].dIc = bodies[torso].Ic.derive(vj);
    // Add up arm, non-support-leg and head composite spatial inertia derivatives.
    bodies[torso].dIc += bodies[shoulderLeft].X.transform(bodies[shoulderLeft].dIc);
    bodies[torso].dIc += bodies[shoulderRight].X.transform(bodies[shoulderRight].dIc);
    bodies[torso].dIc += bodies[neck].X.transform(bodies[neck].dIc);
    bodies[torso].dIc += bodies[pelvis].X.transform(bodies[pelvis].dIc);

    // Compute composite force local to the torso
    SpatialVector<> aj = bodies[supportPelvis].Xinv
                        * (bodies[supportPelvis].mode * (-jointDynamics.joint[supportLeg0][2]));
    bodies[torso].fc = (vj ^ bodies[torso].pc) + bodies[torso].Ic * aj + bodies[torso].dIc * vj;
    // Add up arm, non-support-leg and head composite forces
    bodies[torso].fc += bodies[shoulderLeft].Xinv * bodies[shoulderLeft].fc;
    bodies[torso].fc += bodies[shoulderRight].Xinv * bodies[shoulderRight].fc;
    bodies[torso].fc += bodies[neck].Xinv * bodies[neck].fc;
    bodies[torso].fc += bodies[pelvis].Xinv * bodies[pelvis].fc;
  }

  static void calculateAggregatesDownwards(bool leftSupportLeg, const JointDynamicsBH& jointDynamics,
                                           Body bodies[MassCalibrationBH::numOfLimbs])
  {
    const MassCalibrationBH::Limb torso = MassCalibrationBH::torso;
    const MassCalibrationBH::Limb pelvis = leftSupportLeg ? MassCalibrationBH::pelvisLeft // Support pelvis
                                                        : MassCalibrationBH::pelvisRight;
    const JointDataBH::Joint leg0 = leftSupportLeg ? JointDataBH::LHipYawPitch
                                                 : JointDataBH::RHipYawPitch;
    bodies[pelvis].Ic = bodies[pelvis].Xinv.transform(bodies[torso].Ic) + bodies[pelvis].I;
    SpatialVector<> vj = bodies[pelvis + 1].Xinv * (bodies[pelvis + 1].mode * (-jointDynamics.joint[leg0 + 1][1]));
    bodies[pelvis].pc = bodies[pelvis].X * bodies[torso].pc + bodies[pelvis].Ic * vj;
    bodies[pelvis].dIc = bodies[pelvis].Xinv.transform(bodies[torso].dIc) + bodies[pelvis].Ic.derive(vj);
    SpatialVector<> aj = bodies[pelvis + 1].Xinv * (bodies[pelvis + 1].mode * (-jointDynamics.joint[leg0 + 1][2]));
    bodies[pelvis].fc = bodies[pelvis].X * bodies[torso].fc + (vj ^ bodies[pelvis].pc) + bodies[pelvis].Ic * aj + bodies[pelvis].dIc * vj;
    for(int i = 1; i <= 5; ++i)
    {
      bodies[pelvis + i].Ic = bodies[pelvis + i].Xinv.transform(bodies[pelvis + i - 1].Ic) + bodies[pelvis + i].I;
      bodies[pelvis + i].pc = bodies[pelvis + i].X * bodies[pelvis + i - 1].pc;
      bodies[pelvis + i].dIc = bodies[pelvis + i].Xinv.transform(bodies[pelvis + i - 1].dIc);
      bodies[pelvis + i].fc = bodies[pelvis + i].X * bodies[pelvis + i - 1].fc;
      if(i < 5) // Body pelvis+5 is the foot on the ground which has 0 velocity, so it does not add any local momentum
      {
        vj = bodies[pelvis + i + 1].Xinv * (bodies[pelvis + i + 1].mode * (-jointDynamics.joint[leg0 + i + 1][1]));
        bodies[pelvis + i].pc += bodies[pelvis + i].Ic * vj;
        bodies[pelvis + i].dIc += bodies[pelvis + i].Ic.derive(vj);
        aj = bodies[pelvis + i + 1].Xinv * (bodies[pelvis + i + 1].mode * (-jointDynamics.joint[leg0 + i + 1][2]));
        bodies[pelvis + i].fc += (vj ^ bodies[pelvis + i].pc) + bodies[pelvis + i].Ic * aj + bodies[pelvis + i].dIc * vj;
      }
    }

    // Add gravity compensation
    bodies[pelvis + 5].fc += bodies[pelvis + 5].Ic * bodies[pelvis + 5].a; // a is -g here.
  }
};
