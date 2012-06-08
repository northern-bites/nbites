#ifndef UnfreezeCommand_h
#define UnfreezeCommand_h

#include <boost/shared_ptr.hpp>

#include "BaseFreezeCommand.h"

class UnfreezeCommand : public BaseFreezeCommand {
public:
    typedef boost::shared_ptr<UnfreezeCommand> ptr;

    UnfreezeCommand() : BaseFreezeCommand(MotionConstants::DEFAULT_ON_STIFFNESS){}
    ~UnfreezeCommand(){};

};

#endif
