/**
* @file InertiaSensorCalibrator.cpp
* Implementation of module InertiaSensorCalibrator.
* @author Colin Graf
*/

#include "InertiaSensorCalibrator.h"
#include "Tools/Math/Pose3D.h"

using namespace std;

MAKE_MODULE(InertiaSensorCalibrator, Sensing)

PROCESS_WIDE_STORAGE(InertiaSensorCalibrator) InertiaSensorCalibrator::theInstance = 0;

InertiaSensorCalibrator::InertiaSensorCalibrator()
{
  reset();
}

void InertiaSensorCalibrator::reset()
{
  lastTime = 0;
  lastMotion = MotionRequestBH::specialAction;
  calibrated = false;
  timeWhenPenalized = 0;
  collectionStartTime = 0;
  cleanCollectionStartTime = 0;
}

void InertiaSensorCalibrator::update(InertiaSensorDataBH& inertiaSensorData)
{
  DEBUG_RESPONSE_ONCE("module:InertiaSensorCalibrator:reset", reset(););

  // frame time check
  if(theFrameInfoBH.time <= lastTime)
  {
    if(theFrameInfoBH.time == lastTime)
      return; // done!
    reset();
  }

  // update timeLastPenalty
  if(theRobotInfoBH.penalty != PENALTY_NONE && lastPenalty == PENALTY_NONE)
    timeWhenPenalized = theFrameInfoBH.time;

  // detect changes in joint calibration
#ifndef RELEASE
  bool jointCalibrationChanged = false;
  for(int i = JointDataBH::LHipYawPitch; i <= JointDataBH::LAnkleRoll; ++i)
    if(theJointCalibrationBH.joints[i].offset != lastJointCalibration.joints[i].offset)
    {
      jointCalibrationChanged = true;
      lastJointCalibration.joints[i].offset = theJointCalibrationBH.joints[i].offset;
    }
  for(int i = JointDataBH::RHipYawPitch; i <= JointDataBH::RAnkleRoll; ++i)
    if(theJointCalibrationBH.joints[i].offset != lastJointCalibration.joints[i].offset)
    {
      jointCalibrationChanged = true;
      lastJointCalibration.joints[i].offset = theJointCalibrationBH.joints[i].offset;
    }
  if(jointCalibrationChanged)
    reset();
#endif

  const Vector2BH<> gyro = Vector2BH<>(theSensorDataBH.data[SensorDataBH::gyroX], theSensorDataBH.data[SensorDataBH::gyroY]);
  const Vector3BH<> acc = Vector3BH<>(theSensorDataBH.data[SensorDataBH::accX], theSensorDataBH.data[SensorDataBH::accY], theSensorDataBH.data[SensorDataBH::accZ]);
 
  // it's prediction time!
  if(lastTime && calibrated)
  {
    const float timeDiff = float(theFrameInfoBH.time - lastTime) * 0.001f; // in seconds
    accXBias.predict(0.f, sqrBH(accBiasProcessNoise.x * timeDiff));
    accYBias.predict(0.f, sqrBH(accBiasProcessNoise.y * timeDiff));
    accZBias.predict(0.f, sqrBH(accBiasProcessNoise.z * timeDiff));
    gyroXBias.predict(0.f, sqrBH(gyroBiasProcessNoise.x * timeDiff));
    gyroYBias.predict(0.f, sqrBH(gyroBiasProcessNoise.y * timeDiff));
  }

  // detect unstable stuff...
  const MotionRequestBH::Motion& currentMotion(theMotionSelectionBH.targetMotion);
  bool unstable = false;
  if(currentMotion != lastMotion || // motion change
     currentMotion != theMotionInfoBH.motion ||  // interpolating
     theMotionInfoBH.motion != MotionRequestBH::stand) // only calibrate while standing
  {
    unstable = true;
  }
  else if(currentMotion == MotionRequestBH::walk)
  {
    unstable = true;
  }
  else if(currentMotion != MotionRequestBH::walk && currentMotion != MotionRequestBH::stand)
  {
    unstable = true;
  }
  else if(theRobotInfoBH.penalty != PENALTY_NONE && ((theRobotInfoBH.secsTillUnpenalised * 1000 < (int) penalizedTimeFrame && theFrameInfoBH.getTimeSince(theGameInfoBH.timeLastPackageReceived) < 2000) || theFrameInfoBH.getTimeSince(timeWhenPenalized) < (int) penalizedTimeFrame))
  {
    unstable = true;
  }
  else if(accValues.getNumberOfEntries() >= accValues.getMaxEntries())
  {
    unstable = true;
  }

  // update cleanCollectionStartTime
  if(unstable)
    cleanCollectionStartTime = 0;
  else if(!cleanCollectionStartTime)
    cleanCollectionStartTime = theFrameInfoBH.time;

  // restart sensor value collecting?
  const bool standing = currentMotion == MotionRequestBH::stand || (currentMotion == MotionRequestBH::walk && theWalkingEngineOutputBH.standing);
  const bool walking = currentMotion == MotionRequestBH::walk && !theWalkingEngineOutputBH.standing;

  if(unstable || (walking && theWalkingEngineOutputBH.positionInWalkCycle < lastPositionInWalkCycle) || (standing && theFrameInfoBH.time - collectionStartTime > 1000))
  {
    // add collection within the time frame to the collection buffer
    ASSERT(accValues.getNumberOfEntries() == gyroValues.getNumberOfEntries());
    if(cleanCollectionStartTime && theFrameInfoBH.time - cleanCollectionStartTime > timeFrame &&
       accValues.getNumberOfEntries())
    {
      ASSERT(collections.getNumberOfEntries() < collections.getMaxEntries());
      collections.add(Collection(accValues.getSum() / float(accValues.getNumberOfEntries()),
                                 gyroValues.getSum() / float(gyroValues.getNumberOfEntries()),
                                 collectionStartTime + (theFrameInfoBH.time - collectionStartTime) / 2, standing));
    }

    // restart collecting
    accValues.init();
    gyroValues.init();
    collectionStartTime = 0;

    // look if there are any useful buffered collections
    for(int i = collections.getNumberOfEntries() - 1; i >= 0; --i)
    {
      const Collection& collection(collections.getEntry(i));
      if(theFrameInfoBH.time - collection.timeStamp < timeFrame)
        break;
      if(cleanCollectionStartTime && cleanCollectionStartTime < collection.timeStamp)
      {
        // use this collection
        Vector3BH<>& accBiasMeasurementNoise = collection.standing ? accBiasStandMeasurementNoise : accBiasWalkMeasurementNoise;
        Vector2BH<>& gyroBiasMeasurementNoise = collection.standing ? gyroBiasStandMeasurementNoise : gyroBiasWalkMeasurementNoise;
        if(!calibrated)
        {
          calibrated = true;
          accXBias.init(collection.accAvg.x, sqrBH(accBiasMeasurementNoise.x));
          accYBias.init(collection.accAvg.y, sqrBH(accBiasMeasurementNoise.y));
          accZBias.init(collection.accAvg.z, sqrBH(accBiasMeasurementNoise.z));
          gyroXBias.init(collection.gyroAvg.x, sqrBH(gyroBiasMeasurementNoise.x));
          gyroYBias.init(collection.gyroAvg.y, sqrBH(gyroBiasMeasurementNoise.y));
        }
        else
        {
          accXBias.update(collection.accAvg.x, sqrBH(accBiasMeasurementNoise.x));
          accYBias.update(collection.accAvg.y, sqrBH(accBiasMeasurementNoise.y));
          accZBias.update(collection.accAvg.z, sqrBH(accBiasMeasurementNoise.z));
          gyroXBias.update(collection.gyroAvg.x, sqrBH(gyroBiasMeasurementNoise.x));
          gyroYBias.update(collection.gyroAvg.y, sqrBH(gyroBiasMeasurementNoise.y));
        }        
      }
      collections.removeFirst();
    }
  }

  // collecting....
  if(!unstable)
  {
    // calculate rotation based on foot - torso transformation
    const Pose3DBH& footLeft(theRobotModelBH.limbs[MassCalibrationBH::footLeft]);
    const Pose3DBH& footRight(theRobotModelBH.limbs[MassCalibrationBH::footRight]);
    const Pose3DBH footLeftInvert(footLeft.invert());
    const Pose3DBH footRightInvert(footRight.invert());
    if(abs(footLeftInvert.translation.z - footRightInvert.translation.z) < 3.f/* magic number */)
    {
      // use average of the calculated rotation of each leg
      calculatedRotation = RotationMatrixBH(Vector3BH<>(
                                            (atan2(footLeftInvert.rotation.c1.z, footLeftInvert.rotation.c2.z) + atan2(footRightInvert.rotation.c1.z, footRightInvert.rotation.c2.z)) * 0.5f,
                                            (atan2(-footLeftInvert.rotation.c0.z, footLeftInvert.rotation.c2.z) + atan2(-footRightInvert.rotation.c0.z, footRightInvert.rotation.c2.z)) * 0.5f,
                                            0.f));
    }
    else if(footLeftInvert.translation.z > footRightInvert.translation.z)
    {
      // use left foot
      calculatedRotation = footLeftInvert.rotation;
    }
    else
    {
      // use right foot
      calculatedRotation = footRightInvert.rotation;
    }

    // calculate expected acceleration sensor reading
    Vector3BH<> accGravOnly(calculatedRotation.c0.z, calculatedRotation.c1.z, calculatedRotation.c2.z);
    accGravOnly *= -9.80665f;
    accGravOnly.x /= theSensorCalibrationBH.accXGain;
    accGravOnly.y /= theSensorCalibrationBH.accYGain;
    accGravOnly.z /= theSensorCalibrationBH.accZGain;

    // add sensor reading to the collection
    ASSERT(accValues.getNumberOfEntries() < accValues.getMaxEntries());
    accValues.add(acc - accGravOnly);
    gyroValues.add(gyro);
    if(!collectionStartTime)
      collectionStartTime = theFrameInfoBH.time;
  }

  // provide calibrated inertia readings
  inertiaSensorData.calibrated = calibrated;
  if(!calibrated)
  {
    inertiaSensorData.gyro.x = inertiaSensorData.gyro.y = InertiaSensorDataBH::off;
    inertiaSensorData.acc.x = inertiaSensorData.acc.y = inertiaSensorData.acc.z = InertiaSensorDataBH::off;
  }
  else
  {
    inertiaSensorData.gyro.x = gyro.x - gyroXBias.value;
    inertiaSensorData.gyro.y = gyro.y - gyroYBias.value;
    inertiaSensorData.acc.x = acc.x - accXBias.value;
    inertiaSensorData.acc.y = acc.y - accYBias.value;
    inertiaSensorData.acc.z = acc.z - accZBias.value;

    inertiaSensorData.gyro.x *= theSensorCalibrationBH.gyroXGain;
    inertiaSensorData.gyro.y *= theSensorCalibrationBH.gyroYGain;
    inertiaSensorData.acc.x *= theSensorCalibrationBH.accXGain;
    inertiaSensorData.acc.y *= theSensorCalibrationBH.accYGain;
    inertiaSensorData.acc.z *= theSensorCalibrationBH.accZGain;
  }


  MODIFY("module:InertiaSensorCalibrator:calibrated", calibrated);
  MODIFY("module:InertiaSensorCalibrator:gyroXBias", gyroXBias.value);
  MODIFY("module:InertiaSensorCalibrator:gyroYBias", gyroYBias.value);
  MODIFY("module:InertiaSensorCalibrator:accXBias", accXBias.value);
  MODIFY("module:InertiaSensorCalibrator:accYBias", accYBias.value);
  MODIFY("module:InertiaSensorCalibrator:accZBias", accZBias.value);

  // store some values for the next iteration
  lastTime = theFrameInfoBH.time;
  lastMotion = theMotionSelectionBH.targetMotion;
  lastPositionInWalkCycle = theWalkingEngineOutputBH.positionInWalkCycle;
  lastPenalty = theRobotInfoBH.penalty;
}
