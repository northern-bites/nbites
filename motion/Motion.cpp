

#include <boost/shared_ptr.hpp>

#include "synchro.h"
#include "Motion.h"
#include "_motionmodule.h"

using namespace boost;

Motion::Motion (shared_ptr<Synchro> _synchro, Sensors *s)
  : MotionCore(_synchro, s)
{
  set_motion_interface(this);
  c_init_motion();
}
