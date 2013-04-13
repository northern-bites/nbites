#ifndef FreezeCommand_h
#define FreezeCommand_h

#include "BaseFreezeCommand.h"

namespace man
{
namespace motion
{
class FreezeCommand : public BaseFreezeCommand 
{
public:
    typedef boost::shared_ptr<FreezeCommand> ptr;

    FreezeCommand() : BaseFreezeCommand(MotionConstants::DEFAULT_OFF_STIFFNESS){}
    ~FreezeCommand(){};

};
} // namespace motion
} // namespace man

#endif
