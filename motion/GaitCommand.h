#ifndef _GaitCommand_h_DEFINED
#define _GaitCommand_h_DEFINED

#include "WalkingConstants.h"
#include "MotionConstants.h"
class GaitCommand : public MotionCommand
{
public:
    GaitCommand(GaitID _gid)
        : MotionCommand(MotionConstants::GAIT),gait(WALK_PARAMS[_gid])
        {
            setChainList();
        }
    const WalkingParameters getGait() const {return gait;}
private:
    virtual void setChainList() {
        chainList.insert(chainList.end(),
                         MotionConstants::BODY_JOINT_CHAINS,
                         MotionConstants::BODY_JOINT_CHAINS
                         + MotionConstants::BODY_JOINT_NUM_CHAINS);
    }
private:
    const WalkingParameters gait;
};

#endif
