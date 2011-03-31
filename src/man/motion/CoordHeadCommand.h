
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
//#include "NBMath.h"
//include math

class CoordHeadCommand : public SetHeadCommand
{
 public:
 CoordHeadCommand(const float _x, const float _y,
		  //_x, _y relative to robot center
		  const float _z,
		  //_z relative to ground
		  NaoPose pose,
		  const float _maxSpeedYaw =
		  Kinematics::jointsMaxVelNominal[Kinematics::HEAD_YAW],
		  const float _maxSpeedPitch = 
		  Kinematics::jointsMaxVelNominal[Kinematics::HEAD_PITCH]
		  )
   : SetHeadCommand(atan(_y/_x-pose.getFocalPointInWorldFrameX()),
		    atan((pose.getFocalPointInWorldFrameZ()-_z)/sqrt(pow(_x-pose.getFocalPointInWorldFrameX(),2) + pow(_y-pose.getFocalPointInWorldFrameY(),2)))-((static_cast<float>(3.141592))*40.0/180.0f), //adjust by constant angle for lower camera
		    _maxSpeedYaw, _maxSpeedPitch)
    {
      setChainList();
    }
 private:
  virtual void setChainList() {
    chainList.insert(chainList.end(),
		     MotionConstants::HEAD_JOINT_CHAINS,
		     MotionConstants::HEAD_JOINT_CHAINS
		     + MotionConstants::HEAD_JOINT_NUM_CHAINS);
  }
  //const float CAMERA_ANGLE = NBMath::TO_RAD*40.0; // from reddoc
};

#endif
