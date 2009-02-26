
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

#ifndef _SetHeadCommand_h_DEFINED
#define _SetHeadCommand_h_DEFINED

class SetHeadCommand : public MotionCommand
{
public:
    SetHeadCommand(const float _yaw, const float _pitch)
        : MotionCommand(MotionConstants::SET_HEAD),
          yaw(_yaw),pitch(_pitch)
        {
            setChainList();
        }
    const float getYaw() const {return yaw;}
    const float getPitch() const {return pitch;}
private:
    virtual void setChainList() {
        chainList.insert(chainList.end(),
                         MotionConstants::HEAD_JOINT_CHAINS,
                         MotionConstants::HEAD_JOINT_CHAINS
                         + MotionConstants::HEAD_JOINT_NUM_CHAINS);
    }
private:
    const float yaw,pitch;
};

#endif
