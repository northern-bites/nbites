
// This file is part of Man, a robotic perception, locomotion, and
// team strategy application created by the Northern Bites RoboCup
// team of Bowdoin College in Brunswick, Maine, for the Aldebaran
// Nao robot.
//
// Man is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Man is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser Public License for more details.
//
// You should have received a copy of the GNU General Public License
// and the GNU Lesser Public License along with Man.  If not, see
// <http://www.gnu.org/licenses/>.

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
                 const float _hox,
                 const float _yao,
                 const float _dur,
                 const float _dblSupFrac, const float _stepHeight,
                 const float _footLengthX, const float _dblInactivePerc,
                 const float _lSwHRAdd,const float _rSwHRAdd,
                 const float _lZMPSwOffY,const float _rZMPSwOffY,
                 const float maxx_mms, const float maxy_mms,
                 const float maxtheta_rads)
        :MotionCommand(MotionConstants::GAIT),
         gait(_motion_frame_length_s,_bh, //HACK
              _hox, _yao, _dur,
              _dblSupFrac, _stepHeight,
              _footLengthX, _dblInactivePerc,
              _lSwHRAdd,_rSwHRAdd,
              _lZMPSwOffY,_rZMPSwOffY,
              maxx_mms,maxy_mms,maxtheta_rads)
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
