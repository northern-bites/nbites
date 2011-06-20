
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

#include "JointCommand.h"
#include "ChoppedCommand.h"
#include "LinearChoppedCommand.h"
#include "SmoothChoppedCommand.h"
#include "PreviewChoppedCommand.h"
#include "Common.h"

class ChopShop
{
public:
    ChopShop(boost::shared_ptr<Sensors> s);

    ChoppedCommand::ptr chopCommand(const JointCommand *command);

private:
    boost::shared_ptr<Sensors> sensors;
    float FRAME_LENGTH_S;

    ChoppedCommand::ptr chopLinear(const JointCommand *command,
				   std::vector<float> currentJoints,
				   int numChops);

    ChoppedCommand::ptr chopSmooth(const JointCommand *command,
				   std::vector<float> currentJoints,
				   int numChops);


    std::vector<float> getCurrentJoints();

};

#endif
