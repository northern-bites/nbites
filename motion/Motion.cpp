

#include <boost/shared_ptr.hpp>

#include "synchro.h"
#include "Motion.h"
#include "_motionmodule.h"

//using namespace boost;

#ifdef NAOQI1
Motion::Motion (ALPtr<ALMotionProxy> _proxy,shared_ptr<Synchro> _synchro, Sensors *s)
#else
Motion::Motion (ALMotionProxy * _proxy,shared_ptr<Synchro> _synchro, Sensors *s)
#endif
    : MotionCore(_proxy,_synchro, s)
{
  set_motion_interface(this);
  c_init_motion();
}
