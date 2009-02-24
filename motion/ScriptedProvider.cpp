
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

#include <boost/shared_ptr.hpp>
#include "ScriptedProvider.h"

using boost::shared_ptr;

ScriptedProvider::ScriptedProvider(float motionFrameLength,
        shared_ptr<Sensors> s)
  : MotionProvider(SCRIPTED_PROVIDER),
    sensors(s),
    FRAME_LENGTH_S(motionFrameLength),
    chopper(sensors, FRAME_LENGTH_S),
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
    vector<ChainQueue>::iterator i;
    i = chainQueues.begin();
    while ( i != chainQueues.end() )
        i->clear();

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

//Checks if this chain is currently providing angles so that external
//classes can check the status of this one
void ScriptedProvider::setActive(){
    if(isDone())
        inactive();
    else
        active();
}

bool ScriptedProvider::isDone() {
    return chainQueuesEmpty() && commandQueueEmpty();
}

bool ScriptedProvider::chainQueuesEmpty(){
    bool isEmpty = true;
    for (unsigned int i=0 ; i<chainQueues.size() ; i++ ) {
        if ( !chainQueues.at(i).empty() ) {
            isEmpty=false;
        }
    }
    return isEmpty;
}

bool ScriptedProvider::commandQueueEmpty(){
    return bodyCommandQueue.empty();
}

void ScriptedProvider::calculateNextJoints() {

	if (chainQueuesEmpty())
		setNextBodyCommand();


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
void ScriptedProvider::setCommand(const BodyJointCommand *command) {
	if (command->getType() == MotionConstants::BODY_JOINT){
		bodyCommandQueue.push(command);
		setActive();
	}
	else {
		cout << "WRONG MOTION COMMAND IN SWITCHBOARD!!!" << endl;
	}
}


void ScriptedProvider::enqueueSequence(std::vector<const BodyJointCommand*> &seq) {
	// Take in vec of commands and enqueue them all
	pthread_mutex_lock(&scripted_mutex);
	for (vector<const BodyJointCommand*>::iterator i= seq.begin(); i != seq.end(); i++)
		setCommand(*i);
	pthread_mutex_unlock(&scripted_mutex);
}

void ScriptedProvider::setNextBodyCommand() {
	// If there are no more commands, don't try to enqueue one
	if ( !bodyCommandQueue.empty() ) {

		const BodyJointCommand *command = bodyCommandQueue.front();
		bodyCommandQueue.pop();
		ChoppedCommand * choppedBodyCommand = chopper.chopCommand(command);

		vector<ChainQueue>::iterator i;
		while (!choppedBodyCommand->isDone()) {
			// Pass each chain to its chainqueue

			// Skips the HEAD_CHAIN and enqueues all body chains
			i = chainQueues.begin();
			int chainID;
			while ( i != chainQueues.end() ) {
				// Subtract 1 because there is no head chain in the
				// choppedBodyCommand (it's only body joints)
				chainID = i->getChainID();
				i->push( choppedBodyCommand->getNextJoints(chainID) );
				i++;
			}
		}
		delete choppedBodyCommand;
	}
}

vector<vector<float> > ScriptedProvider::getCurrentChains() {
    vector<vector<float> > currentChains(NUM_CHAINS,vector<float>(0));

    vector<float> currentJoints = sensors->getBodyAngles();

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
