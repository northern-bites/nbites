
// This file is part of Man, a robotic perception, locomotion, and
// team strategy application created by the Northern Bites RoboCup
// team of Bowdoin College in Brunswick, Maine, for the Aldebaran
// Nao robot.
//
// Man is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Man is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser Public License for more details.
//
// You should have received a copy of the GNU General Public License
// and the GNU Lesser Public License along with Man.  If not, see
// <http://www.gnu.org/licenses/>.

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

#include "MCL.h"
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
#include "StiffnessCommand.h"

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

	const std::vector <float> getNextJoints() const;
	const std::vector<float> getNextStiffness() const;
    const bool hasNewStiffness() const;
    void signalNextFrame();
	void sendMotionCommand(const BodyJointCommand* command);
	void sendMotionCommand(const HeadJointCommand* command);
	void sendMotionCommand(const WalkCommand* command);
	void sendMotionCommand(const GaitCommand* command);
	void sendMotionCommand(const SetHeadCommand* command);
	void sendMotionCommand(const StiffnessCommand* command);

public:
    void stopHeadMoves(){headProvider.requestStop();}

    bool isWalkActive(){return walkProvider.isActive();}
    bool isHeadActive(){return headProvider.isActive();}
    bool isBodyActive(){return curProvider->isActive();}

    void resetWalkProvider(){ walkProvider.hardReset(); }

    MotionModel getOdometryUpdate(){
        return walkProvider.getOdometryUpdate();
    }

private:
    int processProviders();
    int processStiffness();
    void swapBodyProvider();
    BodyJointCommand * getGaitTransitionCommand(const WalkingParameters * new_gait);
    int realityCheckJoints();

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
    std::vector <float> nextStiffness;

    bool running;
	mutable bool newJoints; //Way to track if we ever use the same joints twice
    mutable bool newStiffness;

    bool readyToSend;

    std::list<const StiffnessCommand *> stiffnessRequests;

    static const float sitDownAngles[Kinematics::NUM_BODY_JOINTS];

    pthread_t       switchboard_thread;
    pthread_cond_t  calc_new_joints_cond;
    mutable pthread_mutex_t calc_new_joints_mutex;
    mutable pthread_mutex_t next_joints_mutex;
    mutable pthread_mutex_t stiffness_mutex;

#ifdef DEBUG_JOINTS_OUTPUT
    FILE* joints_log;
    FILE* effector_log;
#endif

};

#endif
