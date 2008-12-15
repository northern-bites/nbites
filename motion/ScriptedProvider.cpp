#include "ScriptedProvider.h"

ScriptedProvider::ScriptedProvider(float motionFrameLength,
								   Sensors *s)
	: MotionProvider(),
	  sensors(s),
	  FRAME_LENGTH_S(motionFrameLength),
	  chopper(sensors, FRAME_LENGTH_S),
	  nextJoints(),
	  choppedBodyCommand(),
	  bodyCommandQueue()
{
	// No head chain, only body chains
	for (unsigned int chainID=0; chainID<NUM_BODY_CHAINS; chainID++) {
		chainQueues.push_back( ChainQueue( (ChainID)(chainID+1) ) );

	}
	// Create mutexes
	// (?) Need one mutex per queue (?)
	pthread_mutex_init (&scripted_mutex, NULL);
}

ScriptedProvider::~ScriptedProvider() {
	// remove all remaining values from chain queues
}

void ScriptedProvider::requestStop() {
	// Finish motion or stop immediately?
}

void ScriptedProvider::calculateNextJoints() {

	vector <vector <float> > currentChains = getCurrentChains();
	// If all queues are empty, then the next command must
	// be chopped and used.
	bool allEmpty = true;

	for ( int i=0 ; i<chainQueues.size() ; i++ ) {
		if ( !chainQueues.at(i).empty() ) {
			cout << "not empty!" << endl;
			allEmpty=false;
		}
	}

	if (allEmpty) setNextBodyCommand();

	// Make sure first that the queues are not empty
	// If they're empty, then add the current joints to be the
	// next joints. If they're not empty, then add the queued
	// joints as the next Chain joints
	vector<ChainQueue>::iterator i;
	i = chainQueues.begin();

	while ( i != chainQueues.end() ) {
		ChainID chainID = i->getChainID();
		if ( i->empty() ) {
			cout << "chain " << chainID << " is empty" << endl;
			cout << "current has size " << currentChains.at(chainID).size() << endl;
			setNextChainJoints( chainID, currentChains.at(chainID) );
		} else {
			cout << "chain " << chainID << " has size " << i->front().size() << endl;
			setNextChainJoints( chainID, i->front() );
			i->pop();
		}
		i++;
	}

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
void ScriptedProvider::enqueue(const BodyJointCommand *command) {
	bodyCommandQueue.push(command);
}


void ScriptedProvider::enqueueSequence(std::vector<const BodyJointCommand*> &seq) {
	// Take in vec of commands and enqueue them all
	pthread_mutex_lock(&scripted_mutex);
	for (vector<const BodyJointCommand*>::iterator i= seq.begin(); i != seq.end(); i++)
		enqueue(*i);
	pthread_mutex_unlock(&scripted_mutex);
}

void ScriptedProvider::setNextBodyCommand() {
	cout << "bodyCommandQueue.size() = " << bodyCommandQueue.size() << endl;

	// If there are no more commands, don't try to enqueue one
	if ( !bodyCommandQueue.empty() ) {

		const BodyJointCommand *command = bodyCommandQueue.front();
		bodyCommandQueue.pop();
		choppedBodyCommand = chopper.chopCommand(command);
		delete command;

		while (!choppedBodyCommand.empty()) {
			// Pass each chain to its chainqueue
			vector<ChainQueue>::iterator i;
			// Skips the HEAD_CHAIN and enqueues all body chains
			i = chainQueues.begin();
			while ( i != chainQueues.end() ) {
				i->push(choppedBodyCommand.front().at( i->getChainID() ));
				i++;
			}
			choppedBodyCommand.pop();
		}
	}
}

vector<vector<float> > ScriptedProvider::getCurrentChains() {
	vector<vector<float> > currentChains(NUM_BODY_CHAINS);

	vector<float> currentJoints = sensors->getBodyAngles();
	vector<float> currentJointErrors = sensors->getBodyAngleErrors();

	for (unsigned int i=0; i<NUM_JOINTS ; i++){
		currentJoints[i] = currentJoints[i]-currentJointErrors[i];
	}

	unsigned int lastChainJoint = HEAD_JOINTS;
	unsigned int joint=HEAD_JOINTS;
	for (unsigned int chain=LARM_CHAIN/* skip head*/;chain<NUM_CHAINS; chain++) {
		lastChainJoint += chain_lengths[chain];

		for ( ; joint < lastChainJoint ; joint++) {
			currentChains.at(chain-1/*No head in currentChains*/).push_back(currentJoints[joint]);
		}

	}
	return currentChains;
}
