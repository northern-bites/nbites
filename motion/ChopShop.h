
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

#ifndef _ChopShop_h_DEFINED
#define _ChopShop_h_DEFINED

#include <vector>
#include <boost/shared_ptr.hpp>

#include "Sensors.h"
#include "BodyJointCommand.h"
#include "Kinematics.h"
#include "JointCommand.h"
#include "ChoppedCommand.h"

class ChopShop
{
public:
	ChopShop(boost::shared_ptr<Sensors> s, float motionFrameLength);

	ChoppedCommand * chopCommand(const JointCommand *command);

private:
	// Inside most vector: joint values for a chain
	// Next: vector for each choppped move (holding each chain,
	// instead of just holding the joint values in a row)
	// Outside: vector to hold all the chopped moves
    boost::shared_ptr<Sensors> sensors;
	float FRAME_LENGTH_S;

// 	ChoppedCommand * chopSmooth(const JointCommand *command);
	ChoppedCommand * chopLinear(const JointCommand *command);

	std::vector<float> getCurrentJoints();
	std::vector<float> getFinalJoints(const JointCommand *command,
                                      std::vector<float> *currentJoints);

	std::vector<float> getDiffPerChop(int numChops,
                                      std::vector<float> *current,
                                      std::vector<float> *final);
	void vectorToRad(std::vector<float> *vect);

	ChoppedCommand * buildChops(int numChops,
								std::vector<float> *currentJoints,
								std::vector<float> *diffPerChop,
								const JointCommand *command);
};

#endif
