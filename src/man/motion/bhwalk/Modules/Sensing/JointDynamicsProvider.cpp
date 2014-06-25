/**
 * @file JointDynamicsProvider.cpp
 *
 * Implementation of a module which estimates the dynamics of all joints.
 */

#include "JointDynamicsProvider.h"
#include "Representations/Sensing/RobotModel.h"
#include "Representations/MotionControl/IndykickEngineOutput.h"
#include "Eigen/Geometry"

MAKE_MODULE(JointDynamicsProvider, Sensing)

JointDynamicsProvider::JointDynamicsProvider()
  : covariancesNeedUpdate(true),
    xAxis(1.0f, 0.0f, 0.0f), yAxis(0.0f, 1.0f, 0.0f), zAxis(0.0f, 0.0f, 1.0f),
    cycleTime(theFrameInfoBH.cycleTime)
{
  jointDynamicsInitialized = false;

  assembleTimeDependentMatrices();

  /* Calculate axes of the leg joints. */
  float sign = -1.0f; // Left leg.
  leftAxis[0] = Eigen::Quaternionf(Eigen::AngleAxisf(-pi_4 * sign, xAxis)) * zAxis * sign;
  leftAxis[1] = xAxis * sign;
  leftAxis[2] = yAxis;
  leftAxis[3] = yAxis;
  leftAxis[4] = yAxis;
  leftAxis[5] = xAxis * sign;

  sign = 1.0f; // Right leg.
  rightAxis[0] = Eigen::Quaternionf(Eigen::AngleAxisf(-pi_4 * sign, xAxis)) * zAxis * sign;
  rightAxis[1] = xAxis * sign;
  rightAxis[2] = yAxis;
  rightAxis[3] = yAxis;
  rightAxis[4] = yAxis;
  rightAxis[5] = xAxis * sign;

  /*
   * Eigen classes do not initialize their components by default.
   * Since they may contain random garbage, they're zeroed here.
   */
  for(int i = 0; i < numOfNonLegJoints; ++i)
  {
    model[i] = Eigen::Vector3f::Zero();
    stateNoLeg[i] = Eigen::Vector3f::Zero();
  }
  for(int i = 0; i < numOfLegJoints; ++i)
  {
    model[JointDataBH::LHipYawPitch + i] = Eigen::Vector3f::Zero();
    model[JointDataBH::RHipYawPitch + i] = Eigen::Vector3f::Zero();
  }
  stateLeftLeg = Eigen::Vector18f::Zero();
  stateRightLeg = Eigen::Vector18f::Zero();
}

void JointDynamicsProvider::assembleTimeDependentMatrices()
{
  const float cycleTimeMs = cycleTime * 1000.0f;
  /* Row major form! Yeeha! */
  smallA(0, 0) = 1.0f; smallA(0, 1) = cycleTimeMs; smallA(0, 2) = sqrBH(cycleTimeMs) / 2.0f;
  smallA(1, 0) = 0.0f; smallA(1, 1) = 1.0f;        smallA(1, 2) = cycleTimeMs;
  smallA(2, 0) = 0.0f; smallA(2, 1) = 0.0f;        smallA(2, 2) = 1.0f;

  smallAtranspose = smallA.transpose();

  A = Eigen::Matrix18f::Zero();
  for(int i = 0; i < numOfLegJoints; ++i)
  {
    const int index = i * 3;
    A.block<3, 3>(index, index) = smallA;
  }
  Atranspose = A.transpose();

  C = Eigen::Matrix6x18f::Zero();
  for(int i = 0; i < numOfLegJoints; ++i)
    C(i, i * 3) = 1.0f;
  Ctranspose = C.transpose();
}

void JointDynamicsProvider::assembleCovariances()
{
  smallCovarianceProcess = Eigen::Matrix3f::Zero();
  smallCovarianceProcess(0, 0) = sqrBH(positionStddev);
  smallCovarianceProcess(1, 1) = sqrBH(velocityStddev); //* 1e-1f;
  smallCovarianceProcess(2, 2) = sqrBH(accelerationStddev);

  covarianceProcess = Eigen::Matrix18f::Zero();
  for(int i = 0; i < numOfLegJoints; ++i)
  {
    const int index = 3 * i;
    covarianceProcess.block<3, 3>(index, index) = smallCovarianceProcess;
  }

  covarianceGyro = Eigen::Matrix2f::Zero();
  covarianceGyro(0, 0) = sqrBH(gyroStddev.x);
  covarianceGyro(1, 1) = sqrBH(gyroStddev.y);
}

void JointDynamicsProvider::update(JointDynamicsBH& jointDynamics)
{
  DECLARE_PLOT("module:JointDynamicsProvider:leftHipPitchPosition");
  DECLARE_PLOT("module:JointDynamicsProvider:leftHipPitchVelocity");
  DECLARE_PLOT("module:JointDynamicsProvider:leftHipPitchAcceleration");
  DECLARE_PLOT("module:JointDynamicsProvider:leftHipPitchPositionModel");
  DECLARE_PLOT("module:JointDynamicsProvider:leftHipPitchVelocityModel");
  DECLARE_PLOT("module:JointDynamicsProvider:gyroXMeasure");
  DECLARE_PLOT("module:JointDynamicsProvider:gyroXModel");
  DECLARE_PLOT("module:JointDynamicsProvider:gyroYMeasure");
  DECLARE_PLOT("module:JointDynamicsProvider:gyroYModel");
  DECLARE_PLOT("module:JointDynamicsProvider:supportAnkleRollDifference");

  if(theFrameInfoBH.cycleTime != cycleTime)
  {
    cycleTime = theFrameInfoBH.cycleTime;
    assembleTimeDependentMatrices();
  }

  /* Try to set the support leg. */
  if(theMotionRequestBH.motion == MotionRequestBH::indykick)
    jointDynamics.supportLeg = theMotionRequestBH.indykickRequest.supportLeg;
  else if(theMotionInfoBH.motion == MotionRequestBH::indykick)
    jointDynamics.supportLeg = theIndykickEngineOutputBH.executedIndykickRequest.supportLeg;
  else
    jointDynamics.supportLeg = IndykickRequest::unspecified;

  if(covariancesNeedUpdate)
  {
    assembleCovariances();
    covariancesNeedUpdate = false;
  }

  jointRequestsBuffer.add(theJointRequestBH);
  if(theMotionInfoBH.motion == MotionRequestBH::indykick && theIndykickEngineOutputBH.useGyroCorrection)
    for(int i = 0; i < JointDataBH::LAnkleRoll - JointDataBH::LHipYawPitch + 1; ++i)
    {
      JointRequestBH& request = jointRequestsBuffer[0];
      request.angles[JointDataBH::LHipYawPitch + i] = theIndykickEngineOutputBH.notGyroCorrectedLeftLegAngles[i];
      request.angles[JointDataBH::RHipYawPitch + i] = theIndykickEngineOutputBH.notGyroCorrectedRightLegAngles[i];
    }

  if(jointRequestsBuffer.getNumberOfEntries() < maxFrameDelay)
  {
    jointDynamicsInitialized = false;
  }
  else if(!jointDynamicsInitialized)
  {
    for(int i = 0; i < numOfNonLegJoints; ++i)
    {
      Eigen::Vector3f& s = stateNoLeg[i];
      for(int j = 0; j < 3; ++j)
        s[j] = 0.0f;
      covarianceNoLeg[i] = smallCovarianceProcess * 2.0f;
    }
    for(int i = 0; i < numOfLegJoints; ++i)
    {
      const int index = i * 3;
      for(int j = 0; j < 3; ++j)
      {
        stateLeftLeg[index + j] = 0.0f;
        stateRightLeg[index + j] = 0.0f;
      }
      covarianceLeftLeg = covarianceProcess * 2.0f;
      covarianceRightLeg = covarianceProcess * 2.0f;
    }
    jointDynamicsInitialized = true;
  }
  else
  {
    ASSERT(jointRequestsBuffer.getNumberOfEntries() >= maxFrameDelay);
    const JointRequestBH& next = jointRequestsBuffer[frameDelay];

    STOP_TIME_ON_REQUEST("module:JointDynamicsProvider:applyGyroMeasurementModel", applyGyroMeasurementModel(jointDynamics););
    STOP_TIME_ON_REQUEST("module:JointDynamicsProvider:updateModel", updateModel(next, model, velocity););
    STOP_TIME_ON_REQUEST("module:JointDynamicsProvider:predictNoLeg", predictNoLeg(stateNoLeg, covarianceNoLeg););
    STOP_TIME_ON_REQUEST("module:JointDynamicsProvider:predictLeg", predictLeg(stateLeftLeg, covarianceLeftLeg, stateRightLeg, covarianceRightLeg););

    STOP_TIME_ON_REQUEST("module:JointDynamicsProvider:correctNoLeg", correctNoLeg(next););
    STOP_TIME_ON_REQUEST("module:JointDynamicsProvider:correctLeg", correctLeg(next););
    PLOT("module:JointDynamicsProvider:supportAnkleRollDifference",
         toDegrees(jointDynamics.supportLeg == IndykickRequest::unspecified ? 0.0f :
                   (jointDynamics.supportLeg == IndykickRequest::left ? stateLeftLeg[5 * 3 + 0] : stateRightLeg[5 * 3 + 0])));
  }
  STOP_TIME_ON_REQUEST("module:JointDynamicsProvider:mergeStateAndModelToJointDynamics", mergeStateAndModelToJointDynamics(jointDynamics, model););

  // Plot motion of left hip joint.
  PLOT("module:JointDynamicsProvider:leftHipPitchPosition", toDegrees(jointDynamics.joint[JointDataBH::LHipPitch][0]));
  PLOT("module:JointDynamicsProvider:leftHipPitchVelocity", toDegrees(jointDynamics.joint[JointDataBH::LHipPitch][1] * 1000.0f));
  PLOT("module:JointDynamicsProvider:leftHipPitchAcceleration", toDegrees(jointDynamics.joint[JointDataBH::LHipPitch][2] * sqrBH(1000.0f)));
  PLOT("module:JointDynamicsProvider:leftHipPitchPositionModel", toDegrees(velocity[JointDataBH::LHipPitch].prediction));
  PLOT("module:JointDynamicsProvider:leftHipPitchVelocityModel", toDegrees(velocity[JointDataBH::LHipPitch].derivative * 1000.0f));

  PLOT("module:JointDynamicsProvider:gyroXMeasure", gyroMeasurementInDegreesPerSecond.x);
  PLOT("module:JointDynamicsProvider:gyroYMeasure", gyroMeasurementInDegreesPerSecond.y);
  PLOT("module:JointDynamicsProvider:gyroXModel", angularVelocityInDegreesPerSecond.x);
  PLOT("module:JointDynamicsProvider:gyroYModel", angularVelocityInDegreesPerSecond.y);
}

void JointDynamicsProvider::updateModel(const JointRequestBH& next, Eigen::Vector3f model[JointDataBH::numOfJoints],
                                        Differentiator velocity[JointDataBH::numOfJoints])
{
  for(int i = 0; i < JointDataBH::numOfJoints; ++i)
  {
    Eigen::Vector3f& m = model[i];
    Differentiator& vel = velocity[i];
    if(next.angles[i] != JointDataBH::off && next.angles[i] != JointDataBH::ignore)
    {
      const float angle = next.angles[i];
      vel.update(angle);
      m[0] = angle;
      m[1] = vel.derivative;
      m[2] = vel.derivative2;
    }
    else // Guess next model value for the off/ignored joint.
    {
      const float cycleTimeMs = cycleTime * 1000.0f; // TODO: Teach the differentiator differenct cycle times.
      const float nextAngle = m[0] + cycleTimeMs * m[1] + sqrBH(cycleTimeMs) / 2.0f * m[2];
      vel.update(nextAngle);
    }
  }
}

void JointDynamicsProvider::predictNoLeg(Eigen::Vector3f stateNoLeg[numOfNonLegJoints],
                                         Eigen::Matrix3f covarianceNoLeg[numOfNonLegJoints])
{
  for(int i = 0; i < numOfNonLegJoints; ++i)
  {
    Eigen::Vector3f& s = stateNoLeg[i];
    Eigen::Matrix3f& cov = covarianceNoLeg[i];
    s = smallA * s;
    s[0] = normalizeBH(s[0]);
    cov = smallA * cov * smallAtranspose + smallCovarianceProcess;
    symmetrize3(cov);
  }
}

void JointDynamicsProvider::predictLeg(Eigen::Vector18f& stateLeftLeg,
                                       Eigen::Matrix18f& covarianceLeftLeg,
                                       Eigen::Vector18f& stateRightLeg,
                                       Eigen::Matrix18f& covarianceRightLeg)
{
  STOP_TIME_ON_REQUEST("symmetrizeLeftLegCovariance", symmetrize18(covarianceLeftLeg););
  STOP_TIME_ON_REQUEST("symmetrizeRightLegCovariance", symmetrize18(covarianceRightLeg););

  STOP_TIME_ON_REQUEST("updateLeftLeg", stateLeftLeg = A * stateLeftLeg;);
  STOP_TIME_ON_REQUEST("updateLeftLegCovariance", covarianceLeftLeg = A * covarianceLeftLeg * Atranspose + covarianceProcess;);
  STOP_TIME_ON_REQUEST("updateRightLeg", stateRightLeg = A * stateRightLeg;);
  STOP_TIME_ON_REQUEST("updateRightLegCovariance", covarianceRightLeg = A * covarianceRightLeg * Atranspose + covarianceProcess;);

  /* Normalize the angles. */
  STOP_TIME_ON_REQUEST("normalizeStates",
  {
    for(int i = 0; i < numOfLegJoints; ++i)
    {
      const int index = i * 3;
      stateLeftLeg[index] = normalizeBH(stateLeftLeg[index]);
      stateRightLeg[index] = normalizeBH(stateRightLeg[index]);
    }
  });
}

void JointDynamicsProvider::correctNoLeg(const JointRequestBH& next)
{
  for(int i = 0; i < numOfNonLegJoints; ++i)
  {
    Eigen::Vector3f& s = stateNoLeg[i];
    Eigen::Matrix3f& cov = covarianceNoLeg[i];
    /* In the JointDataBH enum, the non-leg-joints appear before the left leg and right leg joints,
       so the next two statements are ok. */
    const float measurement = theFilteredJointDataBH.angles[i];
    Eigen::Vector3f& m = model[i];

    const float covZ = cov(0, 0) + measurementVariance;
    const Eigen::Vector3f kalmanGain = cov.col(0) / covZ;
    const float innovation = normalizeBH(measurement - m[0] - s[0]);

    s += kalmanGain * innovation;
    s[0] = normalizeBH(s[0]);
    cov -= kalmanGain * cov.row(0);
    symmetrize3(cov);
  }
}

void JointDynamicsProvider::correctLeg(const JointRequestBH& next)
{
  /* Correct left leg. */
  Eigen::Vector6f innovationLeft;
  Eigen::Vector6f innovationRight;
  for(int i = 0; i < numOfLegJoints; ++i)
  {
    const int jdLeftIndex = i + JointDataBH::LHipYawPitch;
    const int jdRightIndex = i + JointDataBH::RHipYawPitch;
    const int stateIndex = 3 * i;
    const float measurementLeft = theFilteredJointDataBH.angles[jdLeftIndex];
    const float measurementRight = theFilteredJointDataBH.angles[jdRightIndex];
    const float innovationILeft = measurementLeft - model[jdLeftIndex][0] - stateLeftLeg(stateIndex);
    const float innovationIRight = measurementRight - model[jdRightIndex][0] - stateRightLeg(stateIndex);
    innovationLeft(i) = normalizeBH(innovationILeft);
    innovationRight(i) = normalizeBH(innovationIRight);
  }

  correctLeg(stateLeftLeg, covarianceLeftLeg, innovationLeft);
  correctLeg(stateRightLeg, covarianceRightLeg, innovationRight);
}

void JointDynamicsProvider::correctLeg(Eigen::Vector18f& state, Eigen::Matrix18f& covariance,
                                       const Eigen::Vector6f& innovation)
{
  const Eigen::Matrix6x18f cTimesCovariance = C * covariance;
  Eigen::Matrix6f covZ = cTimesCovariance * Ctranspose;
  for(int i = 0; i < numOfLegJoints; ++i)
    covZ(i, i) += measurementVariance;
  const Eigen::Matrix18x6f kalmanGain = cTimesCovariance.transpose() * covZ.inverse();
  state += kalmanGain * innovation;
  covariance -= kalmanGain * cTimesCovariance;
  symmetrize18(covariance);
}

void JointDynamicsProvider::update(FutureJointDynamicsBH& futureJointDynamics)
{
  DECLARE_PLOT("module:JointDynamicsProvider:leftHipPitchFuturePosition");

  futureJointDynamics.supportLeg = theJointDynamicsBH.supportLeg;

  /* Work on copies of model, differentiators, and distribution parameters */
  Differentiator velocityCopies[JointDataBH::numOfJoints];
  for(int i = 0; i < JointDataBH::numOfJoints; ++i)
  {
    velocityCopies[i] = velocity[i];
  }
  Eigen::Vector3f stateNoLegCopies[numOfNonLegJoints];
  Eigen::Matrix3f covarianceNoLegCopies[numOfNonLegJoints];
  for(int i = 0; i < numOfNonLegJoints; ++i)
  {
    stateNoLegCopies[i] = stateNoLeg[i];
    covarianceNoLegCopies[i] = covarianceNoLeg[i];
  }
  Eigen::Vector18f stateLeftLegCopy = stateLeftLeg;
  Eigen::Matrix18f covarianceLeftLegCopy = covarianceLeftLeg;
  Eigen::Vector18f stateRightLegCopy = stateRightLeg;
  Eigen::Matrix18f covarianceRightLegCopy = covarianceRightLeg;

  const int frameDelay = 4;
  for(int i = frameDelay - 1; i >= 0; --i)
  {
    const JointRequestBH& jr = jointRequestsBuffer.getEntry(i);
    /* Update velocity differentiators */
    for(int j = 0; j < JointDataBH::numOfJoints; ++j)
      velocityCopies[j].update(jr.angles[j]);
    /* Predict non leg joints. */
    predictNoLeg(stateNoLegCopies, covarianceNoLegCopies);
    /* Predict left and right leg joints. */
    predictLeg(stateLeftLegCopy, covarianceLeftLegCopy, stateRightLegCopy, covarianceRightLegCopy);
  }

  /* Get model from velocity differentiator copies. */
  Eigen::Vector3f modelCopies[JointDataBH::numOfJoints];
  for(int j = 0; j < JointDataBH::numOfJoints; ++j)
  {
    Eigen::Vector3f& m = modelCopies[j];
    const Differentiator& v = velocityCopies[j];
    m[0] = jointRequestsBuffer.getEntry(0).angles[j];
    m[1] = v.derivative;
    m[2] = v.derivative2;
  }

  mergeStateAndModelToJointDynamics(futureJointDynamics, model /* modelCopies */);

  PLOT("module:JointDynamicsProvider:leftHipPitchFuturePosition", toDegrees(futureJointDynamics.joint[JointDataBH::LHipPitch][0]));
}

void JointDynamicsProvider::mergeStateAndModelToJointDynamics(JointDynamicsBH& jointDynamics,
                                                              Eigen::Vector3f model[JointDataBH::numOfJoints]) const
{
  /* Joint dynamics for non leg joints. */
  for(int i = 0; i < numOfNonLegJoints; ++i)
  {
    const Eigen::Vector3f& m = model[i];
    const Eigen::Vector3f& s = stateNoLeg[i];
    Vector3f& j = jointDynamics.joint[i];
    j[0] = m[0] + s[0];
    j[1] = m[1] + s[1];
    j[2] = m[2] + s[2];
  }

  /* Joint dynamics for leg joints. */
  for(int i = 0; i < numOfLegJoints; ++i)
  {
    const JointDataBH::Joint left0 = JointDataBH::LHipYawPitch;
    const JointDataBH::Joint right0 = JointDataBH::RHipYawPitch;

    Vector3f& leftJoint = jointDynamics.joint[left0 + i];
    Vector3f& rightJoint = jointDynamics.joint[right0 + i];

    const Eigen::Vector3f& leftModel = model[left0 + i];
    const Eigen::Vector3f& rightModel = model[right0 + i];

    const Eigen::Vector3f leftState = stateLeftLeg.segment<3>(i * 3);
    const Eigen::Vector3f rightState = stateRightLeg.segment<3>(i * 3);

    leftJoint[0] = leftModel[0] + leftState[0];
    leftJoint[1] = leftModel[1] + leftState[1];
    leftJoint[2] = leftModel[2] + leftState[2];
    rightJoint[0] = rightModel[0] + rightState[0];
    rightJoint[1] = rightModel[1] + rightState[1];
    rightJoint[2] = rightModel[2] + rightState[2];
  }
}

void JointDynamicsProvider::applyGyroMeasurementModel(JointDynamicsBH& jointDynamics)
{
  if(jointDynamics.supportLeg == IndykickRequest::unspecified
     || !theInertiaSensorDataBH.calibrated
     || theInertiaSensorDataBH.gyro.x == InertiaSensorDataBH::off
     || theInertiaSensorDataBH.gyro.y == InertiaSensorDataBH::off)
    return;

  const float sign = jointDynamics.supportLeg == IndykickRequest::left ? -1.0f : 1.0f;
  const JointDataBH::Joint leg0 = jointDynamics.supportLeg == IndykickRequest::left
    ? JointDataBH::LHipYawPitch : JointDataBH::RHipYawPitch;
  Eigen::Matrix18f& covarianceSupportLeg = jointDynamics.supportLeg == IndykickRequest::left
    ? covarianceLeftLeg : covarianceRightLeg;
  Eigen::Vector18f& stateSupportLeg = jointDynamics.supportLeg == IndykickRequest::left
    ? stateLeftLeg : stateRightLeg;

  Eigen::Vector3f axis[numOfLegJoints];
  axis[0] = Eigen::Quaternionf(Eigen::AngleAxisf(-pi_4 * sign, xAxis)) * zAxis * sign;
  axis[1] = xAxis * sign;
  axis[2] = yAxis;
  axis[3] = yAxis;
  axis[4] = yAxis;
  axis[5] = xAxis * sign;

  Eigen::Quaternionf rotation[numOfLegJoints];
  rotation[0] = Eigen::AngleAxisf(stateSupportLeg[0] + model[leg0 + 0][0], axis[0]);
  for(int i = 1; i < numOfLegJoints; ++i)
    rotation[i] = rotation[i - 1] * Eigen::Quaternionf(Eigen::AngleAxisf(stateSupportLeg[3 * i + 0] + model[leg0 + i][0], axis[i]));

  /* Sum up torso rotation velocity. */
  Eigen::Vector3f torsoAngularVelocity = -axis[0] * (stateSupportLeg[1] + model[leg0 + 0][1]);
  for(int i = 1; i < numOfLegJoints; ++i)
    torsoAngularVelocity -= rotation[i] * axis[i] * (stateSupportLeg[3 * i + 1] + model[leg0 + i][1]);

  angularVelocityInDegreesPerSecond.x = toDegrees(torsoAngularVelocity(0) * 1000.0f);
  angularVelocityInDegreesPerSecond.y = toDegrees(torsoAngularVelocity(1) * 1000.0f);
  gyroMeasurementInDegreesPerSecond.x = toDegrees(theInertiaSensorDataBH.gyro.x);
  gyroMeasurementInDegreesPerSecond.y = toDegrees(theInertiaSensorDataBH.gyro.y);

  Eigen::Matrix2x18f C = Eigen::Matrix2x18f::Zero();
  for(int i = 0; i < numOfLegJoints; ++i)
  {
    const Eigen::Vector3f s = -(rotation[i] * axis[i]);
    const int column = 3 * i + 1;
    C(0, column) = s(0);
    C(1, column) = s(1);
  }
  const Eigen::Matrix18x2f Ctranspose = C.transpose();
  const Eigen::Matrix18x2f covCtranspose = covarianceSupportLeg * Ctranspose;
  const Eigen::Vector2f innovation(theInertiaSensorDataBH.gyro.x / 1000.0f - torsoAngularVelocity(0),
                                   theInertiaSensorDataBH.gyro.y / 1000.0f - torsoAngularVelocity(1));
  const Eigen::Matrix2f covZ = C * covCtranspose + covarianceGyro;
  const Eigen::Matrix18x2f kalmanGain = covCtranspose * covZ.inverse();
  stateSupportLeg += kalmanGain * innovation;
  covarianceSupportLeg -= kalmanGain * covCtranspose.transpose();
  symmetrize18(covarianceSupportLeg);
}

void JointDynamicsProvider::symmetrize3(Eigen::Matrix3f& matrix)
{
  const int n = 3;
  for(int i = 0; i < n - 1; ++i)
    for(int j = i + 1; j < n; ++j)
    {
      const float t = (matrix(i, j) + matrix(j, i)) / 2;
      if(std::abs(t) < 1e-25f)
      {
        matrix(i, j) = 0.0f;
        matrix(j, i) = 0.0f;
      }
      else
      {
        matrix(i, j) = t;
        matrix(j, i) = t;
      }
    }
}

void JointDynamicsProvider::symmetrize18(Eigen::Matrix18f& matrix)
{
  const int n = 18;
  for(int i = 0; i < n - 1; ++i)
    for(int j = i + 1; j < n; ++j)
    {
      const float t = (matrix(i, j) + matrix(j, i)) / 2;
      if(std::abs(t) < 1e-25f)
      {
        matrix(i, j) = 0.0f;
        matrix(j, i) = 0.0f;
      }
      else
      {
        matrix(i, j) = t;
        matrix(j, i) = t;
      }
    }
}

void JointDynamicsProvider::serialize(In* in, Out* out)
{
  const float currentPositionStddev = positionStddev;
  const float currentVelocityStddev = velocityStddev;
  const float currentAccelerationStddev = accelerationStddev;
  const float currentGyroStddevX = gyroStddev.x;
  const float currentGyroStddevY = gyroStddev.y;

  // Convert all angles to degrees before streaming.
  positionStddev = toDegrees(positionStddev);
  velocityStddev = toDegrees(velocityStddev);
  accelerationStddev = toDegrees(accelerationStddev);
  gyroStddev.x = toDegrees(gyroStddev.x);
  gyroStddev.y = toDegrees(gyroStddev.y);

  STREAM_REGISTER_BEGIN
  STREAM_BASE(JointDynamicsProviderBase)
  STREAM_REGISTER_FINISH

  // Convert all angles from degrees to randians after streaming.
  positionStddev = fromDegrees(positionStddev);
  velocityStddev = fromDegrees(velocityStddev);
  accelerationStddev = fromDegrees(accelerationStddev);
  gyroStddev.x = fromDegrees(gyroStddev.x);
  gyroStddev.y = fromDegrees(gyroStddev.y);

  covariancesNeedUpdate |= currentPositionStddev != positionStddev
    || currentVelocityStddev != velocityStddev
    || currentAccelerationStddev != accelerationStddev
    || currentGyroStddevX != gyroStddev.x
    || currentGyroStddevY != gyroStddev.y;
}
