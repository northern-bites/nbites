/**
* @file InertiaSensorCalibrator.h
* Declaration of module InertiaSensorCalibrator.
* @author Colin Graf
*/

#pragma once

#include "Tools/Module/Module.h"
#include "Tools/RingBuffer.h"
#include "Tools/RingBufferWithSum.h"
#include "Tools/Math/Kalman.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/GameInfo.h"
#include "Representations/Infrastructure/RobotInfo.h"
#include "Representations/Sensing/InertiaSensorData.h"
#include "Representations/Sensing/RobotModel.h"
#include "Representations/Sensing/GroundContactState.h"
#include "Representations/MotionControl/MotionSelection.h"
#include "Representations/MotionControl/MotionInfo.h"
#include "Representations/MotionControl/WalkingEngineOutput.h"
#include "Representations/Configuration/JointCalibration.h"
#include "Representations/Configuration/SensorCalibration.h"

MODULE(InertiaSensorCalibrator)
  REQUIRES(SensorDataBH)
  REQUIRES(RobotModelBH)
  REQUIRES(FrameInfoBH)
  REQUIRES(RobotInfoBH)
  REQUIRES(GameInfoBH)
  REQUIRES(GroundContactStateBH)
  REQUIRES(JointCalibrationBH)
  REQUIRES(SensorCalibrationBH)
  USES(MotionSelectionBH)
  USES(MotionInfoBH)
  USES(WalkingEngineOutputBH)
  PROVIDES_WITH_MODIFY(InertiaSensorDataBH)
  DEFINES_PARAMETER(unsigned, timeFrame, 1500) /**< The time frame within unstable situations lead to dropping averaged gyro and acceleration measurements. (in ms) */
  DEFINES_PARAMETER(unsigned, penalizedTimeFrame, 400) /**< Time frame expansion after receiving a penalty or before getting unpanlized (in ms) */
  DEFINES_PARAMETER(Vector2BH<>, gyroBiasProcessNoise, Vector2BH<>(0.05f, 0.05f)) /**< The process noise of the gyro offset estimator. */
  DEFINES_PARAMETER(Vector2BH<>, gyroBiasStandMeasurementNoise, Vector2BH<>(0.01f, 0.01f)) /**< The noise of gyro measurements and the gyro offset while standing. */
  DEFINES_PARAMETER(Vector2BH<>, gyroBiasWalkMeasurementNoise, Vector2BH<>(0.1f, 0.1f)) /**< The noise of gyro measurements and the gyro offset while walking. */
  DEFINES_PARAMETER(Vector3BH<>, accBiasProcessNoise, Vector3BH<>(0.1f, 0.1f, 0.1f)) /**< The process noise of the acceleration sensor offset estimator. */
  DEFINES_PARAMETER(Vector3BH<>, accBiasStandMeasurementNoise, Vector3BH<>(0.1f, 0.1f, 0.1f)) /**< The noise of acceleration sensor measurements and the acceleration sensor offset while standing. */
  DEFINES_PARAMETER(Vector3BH<>, accBiasWalkMeasurementNoise, Vector3BH<>(1.f, 1.f, 1.f)) /**< The noise of acceleration sensor measurements and the acceleration sensor offset while walking. */
END_MODULE

/**
* @class InertiaSensorCalibrator
* A module for determining the bias of the inertia sensor readings.
*/
class InertiaSensorCalibrator : public InertiaSensorCalibratorBase
{
public:
  static PROCESS_WIDE_STORAGE(InertiaSensorCalibrator) theInstance;
  /** Default constructor. */
  InertiaSensorCalibrator();

  /**
  * Resets all internal values (including determined calibration) of this module.
  */
  void reset();

  /**
  * Updates the InertiaSensorDataBH representation.
  * @param inertiaSensorData The inertia sensor data representation which is updated by this module.
  */
  void update(InertiaSensorDataBH& inertiaSensorData);

private:
  /**
  * Class for buffering averaged gyro and acceleration sensor readings.
  */
  class Collection
  {
  public:
    Vector3BH<> accAvg; /**< The average of acceleration sensor readings of one walking phase or 1 secBH. */
    Vector2BH<> gyroAvg; /**< The average of gyro sensor eadings of one walking phase or 1 secBH. */
    unsigned int timeStamp; /**< When this collection was created. */
    bool standing; /**< Whether the robot was standing while collecting sensor readings. */

    /**
    * Constructs a collection.
    */
    Collection(const Vector3BH<>& accAvg, const Vector2BH<>& gyroAvg, unsigned int timeStamp, bool standing) :
      accAvg(accAvg), gyroAvg(gyroAvg), timeStamp(timeStamp), standing(standing) {}

    /**
    * Default constructor.
    */
    Collection() {};
  };

  bool calibrated; /**< Whether the filters are initialized. */
  Kalman<float> accXBias; /**< The calibration bias of accX. */
  Kalman<float> accYBias; /**< The calibration bias of accY. */
  Kalman<float> accZBias; /**< The calibration bias of accZ. */
  Kalman<float> gyroXBias; /**< The calibration bias of gyroX. */
  Kalman<float> gyroYBias; /**< The calibration bias of gyroY. */

  unsigned timeWhenPenalized; /**< When the robot received the last penalty */
  unsigned int collectionStartTime; /**< When the current collection was started. */
  unsigned int cleanCollectionStartTime; /**< When the last unstable situation was over. */

  RingBufferWithSumBH<Vector3BH<>, 300> accValues; /**< Ringbuffer for collecting the acceleration sensor values of one walking phase or 1 secBH. */
  RingBufferWithSumBH<Vector2BH<>, 300> gyroValues; /**< Ringbuffer for collecting the gyro sensor values of one walking phase or 1 secBH. */

  RingBufferBH<Collection, 50> collections; /**< Buffered averaged gyro and accleration sensor readings. */

  unsigned int lastTime; /**< The time of the previous iteration. */
  MotionRequestBH::Motion lastMotion; /**< The executed motion of the previous iteration. */
  double lastPositionInWalkCycle; /**< The walk cycle position of the previous iteration. */
  unsigned lastPenalty; /** The penalty state of the previous iteration. */

  RotationMatrixBH calculatedRotation; /**< Body rotation, which was calculated using kinematics. */

#ifndef RELEASE
  JointCalibrationBH lastJointCalibration; /**< Some parts of the joint calibration of the previous iteration. */
#endif
};
