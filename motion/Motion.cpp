

#include "Motion.h"
#include "_motionmodule.h"

Motion::Motion (Sensors *s)
  : MotionCore(s)
{
  set_motion_interface(this);
  c_init_motion();
}
