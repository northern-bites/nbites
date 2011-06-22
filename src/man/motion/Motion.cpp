
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

#include <boost/shared_ptr.hpp>
using namespace boost;

#include "synchro.h"
#include "Motion.h"
#include "PyMotion.h"
//#include "NaoEnactor.h"

Motion::Motion (shared_ptr<Synchro> _synchro,
                shared_ptr<MotionEnactor> _enactor,
                shared_ptr<Sensors> s,
                shared_ptr<Profiler> p,
                shared_ptr<NaoPose> _pose)
  : Thread(_synchro, "Motion"),
    switchboard(s,p,_pose),
    interface(&switchboard),
    enactor(_enactor),
    profiler(p),
    pose(_pose)
{
    set_motion_interface(&interface);
}

Motion::~Motion() {
    enactor->setSwitchboard(NULL);
}
int Motion::start() {
    switchboard.start();

    return Thread::start();
}

void Motion::stop() {
    switchboard.stop();
    Thread::stop();
}

void Motion::run(){
    std::cout <<"Motion::run"<< std::endl;
    Thread::trigger->on();

    //Setup the callback  in the enactor so it knows to call the switchboard
    enactor->setSwitchboard(&switchboard);

    switchboard.run();
    Thread::trigger->off();
}
