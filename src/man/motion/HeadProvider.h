
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

#ifndef _HeadProvider_h_DEFINED
#define _HeadProvider_h_DEFINED

#include <vector>
#include <queue>
#include <boost/shared_ptr.hpp>

#include "MotionProvider.h"
#include "HeadJointCommand.h"
#include "SetHeadCommand.h"
#include "CoordHeadCommand.h"
#include "Sensors.h"
#include "ChopShop.h"
#include "Kinematics.h"

class HeadProvider : public MotionProvider {
public:
    HeadProvider(boost::shared_ptr<Sensors> s,
                 boost::shared_ptr<NaoPose> p);
    virtual ~HeadProvider();

    void requestStopFirstInstance();
    void calculateNextJointsAndStiffnesses();

    void hardReset();

    void enqueueSequence(std::vector<HeadJointCommand::ptr> &seq);
    void setCommand(const HeadJointCommand::ptr command);
    void setCommand(const SetHeadCommand::ptr command);
    void setCommand(const CoordHeadCommand::ptr command);

private:
    enum HeadMode {
        SCRIPTED,
        SET
    };

    void transitionTo(HeadMode newMode);
    void setMode();
    void scriptedMode();
    void stopScripted();
    void stopSet();

    void setActive();
    bool isDone();

    boost::shared_ptr<Sensors> sensors;
    boost::shared_ptr<NaoPose> pose;
    ChopShop chopper;
    std::vector< std::vector<float> > nextJoints;

    ChoppedCommand::ptr currChoppedCommand;
    AbstractCommand::ptr currHeadCommand;
    // Queue of all future commands
    std::queue<HeadJointCommand::ptr> headCommandQueue;

    HeadMode curMode;
    float yawDest,pitchDest,lastYawDest,lastPitchDest;
    float pitchMaxSpeed, yawMaxSpeed;
    float headSetStiffness;

    pthread_mutex_t head_provider_mutex;

    std::vector<float> getCurrentHeads();
    void setNextHeadCommand();

    static const float SPEED_CLIP_FACTOR = 0.1f;
};

#endif
