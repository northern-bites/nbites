#ifndef BHWALKMODULE_GUKHG02Q
#define BHWALKMODULE_GUKHG02Q

#include <Module.h>

class WalkingEngine;

class Pose2D;
class Pose2DBH;

class FrameInfoBlock;
class GameStateBlock;
class JointBlock;
class JointCommandBlock;
class KickRequestBlock;
class OdometryBlock;
class RobotInfoBlock;
class SensorBlock;
class WalkInfoBlock;
class WalkParamBlock;
class WalkRequestBlock;

class BHWalkModule: public Module {
public:
  BHWalkModule();
  ~BHWalkModule();
  void specifyMemoryDependency();
  void specifyMemoryBlocks();
  void initSpecificModule();

  void processFrame();

  void handleStepIntoKick();

private:
  void processWalkParams();
  void processWalkRequest();
  void setArms(Joints angles, float timeInSeconds);
  void getArmsForState(int state, Joints angles);
  void determineStartingArmState();

  void setMassCalibration();
  void setRobotDimensions();

  void selectivelySendStiffness();

  void setPose2D(Pose2D &dest, const Pose2DBH &src);

  bool doingSlowStand();
  void doSlowStand();
  bool shouldStartSlowStand();
  void startSlowStand();
  float standJointErr(int joint);

  bool readyToStartKickAfterStep();

  void setKickStepParams();

  void setWalkHeight(float z);

private:
  FrameInfoBlock *frame_info_;
  JointBlock *raw_joints_;
  JointBlock *joints_;
  JointCommandBlock *commands_;
  KickRequestBlock *kick_request_;
  OdometryBlock *odometry_;
  RobotInfoBlock *robot_info_;
  SensorBlock *sensors_;
  WalkInfoBlock *walk_info_;
  WalkParamBlock *walk_params_;
  WalkRequestBlock *walk_request_;

  WalkingEngine *walk_engine_;

  float slow_stand_start;
  float slow_stand_end;
  float walk_requested_start_time;

  //static const int jointMapping[NUM_JOINTS];
  int utJointToBHJoint[NUM_JOINTS];

  float kick_distance_;
  float kick_angle_;
  bool prev_kick_active_;
  bool arms_close_to_targets_;
  int arm_state_;
  float arm_state_change_;
  float last_walk_or_stand_;

  Joints armStart;

  enum StepIntoKickState {
    PERFORMING,
    FINISHED_WITH_STEP,
    NONE
  };
  StepIntoKickState step_into_kick_state_;
  float time_step_into_kick_finished_;

public:
  static const float STAND_ANGLES[NUM_JOINTS];
};

#endif /* end of include guard: BHWALKMODULE_GUKHG02Q */
