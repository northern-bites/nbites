#ifndef UnfreezeCommand_h
#define UnfreezeCommand_h

#include "BaseFreezeCommand.h"

class UnfreezeCommand : public BaseFreezeCommand {
public:
    UnfreezeCommand() : BaseFreezeCommand(MotionConstants::DEFAULT_ON_STIFFNESS){}
    ~UnfreezeCommand(){};

};

#endif
