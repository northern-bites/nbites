
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

#include "LinearChoppedCommand.h"
#include "MotionConstants.h"

using namespace std;
using namespace Kinematics;

LinearChoppedCommand::LinearChoppedCommand(const JointCommand::ptr command,
					   vector<float> currentJoints,
					   int chops )
    : ChoppedCommand(command, chops)
{
    buildCurrentChains(currentJoints);

    vector<float> finalJoints = ChoppedCommand::getFinalJoints(command,
							       currentJoints);
    vector<float> diffPerChop = getDiffPerChop(currentJoints,
					       finalJoints,
					       numChops );

    buildDiffChains( diffPerChop );
}

void LinearChoppedCommand::buildCurrentChains( vector<float> currentJoints ) {
    vector<float>::iterator firstCurrentJoint = currentJoints.begin();
    vector<float>::iterator chainStart, chainEnd;

    for (unsigned int chain = 0; chain < NUM_CHAINS ; ++chain) {
	vector<float> *currentChain = getCurrentChain(chain);

	chainStart = firstCurrentJoint + chain_first_joint[chain];
	chainEnd = firstCurrentJoint + chain_last_joint[chain] + 1;
	currentChain->assign( chainStart, chainEnd );
    }
}

void LinearChoppedCommand::buildDiffChains( vector<float>diffPerChop ) {
    vector<float>::iterator firstDiffJoint = diffPerChop.begin();
    vector<float>::iterator chainStart,chainEnd;

    for (unsigned int chain = 0; chain < NUM_CHAINS ; ++chain) {
	vector<float> *diffChain = getDiffChain(chain);

	chainStart = firstDiffJoint + chain_first_joint[chain];
	chainEnd = firstDiffJoint + chain_last_joint[chain] + 1;
	diffChain->assign( chainStart, chainEnd );

    }
}


vector<float>* LinearChoppedCommand::getDiffChain(int id) {
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

vector<float> LinearChoppedCommand::getNextJoints(int id) {
    if (numChopped.at(id) <= numChops) {
	// Increment the current chain
	incrCurrChain(id);

	// Since we changed the command's current status, we
	// need to check to see if it's finished yet.
	checkDone();

	// Return a copy of the current chain at this id
	return *getCurrentChain(id);

    } else {
	// Don't increment anymore and just return the current chain
	return *getCurrentChain(id);
    }
}
vector<float>* LinearChoppedCommand::getCurrentChain(int id) {
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

void LinearChoppedCommand::incrCurrChain(int id) {
    vector<float> * currentChain = getCurrentChain(id);
    vector<float> * diffChain = getDiffChain(id);

    // Set iterators to diff and current vectors
    vector<float>::iterator curr = currentChain->begin();
    vector<float>::iterator diff = diffChain->begin();

    numChopped.at(id)++;
    while (curr != currentChain->end() && diff != diffChain->end() ) {
	*curr += *diff;
	++curr;
	++diff;
    }
}

vector<float> LinearChoppedCommand::getDiffPerChop( vector<float> current,
						    vector<float> final,
						    int numChops ) {
    vector<float> diffPerChop;

    for (unsigned int joint_id=0; joint_id < NUM_JOINTS ;++joint_id) {
	diffPerChop.push_back( (final.at(joint_id) -
				current.at(joint_id)) / (float)numChops);
    }

    return diffPerChop;
}
