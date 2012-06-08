
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
#include "Common.h"

namespace MotionConstants {

	enum MotionType {
		WALK = 0,
        STEP,
		DESTINATION,
		BODY_JOINT,
		HEAD_JOINT,
        GAIT,
        SET_HEAD,
		COORD_HEAD,
        STIFFNESS,
        FREEZE,
        UNFREEZE
	};

	const static unsigned int WALK_NUM_CHAINS = Kinematics::NUM_BODY_CHAINS;
	const static unsigned int STEP_NUM_CHAINS = Kinematics::NUM_BODY_CHAINS;
	const static unsigned int BODY_JOINT_NUM_CHAINS = Kinematics::NUM_BODY_CHAINS;
	const static unsigned int HEAD_JOINT_NUM_CHAINS = 1;
    const static unsigned int GAIT_NUM_CHAINS = Kinematics::NUM_BODY_CHAINS;
    const static unsigned int STIFFNESS_NUM_CHAINS = Kinematics::NUM_CHAINS;
    const static unsigned int FREEZE_NUM_CHAINS = Kinematics::NUM_CHAINS;


	const static int WALK_CHAINS[WALK_NUM_CHAINS] =
      {Kinematics::LARM_CHAIN,
       Kinematics::LLEG_CHAIN,
       Kinematics::RLEG_CHAIN,
       Kinematics::RARM_CHAIN };

	const static int STEP_CHAINS[STEP_NUM_CHAINS] =
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

    const static int STIFFNESS_CHAINS[STIFFNESS_NUM_CHAINS] =
      { Kinematics::HEAD_CHAIN,
        Kinematics::LARM_CHAIN,
        Kinematics::LLEG_CHAIN,
        Kinematics::RLEG_CHAIN,
        Kinematics::RARM_CHAIN };

    const static int FREEZE_CHAINS[FREEZE_NUM_CHAINS] =
      { Kinematics::HEAD_CHAIN,
        Kinematics::LARM_CHAIN,
        Kinematics::LLEG_CHAIN,
        Kinematics::RLEG_CHAIN,
        Kinematics::RARM_CHAIN };

    static const float DEFAULT_ON_STIFFNESS = 0.85f;
    static const float DEFAULT_OFF_STIFFNESS = -1.0f;
    static const float MAX_STIFFNESS = 1.0f;
    static const float MIN_STIFFNESS = 0.0f;
    static const float NO_STIFFNESS = -1.0f;

    static const bool null_head_mask[Kinematics::NUM_CHAINS] =
    {true, false, false, false, false};
    static const bool null_body_mask[Kinematics::NUM_CHAINS] =
    {false, true, true, true, true};

    static const float SIT_DOWN_ANGLES[] =
    { 0.0f, 0.0f,
     1.57f, 0.0f, -1.13f, -1.0f,
      0.0f, 0.0f, -0.96f,  2.18f,
    -1.22f, 0.0f,  0.0f,   0.0f,
    -0.96f, 2.18f,-1.22f,  0.0f,
     1.57f, 0.0f,  1.13f,  1.01f};

    static const std::vector<float> OFF_STIFFNESSES(
                             Kinematics::NUM_JOINTS,
                             MotionConstants::DEFAULT_OFF_STIFFNESS);
};
#endif
