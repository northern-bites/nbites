
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
#include <iostream>
using namespace std;

#include "Motion.h"
#include "PyMotion.h"

//#include "NaoEnactor.h"

Motion::Motion (boost::shared_ptr<MotionEnactor> _enactor,
                boost::shared_ptr<Sensors> s,
                boost::shared_ptr<NaoPose> _pose,
                man::memory::MMotion::ptr mMotion)
  : Thread("Motion"),
    switchboard(s, _pose, mMotion),
    interface(&switchboard),
    enactor(_enactor),
    pose(_pose)
{
    set_motion_interface(&interface);
}

Motion::~Motion() {
    cout << "Motion destructor" << endl;
    enactor->resetSwitchboard();
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
    //Setup the callback  in the enactor so it knows to call the switchboard
    enactor->setSwitchboard(&switchboard);
    switchboard.run();
}
