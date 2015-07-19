#pragma once

#include "MotionConstants.h"
#include "MotionCommand.h"

namespace man
{
namespace motion
{

class KickCommand : public MotionCommand
{
public:
    typedef boost::shared_ptr<KickCommand> ptr;

    KickCommand(int kick_type, int time)
        : MotionCommand(MotionConstants::KICK),
          kickType(kick_type),
          timeStamp(time)
    {
        setChainList();
    }

    virtual ~KickCommand() {}

public:
    int kickType;
    int timeStamp;

    // friend std::ostream& operator<< (std::ostream &o, const KickCommand &w)
    // {
    //     return o << "KickCommand(" << 
    // }

protected:
    virtual void setChainList() {
        chainList.assign(MotionConstants::WALK_CHAINS,
                         MotionConstants::WALK_CHAINS +
                         MotionConstants::WALK_NUM_CHAINS);
    }
};

} // namespace motion
} // namespace man
