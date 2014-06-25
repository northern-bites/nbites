/**
* @file MessageIDs.h
*
* Declaration of ids for debug messages.
*
* @author Martin Lötzsch
*/

#pragma once

#include "Tools/Enum.h"

/**
* IDs for debug messages
*
* To distinguish debug messages, they all have an id.
*/
ENUM(MessageID,
  undefined,
  idProcessBegin,
  idProcessFinished,

  // data (ids should remain constant over code changes, so old log files will still work)
  idImageBH,
  idJPEGImageBH,
  idJointDataBH,
  idSensorDataBH,
  idKeyStatesBH,
  idOdometryDataBH,
  idFrameInfoBH,
  idFilteredJointDataBH,
  idLinePerceptBH,
  idGoalPerceptBH,
  idBallPerceptBH,
  idGroundTruthBallModelBH,
  idGroundTruthRobotPoseBH,
  idObstacleSpotsBH,
  idCameraMatrixBH,
  idCameraInfoBH,
  idImageCoordinateSystemBH,
  idMotionInfoBH,
  idRobotPoseBH,
  idBallModelBH,
  idFilteredSensorDataBH,
  idImageInfoBH,
  idOrientationDataBH,
  idGameInfoBH,
  idRobotInfoBH,
  idOpponentTeamInfoBH,
  idSideConfidenceBH,
  idRobotsModelBH,
  idGroundTruthRobotsModelBH,
  idGroundTruthOdometryDataBH,
  idGroundTruthOrientationDataBH,
  idColorReferenceBH,
  idOwnTeamInfoBH,
  idObstacleModelBH,
  idBehaviorControlOutputBH,
  idCombinedWorldModelBH,
  idFieldBoundaryBH,
  idRobotHealthBH,
  idActivationGraphBH,
  idThumbnailBH,
  idRobotBalanceBH,
  idStopwatch,
  idExpRobotPerceptBH,
  idObstacleWheelBH,
  idBodyContourBH,
  // insert new data ids here

  numOfDataMessageIDs, /**< everything below this does not belong into log files */

  // ids used in team communication
  idNTPHeaderBH = numOfDataMessageIDs,
  idNTPIdentifierBH,
  idNTPRequestBH,
  idNTPResponseBH,
  idRobotBH,
  idTeamMateBallModelBH,
  idTeamMateObstacleModelBH,
  idTeamMateRobotPoseBH,
  idTeamMateSideConfidenceBH,
  idTeamMateBehaviorStatusBH,
  idMotionRequestBH,
  idTeamMateGoalPerceptBH,
  idTeamMateRobotsModelBH,
  idTeamMateFreePartOfOpponentGoalModelBH,
  idTeamMateIsPenalizedBH,
  idTeamMateHasGroundContactBH,
  idTeamMateIsUprightBH,
  idTeamMateCombinedWorldModelBH,
  idTeamHeadControlBH,
  idTeamMateTimeSinceLastGroundContactBH,
  idTeamCameraHeightBH,
  idTeamMateFieldCoverageBH,
  idObstacleClustersBH,
  // insert new team comm ids here

  // infrastructure
  idText,
  idDebugRequest,
  idDebugResponse,
  idDebugDataResponse,
  idDebugDataChangeRequest,
  idStreamSpecification,
  idModuleTable,
  idModuleRequest,
  idQueueFillRequest,
  idLogResponse,
  idDrawingManager,
  idDrawingManager3D,
  idDebugImage,
  idDebugJPEGImage,
  idDebugDrawing,
  idDebugDrawing3D,
  idMotionNet,
  idJointRequest,
  idLEDRequest,
  idPlot,
  idConsole,
  idRobotname,
  idRobotDimensions,
  idJointCalibration,
  idUSRequest,
  idWalkingEngineKick
);
