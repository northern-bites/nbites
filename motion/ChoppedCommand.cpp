
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

ChoppedCommand::ChoppedCommand(const JointCommand *command,
							   vector<float> currentJoints,
							   int chops )
	: numChops(chops),
	  numChopped(NUM_CHAINS,0),
	  motionType( command->getType() ),
	  interpolationType( command->getInterpolation() ),
	  finished(false)
{
	buildCurrentChains(currentJoints);

	vector<float> finalJoints = getFinalJoints(command, currentJoints);

	vector<float> diffPerChop = getDiffPerChop(currentJoints,
											   finalJoints,
											   numChops );

	buildDiffChains( diffPerChop );

}

void ChoppedCommand::buildCurrentChains( vector<float> currentJoints ) {
	vector<float>::iterator firstCurrentJoint = currentJoints.begin();
	vector<float>::iterator chainStart, chainEnd;

	for (unsigned int chain = 0; chain < NUM_CHAINS ; ++chain) {
		vector<float> *currentChain = getCurrentChain(chain);

		chainStart = firstCurrentJoint + chain_first_joint[chain];
		chainEnd = firstCurrentJoint + chain_last_joint[chain] + 1;
		currentChain->assign( chainStart, chainEnd );
	}
}

void ChoppedCommand::buildDiffChains( vector<float>diffPerChop ) {
	vector<float>::iterator firstDiffJoint = diffPerChop.begin();
	vector<float>::iterator chainStart,chainEnd;

	for (unsigned int chain = 0; chain < NUM_CHAINS ; ++chain) {
		vector<float> *diffChain = getDiffChain(chain);

		chainStart = firstDiffJoint + chain_first_joint[chain];
		chainEnd = firstDiffJoint + chain_last_joint[chain] + 1;
		diffChain->assign( chainStart, chainEnd );

	}
}

vector<float>* ChoppedCommand::getCurrentChain(int id) {
	switch (id) {
	case HEAD_CHAIN:
		return &currentHead;
	case LARM_CHAIN:
		return &currentLArm;
	case LLEG_CHAIN:
		return &currentLLeg;
	case RLEG_CHAIN:
		return &currentRLeg;
	case RARM_CHAIN:
		return &currentRArm;
	default:
		std::cout << "INVALID CHAINID" << std::endl;
		return new vector<float>(0);
	}
}


vector<float>* ChoppedCommand::getDiffChain(int id) {
	switch (id) {
	case HEAD_CHAIN:
		return &diffHead;
	case LARM_CHAIN:
		return &diffLArm;
	case LLEG_CHAIN:
		return &diffLLeg;
	case RLEG_CHAIN:
		return &diffRLeg;
	case RARM_CHAIN:
		return &diffRArm;
	default:
		std::cout << "INVALID CHAINID" << std::endl;
		return new vector<float>(0);
	}
}

vector<float> ChoppedCommand::getNextJoints(int id) {
	if ( interpolationType == INTERPOLATION_LINEAR ) {
		return *getNextLinearJoints(id);
	}
	else if ( interpolationType == INTERPOLATION_SMOOTH )
		return getNextSmoothJoints(id);
}

vector<float>* ChoppedCommand::getNextLinearJoints(int id) {
	if (numChopped.at(id) <= numChops) {
		// Increment the current chain

		incrCurrChain(id);
		// Since we changed the command's current status, we
		// need to check to see if it's finished yet.
		checkDone();

		// Return a copy of the current chain at this id
		return getCurrentChain(id);

	} else {
		// Don't increment anymore and just return the current chain
		return getCurrentChain(id);
	}
}

vector<float> ChoppedCommand::getNextSmoothJoints(int id) {

}

void ChoppedCommand::incrCurrChain(int id) {
	vector<float> * currentChain = getCurrentChain(id);
	vector<float> * diffChain = getDiffChain(id);

	// Set iterators to diff and current vectors
	vector<float>::iterator i = currentChain->begin();
	vector<float>::iterator j = diffChain->begin();

	numChopped.at(id)++;
	while (i != currentChain->end() &&
		   j != diffChain->end() ) {
		*i += *j;
		++i;
		++j;
	}
}

bool ChoppedCommand::isDone() {
	// Check all the vectors to see if they're all
	// fully chopped
	return finished;
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


vector<float> ChoppedCommand::getDiffPerChop( vector<float> current,
											  vector<float> final,
											  int numChops ) {
	vector<float> diffPerChop;

	for (unsigned int joint_id=0; joint_id < NUM_JOINTS ;++joint_id) {
		diffPerChop.push_back( (final.at(joint_id) -
								current.at(joint_id)) / (float)numChops);
	}

	return diffPerChop;
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
