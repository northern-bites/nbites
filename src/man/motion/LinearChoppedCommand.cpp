#include "LinearChoppedCommand.h"
#include "MotionConstants.h"

using namespace Kinematics;

namespace man
{
namespace motion
{

LinearChoppedCommand::LinearChoppedCommand(const JointCommand::ptr command,
					   std::vector<float> currentJoints,
					   int chops )
    : ChoppedCommand(command, chops)
{
    buildCurrentChains(currentJoints);

    std::vector<float> finalJoints = ChoppedCommand::getFinalJoints(command,
							       currentJoints);
    std::vector<float> diffPerChop = getDiffPerChop(currentJoints,
					       finalJoints,
					       numChops );

    buildDiffChains( diffPerChop );
}

    void LinearChoppedCommand::buildCurrentChains( std::vector<float> currentJoints ) {
	std::vector<float>::iterator firstCurrentJoint = currentJoints.begin();
	std::vector<float>::iterator chainStart, chainEnd;

    for (unsigned int chain = 0; chain < NUM_CHAINS ; ++chain) {
	std::vector<float> *currentChain = getCurrentChain(chain);

	chainStart = firstCurrentJoint + chain_first_joint[chain];
	chainEnd = firstCurrentJoint + chain_last_joint[chain] + 1;
	currentChain->assign( chainStart, chainEnd );
    }
}

    void LinearChoppedCommand::buildDiffChains( std::vector<float>diffPerChop ) {
	std::vector<float>::iterator firstDiffJoint = diffPerChop.begin();
	std::vector<float>::iterator chainStart,chainEnd;

    for (unsigned int chain = 0; chain < NUM_CHAINS ; ++chain) {
	std::vector<float> *diffChain = getDiffChain(chain);

	chainStart = firstDiffJoint + chain_first_joint[chain];
	chainEnd = firstDiffJoint + chain_last_joint[chain] + 1;
	diffChain->assign( chainStart, chainEnd );

    }
}


    std::vector<float>* LinearChoppedCommand::getDiffChain(int id) {
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
	return new std::vector<float>(0);
    }
}

    std::vector<float> LinearChoppedCommand::getNextJoints(int id) {
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
    std::vector<float>* LinearChoppedCommand::getCurrentChain(int id) {
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
	return new std::vector<float>(0);
    }
}

void LinearChoppedCommand::incrCurrChain(int id) {
    std::vector<float> * currentChain = getCurrentChain(id);
    std::vector<float> * diffChain = getDiffChain(id);

    // Set iterators to diff and current vectors
    std::vector<float>::iterator curr = currentChain->begin();
    std::vector<float>::iterator diff = diffChain->begin();

    numChopped.at(id)++;
    while (curr != currentChain->end() && diff != diffChain->end() ) {
	*curr += *diff;
	++curr;
	++diff;
    }
}

    std::vector<float> LinearChoppedCommand::getDiffPerChop( std::vector<float> current,
							     std::vector<float> final,
						    int numChops ) {
	std::vector<float> diffPerChop;

    for (unsigned int joint_id=0; joint_id < NUM_JOINTS ;++joint_id) {
	diffPerChop.push_back( (final.at(joint_id) -
				current.at(joint_id)) / (float)numChops);
    }

    return diffPerChop;
}

} // namespace motion
} // namespace man
