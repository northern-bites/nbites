
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

#include "ChopShop.h"
#include "Kinematics.h"

using namespace std;

using namespace boost;

using namespace Kinematics;

ChopShop::ChopShop (shared_ptr<Sensors> s)
    : sensors(s)
{

}

// Breaks command into FRAME_LENGTH_S size pieces,
// adds it to the queue
/*************************************************************************/
/*******  THIS WILL DELETE THE JOINT COMMAND PASSED TO IT!   *************/
/*************************************************************************/
ChoppedCommand::ptr ChopShop::chopCommand(const JointCommand *command) {
    ChoppedCommand::ptr chopped;
    int numChops = 1;
    if (command->getDuration() > MOTION_FRAME_LENGTH_S) {
	numChops = static_cast<int>(command->getDuration() / MOTION_FRAME_LENGTH_S);
    }

    vector<float> currentJoints = getCurrentJoints();

    if (command->getInterpolation() == INTERPOLATION_LINEAR) {
	chopped = chopLinear(command, currentJoints, numChops);
    }

    else if (command->getInterpolation() == INTERPOLATION_SMOOTH) {
	chopped =  chopSmooth(command, currentJoints, numChops);
    }

    else {
	cout << "ILLEGAL INTERPOLATION VALUE. CHOPPING SMOOTHLY" << endl;
	chopped = chopSmooth(command, currentJoints, numChops) ;
    }

    // for CoM preview estimates
    ChoppedCommand::ptr preview ( new PreviewChoppedCommand(chopped) );

    // Deleting command!
    delete command;
    return preview;
}

//Smooth interpolation motion
ChoppedCommand::ptr ChopShop::chopSmooth(const JointCommand *command,
					 vector<float> currentJoints, int numChops) {
    return ChoppedCommand::ptr  ( new SmoothChoppedCommand(
				      command,
				      currentJoints,
				      numChops ) );
}

/*
 * Linear interpolation chopping:
 * Retrieves current joint angels and acquiries the differences
 * between the current and the intended final. Send them to
 */
ChoppedCommand::ptr ChopShop::chopLinear(const JointCommand *command,
					 vector<float> currentJoints,
					 int numChops) {
    return ChoppedCommand::ptr ( new LinearChoppedCommand(
				     command,
				     currentJoints,
				     numChops ) );
}

vector<float> ChopShop::getCurrentJoints() {
    return sensors->getMotionBodyAngles();
}
