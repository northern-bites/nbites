
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

#include "SmoothChoppedCommand.h"
#include "MotionConstants.h"
#include <cmath>

using namespace std;
using namespace Kinematics;

SmoothChoppedCommand::SmoothChoppedCommand(const JointCommand::ptr command,
					   vector<float> startJoints,
					   int chops )
    : ChoppedCommand(command, chops)
{
    buildStartChains(startJoints);

    vector<float> finalJoints = ChoppedCommand::getFinalJoints(command,
							       startJoints);
    buildDiffChains( finalJoints );
}

void SmoothChoppedCommand::buildStartChains(const vector<float> &startJoints ) {
    vector<float>::const_iterator firstStartJoint = startJoints.begin();
    vector<float>::const_iterator chainStart, chainEnd;

    for (unsigned int chain = 0; chain < NUM_CHAINS ; ++chain) {
	vector<float> *startChain = getStartChain(chain);

	chainStart = firstStartJoint + chain_first_joint[chain];
	chainEnd = firstStartJoint + chain_last_joint[chain] + 1;
	startChain->assign( chainStart, chainEnd );
    }
}

void SmoothChoppedCommand::buildDiffChains( const vector<float> &finalJoints ) {
    setDiffChainsToFinalJoints(finalJoints);
    subtractBodyStartFromFinalAngles();
}

void SmoothChoppedCommand::setDiffChainsToFinalJoints(
    const vector<float> &finalJoints ) {

    vector<float>::const_iterator firstFinalJoint = finalJoints.begin();
    vector<float>::const_iterator chainStart,chainEnd;

    for (unsigned int chain = 0; chain < NUM_CHAINS ; ++chain) {
	vector<float> *diffChain = getDiffChain(chain);

	chainStart = firstFinalJoint + chain_first_joint[chain];
	chainEnd = firstFinalJoint + chain_last_joint[chain] + 1;
	diffChain->assign( chainStart, chainEnd );
    }
}

void SmoothChoppedCommand::subtractBodyStartFromFinalAngles() {
    for (unsigned int chain = 0; chain < NUM_CHAINS ; chain++ )
	subtractChainStartFromFinalAngles(chain);
}

void SmoothChoppedCommand::subtractChainStartFromFinalAngles(int chain){
    vector<float>* startChain = getStartChain(chain);
    vector<float>* diffChain = getDiffChain(chain);

    vector<float>::iterator startAngle = startChain->begin();
    vector<float>::iterator diffAngle = diffChain->begin();

    while ( startAngle != startChain->end() ){
	*diffAngle -= *startAngle;
	diffAngle++;
	startAngle++;
    }
}

vector<float>* SmoothChoppedCommand::getDiffChain(int id) {
    switch (id) {
    case HEAD_CHAIN:
	return &totalDiffHead;
    case LARM_CHAIN:
	return &totalDiffLArm;
    case LLEG_CHAIN:
	return &totalDiffLLeg;
    case RLEG_CHAIN:
	return &totalDiffRLeg;
    case RARM_CHAIN:
	return &totalDiffRArm;
    default:
	std::cout << "INVALID CHAINID" << std::endl;
	return new vector<float>(0);
    }
}

vector<float> SmoothChoppedCommand::getNextJoints(int id) {
    if ( !isChainFinished(id) ) {
	numChopped.at(id)++;
	checkDone();
    }

    return getNextChainFromCycloid(id);
}

vector<float> SmoothChoppedCommand::getNextChainFromCycloid(int id) {
    float t = getCycloidStep(id);
    vector<float> nextChain;
    vector<float>* diffChain = getDiffChain(id);
    vector<float>* startChain = getStartChain(id);
    vector<float>::iterator diffAngle = diffChain->begin();
    vector<float>::iterator startAngle = startChain->begin();

    while ( diffAngle != diffChain->end() ) {
	nextChain.push_back(*startAngle + getCycloidAngle(*diffAngle,t) );
	diffAngle++;
	startAngle++;
    }

    return nextChain;
}

float SmoothChoppedCommand::getCycloidAngle(float d_theta, float t) {
    return ( (d_theta/(2*M_PI_FLOAT)) * (t - sin(t)) );
}

float SmoothChoppedCommand::getCycloidStep( int id ) {
    return ( ( static_cast<float>(numChopped.at(id)) /
	       static_cast<float>(numChops) ) * M_PI_FLOAT*2.0f);
}

bool SmoothChoppedCommand::isChainFinished(int id) {
    return (numChopped.at(id) >= numChops);
}
vector<float>* SmoothChoppedCommand::getStartChain(int id) {
    switch (id) {
    case HEAD_CHAIN:
	return &startHead;
    case LARM_CHAIN:
	return &startLArm;
    case LLEG_CHAIN:
	return &startLLeg;
    case RLEG_CHAIN:
	return &startRLeg;
    case RARM_CHAIN:
	return &startRArm;
    default:
	std::cout << "INVALID CHAINID" << std::endl;
	return new vector<float>(0);
    }
}


