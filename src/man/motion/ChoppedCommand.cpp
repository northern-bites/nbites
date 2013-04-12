#include "ChoppedCommand.h"
#include "MotionConstants.h"
#include "JointCommand.h"

using namespace Kinematics;

namespace man
{
namespace motion
{

ChoppedCommand::ChoppedCommand(const JointCommand::ptr command, int chops )
    : numChops(chops),
      numChopped(NUM_CHAINS,0),
      motionType( command->getType() ),
      interpolationType( command->getInterpolation() ),
      finished(false),
      sourceCommand(command)
{
    constructStiffness(command);
    sourceCommand->framesRemaining(chops);
}

void ChoppedCommand::constructStiffness(const JointCommand::ptr command) 
{

    const std::list<int>* chainList = command->getChainList();
    std::list<int>::const_iterator i = chainList->begin();
    for( ; i != chainList->end() ; ++i)
    {
        constructChainStiffness(static_cast<ChainID>(*i),
                                command);
    }
}

void ChoppedCommand::constructChainStiffness(
    ChainID id,
    const JointCommand::ptr command) {
    const std::vector<float> body_stiff = command->getStiffness();
    std::vector<float>::const_iterator bodyStart = body_stiff.begin();

    std::vector<float> *chain = getStiffnessRef(id);

    chain->assign(bodyStart + chain_first_joint[id],
                  bodyStart + chain_last_joint[id] + 1);
}

// Check's to see if the command has executed the required
// number of steps
void ChoppedCommand::checkDone() {
    bool allDone = true;

    // If body joint command, must check all chains
    if (motionType == MotionConstants::BODY_JOINT){
        for (unsigned int i = LARM_CHAIN; i <NUM_CHAINS ; ++i){
            if (numChopped.at(i) < numChops){
                allDone = false;
                break;
            }
        }
        // Head command only needs to check head chain
    } else if (motionType == MotionConstants::HEAD_JOINT) {
        if (numChopped.at(HEAD_CHAIN) < numChops){
            allDone = false;
        }
    }

    finished = allDone;

    if (finished)
	sourceCommand->finishedExecuting();
}

std::vector<float> ChoppedCommand::getFinalJoints(
    const JointCommand::ptr command,
    std::vector<float> currentJoints) 
{
    std::vector<float> finalJoints(0);
    std::vector<float>::iterator currentStart = currentJoints.begin();
    std::vector<float>::iterator currentEnd = currentJoints.begin();

    for (uint chain=0; chain < NUM_CHAINS; ++chain){
        // First, get chain joints from command
        const std::vector<float> nextChain =
            command->getJoints(static_cast<ChainID>(chain));

        // Set the end iterator
        currentEnd += chain_lengths[chain];

        // If the next chain is not queued (empty), add current joints
        if ( nextChain.empty() ) {
            finalJoints.insert(finalJoints.end(), currentStart, currentEnd );
        }else {
            // Add each chain of joints to the final joints
            finalJoints.insert( finalJoints.end(),
                                nextChain.begin(),
                                nextChain.end() );
        }
        // Set the start iterator into the right position for the
        // next chain
        currentStart += chain_lengths[chain];
    }

    return finalJoints;
}

const std::vector<float>
ChoppedCommand::getStiffness( ChainID chainID ) const
{
    switch (chainID) {
    case HEAD_CHAIN:
        return head_stiff;
    case LARM_CHAIN:
        return larm_stiff;
    case LLEG_CHAIN:
        return lleg_stiff;
    case RLEG_CHAIN:
        return rleg_stiff;
    case RARM_CHAIN:
        return rarm_stiff;
    case RANKLE_CHAIN:
    case LANKLE_CHAIN:
        break;
    }
    std::cout << "ChoppedCommand-Should not have reached this point! ERROR!" << std::endl;
    return larm_stiff;
}

std::vector<float>*
ChoppedCommand::getStiffnessRef( ChainID chainID )
{
    switch (chainID) {
    case HEAD_CHAIN:
        return &head_stiff;
    case LARM_CHAIN:
        return &larm_stiff;
    case LLEG_CHAIN:
        return &lleg_stiff;
    case RLEG_CHAIN:
        return &rleg_stiff;
    case RARM_CHAIN:
        return &rarm_stiff;
    case RANKLE_CHAIN:
    case LANKLE_CHAIN:
        break;
    }
    std::cout << "ChoppedCommand-Should not have reached this point! ERROR!" << std::endl;
    return &larm_stiff;
}

} // namespace motion
} // namespace man
