/**
* @file BLAME.h
* This file declares a module that creates the walking motions.
* @author <A href="mailto:judy@tzi.de">Judith Müller</A>
*/

#pragma once

#ifndef WIN32
#include <dirent.h>
#else
#include <winsock.h>
#endif
#include <cstdio>
#include <cstring>

#include "BIKEParameters.h"
#include "Tools/ModuleBH/Module.h"
#include "Tools/Streams/InStreams.h"
#include "BlameData.h"
#include "Representations/MotionControl/BikeEngineOutput.h"
#include "Representations/MotionControl/WalkingEngineOutput.h"
#include "Representations/MotionControl/HeadJointRequest.h"
#include "Platform/File.h"
#include "Representations/Sensing/TorsoMatrix.h"

MODULE(BLAME)
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
  PROVIDES_WITH_MODIFY(BikeEngineOutputBH)
END_MODULE

class BLAME : public BLAMEBase
{
private:

  BlameData data;
  bool compensate, compensated;

  std::vector<BIKEParameters> params;

public:

  void update(BikeEngineOutputBH& blameOutput);

  /**
  * Default constructor.
  */
  BLAME();

  ~BLAME() {};
};
