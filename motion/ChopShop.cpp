
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
shared_ptr<ChoppedCommand> ChopShop::chopCommand(const JointCommand *command) {
	shared_ptr<ChoppedCommand> chopped;
	// It's a BJC so it deals with 4 chains
	if (command->getInterpolation() == INTERPOLATION_LINEAR) {
		chopped = chopLinear(command);
	}

// 	else if (command->getInterpolation() == INTERPOLATION_SMOOTH) {
// 		chopped =  chopSmooth(command);
// 	}

	else {
		cout << "ILLEGAL INTERPOLATION VALUE. CHOPPING LINEARLY" << endl;
		chopped = chopLinear(command) ;
	}
	// Deleting command!
	delete command;
	return chopped;
}

// Smooth interpolation motion
// ChoppedCommand *
// ChopShop::chopSmooth(const JointCommand *command) {

// 	// PLACE HOLDER
// 	ChoppedCommand* a = new ChoppedCommand();
// 	return a;
// }

/*
 * Linear interpolation chopping:
 * Retrieves current joint angels and acquiries the differences
 * between the current and the intended final. Send them to
 *
 *
 *
 */
shared_ptr<ChoppedCommand>
ChopShop::chopLinear(const JointCommand *command) {
	// Get number of chops according to duration
	int numChops = (int)(command->getDuration()/FRAME_LENGTH_S);

	vector<float> currentJoints = getCurrentJoints();

	// Add final joints for all chains
	vector<float> finalJoints = getFinalJoints(command, &currentJoints);

	vector<float> diffPerChop = getDiffPerChop(numChops,
											   &currentJoints,
											   &finalJoints);

	// Construct the chopped command object and return it!
	ChoppedCommand * chopped = new ChoppedCommand(&currentJoints,
												  &diffPerChop,
												  numChops,command->getType());
	shared_ptr<ChoppedCommand> pChopped(chopped);
	return pChopped;
}

vector<float> ChopShop::getCurrentJoints() {
    return sensors->getMotionBodyAngles();
}

// Get the goal joints from the joint command
vector<float> ChopShop::getFinalJoints(const JointCommand *command,
									   vector<float>* currentJoints) {
	vector<float> finalJoints;
	vector<float>::iterator currentStart = currentJoints->begin();
	vector<float>::iterator currentEnd = currentJoints->begin();

	for (unsigned int chain=0; chain < NUM_CHAINS;chain++) {
		// First, get chain joints from command
		const vector<float> *nextChain = command->getJoints((ChainID)chain);

		// Set the end iterator
		currentEnd += chain_lengths[chain];

		// If the next chain is not queued (empty), add current joints
		if ( nextChain == 0 ||
			 nextChain->size() == 0 ||
			 nextChain->empty() ) {
			finalJoints.insert(finalJoints.end(), currentStart, currentEnd );

		}else {
			// Add each chain of joints to the final joints
			finalJoints.insert( finalJoints.end(),
								nextChain->begin(),
								nextChain->end() );
		}
		// Set the start iterator into the right position for the
		// next chain
		currentStart += chain_lengths[chain];
	}
	return finalJoints;

}
vector<float> ChopShop::getDiffPerChop(int numChops,
									   vector<float> *current,
									   vector<float> *final) {
	vector<float> diffPerChop;

	for (unsigned int joint_id=0; joint_id < NUM_JOINTS ;++joint_id) {
		diffPerChop.push_back( (final->at(joint_id) -
								current->at(joint_id)) / (float)numChops);
	}

	return diffPerChop;
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
