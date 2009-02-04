
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
#include "HeadProvider.h"

using boost::shared_ptr;

HeadProvider::HeadProvider(float motionFrameLength, shared_ptr<Sensors> s)
	: MotionProvider(),
	  sensors(s),
	  FRAME_LENGTH_S(motionFrameLength),
	  chopper(sensors, FRAME_LENGTH_S),
      nextJoints(),
	  headQueue(HEAD_CHAIN),
	  headCommandQueue()
{
    pthread_mutex_init (&head_mutex, NULL);
}


// Motion Provider Methods
HeadProvider::~HeadProvider() {
    // remove all remaining values from chain queues
}

void HeadProvider::requestStop() {
    // Finish motion or stop immediately?
}

void HeadProvider::calculateNextJoints() {
    vector<float> currentHeads = getCurrentHeads();

    if ( headQueue.empty() )
        setNextHeadCommand();

    if (!headQueue.empty() ) {
        setNextChainJoints( HEAD_CHAIN, headQueue.front() );
        headQueue.pop();
    }
    else {
        setNextChainJoints( HEAD_CHAIN, getCurrentHeads() );
    }
    //setActive();
}

void HeadProvider::setCommand(HeadJointCommand *command) {
	if (command->getType() == MotionConstants::HEAD_JOINT){
		headCommandQueue.push(command);
		setActive();
	}
}

void HeadProvider::enqueueSequence(std::vector<HeadJointCommand*> &seq) {
	// Take in vec of commands and enqueue them all
	pthread_mutex_lock(&head_mutex);
	for (vector<HeadJointCommand*>::iterator i= seq.begin(); i != seq.end(); i++)
		setCommand(*i);
	pthread_mutex_unlock(&head_mutex);
}

void HeadProvider::setNextHeadCommand() {


	if ( !headCommandQueue.empty() ) {
		HeadJointCommand *command = headCommandQueue.front();
		queue<vector<vector<float> > >*	choppedHeadCommand =
			chopper.chopCommand(command);
		headCommandQueue.pop();
		delete command;

		while (!choppedHeadCommand->empty()) {
			// Push commands onto head queue
			headQueue.push(choppedHeadCommand->front().at(HEAD_CHAIN));
			choppedHeadCommand->pop();
		}

		delete choppedHeadCommand;
	}

}

vector<float> HeadProvider::getCurrentHeads() {
    vector<float> currentHeads(HEAD_JOINTS);

    for (unsigned int i=0; i<HEAD_JOINTS ; i++) {
        currentHeads[i] =
            sensors->getBodyAngle(i) - sensors->getBodyAngleError(i);
    }

    return currentHeads;
}

void HeadProvider::setActive(){
    isDone() ? inactive() : active();
}


bool HeadProvider::isDone(){
    return  headQueue.empty()  && headCommandQueue.empty();
}
