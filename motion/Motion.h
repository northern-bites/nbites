
#ifndef _Motion_h_DEFINED
#define _Motion_h_DEFINED

#include "BodyJointCommand.h"
#include "WalkCommand.h"
#include "MotionCore.h"
#include "Sensors.h"

class Motion
  : public MotionCore
{
  public:
    Motion(Sensors *s);
    virtual ~Motion() {}
};

#endif
