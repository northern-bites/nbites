#ifndef OffFreezeCommand_h
#define OffFreezeCommand_h

#include "FreezeCommand.h"

class OffFreezeCommand : public FreezeCommand {
public:
    OffFreezeCommand() : FreezeCommand(MotionConstants::DEFAULT_ON_STIFFNESS){}
    ~OffFreezeCommand(){};

};

#endif
