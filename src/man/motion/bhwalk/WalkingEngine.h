  /**
* @file WalkingEngine.h
* Declaration a module that creates the walking motions
* @author Colin Graf
*/

#pragma once

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
#include "Tools/Math/Matrix.h"
#include "Tools/Range.h"
#include "Tools/RingBuffer.h"
#include "Tools/RingBufferWithSum.h"
#include "Tools/Optimization/ParticleSwarm.h"
#include "WalkingEngineTools.h"
#include "WalkingEngineKicks.h"
#include "SubPhaseParameters.h"
#include <string>

MODULE(WalkingEngine)
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
  PROVIDES_WITH_MODIFY(WalkingEngineOutputBH)
  REQUIRES(WalkingEngineOutputBH)
  PROVIDES_WITH_MODIFY(WalkingEngineStandOutputBH)

  DEFINES_PARAMETER(VectorYZ, standComPosition, VectorYZ(50.f, 262.0f)) /**< The position of the center of mass relative to the right foot when standing */
  DEFINES_PARAMETER(float, standBodyTilt, 0.f) /**< The tilt of the torso when standing */
  DEFINES_PARAMETER(Vector2BH<>, standArmJointAngles, Vector2BH<>(0.2f, 0.f)) /**< The joint angles of the left arm when standing */
  DEFINES_PARAMETER(int, standHardnessAnklePitch, 85) /**< The hardness of the ankle pitch joint for standing and walking */
  DEFINES_PARAMETER(int, standHardnessAnkleRoll, 85) /**< The hardness of the ankle roll joint for standing and walking */

  DEFINES_PARAMETER(Vector2BH<>, walkRef, Vector2BH<>(16.f, 50.f)) /**< The position of the pendulum pivot point in Q */
  DEFINES_PARAMETER(Vector2BH<>, walkRefAtFullSpeedX, Vector2BH<>(9.5f, 40.f)) /**< The position of the pendulum pivot point when walking forwards with maximum speed */
  DEFINES_PARAMETER(RangeBH<>, walkRefXPlanningLimit, RangeBH<>(-2.f, 3.f)) /**< The limit for shifting the pendulum pivot point towards the x-axis when planning the next step size */
  DEFINES_PARAMETER(RangeBH<>, walkRefXLimit, RangeBH<>(-30.f, 30.f)) /**< The limit for shifting the pendulum pivot point towards the x-axis when balancing */
  DEFINES_PARAMETER(RangeBH<>, walkRefYLimit, RangeBH<>(-30.f, 30.f)) /**< The limit for shifting the pendulum pivot point towards the y-axis when balancing */
  DEFINES_PARAMETER(RangeBH<>, walkStepSizeXPlanningLimit, RangeBH<>(-50.f, 60.f)) /**< The minimum and maximum step size used to plan the next step size */
  DEFINES_PARAMETER(RangeBH<>, walkStepSizeXLimit, RangeBH<>(-55.f, 66.f)) /**< The minimum and maximum step size when balancing */
  DEFINES_PARAMETER(float, walkStepDuration, 525.f) /**< the duration of a full step cycle (two half steps) */
  DEFINES_PARAMETER(float, walkStepDurationAtFullSpeedX, 525.f) /**< the duration of a full step cycle when walking forwards with maximum speed */
  DEFINES_PARAMETER(float, walkStepDurationAtFullSpeedY, 180.f) /**< the duration of a full step cycle when walking sidewards with maximum speed */
  DEFINES_PARAMETER(Vector2BH<>, walkHeight, Vector2BH<>(262.f, 262.f)) /**< the height of the 3d linear inverted pendulum plane (for the pendulum motion towards the x-axis and the pendulum motion towards the y-axis) */
  DEFINES_PARAMETER(float, walkArmRotationAtFullSpeedX, 0.1f) /**< The maximum deflection for the arm swinging motion */
  DEFINES_PARAMETER(SubPhaseParameters, walkMovePhase, SubPhaseParameters(0.f, 1.f)) /**< The beginning and length of the trajectory used to move the swinging foot to its new position */
  DEFINES_PARAMETER(SubPhaseParameters, walkLiftPhase, SubPhaseParameters(0.f, 1.f)) /**< The beginning and length of the trajectory used to lift the swinging foot */
  DEFINES_PARAMETER(Vector3BH<>, walkLiftOffset, Vector3BH<>(0.f, 5.f, 17.f)) /**< The height the swinging foot is lifted */
  DEFINES_PARAMETER(Vector3BH<>, walkLiftOffsetAtFullSpeedX, Vector3BH<>(0.f, 5.f, 17.f)) /**< The height the swinging foot is lifted when walking full speed in x-direction */
  DEFINES_PARAMETER(Vector3BH<>, walkLiftOffsetAtFullSpeedY, Vector3BH<>(0.f, 20.f, 25.f)) /**< The height the swinging foot is lifted when walking full speed in y-direction */
  DEFINES_PARAMETER(Vector3BH<>, walkLiftRotation, Vector3BH<>(-0.05f, -0.1f, 0.f)) /**< The amount the swinging foot is rotated while getting lifted */
  DEFINES_PARAMETER(float, walkSupportRotation, 0.f) /**< A rotation added to the supporting foot to boost the com acceleration */
  DEFINES_PARAMETER(Vector3BH<>, walkComLiftOffset, Vector3BH<>(0.f, 0.f, 2.3f)) /**< The height the center of mass is lifted within a single support phase */
  DEFINES_PARAMETER(float, walkComBodyRotation, 0.05f) /**< How much the torso is rotated to achieve the center of mass shift along the y-axis */

  DEFINES_PARAMETER(Pose2DBH, speedMax, Pose2DBH(0.5f, Vector2BH<>(120.f, 50.f))) /**< The maximum walking speed (in "size of two steps") */
  DEFINES_PARAMETER(float, speedMaxBackwards, 80.f) /**< The maximum walking speed for backwards walking (in "size of two steps") */
  DEFINES_PARAMETER(Pose2DBH, speedMaxChange, Pose2DBH(0.1f, Vector2BH<>(8.f, 20.f))) /**< The maximum walking speed deceleration that is used to avoid overshooting of the walking target */

  DEFINES_PARAMETER(bool, balance, true) /**< Whether sensory feedback should be used or not */
  DEFINES_PARAMETER(Vector2BH<>, balanceBodyRotation, Vector2BH<>(0.8f, 0.f)) /**< A  torso rotation p-control factor */
  DEFINES_PARAMETER(Vector2BH<>, balanceCom, Vector2BH<>(0.054f, 0.054f)) /**< A measured center of mass position adoption factor */
  DEFINES_PARAMETER(Vector2BH<>, balanceComVelocity, Vector2BH<>(0.14f, 0.14f))  /**< A measured center of mass velocity adoption factor */
  DEFINES_PARAMETER(Vector2BH<>, balanceRef, Vector2BH<>(0.f, 0.08f)) /**< A pendulum pivot point p-control factor */
  DEFINES_PARAMETER(Vector2BH<>, balanceNextRef, Vector2BH<>(0.2f, 0.f)) /**< A pendulum pivot point of the upcoming single support phase p-control factor */
  DEFINES_PARAMETER(Vector2BH<>, balanceStepSize, Vector2BH<>(0.1f, -0.04f)) /**< A step size i-control factor */

  DEFINES_PARAMETER(float, observerMeasurementDelay, 40.f) /**< The delay between setting a joint angle and the ability of measuring the result */
  DEFINES_PARAMETER(Vector2f, observerMeasurementDeviation, Vector2f(2.f, 2.f)) /**< The measurement uncertainty of the computed "measured" center of mass position */
  DEFINES_PARAMETER(Vector4f, observerProcessDeviation, Vector4f(0.1f, 0.1f, 3.f, 3.f))  /**< The noise of the filtering process that estimates the position of the center of mass */

  DEFINES_PARAMETER(Pose2DBH, odometryScale, Pose2DBH(1.f, Vector2BH<>(1.f, 1.f))) /**< A scaling factor for computed odometry data */

  /* Parameters to calculate the correction of the torso's angular velocity. */
  DEFINES_PARAMETER(float, gyroStateGain, 0.01f) /**< Control weight (P) of the torso's angular velocity error. */
  DEFINES_PARAMETER(float, gyroDerivativeGain, 0.0001f) /**< Control weight (D) of the approximated rate of change of the angular velocity error. */
  DEFINES_PARAMETER(float, gyroSmoothing, 0.5f) /**< Smoothing (between 0 and 1!) to calculate the moving average of the y-axis gyro measurements. */

  DEFINES_PARAMETER(float, minRotationToReduceStepSize, 1.3f) /** I have no idea what im doing! Colin pls fix this! **/
END_MODULE

/**
* A module that creates walking motions using a three-dimensional linear inverted pendulum
*/
class WalkingEngine : public WalkingEngineBase
{
public:
  /**
  * Called from a MessageQueue to distribute messages
  * @param message The message that can be read
  * @return true if the message was handled
  */
  static bool handleMessage(InMessage& message);

  /**
  * Default constructor
  */
  WalkingEngine();

  /**
  * Destructor
  */
  ~WalkingEngine();

  /** Helper for optimizer. */
  class Parameters : public WalkingEngineBase
  {
    public:
    Parameters() : WalkingEngineBase() {}
    private:
    void update(WalkingEngineOutputBH&) {}
    void update(WalkingEngineStandOutputBH&) {}
  };

  /**
  * The size of a single step
  */
  class StepSize
  {
  public:
    Vector3BH<> translation; /**< The translational component */
    float rotation; /**< The rotational component */

    StepSize() : rotation(0.f) {}

    StepSize(float rotation, float x, float y) : translation(x, y, 0.f), rotation(rotation) {}
  };

  /**
  * A description of the posture of the legs
  */
  class LegPosture
  {
  public:
    Pose3DBH leftOriginToFoot; /**< The position of the left foot */
    Pose3DBH rightOriginToFoot; /**< The position of the right foot */
    Vector3BH<> leftOriginToCom; /**< The position of the center of mass relative to the origin that was used to describe the position of the left foot */
    Vector3BH<> rightOriginToCom;  /**< The position of the center of mass relative to the origin that was used to describe the position of the right foot */
  };

  /**
  * A description of the posture of the head and the arms
  */
  class ArmAndHeadPosture
  {
  public:
    float headJointAngles[2]; /**< head joint angles */
    float leftArmJointAngles[4]; /**< left arm joint angles */
    float rightArmJointAngles[4]; /**< right arm joint angles */
  };

  /**
  * A description of the posture of the whole body
  */
  class Posture : public LegPosture, public ArmAndHeadPosture {};

  ENUM(MotionType,
    stand,
    stepping
  );

  ENUM(PhaseType,
    standPhase,
    leftSupportPhase,
    rightSupportPhase
  );

  class PendulumPhase
  {
  public:
    unsigned int id; /**< A phase descriptor */
    PhaseType type; /**< What kind of phase is this? */
    Vector2BH<> k; /**< The constant of the pendulum motion function sqrt(g/h) */
    Vector2BH<> r; /**< The pendulum pivot point  (in Q) used to compute the pendulum motion */
    Vector2BH<> rOpt; /**< The initially planned target position of the pendulum pivot point */
    Vector2BH<> rRef; /**< The target position of the pendulum pivot point that has been adjusted to achieve the step size */
    Vector2BH<> x0; /**< The position of the center of mass relative to pendulum pivot point */
    Vector2BH<> xv0; /**< The velocity of the center of mass */
    float td; /**< The time in seconds left till the next pendulum phase */
    float tu; /**< The time in seconds passed since the beginning of the pendulum phase */
    StepSize s; /**< The step size used to reach the pendulum pivot point */
    Vector3BH<> l; /**< The height the foot of the swinging foot was lifted to implement the step size */
    Vector3BH<> lRotation; /**< A rotation applied to the foot while lifting it */
    Vector3BH<> cl; /**< An offset added to the desired center of mass position while lifting the foot */
    bool toStand; /**< Whether the next phase will be a standPhase */
    bool fromStand; /**< Whether the previous phase was a standPhase */
    WalkRequest::KickType kickType; /**< The type of kick executed during the phase */
  };

  class PendulumPlayer
  {
  public:
    WalkingEngine* engine;

    PendulumPhase phase;
    PendulumPhase nextPhase;

    void seek(float deltaTime);

    void getPosture(LegPosture& posture, float* leftArmAngle, float* rightArmAngle, Pose2DBH* stepOffset);
    void getPosture(Posture& posture);

    WalkingEngineKickPlayer kickPlayer;
  };

  static PROCESS_WIDE_STORAGE(WalkingEngine) theInstance; /**< Points to the only instance of this class in this process or is 0 if there is none */

  // computed parameters
  RotationMatrixBH standBodyRotation; /**< The basic orientation of the torso */
  Vector2BH<> walkK; /**< The constant of the pendulum motion function sqrt(g/h) */
  float walkPhaseDuration; /**< The basic duration of a single support phase */
  float walkPhaseDurationAtFullSpeedX; /**< The duration of single support phase when walking full with full speed in x-direction */
  float walkPhaseDurationAtFullSpeedY; /**< The duration of single support phase when walking full with full speed in y-direction */
  RangeBH<> walkXvdXPlanningLimit; /**< A limit of the center of mass velocity used to plan the center of mass trajectory */
  RangeBH<> walkXvdXLimit; /**< A limit of the center of mass to protect the walking engine from executing steps that are too large */

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

  // Northern Bites hack for hand speed info
  void updateHandSpeeds();
  Vector2BH<> lastLeftHandPos, lastRightHandPos;
  float leftHandSpeed;
  float rightHandSpeed;

  /**
  * Intercept parameter streaming to compute derived paramaters.
  * Note that this does not work during the construction of the module.
  * @param in The stream from which the object is read
  * @param out The stream to which the object is written.
  */
  void serialize(In* in, Out* out);

  /** Initialize derived parameters. */
  void init();

  void reset();

  /**
  * The central update method to generate the walking motion
  * @param walkingEngineOutput The WalkingEngineOutputBH (mainly the resulting joint angles)
  */
  void update(WalkingEngineOutputBH& walkingEngineOutput);
  MotionType currentMotionType;
  JointRequestBH jointRequest;
  PendulumPlayer pendulumPlayer;
  PendulumPlayer predictedPendulumPlayer;
  WalkingEngineKicks kicks;

  /**
  * The update method to generate the standing stance
  * @param standOutput The WalkingEngineStandOutputBH (mainly the resulting joint angles)
  */
  void update(WalkingEngineStandOutputBH& standOutput) {(JointRequestBH&)standOutput = jointRequest;}

  // attributes used by module:WalkingEngine:optimize debug response
  ParticleSwarm optimizeOptimizer;
  RingBufferWithSumBH<float, 300> optimizeFitness;
  bool optimizeStarted;
  unsigned int optimizeStartTime;
  Parameters optimizeBestParameters;

  void updateMotionRequest();
  MotionType requestedMotionType;
  Pose2DBH requestedWalkTarget;
  Pose2DBH lastCopiedWalkTarget;

  void updatePendulumPlayer();
  Vector2BH<> observedComOffset;

  void computeMeasuredPosture();
  Vector3BH<> measuredLeftToCom;
  Vector3BH<> measuredRightToCom;

  void computeExpectedPosture();
  Vector3BH<> expectedLeftToCom;
  Vector3BH<> expectedRightToCom;
  Vector2BH<> expectedComVelocity;
  Pose2DBH observedStepOffset;

  void computeEstimatedPosture();
  Vector3BH<> estimatedLeftToCom;
  Vector3BH<> estimatedRightToCom;
  Vector2BH<> estimatedComVelocity;
  Vector3BH<> lastExpectedLeftToCom;
  Vector3BH<> lastExpectedRightToCom;
  Vector2BH<> lastExpectedComVelocity;
  Matrix3x3f covX;
  Matrix3x3f covY;

  void computeError();
  Vector3BH<> errorLeft;
  Vector3BH<> errorRight;
  Vector2BH<> errorVelocity;
  RingBufferWithSumBH<float, 300> instability;
  bool finishedWithTarget;

  void correctPendulumPlayer();

  void updatePredictedPendulumPlayer();
  void applyCorrection(PendulumPhase& phase, PendulumPhase& nextPhase);
  Vector2BH<> measuredPx; /**< The measured com position (in Q) */
  Vector2BH<> measuredR; /**< The measured "zmp" */

  void generateTargetPosture();
  Posture targetPosture;

  void generateJointRequest();
  Vector3BH<> bodyToCom;
  Vector3BH<> lastAverageComToAnkle;
  RotationMatrixBH lastBodyRotationMatrix;
  RingBufferBH<float, 10> relativeRotations;
  float lastSmoothedGyroY; /**< Moving average of the y-axis gyro from the previous motion frame. */
  float lastGyroErrorY; /**< Last y-axis gyro deviation from the commanded angular velocity of the torso. */

  void generateOutput(WalkingEngineOutputBH& WalkingEngineOutputBH);
  void generateDummyOutput(WalkingEngineOutputBH& WalkingEngineOutputBH);

  void generateFirstPendulumPhase(PendulumPhase& phase);
  void generateNextPendulumPhase(const PendulumPhase& phase, PendulumPhase& nextPhase);
  RingBufferBH<PendulumPhase, 5> phaseBuffer;
  void computeNextPendulumParamtersY(PendulumPhase& nextPhase, float walkPhaseDurationX, float walkPhaseDurationY) const;
  void computeNextPendulumParamtersX(PendulumPhase& nextPhase) const;

  void updatePendulumPhase(PendulumPhase& phase, PendulumPhase& nextPhase, bool init) const;
  void repairPendulumParametersY(PendulumPhase& phase, const PendulumPhase& nextPhase) const;
  void updatePendulumParametersY(PendulumPhase& phase, PendulumPhase& nextPhase) const;
  void updatePendulumParametersX(PendulumPhase& phase, PendulumPhase& nextPhase, bool init) const;

  void generateNextStepSize(PhaseType nextSupportLeg, StepSize& stepSize);
  Pose2DBH lastRequestedSpeedRel;
  Pose2DBH lastSelectedSpeed;

  void computeOdometryOffset();
  Pose2DBH odometryOffset;
  Pose3DBH lastFootLeft;
  Pose3DBH lastFootRight;
  Vector3BH<> lastOdometryOrigin;
  Pose2DBH upcomingOdometryOffset;

  static float asinh(float x);
  static float atanh(float x);

  /**
  * A smoothed blend function with f(0)=0, f'(0)=0, f(1)=1, f'(1)=0, f(2)=0, f'(2)=0
  * @param x The function parameter [0...2]
  * @return The function value
  */
  static float blend(float x);

  // debug drawings
  void declareDrawings(const WalkingEngineOutputBH& walkingEngineOutput) const;

  // The different coordinate systems
  void drawW() const;
  void drawP() const;
  void drawQ(const WalkingEngineOutputBH& walkingEngineOutput) const;

  /*
  void drawFootTrajectory(const WalkingEngineOutputBH& walkingEngineOutput) const;
  void drawPreview(WalkRequest::KickType previewKickType, const WalkingEngineOutputBH& walkingEngineOutput) const;
  void drawSkeleton(const WalkingEngineOutputBH& walkingEngineOutput) const;
  void drawSkeleton2(const WalkingEngineOutputBH& walkingEngineOutput) const;
  void drawJoints(const WalkingEngineOutputBH& walkingEngineOutput) const;
  void drawJoints2(const WalkingEngineOutputBH& walkingEngineOutput) const;
  void drawPendulum(const WalkingEngineOutputBH& walkingEngineOutput) const;
  void drawInvKin1(const WalkingEngineOutputBH& walkingEngineOutput) const;
  void drawInvKin2(const WalkingEngineOutputBH& walkingEngineOutput) const;
  void drawInvKin3(const WalkingEngineOutputBH& walkingEngineOutput) const;
  void drawShowcaseCenterOfMass(const WalkingEngineOutputBH& walkingEngineOutput) const;
  void drawShowcaseContactAreaLeft() const;
  void drawShowcaseContactAreaRight() const;
  void drawShowcaseSupportAreaLeft() const;
  void drawShowcaseSupportAreaBoth(const WalkingEngineOutputBH& walkingEngineOutput) const;
  void drawShowcaseIpTrajectory(const WalkingEngineOutputBH& walkingEngineOutput) const;
  void drawShowcaseIp(const WalkingEngineOutputBH& walkingEngineOutput) const;
  void drawShowcaseIpSphere(const WalkingEngineOutputBH& walkingEngineOutput) const;
  void drawShowcase3dlipmPlane(const WalkingEngineOutputBH& walkingEngineOutput) const;
  void drawShowcaseSimpleIp(const WalkingEngineOutputBH& walkingEngineOutput) const;
  void drawShowcaseZmp(const WalkingEngineOutputBH& walkingEngineOutput) const;
  void drawShowcaseFloorGrid(const Pose3DBH& torsoMatrix) const;
  static const Vector3BH<> drawFootPoints[];
  static const unsigned int drawNumOfFootPoints;
  void drawComPlot();
  Vector2BH<> drawComPlotOrigin;
  Vector2BH<> drawComPlotLastR;
  unsigned int drawComPlotState;
  //unsigned int drawMeasurementDelay;
  */
  static const Vector3BH<> drawFootPoints[];
  static const unsigned int drawNumOfFootPoints;
  void drawZmp();
};
