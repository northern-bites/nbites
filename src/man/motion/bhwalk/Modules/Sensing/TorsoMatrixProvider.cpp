/**
* @file TorsoMatrixProvider.cpp
* Implementation of module TorsoMatrixProvider.
* @author Colin Graf
*/

#include "TorsoMatrixProvider.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "DebugConfig.h"
#include <iostream>
#include <string>

MAKE_MODULE(TorsoMatrixProvider, Sensing)

PROCESS_WIDE_STORAGE(TorsoMatrixProvider) TorsoMatrixProvider::theInstance = 0;

void TorsoMatrixProvider::update(TorsoMatrixBH& torsoMatrix)
{
  // remove the z-rotation from the orientation data rotation matrix
  Vector3BH<> g(theOrientationDataBH.rotation.c1.z, -theOrientationDataBH.rotation.c0.z, 0.f);
  float w = std::atan2(std::sqrt(g.x * g.x + g.y * g.y), theOrientationDataBH.rotation.c2.z);
  RotationMatrixBH torsoRotation(g, w);

  // calculate "center of hip" position from left foot
  Pose3DBH fromLeftFoot(torsoRotation);
  fromLeftFoot.conc(theRobotModelBH.limbs[MassCalibrationBH::footLeft]);
  fromLeftFoot.translate(0, 0, -theRobotDimensionsBH.heightLeg5Joint);
  fromLeftFoot.translation *= -1.;
  fromLeftFoot.rotation = torsoRotation;

  // calculate "center of hip" position from right foot
  Pose3DBH fromRightFoot(torsoRotation);
  fromRightFoot.conc(theRobotModelBH.limbs[MassCalibrationBH::footRight]);
  fromRightFoot.translate(0, 0, -theRobotDimensionsBH.heightLeg5Joint);
  fromRightFoot.translation *= -1.;
  fromRightFoot.rotation = torsoRotation;

  // get foot z-rotations
  const Pose3DBH& leftFootInverse(theRobotModelBH.limbs[MassCalibrationBH::footLeft].invert());
  const Pose3DBH& rightFootInverse(theRobotModelBH.limbs[MassCalibrationBH::footRight].invert());
  const float leftFootZRotation = leftFootInverse.rotation.getZAngle();
  const float rightFootZRotation = rightFootInverse.rotation.getZAngle();

  // determine used foot
  const bool useLeft = fromLeftFoot.translation.z > fromRightFoot.translation.z;
  torsoMatrix.leftSupportFoot = useLeft;

  // calculate foot span
  const Vector3BH<> newFootSpan(fromRightFoot.translation - fromLeftFoot.translation);

  // and construct the matrix
  Pose3DBH newTorsoMatrix;
  newTorsoMatrix.translate(newFootSpan.x / (useLeft ? 2.f : -2.f), newFootSpan.y / (useLeft ? 2.f : -2.f), 0);
  newTorsoMatrix.conc(useLeft ? fromLeftFoot : fromRightFoot);

  // calculate torso offset
  if(torsoMatrix.translation.z != 0) // the last torso matrix should be valid
  {
    Pose3DBH& torsoOffset(torsoMatrix.offset);
    torsoOffset = torsoMatrix.invert();
    torsoOffset.translate(lastFootSpan.x / (useLeft ? 2.f : -2.f), lastFootSpan.y / (useLeft ? 2.f : -2.f), 0);
    torsoOffset.rotateZ(useLeft ? float(leftFootZRotation - lastLeftFootZRotation) : float(rightFootZRotation - lastRightFootZRotation));
    torsoOffset.translate(newFootSpan.x / (useLeft ? -2.f : 2.f), newFootSpan.y / (useLeft ? -2.f : 2.f), 0);
    torsoOffset.conc(newTorsoMatrix);
  }

  // adopt new matrix and footSpan
  (Pose3DBH&)torsoMatrix = newTorsoMatrix;
  lastLeftFootZRotation = leftFootZRotation;
  lastRightFootZRotation = rightFootZRotation;
  lastFootSpan = newFootSpan;

  // valid?
  torsoMatrix.isValid = theGroundContactStateBH.contact;

  //
  PLOT("module:TorsoMatrixProvider:footSpanX", newFootSpan.x);
  PLOT("module:TorsoMatrixProvider:footSpanY", newFootSpan.y);
  PLOT("module:TorsoMatrixProvider:footSpanZ", newFootSpan.z);

  PLOT("module:TorsoMatrixProvider:torsoMatrixX", torsoMatrix.translation.x);
  PLOT("module:TorsoMatrixProvider:torsoMatrixY", torsoMatrix.translation.y);
  PLOT("module:TorsoMatrixProvider:torsoMatrixZ", torsoMatrix.translation.z);
}

/*
void TorsoMatrixProvider::update(FilteredOdometryOffset& odometryOffset)
{
  Pose2DBH odometryOffset;

  if(lastTorsoMatrix.translation.z != 0.)
  {
    Pose3DBH odometryOffset3D(lastTorsoMatrix);
    odometryOffset3D.conc(theTorsoMatrixBH.offset);
    odometryOffset3D.conc(theTorsoMatrixBH.invert());

    odometryOffset.translation.x = odometryOffset3D.translation.x;
    odometryOffset.translation.y = odometryOffset3D.translation.y;
    odometryOffset.rotation = odometryOffset3D.rotation.getZAngle();
  }

  PLOT("module:TorsoMatrixProvider:odometryOffsetX", odometryOffset.translation.x);
  PLOT("module:TorsoMatrixProvider:odometryOffsetY", odometryOffset.translation.y);
  PLOT("module:TorsoMatrixProvider:odometryOffsetRotation", toDegrees(odometryOffset.rotation));

  (Pose3DBH&)lastTorsoMatrix = theTorsoMatrixBH;
}
*/

void TorsoMatrixProvider::update(OdometryDataBH& odometryData)
{
  Pose2DBH odometryOffset;

  if(lastTorsoMatrix.translation.z != 0.)
  {
    Pose3DBH odometryOffset3D(lastTorsoMatrix);
    odometryOffset3D.conc(theTorsoMatrixBH.offset);
    odometryOffset3D.conc(theTorsoMatrixBH.invert());

    odometryOffset.translation.x = odometryOffset3D.translation.x;
    odometryOffset.translation.y = odometryOffset3D.translation.y;

#ifdef V5_ROBOT
    // Calculate odometryOffset.rotation from the filtered Z-axis gyro data
    float angleZ = -theSensorDataBH.data[SensorDataBH::angleZ];
    odometryOffset.rotation = angleZ - lastAngleZ;
    lastAngleZ = angleZ;
#else
    odometryOffset.rotation = odometryOffset3D.rotation.getZAngle();
#endif
  }

  PLOT("module:TorsoMatrixProvider:odometryOffsetX", odometryOffset.translation.x);
  PLOT("module:TorsoMatrixProvider:odometryOffsetY", odometryOffset.translation.y);
  PLOT("module:TorsoMatrixProvider:odometryOffsetRotation", toDegrees(odometryOffset.rotation));

  odometryData += odometryOffset;

  (Pose3DBH&)lastTorsoMatrix = theTorsoMatrixBH;
}

