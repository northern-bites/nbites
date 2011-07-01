
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

#include "Kinematics.h"
#include "WalkProvider.h"
#include "WalkingConstants.h"
#include "ScriptedProvider.h"
#include "HeadProvider.h"
#include "CoordHeadProvider.h"
#include "NullHeadProvider.h"
#include "NullBodyProvider.h"
#include "Sensors.h"
#include "MotionConstants.h"
#include "Profiler.h"

#include "BodyJointCommand.h"
#include "HeadJointCommand.h"
#include "WalkCommand.h"
#include "DestinationCommand.h"
#include "Gait.h"
#include "SetHeadCommand.h"
#include "CoordHeadCommand.h"

#ifdef DEBUG_MOTION
#  define DEBUG_JOINTS_OUTPUT
#endif

class MotionSwitchboard {
public:
    MotionSwitchboard(boost::shared_ptr<Sensors> s,
            boost::shared_ptr<NaoPose> pose);
    ~MotionSwitchboard();

    void start();
    void stop();
    void run();

    const std::vector <float> getNextJoints() const;
    const std::vector<float> getNextStiffness() const;
    void signalNextFrame();
    void sendMotionCommand(const HeadJointCommand::ptr command);
    void sendMotionCommand(const BodyJointCommand::ptr command);
    void sendMotionCommand(const WalkCommand::ptr command);
    void sendMotionCommand(const Gait::ptr command);
    void sendMotionCommand(const SetHeadCommand::ptr command);
    void sendMotionCommand(const CoordHeadCommand::ptr command);
    void sendMotionCommand(const FreezeCommand::ptr command);
    void sendMotionCommand(const UnfreezeCommand::ptr command);
    void sendMotionCommand(const StepCommand::ptr command);
    void sendMotionCommand(const DestinationCommand::ptr command);

public:
    void stopHeadMoves(){headProvider.requestStop();}
    void stopBodyMoves(){
	curProvider->requestStop();
    }

    bool isWalkActive(){return walkProvider.isActive();}
    bool isHeadActive(){return headProvider.isActive();}
    bool isBodyActive(){return curProvider->isActive();}

    void resetWalkProvider(){ walkProvider.hardReset(); }
    void resetScriptedProvider(){ scriptedProvider.hardReset(); }

    MotionModel getOdometryUpdate(){
        return walkProvider.getOdometryUpdate();
    }

    int getFrameCount() const { return frameCount; }

private:
    void preProcess();
    void processJoints();
    void processStiffness();
    bool postProcess();
    void preProcessHead();
    void preProcessBody();
    void processHeadJoints();
    void processBodyJoints();
    void clipHeadJoints(std::vector<float>& joints);
    void safetyCheckJoints();
    void swapBodyProvider();
    void swapHeadProvider();
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
    CoordHeadProvider coordHeadProvider;
    NullHeadProvider nullHeadProvider;
    NullBodyProvider nullBodyProvider;

    MotionProvider * curProvider;
    MotionProvider * nextProvider;

    MotionProvider * curHeadProvider;
    MotionProvider * nextHeadProvider;

    std::vector <float> sensorAngles;
    std::vector <float> nextJoints;
    std::vector <float> nextStiffnesses;
    std::vector <float> lastJoints;

    int frameCount;
    bool running;
    mutable bool newJoints; //Way to track if we ever use the same joints twice

    bool readyToSend;

    static const float sitDownAngles[Kinematics::NUM_BODY_JOINTS];

    pthread_t       switchboard_thread;
    pthread_cond_t  calc_new_joints_cond;
    mutable pthread_mutex_t calc_new_joints_mutex;
    mutable pthread_mutex_t next_provider_mutex;
    mutable pthread_mutex_t next_joints_mutex;
    mutable pthread_mutex_t stiffness_mutex;

    bool noWalkTransitionCommand;

#ifdef DEBUG_JOINTS_OUTPUT
    FILE* joints_log;
    FILE* stiffness_log;
    FILE* effector_log;
#endif

};

#endif
