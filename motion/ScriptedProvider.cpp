#include "ScriptedProvider.h"

ScriptedProvider::ScriptedProvider(float motionFrameLength,
								   Sensors *s)
	: MotionProvider(),
	  sensors(s),
	  FRAME_LENGTH_S(motionFrameLength),
	  chopper(sensors, FRAME_LENGTH_S),
	  nextJoints(),
	  choppedBodyCommand(),
	  choppedHeadCommand(),
	  bodyCommandQueue()
{
	for (int chainID=0; chainID<NUM_CHAINS; chainID++) {
		chainQueues.push_back( ChainQueue((ChainID)chainID) );
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

	if ( !chainQueues.at(HEAD_CHAIN).empty() )
		setNextHeadCommand();

	for ( int i=LARM_CHAIN ; i<chainQueues.size() ; i++ ) {
		if ( !chainQueues.at(i).empty() ){
			cout << "not empty!" << endl;
			allEmpty=false;
		}
	}

	if (allEmpty) setNextBodyCommand();

	// Make sure first that the queues are not empty
	// If they're empty, then add the current joints to be the
	// next joints. If they're not empty, then add the queued
	// joints as the next Chain joints
	for (int chainID=1 ; chainID < chainQueues.size() ; chainID++) {
		cout << "chain " << chainID << " size = " << chainQueues.at(chainID).size() << endl;
		if ( chainQueues.at(chainID).empty() ) {
			setNextChainJoints( (ChainID)chainID, currentChains.at(chainID) );
		} else {
			setNextChainJoints( (ChainID)chainID, chainQueues.at(chainID).front() );
			chainQueues.at(chainID).pop();
		}
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
void ScriptedProvider::enqueue(const HeadJointCommand *command) {
	headCommandQueue.push(command);
}

void ScriptedProvider::setNextBodyCommand(){
	cout << "bodyCommandQueue.size() = " << bodyCommandQueue.size() << endl;

	// If there are no more commands, don't try to enqueue one
	if ( !bodyCommandQueue.empty() ) {
		
		const BodyJointCommand *command = bodyCommandQueue.front();
		bodyCommandQueue.pop();
		choppedBodyCommand = chopper.chopCommand(command);
		delete command;

		while (!choppedBodyCommand.empty()){
			// Pass each chain to its chainqueue

			// Skips the HEAD_CHAIN and enqueues all body chains
			for (int chainID=1; chainID<chainQueues.size(); chainID++) {
				chainQueues.at(chainID).push(choppedBodyCommand.front().at(chainID));
			}
			choppedBodyCommand.pop();
		}
	}
}

void ScriptedProvider::setNextHeadCommand() {

	if ( !headCommandQueue.empty() ) {
		const HeadJointCommand *command = headCommandQueue.front();
		headCommandQueue.pop();
		choppedHeadCommand = chopper.chopCommand(command);
		delete command;

		while (!choppedHeadCommand.empty()){
			// Pass each chain to its chainqueue
			chainQueues.at(HEAD_CHAIN).push(choppedHeadCommand.front().at(HEAD_CHAIN));
			choppedHeadCommand.pop();

		}



	}

}


void ScriptedProvider::enqueueSequence(std::vector<BodyJointCommand*> &seq) {
	// Take in vec of commands and enqueue them all
	pthread_mutex_lock(&scripted_mutex);
	for (vector<BodyJointCommand*>::iterator i= seq.begin(); i != seq.end(); i++)
		enqueue(*i);
	pthread_mutex_unlock(&scripted_mutex);

}

vector<vector<float> > ScriptedProvider::getCurrentChains() {
	vector<vector<float> > currentChains(5);

	vector<float> currentJoints = sensors->getBodyAngles();
	vector<float> currentJointErrors = sensors->getBodyAngleErrors();

	for (int i=0; i<NUM_JOINTS ; i++){
		currentJoints[i] = currentJoints[i]-currentJointErrors[i];
	}

	int lastChainJoint = 0;
	for (int chain=0,joint=0; chain<NUM_CHAINS; chain++) {
		lastChainJoint += chain_lengths[chain];

		for ( ; joint < lastChainJoint ; joint++){
			currentChains.at(chain).push_back(currentJoints[joint]);
		}

	}
	return currentChains;
}
