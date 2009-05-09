
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

#include <iostream>
#include <list>
#include "ChopShop.h"

using namespace std;

using namespace boost;

using namespace Kinematics;

ChopShop::ChopShop (shared_ptr<Sensors> s, float motionFrameLength)
	: sensors(s),
	  FRAME_LENGTH_S(motionFrameLength)
{

}

// Breaks command into FRAME_LENGTH_S size pieces,
// adds it to the queue
/*************************************************************************/
/*******  THIS WILL DELETE THE JOINT COMMAND PASSED TO IT!   *************/
/*************************************************************************/
shared_ptr<ChoppedCommand>
ChopShop::chopCommand(const JointCommand *command) {
	shared_ptr<ChoppedCommand> chopped;

	int numChops = (int)(command->getDuration() / FRAME_LENGTH_S);
	vector<float> currentJoints = getCurrentJoints();

	// It's a BJC so it deals with 4 chains
	if (command->getInterpolation() == INTERPOLATION_LINEAR) {
		chopped = chopLinear(command, currentJoints, numChops);
	}

 	else if (command->getInterpolation() == INTERPOLATION_SMOOTH) {
 		chopped =  chopSmooth(command, currentJoints, numChops);
 	}

	else {
		cout << "ILLEGAL INTERPOLATION VALUE. CHOPPING LINEARLY" << endl;
		chopped = chopLinear(command, currentJoints, numChops) ;
	}
	// Deleting command!
	delete command;
	return chopped;
}

//Smooth interpolation motion
shared_ptr<ChoppedCommand>
ChopShop::chopSmooth(const JointCommand *command,
					 vector<float> currentJoints,
					 int numChops) {
	shared_ptr<ChoppedCommand> chopped( new SmoothChoppedCommand(
											command,
											currentJoints,
											numChops ) );

	return chopped;
}

/*
 * Linear interpolation chopping:
 * Retrieves current joint angels and acquiries the differences
 * between the current and the intended final. Send them to
 *
 *
 *
 */
shared_ptr<ChoppedCommand>
ChopShop::chopLinear(const JointCommand *command,
					 vector<float> currentJoints,
					 int numChops) {

	shared_ptr<ChoppedCommand> chopped( new LinearChoppedCommand(
											command,
											currentJoints,
											numChops ) );

	return chopped;
}

vector<float> ChopShop::getCurrentJoints() {
    return sensors->getMotionBodyAngles();
}



void ChopShop::vectorToRad(vector<float> *vect) {
	vector<float>::iterator i;
	i = vect->begin();
	while ( i != vect->end() ) {
		// Convert joints to radians from degrees
		*i = *i * TO_RAD;
		i++;
	}

}
