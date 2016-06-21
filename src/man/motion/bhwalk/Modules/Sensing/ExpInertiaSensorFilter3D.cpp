/**
* @file ExpInertiaSensorFilter3D.cpp
* Implementation of module ExpInertiaSensorFilter3D.
* @author Colin Graf
*/

#include "ExpInertiaSensorFilter3D.h"
#include "Tools/Debugging/DebugDrawings.h" // PLOT
#include "Representations/MotionControl/MotionInfo.h"
#include "Representations/MotionControl/WalkingEngineOutput.h"
#include <algorithm>

using namespace std;

MAKE_MODULE(ExpInertiaSensorFilter3D, Sensing)

ExpInertiaSensorFilter3D::ExpInertiaSensorFilter3D() : lastTime(0)
{
}

void ExpInertiaSensorFilter3D::init()
{
  gyroNoiseSqr.x = sqrBH(gyroNoise.x);
  gyroNoiseSqr.y = sqrBH(gyroNoise.y);

  relativeKinematicsNoiseSqr.x = sqrBH(relativeKinematicsNoise.x);
  relativeKinematicsNoiseSqr.y = sqrBH(relativeKinematicsNoise.y);
  relativeKinematicsNoiseSqr.z = sqrBH(relativeKinematicsNoise.z);

  horizontalProcessNoiseSqr.x = sqrBH(horizontalProcessNoise.x);
  horizontalProcessNoiseSqr.y = sqrBH(horizontalProcessNoise.y);
  horizontalProcessNoiseSqr.z = sqrBH(horizontalProcessNoise.z);

  accSensorCov.c[0].x = sqrBH(accNoise.x);
  accSensorCov.c[1].y = sqrBH(accNoise.y);
  accSensorCov.c[2].z = sqrBH(accNoise.z);

  angleSensorCov.c[0].x = sqrBH(absoluteKinematicsNoise.x);
  angleSensorCov.c[1].y = sqrBH(absoluteKinematicsNoise.y);
}

void ExpInertiaSensorFilter3D::reset()
{
  x = State();
  cov.c[0].x = sqrBH(absoluteKinematicsNoise.x * 10.f);
  cov.c[1].y = sqrBH(absoluteKinematicsNoise.y * 10.f);

  lastLeftFoot = lastRightFoot = Pose3DBH();
  lastTime = theFrameInfoBH.time - (unsigned int)(theFrameInfoBH.cycleTime * 1000.f);
}

void ExpInertiaSensorFilter3D::update(OrientationDataBH& orientationData)
{
  init();
  DECLARE_PLOT("module:InertiaSensorFilter:orientationX");
  DECLARE_PLOT("module:InertiaSensorFilter:orientationY");
  DECLARE_PLOT("module:InertiaSensorFilter:velocityX");
  DECLARE_PLOT("module:InertiaSensorFilter:velocityY");
  DECLARE_PLOT("module:InertiaSensorFilter:velocityZ");

  // check whether the filter shall be reset
  if(!lastTime || theFrameInfoBH.time <= lastTime)
  {
    if(theFrameInfoBH.time == lastTime)
      return; // weird log file replaying?
    reset();
  }

  // get foot positions
  Pose3DBH leftFoot(theRobotModelBH.limbs[MassCalibrationBH::footLeft]);
  Pose3DBH rightFoot(theRobotModelBH.limbs[MassCalibrationBH::footRight]);
  leftFoot.translate(0.f, 0.f, -theRobotDimensions.heightLeg5Joint);
  rightFoot.translate(0.f, 0.f, -theRobotDimensions.heightLeg5Joint);
  const Pose3DBH leftFootInvert(leftFoot.invert());
  const Pose3DBH rightFootInvert(rightFoot.invert());

  // calculate rotation and position offset using the robot model (joint data)
  const Pose3DBH leftOffset(lastLeftFoot.translation.z != 0.f ? Pose3DBH(lastLeftFoot).conc(leftFootInvert) : Pose3DBH());
  const Pose3DBH rightOffset(lastRightFoot.translation.z != 0.f ? Pose3DBH(lastRightFoot).conc(rightFootInvert) : Pose3DBH());

  // detect the foot that is on ground
  const bool useLeft = leftFootInvert.translation.z > rightFootInvert.translation.z;

  // calculate velocity
  const float timeScale = 1.f / theFrameInfoBH.cycleTime;
  Vector3BH<> velocity = useLeft ? leftOffset.translation : rightOffset.translation;
  velocity *= timeScale * 0.001f; // => m/s

  // upright?
  if(theInertiaSensorDataBH.acc.x != InertiaSensorDataBH::off)
    safeRawAngle = Vector2f(theSensorDataBH.data[SensorDataBH::angleX], theSensorDataBH.data[SensorDataBH::angleY]);
  bool isUpright = abs(safeRawAngle.x) < uprightThreshold.x && abs(safeRawAngle.y) < uprightThreshold.y;

  // update the filter
  if(isUpright)
  {
    if(theInertiaSensorDataBH.gyro.x != InertiaSensorDataBH::off)
    {
      Vector3BH<> gyroVec(atan(theInertiaSensorDataBH.gyro.x * theFrameInfoBH.cycleTime), atan(theInertiaSensorDataBH.gyro.y * theFrameInfoBH.cycleTime), 0);
      const RotationMatrixBH offset = RotationMatrixBH::fromRotationZ(getRotationZ(useLeft ? leftOffset.rotation :  rightOffset.rotation)) * RotationMatrixBH(gyroVec, sqrt(sqrBH(gyroVec.x) + sqrBH(gyroVec.y)));
      processUpdate(&offset, Vector3f(gyroNoiseSqr.x, gyroNoiseSqr.y, relativeKinematicsNoiseSqr.z));
    }
    else
      processUpdate(useLeft ? &leftOffset.rotation :  &rightOffset.rotation, relativeKinematicsNoiseSqr);
  }
  else
  {
    processUpdate(0, horizontalProcessNoiseSqr);
  }

  // insert calculated rotation
  if(isUpright)
  {
    const RotationMatrixBH& usedRotation(useLeft ? leftFootInvert.rotation : rightFootInvert.rotation);
    Vector2f computedAngle(atan2(usedRotation.c1.z, usedRotation.c2.z), atan2(-usedRotation.c0.z, usedRotation.c2.z));
    angleReadingUpdate(computedAngle);
  }
  else // insert acceleration sensor values
  {
    if(theInertiaSensorDataBH.acc.x != InertiaSensorDataBH::off)
      accReadingUpdate(Vector3f(theInertiaSensorDataBH.acc.x, theInertiaSensorDataBH.acc.y, theInertiaSensorDataBH.acc.z));
  }

  // fill the representation
  orientationData.rotation = x.rotation;
  orientationData.velocity = velocity;

  // store some data for the next iteration
  lastLeftFoot = leftFoot;
  lastRightFoot = rightFoot;
  lastTime = theFrameInfoBH.time;

  // plots
  PLOT("module:ExpInertiaSensorFilter3D:orientationX", toDegrees(atan2(x.rotation.c1.z, x.rotation.c2.z)));
  PLOT("module:ExpInertiaSensorFilter3D:orientationY", toDegrees(atan2(-x.rotation.c0.z, x.rotation.c2.z)));
  PLOT("module:ExpInertiaSensorFilter3D:velocityX", orientationData.velocity.x);
  PLOT("module:ExpInertiaSensorFilter3D:velocityY", orientationData.velocity.y);
  PLOT("module:ExpInertiaSensorFilter3D:velocityZ", orientationData.velocity.z);
}

void ExpInertiaSensorFilter3D::processUpdate(const RotationMatrixBH* rotationOffset, const Vector3f& noiseSqr)
{
  if(rotationOffset)
  {
    generateSigmaPoints();

    // update sigma points
      for(int i = 0; i < 5; ++i)
        sigmaPoints[i].rotation = getRotationW(sigmaPoints[i].rotation * (*rotationOffset));

    // get new mean and cov
    meanOfSigmaPoints();
    covOfSigmaPoints();
  }

  // add process noise
  Matrix3x3BH<> processCov(Vector3BH<>(noiseSqr.x, 0.f, 0.f), Vector3BH<>(0.f, noiseSqr.y, 0.f), Vector3BH<>(0.f, 0.f, noiseSqr.z));
  Matrix3x3BH<> noise = x.rotation * processCov * x.rotation.transpose();
  cov[0].x += noise[0].x;
  cov[0].y += noise[0].y;
  cov[1].x += noise[1].x;
  cov[1].y += noise[1].y;
}

void ExpInertiaSensorFilter3D::accReadingModel(const State& sigmaPoint, Vector3f& reading) const
{
  reading = Vector3f(sigmaPoint.rotation.c0.z, sigmaPoint.rotation.c1.z, sigmaPoint.rotation.c2.z);
  reading *= -9.80665f;
}

void ExpInertiaSensorFilter3D::accReadingUpdate(const Vector3f& reading)
{
  generateSigmaPoints();

  for(int i = 0; i < 5; ++i)
    accReadingModel(sigmaPoints[i], sigmaAccReadings[i]);

  meanOfSigmaAccReadings();

  covOfSigmaAccReadingsAndSigmaPoints();
  covOfSigmaAccReadings();

  const Matrix2x3f kalmanGain = accReadingsSigmaPointsCov.transpose() * (accReadingsCov + accSensorCov).invert();
  const Vector2f innovation = kalmanGain * (reading - accReadingMean);
  x += innovation;
  cov -= kalmanGain * accReadingsSigmaPointsCov;
}

void ExpInertiaSensorFilter3D::angleReadingModel(const State& sigmaPoint, Vector2f& reading) const
{
  reading = Vector2f(atan2(sigmaPoint.rotation.c1.z, sigmaPoint.rotation.c2.z), atan2(-sigmaPoint.rotation.c0.z, sigmaPoint.rotation.c2.z));
}

void ExpInertiaSensorFilter3D::angleReadingUpdate(const Vector2f& reading)
{
  generateSigmaPoints();

  for(int i = 0; i < 5; ++i)
    angleReadingModel(sigmaPoints[i], sigmaAngleReadings[i]);

  meanOfSigmaAngleReadings();
  covOfSigmaAngleReadingsAndSigmaPoints();
  covOfSigmaAngleReadings();

  const Matrix2x2f kalmanGain = angleReadingsSigmaPointsCov.transpose() * (angleReadingsCov + angleSensorCov).invert();
  const Vector2f innovation = kalmanGain * (reading - angleReadingMean);
  x += innovation;
  cov -= kalmanGain * angleReadingsSigmaPointsCov;
}

void ExpInertiaSensorFilter3D::generateSigmaPoints()
{
  cholOfCov();
  sigmaPoints[0] = x;
  sigmaPoints[1] = x + l.c[0];
  sigmaPoints[2] = x + (-l.c[0]);
  sigmaPoints[3] = x + l.c[1];
  sigmaPoints[4] = x + (-l.c[1]);
}

void ExpInertiaSensorFilter3D::meanOfSigmaPoints()
{
  x = sigmaPoints[0];
  //for(int i = 0; i < 5; ++i) // ~= 0 .. inf
  for(int i = 0; i < 1; ++i)
  {
    Vector2f chunk((sigmaPoints[0] - x) +
                    ((sigmaPoints[1] - x) + (sigmaPoints[2] - x)) +
                    ((sigmaPoints[3] - x) + (sigmaPoints[4] - x)));
    chunk *= (1.f / 5.f);
    x += chunk;
  }
}

void ExpInertiaSensorFilter3D::covOfSigmaPoints()
{
  cov = tensor(sigmaPoints[0] - x) +
        (tensor(sigmaPoints[1] - x) + tensor(sigmaPoints[2] - x)) +
        (tensor(sigmaPoints[3] - x) + tensor(sigmaPoints[4] - x));
  cov *= 0.5f;
}

ExpInertiaSensorFilter3D::State ExpInertiaSensorFilter3D::State::operator+(const Vector2f& value) const
{
  return State(*this) += value;
}

ExpInertiaSensorFilter3D::State& ExpInertiaSensorFilter3D::State::operator+=(const Vector2f& value)
{
  //rotation *= RotationMatrixBH(rotation.invert() * Vector3BH<>(value.x, value.y, 0.f));
  rotation = getRotationW(RotationMatrixBH(Vector3BH<>(value.x, value.y, 0.f)) * rotation);
  return *this;
}

Vector2f ExpInertiaSensorFilter3D::State::operator-(const State& other) const
{
  //Vector3BH<> angleAxis = other.rotation * ((const RotationMatrixBH&)(other.rotation.invert() * rotation)).getAngleAxis();
  //ASSERT(fabs(angleAxis.z) < 0.001f);
  Vector3BH<> angleAxis = getRotationW(rotation * other.rotation.invert()).getAngleAxis();
  return Vector2f(angleAxis.x, angleAxis.y);
}

void ExpInertiaSensorFilter3D::cholOfCov()
{
  // improved symmetry input
  const float a11 = cov.c[0].x;
  const float a21 = (cov.c[0].y + cov.c[1].x) * 0.5f;

  const float a22 = cov.c[1].y;

  // output
  float& l11(l.c[0].x);
  float& l21(l.c[0].y);

  float& l22(l.c[1].y);

  // compute cholesky decomposition

  //ASSERT(a11 >= 0.f);
  l11 = sqrt(std::max<>(a11, 0.f));
  if(l11 == 0.f) l11 = 0.0000000001f;
  l21 = a21 / l11;

  //ASSERT(a22 - l21 * l21 >= 0.f);
  l22 = sqrt(std::max<>(a22 - l21 * l21, 0.f));
  if(l22 == 0.f) l22 = 0.0000000001f;

#if 0
  const float test_a11 = l11 * l11;
  const float test_a21 = l21 * l11;

  const float test_a22 = l21 * l21 + l22 * l22;

  ASSERT(fabs(test_a11 - a11) < 0.01f);
  ASSERT(fabs(test_a21 - a21) < 0.01f);

  ASSERT(fabs(test_a22 - a22) < 0.01f);
#endif
}

void ExpInertiaSensorFilter3D::meanOfSigmaAccReadings()
{
  accReadingMean = sigmaAccReadings[0];
  //for(int i = 0; i < 5; ++i) // ~= 0 .. inf
  for(int i = 0; i < 1; ++i)
  {
    Vector3f chunk((sigmaAccReadings[0] - accReadingMean) +
                    ((sigmaAccReadings[1] - accReadingMean) + (sigmaAccReadings[2] - accReadingMean)) +
                    ((sigmaAccReadings[3] - accReadingMean) + (sigmaAccReadings[4] - accReadingMean)));
    chunk *= (1.f / 5.f);
    accReadingMean += chunk;
  }
}

void ExpInertiaSensorFilter3D::covOfSigmaAccReadingsAndSigmaPoints()
{
  accReadingsSigmaPointsCov = (
    (tensor(sigmaAccReadings[1] - accReadingMean, l.c[0]) + tensor(sigmaAccReadings[2] - accReadingMean, -l.c[0])) +
    (tensor(sigmaAccReadings[3] - accReadingMean, l.c[1]) + tensor(sigmaAccReadings[4] - accReadingMean, -l.c[1])));
  accReadingsSigmaPointsCov *= 0.5f;
}

void ExpInertiaSensorFilter3D::covOfSigmaAccReadings()
{
  accReadingsCov = (tensor(sigmaAccReadings[0] - accReadingMean) +
                 (tensor(sigmaAccReadings[1] - accReadingMean) + tensor(sigmaAccReadings[2] - accReadingMean)) +
                 (tensor(sigmaAccReadings[3] - accReadingMean) + tensor(sigmaAccReadings[4] - accReadingMean)));
  accReadingsCov *= 0.5f;
}

void ExpInertiaSensorFilter3D::meanOfSigmaAngleReadings()
{
  angleReadingMean = sigmaAngleReadings[0];
  //for(int i = 0; i < 5; ++i) // ~= 0 .. inf
  for(int i = 0; i < 1; ++i)
  {
    Vector2f chunk((sigmaAngleReadings[0] - angleReadingMean) +
                    ((sigmaAngleReadings[1] - angleReadingMean) + (sigmaAngleReadings[2] - angleReadingMean)) +
                    ((sigmaAngleReadings[3] - angleReadingMean) + (sigmaAngleReadings[4] - angleReadingMean)));
    chunk *= (1.f / 5.f);
    angleReadingMean += chunk;
  }
}

void ExpInertiaSensorFilter3D::covOfSigmaAngleReadingsAndSigmaPoints()
{
  angleReadingsSigmaPointsCov = (
    (tensor(sigmaAngleReadings[1] - angleReadingMean, l.c[0]) + tensor(sigmaAngleReadings[2] - angleReadingMean, -l.c[0])) +
    (tensor(sigmaAngleReadings[3] - angleReadingMean, l.c[1]) + tensor(sigmaAngleReadings[4] - angleReadingMean, -l.c[1])));
  angleReadingsSigmaPointsCov *= 0.5f;
}

void ExpInertiaSensorFilter3D::covOfSigmaAngleReadings()
{
  angleReadingsCov = (tensor(sigmaAngleReadings[0] - angleReadingMean) +
                 (tensor(sigmaAngleReadings[1] - angleReadingMean) + tensor(sigmaAngleReadings[2] - angleReadingMean)) +
                 (tensor(sigmaAngleReadings[3] - angleReadingMean) + tensor(sigmaAngleReadings[4] - angleReadingMean)));
  angleReadingsCov *= 0.5f;
}

float ExpInertiaSensorFilter3D::getRotationZ(const RotationMatrixBH& rot3d)
{
  /*
  Vector3BH<> d = Vector3BH<>(-rot3d[0][2], -rot3d[1][2], rot3d[2][2]);
  Vector3BH<> g = Vector3BH<>(0, 0, 1.f) ^ d;
  float w = atan2(sqrt(d.x * d.x + d.y * d.y), d.z);
  RotationMatrixBH withoutZ(g, w);
  RotationMatrixBH zOnly = rot3d * withoutZ.invert();
  return Matrix2x2f(Vector2f(zOnly.c0.x, zOnly.c0.y), Vector2f(zOnly.c1.x, zOnly.c1.y));
  */

  // (this is an optimized version of the code above)
  float x = rot3d[1][2], y = -rot3d[0][2];
  const float z = rot3d[2][2];
  const float gLenSqr = x * x + y * y;
  const float gLen = sqrt(gLenSqr);
  const float wLen = sqrt(gLenSqr + z * z);
  if(gLen != 0.f)
  {
    x /= gLen;
    y /= gLen;
  }
  const float si = -gLen / wLen, co = z / wLen;
  const float v = 1 - co;
  const float d0x = x * x * v + co;
  const float d0y = x * y * v;
  const float d0z = -y * si;
  const float c0x = rot3d[0][0] * d0x + rot3d[1][0] * d0y + rot3d[2][0] * d0z;
  const float c0y = rot3d[0][1] * d0x + rot3d[1][1] * d0y + rot3d[2][1] * d0z;
  return atan2(c0y, c0x);
}

RotationMatrixBH ExpInertiaSensorFilter3D::getRotationW(const RotationMatrixBH& rot)
{
  Vector3BH<> d = Vector3BH<>(-rot[0][2], -rot[1][2], rot[2][2]);
  Vector3BH<> g(-d.y, d.x, 0.f);
  //ASSERT(g == (Vector3BH<>(0, 0, 1.f) ^ d));
  float w = atan2(sqrt(d.x * d.x + d.y * d.y), d.z);
  return RotationMatrixBH(g, w);
}
