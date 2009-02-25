/**
 * The switchboard is literally a switching mechanism to select between the
 * different ways that we can publish joint values to the robot (called
 * MotionProviders). For example, the walk engine and the queueing system both
 * provide such functionality.
 *
 * The appropriate MotionEnactor will then take the nextJoints and pass them
 * down to the robot/simulator correctly.
 */

#ifndef _MotionSwitchboard_h_DEFINED
#define _MotionSwitchboard_h_DEFINED

#include <pthread.h>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "motionconfig.h" // for cmake set debugging flags like MOTION_DEBUG

#include "Kinematics.h"
#include "WalkProvider.h"
#include "WalkingConstants.h"
#include "ScriptedProvider.h"
#include "HeadProvider.h"
#include "Sensors.h"
#include "MotionConstants.h"

#include "BodyJointCommand.h"
#include "HeadJointCommand.h"
#include "WalkCommand.h"
#include "GaitCommand.h"
#include "SetHeadCommand.h"
#ifdef DEBUG_MOTION
#  define DEBUG_JOINTS_OUTPUT
#endif

class MotionSwitchboard {
public:
    MotionSwitchboard(boost::shared_ptr<Sensors> s);
    ~MotionSwitchboard();

    void start();
    void stop();
    void run();

	const std::vector <float> getNextJoints();
	void sendMotionCommand(const BodyJointCommand* command);
	void sendMotionCommand(const HeadJointCommand* command);
	void sendMotionCommand(const WalkCommand* command);
	void sendMotionCommand(const GaitCommand* command);
	void sendMotionCommand(const SetHeadCommand* command);
public:
    void stopHeadMoves(){headProvider.requestStop();}

    bool isWalkActive(){return walkProvider.isActive();}
    bool isHeadActive(){return headProvider.isActive();}
    bool isBodyActive(){return curProvider->isActive();}


private:
    int processProviders();
    void swapBodyProvider();
    BodyJointCommand * getGaitTransitionCommand(const WalkingParameters * new_gait);

#ifdef DEBUG_JOINTS_OUTPUT
    void initDebugLogs();
    void closeDebugLogs();
    void updateDebugLogs();
#endif

private:
    boost::shared_ptr<Sensors> sensors;
    WalkProvider walkProvider;
    ScriptedProvider scriptedProvider;
    HeadProvider headProvider;

	MotionProvider * curProvider;
	MotionProvider * nextProvider;

    const WalkingParameters *curGait;
    const WalkingParameters *nextGait;

    std::vector <float> nextJoints;

    bool running;
	bool newJoints; //Way to track if we ever use the same joints twice

	HeadJointCommand *hjc;
	HeadJointCommand *hjc2;
	HeadJointCommand *hjc3;
	std::vector<float> *bodyJoints;
	std::vector<float> *bodyJoints2;

	BodyJointCommand *command;
	BodyJointCommand *command2;
	BodyJointCommand *command3;
    static const float sitDownAngles[NUM_BODY_JOINTS];

	BodyJointCommand *getUp;
	BodyJointCommand *sitDown;


    pthread_t       switchboard_thread;
    pthread_cond_t  calc_new_joints_cond;
    pthread_mutex_t next_joints_mutex;

#ifdef DEBUG_JOINTS_OUTPUT
    FILE* joints_log;
    FILE* effector_log;
#endif

};

#endif
