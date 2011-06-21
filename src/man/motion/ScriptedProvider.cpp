
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

using namespace std;
using namespace Kinematics;
using boost::shared_ptr;

ScriptedProvider::ScriptedProvider(shared_ptr<Sensors> s,
				   shared_ptr<Profiler> p)
    : MotionProvider(SCRIPTED_PROVIDER, p),
      sensors(s),
      chopper(sensors),
      // INITIALIZE WITH NULL/FINISHED CHOPPED COMMAND
      currCommand(new ChoppedCommand()),
      bodyCommandQueue()

{
    // Create mutexes
    // (?) Need one mutex per queue (?)
    pthread_mutex_init (&scripted_mutex, NULL);
}

ScriptedProvider::~ScriptedProvider() {
    // Wait until not active anymore
    while ( isActive() );
    pthread_mutex_destroy(&scripted_mutex);
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
    pthread_mutex_lock(&scripted_mutex);
    while(!bodyCommandQueue.empty()){
        const BodyJointCommand * cmd = bodyCommandQueue.front();
        delete cmd;
        bodyCommandQueue.pop();
    }
    currCommand = ChoppedCommand::ptr(new ChoppedCommand());
    setActive();
    pthread_mutex_unlock(&scripted_mutex);
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
    return currCommand->isDone();
}

// Do we have any BodyCommands left to run through?
bool ScriptedProvider::commandQueueEmpty(){
    return bodyCommandQueue.empty();
}

void ScriptedProvider::calculateNextJointsAndStiffnesses() {
    PROF_ENTER(profiler,P_SCRIPTED);
    pthread_mutex_lock(&scripted_mutex);
    if (currCommandEmpty())
	setNextBodyCommand();


    // Go through the chains and enqueue the next
    // joints from the ChoppedCommand.
    shared_ptr<vector <vector <float> > > currentChains(getCurrentChains());

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
    pthread_mutex_unlock(&scripted_mutex);
    PROF_EXIT(profiler,P_SCRIPTED);
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
    pthread_mutex_lock(&scripted_mutex);
    bodyCommandQueue.push(command);
    setActive();
    pthread_mutex_unlock(&scripted_mutex);
}


void ScriptedProvider::enqueueSequence(std::vector<const BodyJointCommand*> &seq) {
    // Take in vec of commands and enqueue them all
    for (vector<const BodyJointCommand*>::iterator i= seq.begin(); i != seq.end(); i++)
	setCommand(*i);
}

void ScriptedProvider::setNextBodyCommand() {

    // If there are no more commands, don't try to enqueue one
    if ( !bodyCommandQueue.empty() ) {

	const BodyJointCommand *nextCommand = bodyCommandQueue.front();
	bodyCommandQueue.pop();

	// Replace the current command and delete the
	// next command object
	PROF_ENTER(profiler, P_CHOPPED);
	const bool useComPreviews = true;
	currCommand = chopper.chopCommand(nextCommand, useComPreviews);
	PROF_EXIT(profiler, P_CHOPPED);
    }
}

// Get the chain's current positions. Gives a shared pointer to
// a vector containing a vector for each chain. Makes accessing
// each chain very simple.
shared_ptr<vector<vector<float> > > ScriptedProvider::getCurrentChains() {
    shared_ptr<vector<vector<float> > >currentChains(
	new vector<vector<float> >(Kinematics::NUM_CHAINS) );

    vector<float> currentJoints = sensors->getBodyAngles();

    unsigned int lastChainJoint,joint,chain;
    lastChainJoint= 0;
    joint = 0;
    chain = 0;

    vector<vector<float> >::iterator i = currentChains->begin();

    while (i != currentChains->end() ) {
        lastChainJoint += chain_lengths[chain];

        for ( ; joint < lastChainJoint ; joint++) {
            i->push_back(currentJoints.at(joint));
        }

	++i;
	++chain;
    }
    return currentChains;
}
