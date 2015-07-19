/**
* @file Modules/MotionControl/KickEngineData.h
* This file declares a module that creates the walking motions.
* @author <A href="mailto:judy@tzi.de">Judith Müller</A>
*/

#pragma once

#include "KickEngineParameters.h"
#include "Representations/Configuration/RobotDimensions.h"
#include "Representations/Configuration/JointCalibration.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/MotionControl/MotionSelection.h"
#include "Representations/MotionControl/MotionRequest.h"
#include "Tools/Math/Pose3D.h"
#include "Tools/Streams/InStreams.h"
#include "Platform/BHAssert.h"
#include "Representations/Infrastructure/SensorData.h"
#include <vector>
#include "Representations/Configuration/MassCalibration.h"
#include "Representations/Sensing/RobotModel.h"
#include "Tools/RingBufferWithSum.h"
#include "Representations/MotionControl/KickEngineOutput.h"
#include "Representations/Sensing/TorsoMatrix.h"
#include "Representations/MotionControl/HeadJointRequest.h"

class KickEngineData
{
private:
  bool lSupp,
       rSupp,
       toLeftSupport,
       formMode,
       limbOff[Phase::numOfLimbs];

  int phaseNumber,
      motionID;

  float phase,
        cycletime,
        lastRatio;

  unsigned int timeStamp;
  int timeSinceTimeStamp;

  RingBufferWithSumBH <Vector2BH<int>, 10> standLegPos;

  Vector2BH<> comOffset,
          balanceSum,
          gyro,
          lastGyroLeft,
          lastGyroRight,
          gyroErrorLeft,
          gyroErrorRight,
          lastBody,
          bodyError,
          gyroP,
          gyroI,
          gyroD,
          origin;

  Vector2BH<> head;

  Vector3BH<> positions[Phase::numOfLimbs], origins[Phase::numOfLimbs];

  Vector3BH<> torsoRot;

  Vector3BH<> lastCom, ref, actualDiff;

  KickEngineParameters currentParameters;

  RobotModelBH robotModel, comRobotModel;

  JointRequestBH lastBalancedJointRequest, compenJoints;

  bool wasActive, startComp, willBeLeft;

public:
  KickRequest currentKickRequest;
  bool internalIsLeavingPossible;

  bool getMotionIDByName(const MotionRequestBH& motionRequest, const std::vector<KickEngineParameters>& params);
  void calculateOrigins(const RobotDimensionsBH& theRobotDimension, const FilteredJointDataBH& jd);
  bool checkPhaseTime(const FrameInfoBH& frame, const RobotDimensionsBH& rd, const FilteredJointDataBH& jd, const TorsoMatrixBH& torsoMatrix);
  void balanceCOM(JointRequestBH& joints, const RobotDimensionsBH& rd, const MassCalibrationBH& mc, const FilteredJointDataBH& theFilteredJointData);
  void calculatePreviewCom(Vector3BH<> &ref, Vector2BH<> &origin);
  void setStandLeg(const float& originY);
  bool calcJoints(JointRequestBH& jointRequest, const RobotDimensionsBH& rd, const HeadJointRequestBH& hr);
  void calcLegJoints(const JointDataBH::Joint& joint, JointRequestBH& jointRequest, const RobotDimensionsBH& theRobotDimensions);
  static void simpleCalcArmJoints(const JointDataBH::Joint& joint, JointRequestBH& jointRequest, const RobotDimensionsBH& theRobotDimensions, const Vector3BH<>& armPos, const Vector3BH<>& handRotAng);
  void getCOMReference(const Vector3BH<>& lFootPos, const Vector3BH<>& rFootPos, Vector3BH<>& comRef, Vector2BH<>& origin);
  void setStaticReference();
  void mirrorIfNecessary(JointRequestBH& joints);
  void addGyroBalance(JointRequestBH& jointRequest, const JointCalibrationBH& jc, const FilteredSensorDataBH& sd, const float& ratio);
  void addDynPoint(const DynPoint& dynPoint, const RobotDimensionsBH& rd, const TorsoMatrixBH& torsoMatrix);
  void ModifyData(const KickRequest& br, JointRequestBH& kickEngineOutput, std::vector<KickEngineParameters>& params);
  void debugFormMode(std::vector<KickEngineParameters>& params);
  void setCycleTime(float time);
  void calcPhaseState();
  void calcPositions(JointRequestBH& joints, const FilteredJointDataBH& theFilteredJointData);
  void setRobotModel(RobotModelBH rm);
  bool isMotionAlmostOver();
  void setCurrentKickRequest(const MotionRequestBH& mr);
  void setExecutedKickRequest(KickRequest& br);
  void initData(const bool& compensated, const FrameInfoBH& frame, const MotionRequestBH& mr, const RobotDimensionsBH& theRobotDimensions, std::vector<KickEngineParameters>& params, const FilteredJointDataBH& jd, const TorsoMatrixBH& torsoMatrix);
  void setEngineActivation(const float& ratio);
  bool activateNewMotion(const KickRequest& br, const bool& isLeavingPossible);
  bool sitOutTransitionDisturbance(bool& compensate, bool& compensated, const FilteredSensorDataBH& sd, KickEngineOutput& kickEngineOutput, const JointDataBH& jd, const FrameInfoBH& frame);

  Pose3DBH calcDesBodyAngle(JointRequestBH& jointData, const RobotDimensionsBH& robotDimensions, JointDataBH::Joint joint);

  void transferDynPoint(Vector3BH<>& d, const RobotDimensionsBH& rd, const TorsoMatrixBH& torsoMatrix);

  KickEngineData():
    formMode(false),
    motionID(-1),
    cycletime(0.f),
    lastRatio(0.f),
    //Parameter for P, I and D for gyro PID Contol
    gyroP(3.f, -2.5f),
    gyroI(0, 0),
    gyroD(0.03f, 0.01f),
    internalIsLeavingPossible(false)
  {
    for(int i = 0; i < Phase::numOfLimbs; i++)
    {
      limbOff[i] = false;
    }
  }
};
