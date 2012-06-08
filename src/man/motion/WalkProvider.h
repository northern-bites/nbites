
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

#ifndef _WalkProvider_h_DEFINED
#define _WalkProvider_h_DEFINED

/**
 * The walk provider is the central connection point between all the parts
 * of the walking system. It ties together the controller, and inverse
 * kinematics.
 *
 * The walk provider should be asked to calculateNextJoints each timestep
 * It in turn asks for a new CoM target from the controller, and supervises
 * the left and right `WalkingLegs` in reaching that target.
 *
 * At the end, it updates the super class' joints for each leg chain
 * which are read by the switchboard.
 */

#include <vector>

#include <boost/tuple/tuple.hpp>
#include <boost/shared_ptr.hpp>

#include "MotionProvider.h"
#include "WalkingConstants.h"
#include "StepGenerator.h"
#include "WalkCommand.h"
#include "MetaGait.h"
#include "BodyJointCommand.h"
#include "StepCommand.h"
#include "DestinationCommand.h"

#include "Profiler.h"

//NOTE: we need to get passed a reference to the switchboard so we can
//know the length of a motion frame!!
class WalkProvider : public MotionProvider {
public:
    WalkProvider(boost::shared_ptr<Sensors> s,
                 boost::shared_ptr<NaoPose> pose);
    virtual ~WalkProvider();

    void requestStopFirstInstance();
    void calculateNextJointsAndStiffnesses();

    void hardReset();

    void setCommand(const MotionCommand::ptr command)
        {
            pthread_mutex_lock(&walk_provider_mutex);
            setCommand(boost::dynamic_pointer_cast<WalkCommand>(command));
            pthread_mutex_unlock(&walk_provider_mutex);
        }
    void setCommand(const WalkCommand::ptr command);
    void setCommand(const Gait::ptr command);
    void setCommand(const StepCommand::ptr command);
    void setCommand(const DestinationCommand::ptr command);

    std::vector<BodyJointCommand::ptr> getGaitTransitionCommand();
    MotionModel getOdometryUpdate(){
        const std::vector<float> odo = stepGenerator.getOdometryUpdate();
        return MotionModel(odo[0]*MM_TO_CM,odo[1]*MM_TO_CM,odo[2]);
    }

    virtual const SupportFoot getSupportFoot() const {
        return stepGenerator.getSupportFoot();
    }

private:
    virtual void setActive();

    boost::shared_ptr<Sensors> sensors;
    boost::shared_ptr<NaoPose> pose;

    MetaGait metaGait;
    Gait nextGait;
    Gait startGait;

    StepGenerator stepGenerator;
    bool pendingCommands;
    bool pendingStepCommands;
    bool pendingDestCommands;
    bool pendingGaitCommands;
    bool pendingStartGaitCommands;

    mutable pthread_mutex_t walk_provider_mutex;
    WalkCommand::ptr nextCommand;
    StepCommand::ptr nextStepCommand;
    DestinationCommand::ptr nextDestCommand;
};

#endif

