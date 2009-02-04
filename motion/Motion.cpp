

#include <boost/shared_ptr.hpp>
using namespace boost;

#include "synchro.h"
#include "Motion.h"
#include "_motionmodule.h"
#include "SimulatorEnactor.h"
#include "SimulatorEnactor.h"

Motion::Motion (shared_ptr<Synchro> _synchro, MotionEnactor * _enactor, Sensors *s)
  : Thread(_synchro, "MotionCore"),
    switchboard(s),
    enactor(_enactor),
    interface(&switchboard)
{
  //Setup the callback  in the enactor so it knows to call the switchboard
  enactor->setSwitchboard(&switchboard);

  set_motion_interface(&interface);
  c_init_motion();
}

Motion::~Motion() {
    enactor->setSwitchboard(NULL);
}

int Motion::start() {
    // Start the enactor thread
    enactor->start();
    switchboard.start();

    return Thread::start();
}

void Motion::stop() {
    enactor->stop();
    switchboard.stop();
    Thread::stop();
}

void Motion::run(){
    cout <<"Motion::run"<<endl;
    //Ensure the lastest joint values are ready before we start the switchboard
    enactor->postSensors();
    switchboard.run();
}
