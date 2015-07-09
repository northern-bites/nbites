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

MODULE(KickEngine)
  REQUIRES(RobotDimensionsBH)
  REQUIRES(JointCalibrationBH)
  REQUIRES(FrameInfoBH)
  REQUIRES(FilteredSensorDataBH)
  REQUIRES(MotionSelectionBH)
  REQUIRES(MotionRequestBH)
  REQUIRES(FilteredJointDataBH)
  REQUIRES(MassCalibrationBH)
  REQUIRES(RobotModelBH)
  REQUIRES(WalkingEngineStandOutputBH)
  REQUIRES(TorsoMatrixBH)
  REQUIRES(HeadJointRequestBH)
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

  void update(KickEngineOutput& kickEngineOutput);

  /**
  * Default constructor.
  */
  KickEngine();
};
