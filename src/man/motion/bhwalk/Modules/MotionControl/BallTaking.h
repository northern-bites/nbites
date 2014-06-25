/**
* @file BallTaking.h
* @author Thomas Muender
*/

#pragma once

#include "Tools/ModuleBH/Module.h"
#include "Representations/MotionControl/BallTakingOutput.h"
#include "Representations/MotionControl/MotionSelection.h"
#include "Representations/Infrastructure/SensorData.h"
#include "Representations/Sensing/RobotModel.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Modeling/BallModel.h"

#include "Tools/RingBufferWithSum.h"

MODULE(BallTaking)
  REQUIRES(MotionSelectionBH)
  REQUIRES(FilteredJointDataBH)
  REQUIRES(FilteredSensorDataBH)
  REQUIRES(RobotDimensionsBH)
  REQUIRES(MassCalibrationBH)
  REQUIRES(RobotModelBH)
  REQUIRES(FrameInfoBH)
  REQUIRES(BallModelBH)
  PROVIDES_WITH_MODIFY(BallTakingOutputBH)
  DEFINES_PARAMETER(int, phaseDuration, 600)

  DEFINES_PARAMETER(float, xFactor, 50.f)
  DEFINES_PARAMETER(float, rotXFactor, 0.5f)
  DEFINES_PARAMETER(float, zStandFactor, 5.f)
  DEFINES_PARAMETER(float, y2Factor, 0.4f)
  DEFINES_PARAMETER(float, rot, 1.396f)
END_MODULE

class BallTaking : public BallTakingBase
{
private:

  //general stuff
  float side;
  int phase;
  unsigned phaseStart;
  bool phaseLeavingPossible;

  //balldata
  bool valid;
  Vector2BH<> crossing;
  RingBufferWithSumBH<float, 40> moved;
  RingBufferWithSumBH<char, 20> takable;
  
  //phase 0
  int c;

  //phase 1
  float y;
  float zStand;
  float rotZ;
  float rotX;
  float y2;

  //foot pose
  Pose3DBH standLeftFoot, standRightFoot;
  Pose3DBH targetLeftFootPositon, targetRightFootPositon;
  Pose3DBH leftEndPhaseOne, rightEndPhaseOne;
  Pose3DBH leftEndPhaseThree, rightEndPhaseThree;
  Pose3DBH leftEndPhaseFour, rightEndPhaseFour;
  Pose3DBH leftEndPhaseFive, rightEndPhaseFive;

public:

  BallTaking() : side(0), phase(0), phaseStart(0), phaseLeavingPossible(false), c(0)
  {
    standLeftFoot = Pose3DBH(-11.4841f, 49.8687f, -181.846f);
    standRightFoot = Pose3DBH(-11.4841f, -50.1313f, -181.846f);
  }

  void ballData(BallTakingOutputBH& output);

  void update(BallTakingOutputBH& output);

  void phaseZero(BallTakingOutputBH& output);

  void phaseOne(BallTakingOutputBH& output);

  void phaseTwo(BallTakingOutputBH& output);

  void phaseThree(BallTakingOutputBH& output);
  
  void phaseFour(BallTakingOutputBH& output);
  
  void phaseFive(BallTakingOutputBH& output);

  float sinUp(const float& x)
  {
    if(x < 0) return 0.f;
    else if(x > 1) return 1.f;
    else return (std::sin((x * pi) - pi_2) + 1) * 0.5f;
  }
};
