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
using namespace std;

#include "motionconfig.h" // for cmake set debugging flags like MOTION_DEBUG

#include "Kinematics.h"
#include "WalkProvider.h"
#include "ScriptedProvider.h"
#include "HeadProvider.h"
#include "Sensors.h"

#include "BodyJointCommand.h"
#include "HeadJointCommand.h"

#ifdef DEBUG_MOTION
#  define DEBUG_JOINTS_OUTPUT
#endif

class MotionSwitchboard {
public:
    MotionSwitchboard(Sensors *s);
    ~MotionSwitchboard();

    void start();
    void stop();
    void run();

    const vector <float> getNextJoints();

private:
    int processProviders();

#ifdef DEBUG_JOINTS_OUTPUT
    void initDebugLogs();
    void closeDebugLogs();
    void updateDebugLogs();
#endif

private:
    Sensors *sensors;
    WalkProvider walkProvider;
	ScriptedProvider scriptedProvider;
	HeadProvider headProvider;
    vector <float> nextJoints;

    bool running;
	const HeadJointCommand *hjc;
	const HeadJointCommand *hjc2;
	const HeadJointCommand *hjc3;
	vector<float> *bodyJoints;
	vector<float> *bodyJoints2;

	const BodyJointCommand *command;
	const BodyJointCommand *command2;
	const BodyJointCommand *command3;
    static const float sitDownAngles[NUM_BODY_JOINTS];
	const BodyJointCommand *getUp;
	const BodyJointCommand *sitDown;

    pthread_t       switchboard_thread;
    pthread_cond_t  calc_new_joints_cond;
    pthread_mutex_t next_joints_mutex;

#ifdef DEBUG_JOINTS_OUTPUT
    FILE* joints_log;
    FILE* effector_log;
#endif
};

#endif
