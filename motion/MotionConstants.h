
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


#ifndef _MotionConstants_h_DEFINED
#define _MotionConstants_h_DEFINED
#include "Kinematics.h"

namespace MotionConstants {

	enum MotionType {
		WALK = 0,
		BODY_JOINT,
		HEAD_JOINT,
        GAIT,
        SET_HEAD
	};

	const static unsigned int WALK_NUM_CHAINS = Kinematics::NUM_BODY_CHAINS;
	const static unsigned int BODY_JOINT_NUM_CHAINS = Kinematics::NUM_BODY_CHAINS;
	const static unsigned int HEAD_JOINT_NUM_CHAINS = 1;
    const static unsigned int GAIT_NUM_CHAINS = Kinematics::NUM_BODY_CHAINS;

	const static int WALK_CHAINS[WALK_NUM_CHAINS] =
      {Kinematics::LARM_CHAIN,
       Kinematics::LLEG_CHAIN,
       Kinematics::RLEG_CHAIN,
       Kinematics::RARM_CHAIN };

    const static int GAIT_CHAINS[GAIT_NUM_CHAINS] =
      { Kinematics::LARM_CHAIN,
        Kinematics::LLEG_CHAIN,
        Kinematics::RLEG_CHAIN,
        Kinematics::RARM_CHAIN };

	const static int BODY_JOINT_CHAINS[BODY_JOINT_NUM_CHAINS] =
    { Kinematics::LARM_CHAIN,
      Kinematics::LLEG_CHAIN,
      Kinematics::RLEG_CHAIN,
      Kinematics::RARM_CHAIN };

	const static int HEAD_JOINT_CHAINS[HEAD_JOINT_NUM_CHAINS] =
      { Kinematics::HEAD_CHAIN };

};
#endif
