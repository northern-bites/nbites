/**
* @file GroundContactDetector.cpp
* Implementation of module GroundContactDetector.
* @author Colin Graf
*/

#include "GroundContactDetector.h"
#include "Tools/Debugging/DebugDrawings.h" // PLOT
#include "Tools/Streams/InStreams.h"

MAKE_MODULE(GroundContactDetector, Sensing)

GroundContactDetector::GroundContactDetector() : contact(false), contactStartTime(0), useAngle(false)
{
}

void GroundContactDetector::update(GroundContactStateBH& groundContactState)
{
  DECLARE_PLOT("module:GroundContactDetector:angleNoiseX");
  DECLARE_PLOT("module:GroundContactDetector:angleNoiseY");
  DECLARE_PLOT("module:GroundContactDetector:accNoiseX");
  DECLARE_PLOT("module:GroundContactDetector:accNoiseY");
  DECLARE_PLOT("module:GroundContactDetector:accNoiseZ");
  DECLARE_PLOT("module:GroundContactDetector:gyroNoiseX");
  DECLARE_PLOT("module:GroundContactDetector:gyroNoiseY");

  MODIFY("module:GroundContactDetector:contact", contact);

  // Northern Bites don't trust this module, a bug in it means an inactive robot
  groundContactState.contact = true;
  return;

  bool ignoreSensors = (theMotionInfoBH.motion != MotionRequestBH::walk && theMotionInfoBH.motion != MotionRequestBH::stand &&
                        theMotionInfoBH.motion != MotionRequestBH::specialAction && theMotionInfoBH.motion != MotionRequestBH::getUp) ||
                       (theMotionRequestBH.motion != MotionRequestBH::walk && theMotionRequestBH.motion != MotionRequestBH::stand &&
                        theMotionRequestBH.motion != MotionRequestBH::specialAction && theMotionRequestBH.motion != MotionRequestBH::getUp) ||
                       (theMotionInfoBH.motion == MotionRequestBH::walk && theMotionInfoBH.walkRequest.kickType != WalkRequest::none) ||
                       (theMotionRequestBH.motion == MotionRequestBH::walk && theMotionRequestBH.walkRequest.kickType != WalkRequest::none);
  if(!ignoreSensors)
  {
    if(contact)
    {
      calibratedAccZValues.add(theSensorDataBH.data[SensorDataBH::accZ]);

      Vector3BH<> angleDiff = ((const RotationMatrixBH&)(theTorsoMatrixBH.rotation * expectedRotationInv)).getAngleAxis();
      angleNoises.add(Vector2BH<>(sqrBH(angleDiff.x), sqrBH(angleDiff.y)));
      Vector2BH<> angleNoise = angleNoises.getAverage();
      PLOT("module:GroundContactDetector:angleNoiseX", angleNoise.x);
      PLOT("module:GroundContactDetector:angleNoiseY", angleNoise.y);

      if(!useAngle && angleNoises.isFull() && angleNoise.x < contactAngleActivationNoise && angleNoise.y < contactAngleActivationNoise)
        useAngle = true;

      if((useAngle && (angleNoise.x > contactMaxAngleNoise || angleNoise.y > contactMaxAngleNoise)) ||
         (calibratedAccZValues.isFull() && calibratedAccZValues.getAverage() > contactMaxAccZ))
      {
        /*
        if((useAngle && (angleNoise.x > p.contactMaxAngleNoise || angleNoise.y > p.contactMaxAngleNoise)))
          OUTPUT_ERROR("lost ground contact via angle");
        if((calibratedAccZValues.isFull() && calibratedAccZValues.getAverage() > p.contactMaxAccZ))
          OUTPUT_ERROR("lost ground contact via acc");
        */

        contact = false;
        accNoises.clear();
        gyroNoises.clear();
        accValues.clear();
        gyroValues.clear();
        angleNoises.clear();
#ifndef TARGET_SIM
        // if(contactStartTime != 0)
        //   SystemCall::playSound("high.wav");
#endif
      }
    }
    else
    {
      const Vector3BH<> accAverage = accValues.getAverage();
      const Vector2BH<> gyroAverage = gyroValues.getAverage();
      const Vector2BH<> gyro = Vector2BH<>(theSensorDataBH.data[SensorDataBH::gyroX], theSensorDataBH.data[SensorDataBH::gyroY]);
      const Vector3BH<> acc = Vector3BH<>(theSensorDataBH.data[SensorDataBH::accX], theSensorDataBH.data[SensorDataBH::accY], theSensorDataBH.data[SensorDataBH::accZ]);
      accValues.add(acc);
      gyroValues.add(gyro);
      if(accValues.isFull())
      {
        accNoises.add(Vector3BH<>(sqrBH(acc.x - accAverage.x), sqrBH(acc.y - accAverage.y), sqrBH(acc.z - accAverage.z)));
        gyroNoises.add(Vector2BH<>(sqrBH(gyro.x - gyroAverage.x), sqrBH(gyro.y - gyroAverage.y)));
      }
      Vector3BH<> accNoise = accNoises.getAverage();
      Vector2BH<> gyroNoise = gyroNoises.getAverage();
      PLOT("module:GroundContactDetector:accNoiseX", accNoise.x);
      PLOT("module:GroundContactDetector:accNoiseY", accNoise.y);
      PLOT("module:GroundContactDetector:accNoiseZ", accNoise.z);
      PLOT("module:GroundContactDetector:gyroNoiseX", gyroNoise.x);
      PLOT("module:GroundContactDetector:gyroNoiseY", gyroNoise.y);

      if(accNoises.isFull() &&
         accAverage.z < -5.f && std::abs(accAverage.x) < 5.f && std::abs(accAverage.y) < 5.f &&
         accNoise.x < noContactMinAccNoise && accNoise.y < noContactMinAccNoise && accNoise.z < noContactMinAccNoise &&
         gyroNoise.x < noContactMinGyroNoise && gyroNoise.y < noContactMinGyroNoise)
      {
        contact = true;
        useAngle = false;
        contactStartTime = theFrameInfoBH.time;
        angleNoises.clear();
        calibratedAccZValues.clear();
      }
    }
  }

  groundContactState.contact = contact;

  expectedRotationInv = theRobotModelBH.limbs[MassCalibrationBH::footLeft].translation.z > theRobotModelBH.limbs[MassCalibrationBH::footRight].translation.z ? theRobotModelBH.limbs[MassCalibrationBH::footLeft].rotation : theRobotModelBH.limbs[MassCalibrationBH::footRight].rotation;
}
