/**
* @file KickEngine.h
* This file declares a module that creates the walking motions.
* @author <A href="mailto:judy@tzi.de">Judith Müller</A>
*/

#pragma once

#include <dirent.h>
#include <cstdio>
#include <cstring>

#include "KickEngineParameters.h"
#include "Tools/Module/Module.h"
#include "Tools/Streams/InStreams.h"
#include "KickEngineData.h"
#include "Representations/MotionControl/KickEngineOutput.h"
#include "Representations/MotionControl/WalkingEngineOutput.h"
#include "Representations/MotionControl/HeadJointRequest.h"
#include "Platform/File.h"
#include "Representations/Sensing/TorsoMatrix.h"
#include "DebugConfig.h"

// "If you have code quality that's bad you don't care anymore" --Josh
#include "Tools/Module/Module.h"
#include "MotionSelector.h"
#include "Modules/Sensing/JointFilter.h"
#include "Modules/Sensing/RobotModelProvider.h"
#include "Modules/Sensing/InertiaSensorCalibrator.h"
#include "Modules/Sensing/InertiaSensorFilter.h"
#include "Modules/Sensing/SensorFilter.h"
#include "Modules/Sensing/FallDownStateDetector.h"
#include "Modules/Sensing/TorsoMatrixProvider.h"
#include "Modules/Sensing/GroundContactDetector.h"
#include "Modules/Infrastructure/NaoProvider.h"
#include "Representations/Configuration/RobotDimensions.h"
#include "Representations/Configuration/MassCalibration.h"
#include "Representations/Configuration/DamageConfiguration.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Sensing/RobotModel.h"
#include "Representations/Sensing/TorsoMatrix.h"
#include "Representations/Sensing/InertiaSensorData.h"
#include "Representations/Sensing/FallDownState.h"
#include "Representations/Sensing/GroundContactState.h"
#include "Representations/Sensing/OrientationData.h"
#include "Representations/MotionControl/MotionSelection.h"
#include "Representations/MotionControl/MotionRequest.h"
#include "Representations/MotionControl/WalkingEngineOutput.h"
#include "Representations/MotionControl/HeadJointRequest.h"
#include "Representations/MotionControl/ArmMotionEngineOutput.h"
#include "Representations/MotionControl/OdometryData.h"
#include <iostream>

MODULE(KickEngine)
  REQUIRES(MotionSelectionBH)
  REQUIRES(MotionRequestBH)
  REQUIRES(MotionInfoBH)
  REQUIRES(RobotModelBH)
  REQUIRES(RobotDimensionsBH)
  REQUIRES(MassCalibrationBH)
  REQUIRES(HeadJointRequestBH)
  REQUIRES(HardnessSettingsBH)
  REQUIRES(SensorCalibrationBH)
  REQUIRES(JointCalibrationBH)
  REQUIRES(ArmMotionEngineOutputBH)
  REQUIRES(FrameInfoBH)
  REQUIRES(TorsoMatrixBH)
  REQUIRES(GroundContactStateBH)
  REQUIRES(FallDownStateBH)
  REQUIRES(OrientationDataBH)
  REQUIRES(FilteredJointDataBH)
  REQUIRES(JointDataBH)
  REQUIRES(InertiaSensorDataBH)
  REQUIRES(FilteredSensorDataBH)
  REQUIRES(SensorDataBH)
  REQUIRES(DamageConfigurationBH)
  REQUIRES(OdometryDataBH)
  REQUIRES(WalkingEngineStandOutputBH)
  PROVIDES_WITH_MODIFY(KickEngineOutput)
END_MODULE

class KickEngine : public KickEngineBase
{
private:

  KickEngineData data;
  bool compensate, compensated;
  unsigned timeSinceLastPhase;

  std::vector<KickEngineParameters> params;

public:
  NaoProvider *naoProvider;
  JointFilter *jointFilter;
  RobotModelProvider *robotModelProvider;
  GroundContactDetector *groundContactDetector;
  InertiaSensorCalibrator *inertiaSensorCalibrator;
  InertiaSensorFilter *inertiaSensorFilter;
  SensorFilter *sensorFilter;
  FallDownStateDetector *fallDownStateDetector;
  TorsoMatrixProvider *torsoMatrixProvider;
  MotionSelector *motionSelector;

  void update(KickEngineOutput& kickEngineOutput);

  void init();

  /**
  * Default constructor.
  */
  KickEngine();

  ~KickEngine();
};
