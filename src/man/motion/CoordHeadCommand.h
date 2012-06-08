
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

#ifndef _CoordHeadCommand_h_DEFINED
#define _CoordHeadCommand_h_DEFINED

#include "Kinematics.h"
#include "NaoPose.h"
#include <cmath>
#include <iostream>

class CoordHeadCommand : public MotionCommand
{
public:
    typedef boost::shared_ptr<CoordHeadCommand> ptr;

    //_x, _y relative to robot center in centimeters
    CoordHeadCommand(const float _x, const float _y, const float _z,
                     //_z relative to ground in centimeters
                     const float _maxSpeedYaw =
                     Kinematics::jointsMaxVelNominal[Kinematics::HEAD_YAW],
                     const float _maxSpeedPitch =
                     Kinematics::jointsMaxVelNominal[Kinematics::HEAD_PITCH]
        )
        : MotionCommand(MotionConstants::COORD_HEAD),

          // We must convert the given position to millimeters for
          // use with pose information
          relX(_x * CM_TO_MM), relY(_y * CM_TO_MM), relZ(_z * CM_TO_MM),
          maxSpeedYaw(_maxSpeedYaw), maxSpeedPitch(_maxSpeedPitch)
        {
            setChainList();
        }
    const float getRelX() const {return relX;}
    const float getRelY() const {return relY;}
    const float getRelZ() const {return relZ;}
    const float getMaxSpeedYaw() const {return maxSpeedYaw;}
    const float getMaxSpeedPitch() const {return maxSpeedPitch;}
private:
    virtual void setChainList() {
        chainList.insert(chainList.end(),
                         MotionConstants::HEAD_JOINT_CHAINS,
                         MotionConstants::HEAD_JOINT_CHAINS
                         + MotionConstants::HEAD_JOINT_NUM_CHAINS);
    }
    const float relX, relY, relZ;
    const float maxSpeedYaw,maxSpeedPitch;
};

#endif
