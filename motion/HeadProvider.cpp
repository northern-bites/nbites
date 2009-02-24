
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
	: MotionProvider(HEAD_PROVIDER),
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

void HeadProvider::requestStopFirstInstance() {
    // Finish motion or stop immediately?
}

//Method called during the 'SCRIPTED' mode
void HeadProvider::calculateNextJoints() {

    switch(curMode){
    case SCRIPTED:
        scriptedMode();
        break;
    case SET:
        setMode();
        break;
    }

    setActive();
}

//Method called during the 'SET' Mode
void HeadProvider::setMode(){
    float newHeads[Kinematics::HEAD_JOINTS] = {yawDest,pitchDest};
    vector<float> newChainAngles = vector<float>(
                                                 newHeads,newHeads +
                                                 Kinematics::HEAD_JOINTS);
    setNextChainJoints(HEAD_CHAIN,newChainAngles);
}

void HeadProvider::scriptedMode(){
    if ( headQueue.empty() )
        setNextHeadCommand();

    if (!headQueue.empty() ) {
        setNextChainJoints( HEAD_CHAIN, headQueue.front() );
        headQueue.pop();
    }
    else {
        setNextChainJoints( HEAD_CHAIN, getCurrentHeads() );
    }
}

void HeadProvider::setCommand(const SetHeadCommand *command) {
    transitionTo(SET);
}

void HeadProvider::setCommand(const HeadJointCommand *command) {
    transitionTo(SCRIPTED);
    headCommandQueue.push(command);
    setActive();
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
		const HeadJointCommand *command = headCommandQueue.front();
		ChoppedCommand * choppedHeadCommand =
			chopper.chopCommand(command);
		headCommandQueue.pop();
		delete command;

		while (!choppedHeadCommand->isDone()) {
			// Push commands onto head queue
			headQueue.push(choppedHeadCommand->getNextJoints(HEAD_CHAIN));
		}
		delete choppedHeadCommand;
	}

}

vector<float> HeadProvider::getCurrentHeads() {
    vector<float> currentHeads(HEAD_JOINTS);
    const vector<float> currentJoints = sensors->getMotionBodyAngles();

    for (unsigned int i=0; i<HEAD_JOINTS ; i++) {
        currentHeads[i] = currentJoints[i];
    }

    return currentHeads;
}

void HeadProvider::setActive(){
    isDone() ? inactive() : active();
}


bool HeadProvider::isDone(){
    return  headQueue.empty()  && headCommandQueue.empty() && curMode !=SET;
}

void HeadProvider::transitionTo(HeadMode newMode){
    if(newMode != curMode){
        switch(newMode){
        case SCRIPTED:
            //clear anything in the queues
            headQueue.clear();
            while(!headCommandQueue.empty()){
                const HeadJointCommand * cmd = headCommandQueue.front();
                delete cmd;
                headCommandQueue.pop();
            }
            break;
        case SET:
            //record the current head angles from sensors
            vector<float> mAngles = sensors->getMotionBodyAngles();
            yawDest = mAngles[0];
            pitchDest = mAngles[1];
        }
        curMode = newMode;
    }
}
