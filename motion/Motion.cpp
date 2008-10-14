

#include <boost/shared_ptr.hpp>

#include "synchro.h"
#include "Motion.h"
#include "_motionmodule.h"

#include "SimulatorEnactor.h"

#ifdef NAOQI1
Motion::Motion (ALPtr<ALMotionProxy> _proxy,shared_ptr<Synchro> _synchro, Sensors *s)
#else
Motion::Motion (ALMotionProxy * _proxy,shared_ptr<Synchro> _synchro, Sensors *s)
#endif
    : switchboard(),
      enactor(new SimulatorEnactor(&switchboard)),
      interface(&switchboard)
{
  set_motion_interface(this);
  c_init_motion();
}

Motion::~Motion() {
    delete enactor;
}

int Motion::start() {
    // Start the enactor thread
    enactor->start();
    switchboard.start();

    return 0;
}

void Motion::stop() {
    enactor->stop();
}
