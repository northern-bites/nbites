/**
* @file Blackboard.h
* Declaration of a class representing the blackboard.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#pragma once

#include <cstddef>
#include "Platform/Linux/SystemCall.h"

// Declare prototypes of all representations here:
// Infrastructure
class ButtonInterfaceBH;
class JointDataBH;
class JointRequestBH;
class SensorDataBH;
class KeyStatesBH;
class LEDRequestBH;
class ImageBH;
class CameraInfoBH;
class FrameInfoBH;
class CognitionFrameInfoBH;
class RobotInfoBH;
class OwnTeamInfoBH;
class OpponentTeamInfoBH;
class GameInfoBH;
class TeamMateDataBH;
class MotionRobotHealthBH;
class RobotHealthBH;
class TeamDataSenderOutputBH;
class USRequestBH;
class ThumbnailBH;

// Configuration
class CameraSettingsBH;
class FieldDimensionsBH;
class RobotDimensionsBH;
class JointCalibrationBH;
class SensorCalibrationBH;
class CameraCalibrationBH;
class MassCalibrationBH;
class HardnessSettingsBH;
class DamageConfigurationBH;
class HeadLimitsBH;

// Perception
class CameraMatrixBH;
class RobotCameraMatrixBH;
class ImageCoordinateSystemBH;
class BallSpotsBH;
class LineSpotsBH;
class PossibleObstacleSpotsBH;
class BallPerceptBH;
class LinePerceptBH;
class RegionPerceptBH;
class GoalPerceptBH;
class GroundContactStateBH;
class BodyContourBH;
class ColorReferenceBH;
class FieldBoundaryBH;
class ObstacleSpotsBH;

// Modeling
class ArmContactModelBH;
class FallDownStateBH;
class BallModelBH;
class CombinedWorldModelBH;
class GroundTruthBallModelBH;
class ObstacleModelBH;
class USObstacleModelBH;
class RobotPoseBH;
class FilteredRobotPoseBH;
class FootContactModelBH;
class GroundTruthRobotPoseBH;
class RobotsModelBH;
class GroundTruthRobotsModelBH;
class FreePartOfOpponentGoalModelBH;
class FieldCoverageBH;
class GlobalFieldCoverageBH;
class SideConfidenceBH;
class OdometerBH;
class OwnSideModelBH;
class ObstacleWheelBH;
class ObstacleClustersBH;

// BehaviorControl
class ActivationGraphBH;
class BehaviorControlOutputBH;
class BehaviorLEDRequestBH;
class PathBH;

// Sensing
class FilteredJointDataBH;
class FilteredSensorDataBH;
class InertiaSensorDataBH;
class OrientationDataBH;
class GroundTruthOrientationDataBH;
class TorsoMatrixBH;
class RobotModelBH;
class JointDynamicsBH;
class FutureJointDynamicsBH;
class RobotBalanceBH;
class FsrDataBH;
class FsrZmpBH;

// MotionControl
class ArmMotionEngineOutputBH;
class ArmMotionRequestBH;
class OdometryDataBH;
class GroundTruthOdometryDataBH;
class GetUpEngineOutputBH;
class MotionRequestBH;
class HeadMotionRequestBH;
class HeadAngleRequestBH;
class HeadJointRequestBH;
class MotionSelectionBH;
class SpecialActionsOutputBH;
class WalkingEngineOutputBH;
class WalkingEngineStandOutputBH;
class BikeEngineOutputBH;
class MotionInfoBH;
class BallTakingOutputBH;
class IndykickEngineOutputBH;
class KickEngineOutput;

// friends
class Process;
class Cognition;
class Motion;
class Framework;
class CognitionLogger;

/**
* @class Blackboard
* The class represents the blackboard that contains all representation.
* Note: The blackboard only contains references to the objects as attributes.
* The actual representations are constructed on the heap, because many copies of
* of the blackboard exist but only a single set of the representations shared
* by all instances.
*/
class Blackboard
{
public:
  // Add all representations as constant references here:
  // Infrastructure
  ButtonInterfaceBH& theButtonInterfaceBH;
  JointDataBH& theJointDataBH;
  JointRequestBH& theJointRequestBH;
  SensorDataBH& theSensorDataBH;
  KeyStatesBH& theKeyStatesBH;
  LEDRequestBH& theLEDRequestBH;
  ImageBH& theImageBH;
  CameraInfoBH& theCameraInfoBH;
  FrameInfoBH& theFrameInfoBH;
  CognitionFrameInfoBH& theCognitionFrameInfoBH;
  RobotInfoBH& theRobotInfoBH;
  OwnTeamInfoBH& theOwnTeamInfoBH;
  OpponentTeamInfoBH& theOpponentTeamInfoBH;
  GameInfoBH& theGameInfoBH;
  TeamMateDataBH& theTeamMateDataBH;
  MotionRobotHealthBH& theMotionRobotHealthBH;
  RobotHealthBH& theRobotHealthBH;
  TeamDataSenderOutputBH& theTeamDataSenderOutputBH;
  USRequestBH& theUSRequestBH;
  ThumbnailBH& theThumbnailBH;

  // Configuration
  CameraSettingsBH& theCameraSettingsBH;
  FieldDimensionsBH& theFieldDimensionsBH;
  RobotDimensionsBH& theRobotDimensionsBH;
  JointCalibrationBH& theJointCalibrationBH;
  SensorCalibrationBH& theSensorCalibrationBH;
  CameraCalibrationBH& theCameraCalibrationBH;
  MassCalibrationBH& theMassCalibrationBH;
  HardnessSettingsBH& theHardnessSettingsBH;
  DamageConfigurationBH& theDamageConfigurationBH;
  HeadLimitsBH& theHeadLimitsBH;

  // Perception
  CameraMatrixBH& theCameraMatrixBH;
  RobotCameraMatrixBH& theRobotCameraMatrixBH;
  ImageCoordinateSystemBH& theImageCoordinateSystemBH;
  BallSpotsBH& theBallSpotsBH;
  LineSpotsBH& theLineSpotsBH;
  PossibleObstacleSpotsBH& thePossibleObstacleSpotsBH;
  BallPerceptBH& theBallPerceptBH;
  LinePerceptBH& theLinePerceptBH;
  RegionPerceptBH& theRegionPerceptBH;
  GoalPerceptBH& theGoalPerceptBH;
  GroundContactStateBH& theGroundContactStateBH;
  BodyContourBH& theBodyContourBH;
  ColorReferenceBH& theColorReferenceBH;
  FieldBoundaryBH& theFieldBoundaryBH;
  ObstacleSpotsBH& theObstacleSpotsBH;

  // Modeling
  ArmContactModelBH& theArmContactModelBH;
  FallDownStateBH& theFallDownStateBH;
  BallModelBH& theBallModelBH;
  CombinedWorldModelBH& theCombinedWorldModelBH;
  GroundTruthBallModelBH& theGroundTruthBallModelBH;
  ObstacleModelBH& theObstacleModelBH;
  USObstacleModelBH& theUSObstacleModelBH;
  RobotPoseBH& theRobotPoseBH;
  FilteredRobotPoseBH& theFilteredRobotPoseBH;
  FootContactModelBH& theFootContactModelBH;
  GroundTruthRobotPoseBH& theGroundTruthRobotPoseBH;
  RobotsModelBH& theRobotsModelBH;
  GroundTruthRobotsModelBH& theGroundTruthRobotsModelBH;
  FreePartOfOpponentGoalModelBH& theFreePartOfOpponentGoalModelBH;
  FieldCoverageBH& theFieldCoverageBH;
  GlobalFieldCoverageBH& theGlobalFieldCoverageBH;
  SideConfidenceBH& theSideConfidenceBH;
  OdometerBH& theOdometerBH;
  OwnSideModelBH& theOwnSideModelBH;
  ObstacleWheelBH& theObstacleWheelBH;
  ObstacleClustersBH& theObstacleClustersBH;

  // BehaviorControl
  ActivationGraphBH& theActivationGraphBH;
  BehaviorControlOutputBH& theBehaviorControlOutputBH;
  BehaviorLEDRequestBH& theBehaviorLEDRequestBH;
  PathBH& thePathBH;

  // Sensing
  FilteredJointDataBH& theFilteredJointDataBH;
  FilteredSensorDataBH& theFilteredSensorDataBH;
  InertiaSensorDataBH& theInertiaSensorDataBH;
  OrientationDataBH& theOrientationDataBH;
  GroundTruthOrientationDataBH& theGroundTruthOrientationDataBH;
  TorsoMatrixBH& theTorsoMatrixBH;
  RobotModelBH& theRobotModelBH;
  JointDynamicsBH& theJointDynamicsBH;
  FutureJointDynamicsBH& theFutureJointDynamicsBH;
  RobotBalanceBH& theRobotBalanceBH;
  FsrDataBH& theFsrDataBH;
  FsrZmpBH& theFsrZmpBH;

  // MotionControl
  ArmMotionEngineOutputBH& theArmMotionEngineOutputBH;
  ArmMotionRequestBH& theArmMotionRequestBH;
  OdometryDataBH& theOdometryDataBH;
  GroundTruthOdometryDataBH& theGroundTruthOdometryDataBH;
  GetUpEngineOutputBH& theGetUpEngineOutputBH;
  MotionRequestBH& theMotionRequestBH;
  HeadAngleRequestBH& theHeadAngleRequestBH;
  HeadMotionRequestBH& theHeadMotionRequestBH;
  HeadJointRequestBH& theHeadJointRequestBH;
  MotionSelectionBH& theMotionSelectionBH;
  SpecialActionsOutputBH& theSpecialActionsOutputBH;
  WalkingEngineOutputBH& theWalkingEngineOutputBH;
  WalkingEngineStandOutputBH& theWalkingEngineStandOutputBH;
  MotionInfoBH& theMotionInfoBH;
  KickEngineOutput& theKickEngineOutput;

  static PROCESS_WIDE_STORAGE(Blackboard) theInstance; /**< The only real instance in the current process. */

  /**
  * The method is a dummy that is called to prevent the compiler from certain
  * optimizations in a method generated in Module.h.
  * It is empty, but important, not defined inline.
  */
  static void distract();

public:
  /**
  * Default constructor.
  */
  Blackboard();

public:
  /**
  * Virtual destructor.
  * Required for derivations of this class.
  */
  virtual ~Blackboard() {}

  /**
  * Assignment operator.
  * Note: copies will share all representations.
  * @param other The instance that is cloned.
  */
  void operator=(const Blackboard& other);

  /**
  * The operator allocates a memory block that is zeroed.
  * Therefore, all members of this class are initialized with 0.
  * @attention This operator is only called if this class is instantiated by
  * a separate call to new, i.e. it cannot be created as a part of another class.
  * @param size The size of the block in bytes.
  * @return A pointer to the block.
  */
  static void* operator new(std::size_t);

  /**
  * The operator frees a memory block.
  * @param p The address of the block to free.
  */
  static void operator delete(void* p);

  friend class Process; /**< The class Process can set theInstance. */
  friend class Cognition; /**< The class Cognition can read theInstance. */
  friend class Motion; /**< The class Motion can read theInstance. */
  friend class Framework; /**< The class Framework can set theInstance. */
  friend class CognitionLogger; /**< The cogniton logger needs to read theInstance */
  friend class WalkingEngine; 
};
