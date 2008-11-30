
#ifndef _Motion_h_DEFINED
#define _Motion_h_DEFINED

#include <boost/shared_ptr.hpp>

#include "synchro.h"
#include "BodyJointCommand.h"
#include "WalkCommand.h"
#include "MotionCore.h"
#include "Sensors.h"

class Motion
  : public MotionCore
{
  public:
    Motion(boost::shared_ptr<Synchro> synchro, Sensors *s);
    virtual ~Motion() {}
};

#endif
