#ifndef BHWALKMODULE_GUKHG02Q
#define BHWALKMODULE_GUKHG02Q

#include <Module.h>
#include "Tools/RingBufferWithSum.h"
#include "common/RobotInfo.h"

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

enum FootSensorRegion 
{
	left_front,
	left_back,
	left_left,
	left_right,
	right_front,
	right_back,
	right_left,
	right_right,
	none
};

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


		/// Sums the pressure values of the given foot region on the left foot
		/// If the given region is Front, the function will add the value of the top left and top right pressure sensors of the foot
		float sumFsrs(FootSensorRegion r);

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

		// zscore / speed_percentage - the fsr stability walk thresholds
		const float stability_limit[3][2] =
		{
			{1, 0.9},
			{1.3, 0.9},
			{1.65, 0.85},
		};

		const int foot_contacts[8][2] = 
		{
			{fsrLFL,fsrLFR}, // left foot - front
			{fsrLRL,fsrLRR}, // left foot - back
			{fsrLFL,fsrLRL}, // left foot - left
			{fsrLFR,fsrLRR}, // left foot - right
			{fsrRFL,fsrRFR}, // right foot - front
			{fsrRRL,fsrRRR}, // right foot - back
			{fsrRFL,fsrRRL}, // right foot - left
			{fsrRFR,fsrRRR}, // right foot - right
		};
		const float FLYING_DETECT = 0.4f;

		float stable_walk_ready = false;
		float stable_walk_threshold_prior_mean;
		float stable_walk_threshold_prior_stddev;
		float stable_walk_threshold_mean;
		float stable_walk_threshold_stddev;
		const std::string contactNames[4] = {"left","right","front", "back"};

		RingBufferWithSumBH<float, 30>* fsr_mean;
		RingBufferWithSumBH<float, 30>* fsr_stddev;
		RingBufferWithSumBH<float, 100>* fsr_stability;

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
