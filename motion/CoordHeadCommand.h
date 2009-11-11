
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

class CoordHeadCommand : public MotionCommand
{
public:
    CoordHeadCommand(const int _robotX, const int _robotY,
					 const int _cameraHeight = 0, const float _robotBearing.
					 const int _visualGoalX, const int_visualGoalY,
					 const int _visualGoalHeight,
				   const float _maxSpeedYaw =
				   Kinematics::jointsMaxVelNominal[Kinematics::HEAD_YAW],
				   const float _maxSpeedPitch =
				   Kinematics::jointsMaxVelNominal[Kinematics::HEAD_PITCH]
		)
        : MotionCommand(MotionConstants::SET_HEAD),
          relX(_visualGoalX - _robotX), relY(_visualGoalY - _robotY),
		  relHeight(_visualGoalHeight - _cameraHeight ),
		  robotBearing(_robotBearing),
		  maxSpeedYaw(_maxSpeedYaw), maxSpeedPitch(_maxSpeedPitch)
        {
            setChainList();
        }
	const float getGoalYaw() {
		//atan2(relX, relY) - robotBearing*pi/180.
	}
	const float getGoalPitch() {
		//dist = sqrt(getRelX()*getRelX(), getRelY()*getRelY())
		//pitch = atan(getRelHeight()/dist)
	}

private:

    virtual void setChainList() {
        chainList.insert(chainList.end(),
                         MotionConstants::HEAD_JOINT_CHAINS,
                         MotionConstants::HEAD_JOINT_CHAINS
                         + MotionConstants::HEAD_JOINT_NUM_CHAINS);
    }
private:
    const int relX, relY, relHeight;
	const float maxSpeedYaw, maxSpeedPitch, robotBearing;
};

#endif
