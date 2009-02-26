
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


using Kinematics::chain_lengths;

ChoppedCommand::ChoppedCommand(vector<float> *first,
							   vector<float> *diffs,
							   int chops,int type)
	: currentHead(chain_lengths[Kinematics::HEAD_CHAIN],0),
	  currentLArm(chain_lengths[Kinematics::LARM_CHAIN],0),
	  currentLLeg(chain_lengths[Kinematics::LLEG_CHAIN],0),
	  currentRLeg(chain_lengths[Kinematics::RLEG_CHAIN],0),
	  currentRArm(chain_lengths[Kinematics::RARM_CHAIN],0),
	  diffHead(chain_lengths[Kinematics::HEAD_CHAIN],0),
	  diffLArm(chain_lengths[Kinematics::LARM_CHAIN],0),
	  diffLLeg(chain_lengths[Kinematics::LLEG_CHAIN],0),
	  diffRLeg(chain_lengths[Kinematics::RLEG_CHAIN],0),
	  diffRArm(chain_lengths[Kinematics::RARM_CHAIN],0),
	  numChops(chops),
	  numChopped(Kinematics::NUM_CHAINS,0),
	  motionType(type)
{
	// Build the chain diff and current vectors
	vector<float>::iterator i = first->begin();
	vector<float>::iterator j = diffs->begin();
	// Iterate over all the current chain vectors
	// and copy in the first joints
	vector<float> * currentChain;
	vector<float> * diffChain;
	for (unsigned int chain = 0; chain < Kinematics::NUM_CHAINS ; ++chain) {
		currentChain = getCurrentChain(chain);
		diffChain = getDiffChain(chain);
		for (unsigned int joint = 0; joint < chain_lengths[chain] ; ++joint) {
			// Put the next value in the appropriate vector
			currentChain->at(joint) = *i;
			diffChain->at(joint) = *j;

			// Next first joint
			++i;
			// Next diff joint
			++j;
		}
	}
}

vector<float>* ChoppedCommand::getCurrentChain(int id) {
	switch (id) {
	case Kinematics::HEAD_CHAIN:
		return &currentHead;
	case Kinematics::LARM_CHAIN:
		return &currentLArm;
	case Kinematics::LLEG_CHAIN:
		return &currentLLeg;
	case Kinematics::RLEG_CHAIN:
		return &currentRLeg;
	case Kinematics::RARM_CHAIN:
		return &currentRArm;
	default:
		std::cout << "INVALID CHAINID" << std::endl;
		return new vector<float>(0);
	}
}

vector<float>* ChoppedCommand::getDiffChain(int id) {
	switch (id) {
	case Kinematics::HEAD_CHAIN:
		return &diffHead;
	case Kinematics::LARM_CHAIN:
		return &diffLArm;
	case Kinematics::LLEG_CHAIN:
		return &diffLLeg;
	case Kinematics::RLEG_CHAIN:
		return &diffRLeg;
	case Kinematics::RARM_CHAIN:
		return &diffRArm;
	default:
		std::cout << "INVALID CHAINID" << std::endl;
		return new vector<float>(0);
	}
}

vector<float> ChoppedCommand::getNextJoints(int id) {
	if (numChopped.at(id) <= numChops) {
		// Retrieve current and diff chains
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
		return *currentChain;
		// Check if we're finished with the command
	} else {
		std::cout << "COMMAND IS DONE!" <<std::endl;
		return vector<float>(0);
	}
}

bool ChoppedCommand::isDone() {
	// Check all the vectors to see if they're all
	// fully chopped
	bool isDone = false;
	if (motionType == MotionConstants::BODY_JOINT){
		for (unsigned int i = LARM_CHAIN; i <Kinematics::NUM_CHAINS ; ++i){
			if (numChopped.at(i) > numChops)
				isDone = true;
			else
				return false;
		}
	} else if (motionType == MotionConstants::HEAD_JOINT) {
		if (numChopped.at(HEAD_CHAIN) > numChops)
			isDone = true;
		else
			return false;
	} else {
		std::cout << "WHAT IS GOING ON? WRONG MOTIONTYPE, DAMNIT" << std::endl;
	}
	return isDone;
}
