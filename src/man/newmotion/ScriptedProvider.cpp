#include "ScriptedProvider.h"

using namespace Kinematics;

namespace man
{
namespace motion
{

ScriptedProvider::ScriptedProvider()
    : MotionProvider(SCRIPTED_PROVIDER),
      chopper(),
      currCommand(),
      bodyCommandQueue()
{

}

ScriptedProvider::~ScriptedProvider() {
    // Wait until not active anymore
    while ( isActive() );
    // Don't have to delete commandQueue, since it will
    // be empty once we're not active.
}

// We're going to finish the whole command queue
// and then stop doing anything.
// So this method does nothing. At all.
// We'll just naturally finish what we're doing and
// hold steady at the last position.
void ScriptedProvider::requestStopFirstInstance() { }

void ScriptedProvider::hardReset(){
    while(!bodyCommandQueue.empty()){
        BodyJointCommand::ptr next = bodyCommandQueue.front();
        next->finishedExecuting();
        bodyCommandQueue.pop();
    }

    if (currCommand)
        currCommand = ChoppedCommand::ptr();

    setActive();
}

//Checks if this chain is currently providing angles so that external
//classes can check the status of this one
void ScriptedProvider::setActive(){
    if(isDone())
        inactive();
    else
        active();
}

// Are we done moving totally?
bool ScriptedProvider::isDone() {
    return currCommandEmpty() && commandQueueEmpty();
}

// Are we finished with our current motion command?
bool ScriptedProvider::currCommandEmpty() {
    if (currCommand)
	return currCommand->isDone();
    else
	return true;
}

// Do we have any BodyCommands left to run through?
bool ScriptedProvider::commandQueueEmpty(){
    return bodyCommandQueue.empty();
}

void ScriptedProvider::calculateNextJointsAndStiffnesses(
    std::vector<float>&            sensorAngles,
    const messages::InertialState& sensorInertials,
    const messages::FSR&           sensorFSRs)
{
    if (currCommandEmpty())
	setNextBodyCommand(sensorAngles);

    // Go through the chains and enqueue the next
    // joints from the ChoppedCommand.
    boost::shared_ptr<std::vector <std::vector <float> > > currentChains(getCurrentChains(sensorAngles));

    currCommand->nextFrame(); // so Python can keep track of progress

    for (unsigned int id=0; id< Kinematics::NUM_CHAINS; ++id ) {
        Kinematics::ChainID cid = static_cast<Kinematics::ChainID>(id);
	if ( currCommand->isDone() ){
	    setNextChainJoints( cid,
				currentChains->at(cid) );
	}else{
	    setNextChainJoints( cid,
				currCommand->getNextJoints(cid) );
	}
	// Curr command will allways provide the current stiffnesses
	// even if it is finished providing new joint angles.
	setNextChainStiffnesses( cid,
				 currCommand->getStiffness(cid) );
    }

    setActive();
}

/*
 * Adds new command to queue of commands.
 * when the chainQueues are all empty,
 * a command is popped, locked, and chopped.
 * CAUTION: Will DELETE the enqueued command,
 * so do not resend commands. One use per command.
 * Only one BodyJointCommand can be enqueued at
 * a time, even if they deal with different joints or chains.
 */
void ScriptedProvider::setCommand(const BodyJointCommand::ptr command) {
    bodyCommandQueue.push(command);
    setActive();
}


void ScriptedProvider::enqueueSequence(std::vector<BodyJointCommand::ptr> &seq) {
    // Take in vec of commands and enqueue them all
    std::vector<BodyJointCommand::ptr>::iterator i;
    for (i = seq.begin(); i != seq.end(); ++i)
	setCommand(*i);
}

void ScriptedProvider::setNextBodyCommand(std::vector<float>& sensorAngles) {
    // If there are no more commands, don't try to enqueue one
    if ( !bodyCommandQueue.empty() ) {
	BodyJointCommand::ptr nextCommand = bodyCommandQueue.front();
	bodyCommandQueue.pop();

	// Replace the current command
	currCommand = chopper.chopCommand(nextCommand, sensorAngles);
    }
}

// Get the chain's current positions. Gives a shared pointer to
// a vector containing a vector for each chain. Makes accessing
// each chain very simple.
boost::shared_ptr<std::vector<std::vector<float> > > ScriptedProvider::getCurrentChains(std::vector<float>& sensorAngles) {
    boost::shared_ptr<std::vector<std::vector<float> > > currentChains(
	new std::vector<std::vector<float> >(Kinematics::NUM_CHAINS) );

    unsigned int lastChainJoint,joint,chain;
    lastChainJoint= 0;
    joint = 0;
    chain = 0;

    std::vector<std::vector<float> >::iterator i = currentChains->begin();

    while (i != currentChains->end() ) {
        lastChainJoint += chain_lengths[chain];

        for ( ; joint < lastChainJoint ; joint++) {
            i->push_back(sensorAngles.at(joint));
        }

	++i;
	++chain;
    }
    return currentChains;
}

} // namespace motion
} // namespace man
