
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

#ifndef _CoordHeadProvider_h_DEFINED
#define _ScriptedProvider_h_DEFINED

#include <vector>
#include <queue>
#include <boost/shared_ptr.hpp>

#include "MotionProvider.h"
#include "BodyJointCommand.h"
#include "CoordHeadCommand.h"
#include "Sensors.h"
#include "ChopShop.h"
#include "ChoppedCommand.h"
#include "Kinematics.h"

#include "Profiler.h"
#include "NaoPose.h"


class CoordHeadProvider : public MotionProvider {
public:
    CoordHeadProvider(boost::shared_ptr<Sensors> s,
                      boost::shared_ptr<NaoPose> _pose);
    virtual ~CoordHeadProvider();

    void requestStopFirstInstance();
    void calculateNextJointsAndStiffnesses();

    void hardReset();

    void setCommand(const CoordHeadCommand::ptr command);

private:
    enum HeadMode {
        SCRIPTED,
        SET,
        COORD
    };
    void transitionTo(HeadMode newMode);
    void coordMode();
    void stopSet();

    void setActive();
    bool isDone();

    boost::shared_ptr<Sensors> sensors;
    boost::shared_ptr<NaoPose> pose;
    ChopShop chopper;
    std::vector< std::vector<float> > nextJoints;


    ChoppedCommand::ptr currCommand;
    // Queue of all future commands

    HeadMode curMode;
    float yawDest,pitchDest,lastYawDest,lastPitchDest;
    float pitchMaxSpeed, yawMaxSpeed;
    float headSetStiffness;

    pthread_mutex_t coord_head_provider_mutex;

    std::vector<float> getCurrentHeads();
    void setNextHeadCommand();

    //constants
    //lower camera
    static const float CAMERA_ANGLE = 0.6981f;
    //clip constants for setCommand()
    static const float YAW_CLIP = 1.9f;
    static const float PITCH_MIN_CLIP = -0.65f;
    static const float PITCH_MAX_CLIP = 0.5f;
    static const float SPEED_CLIP_FACTOR = 0.35f;
};

#endif
