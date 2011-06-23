
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

#ifndef _ScriptedProvider_h_DEFINED
#define _ScriptedProvider_h_DEFINED

#include <vector>
#include <queue>
#include <boost/shared_ptr.hpp>

#include "MotionProvider.h"
#include "BodyJointCommand.h"
#include "Sensors.h"
#include "ChopShop.h"
#include "ChoppedCommand.h"
#include "Kinematics.h"

#include "Profiler.h"


class ScriptedProvider : public MotionProvider {
public:
    ScriptedProvider(boost::shared_ptr<Sensors> s,
		     boost::shared_ptr<Profiler> p);
    virtual ~ScriptedProvider();

    void requestStopFirstInstance();
    void calculateNextJointsAndStiffnesses();

    void hardReset();

    void enqueueSequence(std::vector<BodyJointCommand::ptr> &seq);
    void setCommand(const BodyJointCommand::ptr command);

private:
    boost::shared_ptr<Sensors> sensors;
    ChopShop chopper;
    std::vector<std::vector<float> > nextJoints;

    // The current chopped command which is being enacted
    ChoppedCommand::ptr currCommand;

    // Queue to hold the next body commands
    std::queue<BodyJointCommand::ptr> bodyCommandQueue;

    pthread_mutex_t scripted_mutex;

    boost::shared_ptr<std::vector <std::vector <float> > > getCurrentChains();

    void setNextBodyCommand();
    void setActive();
    bool isDone();
    bool currCommandEmpty();
    bool commandQueueEmpty();

};

#endif
