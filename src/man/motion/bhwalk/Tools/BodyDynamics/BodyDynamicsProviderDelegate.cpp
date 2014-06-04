/**
 * @file BodyDynamicsProviderDelegate.cpp
 *
 * This file implements a class that provides the dynamics of the robot's limbs.
 */

#include "BodyDynamicsProviderDelegate.h"
#include "Tools/BodyDynamics/InverseDynamics.h"

BodyDynamicsProviderDelegate::BodyDynamicsProviderDelegate(const MassCalibrationBH& theMassCalibrationBH,
                                                           const TorsoMatrixBH& theTorsoMatrixBH,
                                                           const RobotDimensionsBH& theRobotDimensionsBH,
                                                           const FrameInfoBH& theFrameInfoBH)
: supportFootInGround(RotationMatrixBH(), Vector3BH<>(0.0f, 0.0f, -theRobotDimensions.heightLeg5Joint), true),
  theMassCalibrationBH(theMassCalibrationBH), theTorsoMatrixBH(theTorsoMatrixBH),
  theRobotDimensionsBH(theRobotDimensionsBH), theFrameInfoBH(theFrameInfoBH)
{
  init();
}

void BodyDynamicsProviderDelegate::init()
{
  // Set all timestamps of the body-to-support-foot transformations to 0.
  for(int i = 0; i < JointDataBH::numOfJoints; ++i)
    bodyInSupportFootTimestamp[i] = 0;
}

void BodyDynamicsProviderDelegate::update(BodyDynamics& bodyDynamics, const JointDynamicsBH& jointDynamics)
{
  if(!bodyDynamics.inertiasInitialized)
    bodyDynamics.init(theMassCalibrationBH);

  if(jointDynamics.supportLeg == IndykickRequest::unspecified)
    bodyDynamics.supportLeg = theTorsoMatrixBH.leftSupportFoot ? IndykickRequest::left : IndykickRequest::right;
  else
    bodyDynamics.supportLeg = jointDynamics.supportLeg;

  InverseDynamics::calculateBodyForces(bodyDynamics.supportLeg == IndykickRequest::left,
                                       jointDynamics, theRobotDimensionsBH, bodyDynamics.limbs);
}

void BodyDynamicsProviderDelegate::update(BodyDynamicsDerivatives& bodyDynamicsDerivatives,
                                          const BodyDynamics& bodyDynamics,
                                          const JointDynamicsBH& jointDynamics)
{
  bodyDynamicsDerivatives.supportLeg = bodyDynamics.supportLeg;
  const bool leftSupport = bodyDynamics.supportLeg == IndykickRequest::left;
  const JointDataBH::Joint supportLegJointLeg0 = leftSupport
    ? JointDataBH::LHipYawPitch
    : JointDataBH::RHipYawPitch;
  const JointDataBH::Joint nonSupportLegJointLeg0 = leftSupport
    ? JointDataBH::RHipYawPitch
    : JointDataBH::LHipYawPitch;
  const MassCalibrationBH::Limb pelvisNonSupport = leftSupport
    ? MassCalibrationBH::pelvisRight // Kick pelvis
    : MassCalibrationBH::pelvisLeft;

  // Calculate derivatives for the legs.
  for(int i = 0; i < numOfLegJoints; ++i)
  {
    // Calculate derivatives for the support leg
    BodyDynamicsDerivatives::Derivative& pdSupport = bodyDynamicsDerivatives.partialDerivatives[supportLegJointLeg0 + i];
    footForceDerivativeUpwards(i, pdSupport.fcByqDoubleDotFoot, pdSupport.fcByqDotFoot, pdSupport.fcByqFoot, bodyDynamics);
    pdSupport.fcByqGround = supportFootInGround * pdSupport.fcByqFoot;
    pdSupport.fcByqDotGround = supportFootInGround * pdSupport.fcByqDotFoot;
    pdSupport.fcByqDoubleDotGround = supportFootInGround * pdSupport.fcByqDoubleDotFoot;
    // Calculate derivatives for the non-support leg.
    BodyDynamicsDerivatives::Derivative& pdNonSupport = bodyDynamicsDerivatives.partialDerivatives[nonSupportLegJointLeg0 + i];
    footForceDerivativeDownwards(pelvisNonSupport, i, pdNonSupport.fcByqDoubleDotFoot, pdNonSupport.fcByqDotFoot, pdNonSupport.fcByqFoot, bodyDynamics);
    pdNonSupport.fcByqGround = supportFootInGround * pdNonSupport.fcByqFoot;
    pdNonSupport.fcByqDotGround = supportFootInGround * pdNonSupport.fcByqDotFoot;
    pdNonSupport.fcByqDoubleDotGround = supportFootInGround * pdNonSupport.fcByqDoubleDotFoot;
  }

  // Calculate derivatives for the arms.
  for(int i = 0; i < numOfArmJoints; ++i)
  {
    BodyDynamicsDerivatives::Derivative& pdLeft = bodyDynamicsDerivatives.partialDerivatives[JointDataBH::LShoulderPitch + i];
    footForceDerivativeDownwards(MassCalibrationBH::shoulderLeft, i, pdLeft.fcByqDoubleDotFoot, pdLeft.fcByqDotFoot, pdLeft.fcByqFoot, bodyDynamics);
    pdLeft.fcByqGround = supportFootInGround * pdLeft.fcByqFoot;
    pdLeft.fcByqDotGround = supportFootInGround * pdLeft.fcByqDotFoot;
    pdLeft.fcByqDoubleDotGround = supportFootInGround * pdLeft.fcByqDoubleDotFoot;
    BodyDynamicsDerivatives::Derivative& pdRight = bodyDynamicsDerivatives.partialDerivatives[JointDataBH::RShoulderPitch + i];
    footForceDerivativeDownwards(MassCalibrationBH::shoulderRight, i, pdRight.fcByqDoubleDotFoot, pdRight.fcByqDotFoot, pdRight.fcByqFoot, bodyDynamics);
    pdRight.fcByqGround = supportFootInGround * pdRight.fcByqFoot;
    pdRight.fcByqDotGround = supportFootInGround * pdRight.fcByqDotFoot;
    pdRight.fcByqDoubleDotGround = supportFootInGround * pdRight.fcByqDoubleDotFoot;
  }

  // Calculate derivatives for the head.
  BodyDynamicsDerivatives::Derivative& pdNeck = bodyDynamicsDerivatives.partialDerivatives[JointDataBH::HeadYaw];
  footForceDerivativeDownwards(MassCalibrationBH::neck, 0, pdNeck.fcByqDoubleDotFoot, pdNeck.fcByqDotFoot, pdNeck.fcByqFoot, bodyDynamics);
  pdNeck.fcByqGround = supportFootInGround * pdNeck.fcByqFoot;
  pdNeck.fcByqDotGround = supportFootInGround * pdNeck.fcByqDotFoot;
  pdNeck.fcByqDoubleDotGround = supportFootInGround * pdNeck.fcByqDoubleDotFoot;
  BodyDynamicsDerivatives::Derivative& pdHead = bodyDynamicsDerivatives.partialDerivatives[JointDataBH::HeadPitch];
  footForceDerivativeDownwards(MassCalibrationBH::neck, 1, pdHead.fcByqDoubleDotFoot, pdHead.fcByqDotFoot, pdHead.fcByqFoot, bodyDynamics);
  pdHead.fcByqGround = supportFootInGround * pdHead.fcByqFoot;
  pdHead.fcByqDotGround = supportFootInGround * pdHead.fcByqDotFoot;
  pdHead.fcByqDoubleDotGround = supportFootInGround * pdHead.fcByqDoubleDotFoot;
}

void BodyDynamicsProviderDelegate::footForceDerivativeUpwards(const int jointOffset,
                                                              SpatialVector<>& fcByqDoubleDotFoot,
                                                              SpatialVector<>& fcByqDotFoot,
                                                              SpatialVector<>& fcByqFoot,
                                                              const BodyDynamics& bodyDynamics)
{
  const bool leftSupport = bodyDynamics.supportLeg == IndykickRequest::left;
  const MassCalibrationBH::Limb pelvis = leftSupport ? MassCalibrationBH::pelvisLeft // Support pelvis
                                                   : MassCalibrationBH::pelvisRight;
  /* In a kinematic tree rootet at the torso, the natural body would be the successor body of the joint. */
  const MassCalibrationBH::Limb naturalBodyLimb = static_cast<MassCalibrationBH::Limb>(pelvis + jointOffset);
  /* In a kinematic tree rooted at the torso, the natural predecessor would be the predecessor body of the joint. */
  const MassCalibrationBH::Limb naturalPredecessorLimb = !jointOffset ? MassCalibrationBH::torso
                                                                    : static_cast<MassCalibrationBH::Limb>(pelvis + jointOffset - 1);
  const Body& naturalBody = bodyDynamics.limbs[naturalBodyLimb];
  const Body& naturalPredecessor = bodyDynamics.limbs[naturalPredecessorLimb];

  /* The joint at 'jointOffset' connects the natural body to the natural predecessor, since the base body is at the
   * end of the chain, i.e. one could say the natural leaf body.  The reference coordinate system is now on the side
   * of the natural predecessor of the joint, but still there where the joint is located, i.e. the physical position
   * is the same as the natual body's position, but the rotation of the coordinate system is different. */
  const SpatialTransform& nPredInNBody = naturalBody.X;
  const SpatialTransform& nBodyInNPred = naturalBody.Xinv;

  const SpatialTransform nBodyInRef(nBodyInNPred.rot, Vector3BH<>(), true);
  const SpatialTransform nPredInRef(RotationMatrixBH(), -nPredInNBody.pos, true);

  /* Symbols indexed with i mean the things stored in the natural predecessor, since i means the target of the joint.
   * Symbols indexed with (i-1) mean the things stored in the natural body, since (i-1) means the source of the joint. */

  /* === Equation 41. === */
  /* First, transform the axis s_i to the reference system. This is easy, since it is the axis of rotation,
   * it does not change by the rotation transform. Only the sign flips. The axis is the exception of the index rule.
   * Since it connected the bodies 'natural body' and 'natural predecessor' and 'natural body' has the 'naturally' higher
   * index, it is stored in the 'natural body' object. */
  const SpatialVector<> axis = -naturalBody.mode;
  /* Transform the aggregate spatial inertia to the reference coordinate system. */
  const SpatialInertia Ic = nPredInRef.transform(naturalPredecessor.Ic);
  /* Calculate the actual equation. */
  const SpatialInertiaDerivative IcByq = Ic.derive(axis);

  /* === Equation 42. === */
  const SpatialVector<> pcByqDot = Ic * axis;

  /* === Equation 43. === */
  /* Transform the velocity of the natural body into the reference system. */
  const SpatialVector<> velocityBody = nBodyInRef * naturalBody.v;
  /* Transform the aggregate momentum of the natural predecessor into the reference system. */
  const SpatialVector<> pc = nPredInRef * naturalPredecessor.pc;
  /* Calculate the actual equation. */
  const SpatialVector<> pcByq = IcByq * velocityBody + (axis ^ pc);

  /* === Equation 44. === */
  const SpatialVector<>& fcByqDoubleDot = pcByqDot;

  /* === Equation 45. === */
  /* Transform the time derivative of the aggregate spatial inertia to the reference system. */
  const SpatialInertiaDerivative dIc = nPredInRef.transform(naturalPredecessor.dIc);
  /* Transform the velocity ot the natural predecessor into the reference system. */
  const SpatialVector<> velocityPred = nPredInRef * naturalPredecessor.v;
  /* Calculate the actual equation. */
  const SpatialVector<> fcByqDot = pcByq + dIc * axis + (velocityPred ^ pcByqDot);

  /* === Equation 46. === */
  /* Transform the acceleration of the natural body into the reference system. */
  const SpatialVector<> accelBody = nBodyInRef * naturalBody.a;
  /* Transform the aggregate force of the natural predecessor into the reference system. */
  const SpatialVector<> fc = nPredInRef * naturalPredecessor.fc;
  /* Calculate the derivative by q of the time derivative of the aggregate spatial inertia. */
  const SpatialInertiaDerivative dIcByq = dIc.derive(axis);
  /* Calculate the actual equation. */
  const SpatialVector<> fcByq = (axis ^ fc)
    + (IcByq * accelBody)
    + (velocityBody ^ ((axis ^ pc) + (IcByq * velocityBody)))
    + (dIcByq * velocityBody);

  /* In principle all the derivatives have been calculated by now. They only have to be transformed
   * from the reference coordinate system into the coordinate system of the support foot. */
  /* Get the transformation from the natural body into the support foot. */
  const SpatialTransform& nBodyInFoot = getBodyInSupportFoot(naturalBodyLimb, bodyDynamics);
  /* Calculate the transform from reference system into the natural body. */
  const SpatialTransform& refInNBody = nBodyInRef.inverse();
  /* Calculate the transform from the reference system into the support foot system. */
  const SpatialTransform refInFoot = nBodyInFoot * refInNBody;
  /* Do the transformations. */
  fcByqDoubleDotFoot = refInFoot * fcByqDoubleDot;
  fcByqDotFoot = refInFoot * fcByqDot;
  fcByqFoot = refInFoot * fcByq;
}

void BodyDynamicsProviderDelegate::footForceDerivativeDownwards(const MassCalibrationBH::Limb base,
                                                                const int jointOffset,
                                                                SpatialVector<> &fcByqDoubleDotFoot,
                                                                SpatialVector<> &fcByqDotFoot,
                                                                SpatialVector<> &fcByqFoot,
                                                                const BodyDynamics &bodyDynamics)
{
  // In a kinematic tree rootet at the torso, the natural body would be the successor body of the joint.
  const MassCalibrationBH::Limb naturalBody = static_cast<MassCalibrationBH::Limb>(base + jointOffset);
  // In a kinematic tree rooted at the torso, the natural predecessor would be the predecessor body of the joint.
  const MassCalibrationBH::Limb naturalPredecessor = !jointOffset ? MassCalibrationBH::torso
                                                                : static_cast<MassCalibrationBH::Limb>(base + jointOffset - 1);

  // Derivatives for the joint (joint0 + jointOffset; all in (base + jointOffset; downwards!) coordinates)
  // Equation 41
  const SpatialVector<>& axis = bodyDynamics.limbs[naturalBody].mode;
  const SpatialInertiaDerivative IcByq = bodyDynamics.limbs[naturalBody].Ic.derive(axis); // Composite Inertia derived by q

  // Equation 42 (equal to eq. 44 which is fcByqDoubleDot)
  const SpatialVector<>& pcByqDot = bodyDynamics.limbs[naturalBody].Ic * axis; // Composite momentum derived by q dot (velocity)
  ASSERT(!pcByqDot.motion);

  // Equation 43
  const SpatialTransform& X = bodyDynamics.limbs[naturalBody].X; // (base + jointOffset - 1) in (base + jointOffset)
  const SpatialVector<>& velocity = X * bodyDynamics.limbs[naturalPredecessor].v;
  const SpatialVector<>& pcByq = IcByq * velocity + (axis ^ bodyDynamics.limbs[naturalBody].pc);
  ASSERT(!pcByq.motion);

  // Equation 46
  const SpatialVector<> accel = X * bodyDynamics.limbs[naturalPredecessor].a;
  const SpatialInertiaDerivative dIcByq = bodyDynamics.limbs[naturalBody].dIc.derive(axis);
  const SpatialVector<> fcByq = (axis ^ bodyDynamics.limbs[naturalBody].fc)
                              + (IcByq * accel)
                              + (velocity ^ ((axis ^ bodyDynamics.limbs[naturalBody].pc) + IcByq * velocity))
                              + (dIcByq * velocity);
  ASSERT(!fcByq.motion);

  // Equation 45
  const SpatialVector<> fcByqDot = pcByq + bodyDynamics.limbs[naturalBody].dIc * axis
                                  + (bodyDynamics.limbs[naturalBody].v ^ pcByqDot);

  const SpatialTransform& XinFoot = getBodyInSupportFoot(static_cast<MassCalibrationBH::Limb>(naturalBody), bodyDynamics);
  fcByqDoubleDotFoot = XinFoot * pcByqDot; // Eq. 44
  fcByqDotFoot       = XinFoot * fcByqDot;
  fcByqFoot          = XinFoot * fcByq;
}

SpatialVector<> BodyDynamicsProviderDelegate::calculateVelocityDerivativeSummandUpwards(const int jOffset, const int iOffset,
                                                                                        const BodyDynamics& bodyDynamics,
                                                                                        const JointDynamicsBH& jointDynamics)
{
  const bool leftSupport = bodyDynamics.supportLeg == IndykickRequest::left;
  const JointDataBH::Joint supportHipYawPitch = leftSupport ? JointDataBH::LHipYawPitch : JointDataBH::RHipYawPitch;
  const MassCalibrationBH::Limb base = leftSupport ? MassCalibrationBH::pelvisLeft : MassCalibrationBH::pelvisRight;
  ASSERT(jOffset == 1 || jOffset == 2 || jOffset == 4 || jOffset == 5);
  ASSERT(0 <= iOffset && iOffset <= 5);

  const MassCalibrationBH::Limb iNaturalBody = static_cast<MassCalibrationBH::Limb>(base + iOffset);
  const MassCalibrationBH::Limb iNaturalPredecessor = iOffset ? static_cast<MassCalibrationBH::Limb>(base + iOffset - 1)
                                                            : MassCalibrationBH::torso;
  const MassCalibrationBH::Limb jNaturalBody = static_cast<MassCalibrationBH::Limb>(base + jOffset);
  const MassCalibrationBH::Limb jNaturalPredecessor = jOffset ? static_cast<MassCalibrationBH::Limb>(base + jOffset - 1)
                                                            : MassCalibrationBH::torso;

  const SpatialVector<> iAxis = bodyDynamics.limbs[iNaturalBody].Xinv * (-bodyDynamics.limbs[iNaturalBody].mode);
  const SpatialVector<> jAxis = bodyDynamics.limbs[jNaturalBody].Xinv * (-bodyDynamics.limbs[jNaturalBody].mode);

  SpatialVector<> derivative;
  const SpatialTransform jInI = bodyDynamics.limbs[iNaturalPredecessor].XinOrigin.inverse()
                                * bodyDynamics.limbs[jNaturalPredecessor].XinOrigin;
  if(jOffset > iOffset) // If j _preceedes_ i in the tree rooted at the support foot (which has the largest offset)
  {
    const SpatialInertia& Ic = bodyDynamics.limbs[iNaturalPredecessor].Ic;
    const SpatialInertiaDerivative IcByqi = Ic.derive(iAxis);
    derivative = IcByqi * (jInI * jAxis);
  }
  else // If j does not preceede i in the tree rooted at the support foot
  {
    const SpatialInertia& Ic = bodyDynamics.limbs[jNaturalPredecessor].Ic;
    derivative = iAxis ^ (jInI * (Ic * jAxis));
  }

  // Convert to support foot coordinates and multiply by the scalar joint velocity
  const SpatialTransform& iNPInSupportFoot = getBodyInSupportFoot(iNaturalPredecessor, bodyDynamics);
  derivative = iNPInSupportFoot * derivative;
  derivative *= jointDynamics.joint[supportHipYawPitch + iOffset][1];

  return derivative;
}

SpatialVector<> BodyDynamicsProviderDelegate::calculateVelocityDerivativeSummandDownwards(const int jOffset,
                                                const JointDataBH::Joint baseJoint, const MassCalibrationBH::Limb baseBody,
                                                const int iOffset, const BodyDynamics &bodyDynamics,
                                                const JointDynamicsBH &jointDynamics)
{
  const bool leftSupport = bodyDynamics.supportLeg == IndykickRequest::left;
  const MassCalibrationBH::Limb pelvis = leftSupport ? MassCalibrationBH::pelvisLeft : MassCalibrationBH::pelvisRight;
  ASSERT(jOffset == 1 || jOffset == 2 || jOffset == 4 || jOffset == 5);
  ASSERT(0 <= iOffset && iOffset <= 5);

  const MassCalibrationBH::Limb iNaturalBody = static_cast<MassCalibrationBH::Limb>(baseBody + iOffset);
  /* As in the mehtod above, j denotes a balancing joint in the support leg. */
  const MassCalibrationBH::Limb jNaturalBody = static_cast<MassCalibrationBH::Limb>(pelvis + jOffset);
  const MassCalibrationBH::Limb jNaturalPredecessor = jOffset ? static_cast<MassCalibrationBH::Limb>(pelvis + jOffset - 1)
                                                            : MassCalibrationBH::torso;
  const SpatialVector<> jAxis = bodyDynamics.limbs[jNaturalBody].Xinv * (-bodyDynamics.limbs[jNaturalBody].mode);
  const SpatialVector<>& iAxis = bodyDynamics.limbs[iNaturalBody].mode;
  const SpatialTransform jInI = bodyDynamics.limbs[iNaturalBody].XinOrigin.inverse()
                                * bodyDynamics.limbs[jNaturalPredecessor].XinOrigin;
  /* We know that j is in the support leg and i is not, so j *always* preceedes i in the tree rooted at the support foot. */
  const SpatialInertia& Ic = bodyDynamics.limbs[iNaturalBody].Ic;
  const SpatialInertiaDerivative IcByqi = Ic.derive(iAxis);
  SpatialVector<> derivative = IcByqi * (jInI * jAxis);

  /* Convert to support foot coordinates and multiply by the scalar joint velocity */
  const SpatialTransform iNBInSupportFoot = getBodyInSupportFoot(iNaturalBody, bodyDynamics);
  derivative = iNBInSupportFoot * derivative;
  derivative *= jointDynamics.joint[baseJoint + iOffset][1];

  return derivative;
}

void BodyDynamicsProviderDelegate::invalidateBodyInSupportFootTransformations()
{
  for(int j = 0; j < JointDataBH::numOfJoints; ++j)
    bodyInSupportFootTimestamp[j] = 0L;
}

const SpatialTransform& BodyDynamicsProviderDelegate::getBodyInSupportFoot(const MassCalibrationBH::Limb limb,
                                                                           const BodyDynamics& bodyDynamics)
{
  const MassCalibrationBH::Limb supportFoot = bodyDynamics.supportLeg == IndykickRequest::left
                                          ? MassCalibrationBH::footLeft : MassCalibrationBH::footRight;
  if(theFrameInfoBH.time > bodyInSupportFootTimestamp[limb])
  {
    // Calculate current transform
    if(limb == MassCalibrationBH::torso) {
      bodyInSupportFoot[MassCalibrationBH::torso] = bodyDynamics.limbs[supportFoot].XinOrigin.inverse();
    }
    else
    {
      bodyInSupportFoot[limb] = getBodyInSupportFoot(MassCalibrationBH::torso, bodyDynamics);
      bodyInSupportFoot[limb] *= bodyDynamics.limbs[limb].XinOrigin;
    }
    bodyInSupportFootTimestamp[limb] = theFrameInfoBH.time;
  }
  return bodyInSupportFoot[limb];
}
