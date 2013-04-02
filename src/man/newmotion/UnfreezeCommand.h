#ifndef UnfreezeCommand_h
#define UnfreezeCommand_h

#include <boost/shared_ptr.hpp>

#include "BaseFreezeCommand.h"

namespace man
{
namespace motion
{
class UnfreezeCommand : public BaseFreezeCommand 
{
public:
    typedef boost::shared_ptr<UnfreezeCommand> ptr;

    UnfreezeCommand() : BaseFreezeCommand(MotionConstants::DEFAULT_ON_STIFFNESS){}
    ~UnfreezeCommand(){};

};
} // namespace motion
} // namespace man

#endif
