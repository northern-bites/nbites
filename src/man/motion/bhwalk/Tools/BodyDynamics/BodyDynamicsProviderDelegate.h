/**
 * @file BodyDynamicsProviderDelegate.h
 *
 * File that declares a class that does the actual work for the module which
 * provides the current and future dynamics of the robot's limbs.
 *
 * @author Felix Wenk
 */

#pragma once

#include "Representations/Sensing/RobotModel.h"
#include "Representations/Sensing/JointDynamics.h"
#include "Representations/Sensing/BodyDynamics.h"
#include "Representations/Sensing/TorsoMatrix.h"
#include "Representations/Configuration/RobotDimensions.h"
#include "Representations/Configuration/MassCalibration.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/JointData.h"
#include "Representations/MotionControl/MotionInfo.h"

/**
 * @classs BodyDynamicsProviderDelegate
 *
 * A class for computing the dynamics of the robot's limbs
 */
class BodyDynamicsProviderDelegate
{
public:
  BodyDynamicsProviderDelegate(const MassCalibrationBH& theMassCalibrationBH,
                               const TorsoMatrixBH& theTorsoMatrixBH,
                               const RobotDimensionsBH& theRobotDimensionsBH,
                               const FrameInfoBH& theFrameInfoBH);
  void init();
  void update(BodyDynamics& bodyDynamics, const JointDynamicsBH& jointDynamics);
  void update(BodyDynamicsDerivatives& bodyDynamicsDerivatives,
              const BodyDynamics& bodyDynamics, const JointDynamicsBH& jointDynamics);
  void invalidateBodyInSupportFootTransformations();

  /**
   * Returns the spatial transform from the coordinate system of the given body
   * to the support foot coordinate system.
   * If the spatial transform has not been calculated in this frame, it will
   * be calculated.
   */
  const SpatialTransform& getBodyInSupportFoot(const MassCalibrationBH::Limb limb, const BodyDynamics& bodyDynamics);

  /** Spatial transform from the support foot's origin to its projection on the ground. */
  const SpatialTransform supportFootInGround;
private:
  /**
   * Calculates the derivatives of the support foot force by the joint
   * motion of the support-leg-joint with offset 'jointOffset'.
   *
   * Note: This is only for joints of the support-leg as this walks
   * the kinematic tree (rooted at the support foot) upwards towards the torso.
   */
  void footForceDerivativeUpwards(const int jointOffset,
                                  SpatialVector<>& fcByqDoubleDotFoot,
                                  SpatialVector<>& fcByqDotFoot,
                                  SpatialVector<>& fcByqFoot,
                                  const BodyDynamics& bodyDynamics);

  /**
   * Calculates the derivatives of the support foot force by the joint
   * motion of the non-support-leg-joint with offset 'jointOffset'.
   *
   * Note: This is only for non-support-leg joints as this walk
   * the kinematic tree (rooted at the support foot) downwards towards the torso.
   *
   * @param base Limb next to the torso of the kinematic chain containing the
   *             joint indicated by the joint offset.
   * @param jointOffset Number of the joint in this kinematic chain. I.e. jointOffset = 0
   *                    denotes the joint connecting the torso with the base limb,
   *                    jointOffset = 1 denotes the joint connecting the base and base+1 limbs, and so on.
   */
  void footForceDerivativeDownwards(const MassCalibrationBH::Limb base,
                                    const int jointOffset,
                                    SpatialVector<>& fcByqDoubleDotFoot,
                                    SpatialVector<>& fcByqDotFoot,
                                    SpatialVector<>& fcByqFoot,
                                    const BodyDynamics& bodyDynamics);

  /**
   * Calculates one summand of the derivative of the foot-force-derivative by the acceleration
   * of joint 'hipYawPitch + jOffset'. The summand is respective to the derivative of the foot force by the
   * position of the joint 'hipYawPitch + iOffset'.
   *
   * The parameters are given as offsets to the support leg base joint (i.e. 0 means the
   * hip yaw/pitch joint.
   *
   * The summand returned is in support foot coordinates.
   */
  SpatialVector<> calculateVelocityDerivativeSummandUpwards(const int jOffset,
                                                            const int iOffset,
                                                            const BodyDynamics& bodyDynamics,
                                                            const JointDynamicsBH& jointDynamics);

  /**
   * Calculates one summand of the derivative of the foot-force-derivative by the acceleration
   * of joint 'hipYawPitch + jOffset'. The summand is respective to the derivative of the foot force by the
   * position of the joint 'baseJoint + iOffset'.
   *
   * The jOffset is an offset to the support leg hip yaw/pitch joint (i.e. 0 means the yaw/pitch joint itself)
   * and iOffset is an offset to the joint specified by 'baseJoint'. 'baseJoint' must be one of the joints
   * connecting a kinematic sub-chain to the torso, except the hip yaw/pitch joint of the support leg.
   * For that joint, use the method for the upwards direction.
   *
   * The summand returned is in support foot coordinates.
   */
  SpatialVector<> calculateVelocityDerivativeSummandDownwards(const int jOffset,
                                                              const JointDataBH::Joint baseJoint,
                                                              const MassCalibrationBH::Limb baseBody,
                                                              const int iOffset,
                                                              const BodyDynamics& bodyDynamics,
                                                              const JointDynamicsBH& jointDynamics);

  SpatialTransform bodyInSupportFoot[MassCalibrationBH::numOfLimbs];
  unsigned bodyInSupportFootTimestamp[MassCalibrationBH::numOfLimbs];
  enum {
    numOfLegJoints = 6,
    numOfArmJoints = 4,
  };


  /** Reference to the mass calibration of the nao. */
  const MassCalibrationBH& theMassCalibrationBH;
  /** The current torso matrix of the nao. */
  const TorsoMatrixBH& theTorsoMatrixBH;
  /** The dimensions of the nao. */
  const RobotDimensionsBH& theRobotDimensionsBH;
  /** The frame info. */
  const FrameInfoBH& theFrameInfoBH;
};
