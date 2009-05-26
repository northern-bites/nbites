
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

#include "ChoppedCommand.h"
#include "MotionConstants.h"
#include "JointCommand.h"


using namespace std;

using namespace Kinematics;

ChoppedCommand::ChoppedCommand(const JointCommand *command, int chops )
	: numChops(chops),
	  numChopped(NUM_CHAINS,0),
	  motionType( command->getType() ),
	  interpolationType( command->getInterpolation() ),
	  finished(false)
{

}

// Check's to see if the command has executed the required
// number of steps
void ChoppedCommand::checkDone() {
	bool allDone = false;

	// If body joint command, must check all chains
	if (motionType == MotionConstants::BODY_JOINT){
		for (unsigned int i = LARM_CHAIN; i <NUM_CHAINS ; ++i){
			if (numChopped.at(i) >= numChops){
				allDone = true;
			} else {
				allDone = false;
				break;
			}
		}

		// Head command only needs to check head chain
	} else if (motionType == MotionConstants::HEAD_JOINT) {
		if (numChopped.at(HEAD_CHAIN) >= numChops)
			allDone = true;
		else
			allDone = false;
	} else {
		std::cout << "WHAT IS GOING ON? WRONG MOTIONTYPE, DAMMIT" << std::endl;
	}
	finished = allDone;
}

vector<float> ChoppedCommand::getFinalJoints(const JointCommand *command,
									   vector<float> currentJoints) {
	vector<float> finalJoints(0);
	vector<float>::iterator currentStart = currentJoints.begin();
	vector<float>::iterator currentEnd = currentJoints.begin();

	for (unsigned int chain=0; chain < NUM_CHAINS;chain++) {
		// First, get chain joints from command
		const vector<float> *nextChain = command->getJoints((ChainID)chain);

		// Set the end iterator
		currentEnd += chain_lengths[chain];

		// If the next chain is not queued (empty), add current joints
		if ( nextChain == 0 ||
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
