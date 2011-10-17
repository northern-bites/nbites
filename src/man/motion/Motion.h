
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
 * Motion is a module-level connection between all the core aspects of the
 * motion module.
 *
 * It has the following jobs:
 *  - Create a switchboard, which will select between walk engine, chop engine,
 *    etc.
 *  - Create the approriate enactor, which is the layer that communicates
 *    joint angles between the switchboard and the target platform (e.g. robot,
 *    simulator)
 *  - Instantiate a MotionInterface for PyMotion to use to communicate with
 *    Cpp motion module.
 *
 * The 'Motion' thread is really the switchboard thread (see MotionSwitchboard
 * for details). The enactor will also create its own thread, which is dependant
 * on the platform (i.e. simulator or real robot)
 */
#ifndef _Motion_h_DEFINED
#define _Motion_h_DEFINED

#include <boost/shared_ptr.hpp>

#include "MotionSwitchboard.h"
#include "MotionEnactor.h"
#include "MotionInterface.h"
#include "Sensors.h"
#include "NaoPose.h"
#include "synchro/synchro.h"

class Motion : public Thread
{
public:
    Motion(boost::shared_ptr<MotionEnactor> _enactor,
           boost::shared_ptr<Sensors> s,
           boost::shared_ptr<NaoPose> _pose);
    virtual ~Motion();

    int start();
    void stop();
    void run();
    MotionInterface * getInterface(){return &interface;}
private:
    MotionSwitchboard switchboard;
    MotionInterface interface;

    boost::shared_ptr<MotionEnactor> enactor;
    boost::shared_ptr<NaoPose> pose;
};

#endif
