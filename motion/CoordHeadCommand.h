
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

class CoordHeadCommand : public MotionCommand
{
public:
    CoordHeadCommand(const float _xRelMe, const float _yRelMe,
					 const float _relHeight,
					 const float _maxSpeedYaw =
					 Kinematics::jointsMaxVelNominal[Kinematics::HEAD_YAW],
					 const float _maxSpeedPitch =
					 Kinematics::jointsMaxVelNominal[Kinematics::HEAD_PITCH]
		)
        : MotionCommand( MotionConstants::COORD_HEAD ),
          relX( _xRelMe),
		  relY( _yRelMe),
		  relHeight( _relHeight),
		  maxSpeedYaw( _maxSpeedYaw ),
		  maxSpeedPitch( _maxSpeedPitch ),
		  yaw(calcYaw()),
		  pitch(calcPitch())
        {
            setChainList();
        }
	const float getMaxSpeedYaw() const {return maxSpeedYaw; }
	const float getMaxSpeedPitch() const {return maxSpeedPitch; }
	const float getPitch() const { return pitch; }
	const float getYaw() const { return yaw; }
private:
	const float calcYaw() {
		float yawTemp = atan2( relY, relX );
		return yawTemp;
	}
	const float calcPitch() {
		float groundDist = sqrt( relX * relX + relY * relY );
		if (groundDist == 0.0){
			groundDist = 0.1;
		}
		//b/c groundDist is always positive, no need for atan2
		float pitchTemp = atan( relHeight / groundDist);
		//b/c we use lower angled camera we need to adjust by constant angle
		pitchTemp = pitchTemp - 0.6981;//40 degrees to radians (from reddoc)
		return pitchTemp;
	}

    virtual void setChainList() {
        chainList.insert(chainList.end(),
                         MotionConstants::HEAD_JOINT_CHAINS,
                         MotionConstants::HEAD_JOINT_CHAINS
                         + MotionConstants::HEAD_JOINT_NUM_CHAINS);
    }
private:
    const float relX, relY, relHeight;
	const float maxSpeedYaw, maxSpeedPitch;
	const float yaw, pitch;
};

#endif
