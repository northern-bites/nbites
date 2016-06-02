/**
* @file Blackboard.cpp
* Implementation of a class representing the blackboard.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#include "Blackboard.h"
#include <cstring>
#include <cstdlib>

// Infrastructure
#include "Representations/Infrastructure/JointData.h"
#include "Representations/Infrastructure/SensorData.h"
#include "Representations/Infrastructure/KeyStates.h"
#include "Representations/Infrastructure/LEDRequest.h"
#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/CameraInfo.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/RobotInfo.h"
#include "Representations/Infrastructure/TeamInfo.h"
#include "Representations/Infrastructure/GameInfo.h"
#include "Representations/Infrastructure/TeamMateData.h"
#include "Representations/Infrastructure/RobotHealth.h"
#include "Representations/Infrastructure/USRequest.h"
#include "Representations/Infrastructure/RoboCupGameControlData.h"
#include "Representations/Infrastructure/Thumbnail.h"

//Configuration
#include "Representations/Configuration/CameraSettings.h"
#include "Representations/Configuration/FieldDimensions.h"
#include "Representations/Configuration/RobotDimensions.h"
#include "Representations/Configuration/JointCalibration.h"
#include "Representations/Configuration/SensorCalibration.h"
#include "Representations/Configuration/CameraCalibration.h"
#include "Representations/Configuration/MassCalibration.h"
#include "Representations/Configuration/DamageConfiguration.h"
#include "Representations/Configuration/HeadLimits.h"

// Perception
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/ImageCoordinateSystem.h"
#include "Representations/Perception/BallSpots.h"
#include "Representations/Perception/LineSpots.h"
#include "Representations/Perception/PossibleObstacleSpots.h"
#include "Representations/Perception/BallPercept.h"
#include "Representations/Perception/LinePercept.h"
#include "Representations/Perception/RegionPercept.h"
#include "Representations/Perception/GoalPercept.h"
#include "Representations/Perception/BodyContour.h"
#include "Representations/Perception/ColorReference.h"
#include "Representations/Perception/FieldBoundary.h"
#include "Representations/Perception/ObstacleSpots.h"

// Modeling
#include "Representations/Modeling/FallDownState.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Modeling/CombinedWorldModel.h"
#include "Representations/Modeling/ObstacleModel.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/RobotsModel.h"
#include "Representations/Modeling/FreePartOfOpponentGoalModel.h"
#include "Representations/Modeling/FieldCoverage.h"
#include "Representations/Modeling/GlobalFieldCoverage.h"
#include "Representations/Modeling/SideConfidence.h"
#include "Representations/Modeling/Odometer.h"
#include "Representations/Modeling/OwnSideModel.h"
#include "Representations/Modeling/ObstacleWheel.h"
#include "Representations/Modeling/ObstacleClusters.h"

// BehaviorControl
#include "Representations/BehaviorControl/ActivationGraph.h"
#include "Representations/BehaviorControl/BehaviorControlOutput.h"
#include "Representations/BehaviorControl/BehaviorLEDRequest.h"

// Sensing
#include "Representations/Sensing/GroundContactState.h"
#include "Representations/Sensing/InertiaSensorData.h"
#include "Representations/Sensing/OrientationData.h"
#include "Representations/Sensing/TorsoMatrix.h"
#include "Representations/Sensing/RobotModel.h"
#include "Representations/Sensing/JointDynamics.h"
#include "Representations/Sensing/RobotBalance.h"
#include "Representations/Sensing/FsrData.h"
#include "Representations/Sensing/FsrZmp.h"

// MotionControl
#include "Representations/MotionControl/ArmMotionEngineOutput.h"
#include "Representations/MotionControl/ArmMotionRequest.h"
#include "Representations/MotionControl/OdometryData.h"
#include "Representations/MotionControl/GetUpEngineOutput.h"
#include "Representations/MotionControl/MotionRequest.h"
#include "Representations/MotionControl/HeadAngleRequest.h"
#include "Representations/MotionControl/HeadMotionRequest.h"
#include "Representations/MotionControl/HeadJointRequest.h"
#include "Representations/MotionControl/MotionSelection.h"
#include "Representations/MotionControl/SpecialActionsOutput.h"
#include "Representations/MotionControl/WalkingEngineOutput.h"
#include "Representations/MotionControl/BikeEngineOutput.h"
#include "Representations/MotionControl/MotionInfo.h"
#include "Representations/MotionControl/BallTakingOutput.h"
#include "Representations/MotionControl/IndykickEngineOutput.h"
#include "Representations/MotionControl/KickEngineOutput.h"

Blackboard::Blackboard() :
// Initialize all representations by themselves:
// Infrastructure
  theButtonInterfaceBH(theButtonInterfaceBH),
  theJointDataBH(*new JointDataBH),
  theJointRequestBH(*new JointRequestBH),
  theSensorDataBH(*new SensorDataBH),
  theKeyStatesBH(*new KeyStatesBH),
  theLEDRequestBH(*new LEDRequestBH),
  theImageBH(*new ImageBH),
  theCameraInfoBH(*new CameraInfoBH),
  theFrameInfoBH(*new FrameInfoBH),
  theCognitionFrameInfoBH(*new CognitionFrameInfoBH),
  theRobotInfoBH(*new RobotInfoBH),
  theOwnTeamInfoBH(*new OwnTeamInfoBH),
  theOpponentTeamInfoBH(*new OpponentTeamInfoBH),
  theGameInfoBH(*new GameInfoBH),
  theTeamMateDataBH(*new TeamMateDataBH),
  theMotionRobotHealthBH(*new MotionRobotHealthBH),
  theRobotHealthBH(*new RobotHealthBH),
  theTeamDataSenderOutputBH(*new TeamDataSenderOutputBH),
  theUSRequestBH(*new USRequestBH),
  theThumbnailBH(*new ThumbnailBH),

// Configuration
  theCameraSettingsBH(*new CameraSettingsBH),
  theFieldDimensionsBH(*new FieldDimensionsBH),
  theRobotDimensionsBH(*new RobotDimensionsBH),
  theJointCalibrationBH(*new JointCalibrationBH),
  theSensorCalibrationBH(*new SensorCalibrationBH),
  theCameraCalibrationBH(*new CameraCalibrationBH),
  theMassCalibrationBH(*new MassCalibrationBH),
  theHardnessSettingsBH(*new HardnessSettingsBH),
  theDamageConfigurationBH(*new DamageConfigurationBH),
  theHeadLimitsBH(*new HeadLimitsBH),

// Perception
  theCameraMatrixBH(*new CameraMatrixBH),
  theRobotCameraMatrixBH(*new RobotCameraMatrixBH),
  theImageCoordinateSystemBH(*new ImageCoordinateSystemBH),
  theBallSpotsBH(*new BallSpotsBH),
  theLineSpotsBH(*new LineSpotsBH),
  thePossibleObstacleSpotsBH(*new PossibleObstacleSpotsBH),
  theBallPerceptBH(*new BallPerceptBH),
  theLinePerceptBH(*new LinePerceptBH),
  theRegionPerceptBH(*new RegionPerceptBH),
  theGoalPerceptBH(*new GoalPerceptBH),
  theGroundContactStateBH(*new GroundContactStateBH),
  theBodyContourBH(*new BodyContourBH),
  theColorReferenceBH(*new ColorReferenceBH),
  theFieldBoundaryBH(*new FieldBoundaryBH),
  theObstacleSpotsBH(*new ObstacleSpotsBH),

// Modeling
  theArmContactModelBH(theArmContactModelBH),
  theFallDownStateBH(*new FallDownStateBH),
  theBallModelBH(*new BallModelBH),
  theCombinedWorldModelBH(*new CombinedWorldModelBH),
  theGroundTruthBallModelBH(*new GroundTruthBallModelBH),
  theObstacleModelBH(*new ObstacleModelBH),
  theUSObstacleModelBH(*new USObstacleModelBH),
  theRobotPoseBH(*new RobotPoseBH),
  theFilteredRobotPoseBH(theFilteredRobotPoseBH),
  theFootContactModelBH(theFootContactModelBH),
  theGroundTruthRobotPoseBH(*new GroundTruthRobotPoseBH),
  theRobotsModelBH(*new RobotsModelBH),
  theGroundTruthRobotsModelBH(*new GroundTruthRobotsModelBH),
  theFreePartOfOpponentGoalModelBH(*new FreePartOfOpponentGoalModelBH),
  theFieldCoverageBH(*new FieldCoverageBH),
  theGlobalFieldCoverageBH(*new GlobalFieldCoverageBH),
  theSideConfidenceBH(*new SideConfidenceBH),
  theOdometerBH(*new OdometerBH),
  theOwnSideModelBH(*new OwnSideModelBH),
  theObstacleWheelBH(*new ObstacleWheelBH),
  theObstacleClustersBH(*new ObstacleClustersBH),

// BehaviorControl
  theActivationGraphBH(*new ActivationGraphBH),
  theBehaviorControlOutputBH(*new BehaviorControlOutputBH),
  theBehaviorLEDRequestBH(*new BehaviorLEDRequestBH),
  thePathBH(thePathBH),

// Sensing
  theFilteredJointDataBH(*new FilteredJointDataBH),
  theFilteredSensorDataBH(*new FilteredSensorDataBH),
  theInertiaSensorDataBH(*new InertiaSensorDataBH),
  theOrientationDataBH(*new OrientationDataBH),
  theGroundTruthOrientationDataBH(*new GroundTruthOrientationDataBH),
  theTorsoMatrixBH(*new TorsoMatrixBH),
  theRobotModelBH(*new RobotModelBH),
  theJointDynamicsBH(*new JointDynamicsBH),
  theFutureJointDynamicsBH(*new FutureJointDynamicsBH),
  theRobotBalanceBH(*new RobotBalanceBH),
  theFsrDataBH(*new FsrDataBH),
  theFsrZmpBH(*new FsrZmpBH),

// MotionControl
  theArmMotionEngineOutputBH(*new ArmMotionEngineOutputBH),
  theArmMotionRequestBH(*new ArmMotionRequestBH),
  theOdometryDataBH(*new OdometryDataBH),
  theGroundTruthOdometryDataBH(*new GroundTruthOdometryDataBH),
  theGetUpEngineOutputBH(*new GetUpEngineOutputBH),
  theMotionRequestBH(*new MotionRequestBH),
  theHeadAngleRequestBH(*new HeadAngleRequestBH),
  theHeadMotionRequestBH(*new HeadMotionRequestBH),
  theHeadJointRequestBH(*new HeadJointRequestBH),
  theMotionSelectionBH(*new MotionSelectionBH),
  theSpecialActionsOutputBH(*new SpecialActionsOutputBH),
  theWalkingEngineOutputBH(*new WalkingEngineOutputBH),
  theWalkingEngineStandOutputBH(*new WalkingEngineStandOutputBH),
  theMotionInfoBH(*new MotionInfoBH),
  theKickEngineOutput(*new KickEngineOutput)
{
    theInstance = this;
}

void Blackboard::operator=(const Blackboard& other)
{
  memcpy(this, &other, sizeof(Blackboard));
}

void* Blackboard::operator new(std::size_t size)
{
  return calloc(1, size);
}

void Blackboard::operator delete(void* p)
{
  return free(p);
}

void Blackboard::distract()
{
}

PROCESS_WIDE_STORAGE(Blackboard) Blackboard::theInstance = 0;
