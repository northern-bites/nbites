/**
* @file Modules/MotionControl/MotionCombinator.h
* This file declares a module that combines the motions created by the different modules.
* @author <A href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</A>
*/

#pragma once

#include "Tools/ModuleBH/Module.h"
#include "Representations/Configuration/RobotDimensions.h"
#include "Representations/Infrastructure/JointData.h"
#include "Representations/Sensing/FallDownState.h"
#include "Representations/MotionControl/ArmMotionEngineOutput.h"
#include "Representations/MotionControl/MotionSelection.h"
#include "Representations/MotionControl/SpecialActionsOutput.h"
#include "Representations/MotionControl/WalkingEngineOutput.h"
#include "Representations/MotionControl/BikeEngineOutput.h"
#include "Representations/MotionControl/IndykickEngineOutput.h"
#include "Representations/MotionControl/GetUpEngineOutput.h"
#include "Representations/MotionControl/BallTakingOutput.h"
#include "Representations/MotionControl/OdometryData.h"
#include "Representations/MotionControl/MotionInfo.h"
#include "Representations/MotionControl/HeadJointRequest.h"

MODULE(MotionCombinator)
  REQUIRES(FilteredJointDataBH)
  REQUIRES(FallDownStateBH)
  REQUIRES(MotionSelectionBH)
  REQUIRES(WalkingEngineOutputBH)
  REQUIRES(BikeEngineOutputBH)
  REQUIRES(IndykickEngineOutputBH)
  REQUIRES(SpecialActionsOutputBH)
  REQUIRES(WalkingEngineStandOutputBH)
  REQUIRES(GetUpEngineOutputBH)
  REQUIRES(BallTakingOutputBH)
  REQUIRES(HeadJointRequestBH)
  REQUIRES(HardnessSettingsBH)
  REQUIRES(ArmMotionEngineOutputBH)
  PROVIDES_WITH_MODIFY_AND_OUTPUT(OdometryDataBH)
  PROVIDES_WITH_MODIFY_AND_OUTPUT(JointRequestBH)
  PROVIDES_WITH_MODIFY(MotionInfoBH)
  LOADS_PARAMETER(bool, emergencyOffEnabled)
  LOADS_PARAMETER(unsigned, recoveryTime) /**< The number of frames to interpolate after emergency-stop. */
END_MODULE

class MotionCombinator : public MotionCombinatorBase
{
private:
  JointDataBH lastJointData; /**< The measured joint angles the last time when not interpolating. */
  OdometryDataBH odometryData; /**< The odometry data. */
  MotionInfoBH motionInfo; /**< Information about the motion currently executed. */
  Pose2DBH specialActionOdometry; /**< workaround for accumulating special action odometry*/

  void update(OdometryDataBH& odometryData);
  void update(JointRequestBH& jointRequest);
  void update(MotionInfoBH& motionInfo) {motionInfo = this->motionInfo;}

  void saveFall(JointRequestBH& JointRequestBH);
  void centerHead(JointRequestBH& JointRequestBH);
  unsigned currentRecoveryTime;

  bool headJawInSavePosition;
  bool headPitchInSavePosition;

#ifndef RELEASE
  OdometryDataBH lastOdometryData;
  JointRequestBH lastJointRequest;
#endif

  /**
  * The method copies all joint angles from one joint request to another,
  * but only those that should not be ignored.
  * @param source The source joint request. All angles != JointDataBH::ignore will be copied.
  * @param target The target joint request.
  */
  void copy(const JointRequestBH& source, JointRequestBH& target) const;

  /**
  * The method interpolates between two joint requests.
  * @param from The first source joint request. This one is the starting point.
  * @param to The second source joint request. This one has to be reached over time.
  * @param fromRatio The ratio of "from" in the target joint request.
  * @param target The target joint request.
  * @param interpolateHardness Whether to interpolate hardness.
  */
  void interpolate(const JointRequestBH& from, const JointRequestBH& to, float fromRatio, JointRequestBH& target, bool interpolateHardness) const;

public:
  /**
  * Default constructor.
  */
  MotionCombinator();
};
