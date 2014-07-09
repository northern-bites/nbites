/**
* @file CameraMatrix.h
* Declaration of CameraMatrixBH and RobotCameraMatrixBH representation.
* @author Colin Graf
*/

#pragma once

#include "Tools/Math/Pose3D.h"
#include "Representations/Configuration/RobotDimensions.h"
#include "Representations/Configuration/CameraCalibration.h"

/**
* Matrix describing transformation from center of hip to camera.
*/
class RobotCameraMatrixBH : public Pose3DBH
{
public:
  /** Draws the camera matrix. */
  void draw() const;

  void computeRobotCameraMatrix(const RobotDimensionsBH& robotDimensions, float headYaw, float headPitch, const CameraCalibrationBH& cameraCalibration, bool upperCamera);
  RobotCameraMatrixBH() {}
  RobotCameraMatrixBH(const RobotDimensionsBH& robotDimensions, const float headYaw, const float headPitch, const CameraCalibrationBH& cameraCalibration, bool upperCamera);
};

/**
* Matrix describing transformation from ground (center between booth feet) to camera.
*/
STREAMABLE_WITH_BASE(CameraMatrixBH, Pose3DBH,
{
public:
  /** Kind of copy-constructor.
  * @param pose The other pose.
  */
  CameraMatrixBH(const Pose3DBH& pose);

  void computeCameraMatrix(const Pose3DBH& torsoMatrix, const Pose3DBH& robotCameraMatrix, const CameraCalibrationBH& cameraCalibration);
  CameraMatrixBH(const Pose3DBH& torsoMatrix, const Pose3DBH& robotCameraMatrix, const CameraCalibrationBH& cameraCalibration);

  /** Draws the camera matrix. */
  void draw() const,

  (bool)(true) isValid, /**< Matrix is only valid if motion was stable. */
});

class CameraMatrixOther : public CameraMatrixBH {};
class RobotCameraMatrixOther : public RobotCameraMatrixBH {};
