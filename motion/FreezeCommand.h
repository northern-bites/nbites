#ifndef FreezeCommand_h
#define FreezeCommand_h

#include "BaseFreezeCommand.h"

class FreezeCommand : public BaseFreezeCommand {
public:
    FreezeCommand() : BaseFreezeCommand(MotionConstants::DEFAULT_OFF_STIFFNESS){}
    ~FreezeCommand(){};

};

#endif
