
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

#ifndef _HeadJointCommand_h_DEFINED
#define _HeadJointCommand_h_DEFINED

/**
 * Class to store joint commands specifically for the head.
 * Pass in a vector with target angles and the amount of time the robot needs
 * to take to get there. If the vector pointer is NULL, then that means we
 * are queueing up idle time on the head.
 */

#include <vector>
#include <string>

//#include "WalkCommand.h"
#include "Kinematics.h"
#include "JointCommand.h"
#include "MotionConstants.h"

using namespace MotionConstants;
using namespace Kinematics;

class HeadJointCommand : public JointCommand {
public:
	HeadJointCommand(const float time, const std::vector<float> *joints,
					 const Kinematics::InterpolationType _type);
	HeadJointCommand(const HeadJointCommand &other);
	virtual ~HeadJointCommand();

	// TODO @JGM NEED A CHECK ON THE CHAINID REQUEST
	virtual const std::vector<float>* getJoints(ChainID chain) const {
		if (chain == HEAD_CHAIN) {
			return headJoints;
		} else {
			return &noJoints;
		}
	}
private:
	//const skew_function skew_func;
	const std::vector<float> *headJoints;
	virtual void setChainList();
	const std::vector<float> noJoints;
};

#endif
