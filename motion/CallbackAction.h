
#ifndef _CallbackAction_h_DEFINED
#define _CallbackAction_h_DEFINED

#include "WalkCommand.h"
#include "MotionCore.h"

class CallbackAction
  : public WalkCommand
{
  public:
    CallbackAction();
    virtual ~CallbackAction();

    virtual void callback(MotionCore *core);
};

#endif
