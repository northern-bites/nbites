#ifndef OnFreezeCommand_h
#define OnFreezeCommand_h

#include "FreezeCommand.h"

class OnFreezeCommand : public FreezeCommand {
public:
    OnFreezeCommand() : FreezeCommand(MotionConstants::DEFAULT_ON_STIFFNESS){}
    ~OnFreezeCommand(){};

};

#endif
