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


	// If all queues are empty, then the next command must
	// be chopped and used.
	bool allEmpty = true;

<<<<<<< HEAD:motion/ScriptedProvider.cpp
	for ( int i=0 ; i<chainQueues.size() ; i++ ) {
		if ( !chainQueues.at(i).empty() ) {
			cout << "not empty!" << endl;
=======

	for (unsigned int i=0 ; i<chainQueues.size() ; i++ ) {
		if ( !chainQueues.at(i).empty() ) {
>>>>>>> Fixed errors in Scripted and Head Providers. It is now possible to enqueue multiple head and body joint commands without error. Although, head moves seem to move too quickly.:motion/ScriptedProvider.cpp
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
	vector <vector <float> > currentChains = getCurrentChains();

	while ( i != chainQueues.end() ) {
		ChainID chainID = i->getChainID();
		if ( i->empty() ) {
			setNextChainJoints( chainID, currentChains.at(chainID) );
		} else {
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
	// If there are no more commands, don't try to enqueue one
	if ( !bodyCommandQueue.empty() ) {

		const BodyJointCommand *command = bodyCommandQueue.front();
		bodyCommandQueue.pop();
		choppedBodyCommand = chopper.chopCommand(command);
		delete command;

		vector<ChainQueue>::iterator i;
		while (!choppedBodyCommand.empty()) {
			// Pass each chain to its chainqueue

			// Skips the HEAD_CHAIN and enqueues all body chains
			i = chainQueues.begin();
			while ( i != chainQueues.end() ) {
				// Subtract 1 because there is no head chain in the
				// choppedBodyCommand (it's only body joints)
				i->push( choppedBodyCommand.front().at( i->getChainID() ) );
				i++;
			}
			choppedBodyCommand.pop();
		}
	}
}

vector<vector<float> > ScriptedProvider::getCurrentChains() {
	vector<vector<float> > currentChains(NUM_CHAINS,vector<float>(0));

	vector<float> currentJoints = sensors->getBodyAngles();
	vector<float> currentJointErrors = sensors->getBodyAngleErrors();

<<<<<<< HEAD:motion/ScriptedProvider.cpp
	for (unsigned int i=0; i<NUM_JOINTS ; i++){
=======
	for (unsigned int i=0; i<NUM_JOINTS ; i++) {
>>>>>>> Fixed errors in Scripted and Head Providers. It is now possible to enqueue multiple head and body joint commands without error. Although, head moves seem to move too quickly.:motion/ScriptedProvider.cpp
		currentJoints[i] = currentJoints[i]-currentJointErrors[i];
	}

	unsigned int lastChainJoint = 0;
	unsigned int joint = 0;

	for (unsigned int chain=HEAD_CHAIN;chain<NUM_CHAINS; chain++) {
		lastChainJoint += chain_lengths[chain];

		for ( ; joint < lastChainJoint ; joint++) {
			currentChains.at(chain).push_back(currentJoints.at(joint));
		}

	}
	return currentChains;
}
