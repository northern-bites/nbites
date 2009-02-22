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
    GaitCommand( const float _motion_frame_length_s, const float _bh,
                 const float _hox, const float _dur,
                 const float _dblSupFrac, const float _stepHeight,
                 const float _footLengthX, const float _dblInactivePerc,
                 const float _lSwHRAdd,const float _rSwHRAdd,
                 const float _lZMPSwOffY,const float _rZMPSwOffY)
        :MotionCommand(MotionConstants::GAIT),
         gait(_motion_frame_length_s,_bh, //HACK
              _hox, _dur,
              _dblSupFrac, _stepHeight,
              _footLengthX, _dblInactivePerc,
              _lSwHRAdd,_rSwHRAdd,
              _lZMPSwOffY,_rZMPSwOffY)
        {
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
