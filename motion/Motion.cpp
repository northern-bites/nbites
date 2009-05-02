
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
#include "NaoEnactor.h"

template <class EnactorType>
Motion<EnactorType>::Motion (shared_ptr<Synchro> _synchro,
                   shared_ptr<EnactorType> _enactor,
                   shared_ptr<Sensors> s)
  : Thread(_synchro, "Motion"),
    switchboard(s),
    enactor(_enactor),
    interface(&switchboard)
{

    set_motion_interface(&interface);
}

template <class EnactorType>
Motion<EnactorType>::~Motion() {
    enactor->setSwitchboard(NULL);
}

/********* Special definition of the start method for Nao Enactor *********/

template <>
int Motion<NaoEnactor>::start() {
    // Note: no need to call enactor->start() !
    switchboard.start();
    //Setup the callback  in the enactor so it knows to call the switchboard
    enactor->setSwitchboard(&switchboard);

    return Thread::start();
}

template <>
void Motion<NaoEnactor>::stop() {
    switchboard.stop();
    Thread::stop();
}


/********* General definition of the start method for other enactors *********/

template <class EnactorType>
int Motion<EnactorType>::start() {
    enactor->start();
    switchboard.start();
    //Setup the callback  in the enactor so it knows to call the switchboard
    enactor->setSwitchboard(&switchboard);

    return Thread::start();
}

template <class EnactorType>
void Motion<EnactorType>::stop() {
    enactor->stop();
    switchboard.stop();
    Thread::stop();
}

template <class EnactorType>
void Motion<EnactorType>::run(){
    cout <<"Motion::run"<<endl;
    switchboard.run();
    Thread::trigger->off();
}


template class Motion<NaoEnactor>;
template class Motion<ALEnactor>;
