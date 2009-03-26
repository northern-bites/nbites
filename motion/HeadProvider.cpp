
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

//#define DEBUG_HEADPROVIDER

HeadProvider::HeadProvider(float motionFrameLength, shared_ptr<Sensors> s)
	: MotionProvider(HEAD_PROVIDER),
	  sensors(s),
	  FRAME_LENGTH_S(motionFrameLength),
	  chopper(sensors, FRAME_LENGTH_S),
      nextJoints(),
	  currCommand(new ChoppedCommand() ),
	  headCommandQueue()
{
    pthread_mutex_init (&scripted_mode_mutex, NULL);
    pthread_mutex_init (&set_mode_mutex, NULL);
}


// Motion Provider Methods
HeadProvider::~HeadProvider() {
    pthread_mutex_destroy(&scripted_mode_mutex);
    pthread_mutex_destroy(&set_mode_mutex);
    // remove all remaining values from chain queues
}

void HeadProvider::requestStopFirstInstance() {
    // Finish motion or stop immediately?
    //For the head, we will stop immediately:
    pthread_mutex_lock(&scripted_mode_mutex);
    pthread_mutex_lock(&set_mode_mutex);

    stopScripted();
    stopSet();
    setActive();

    pthread_mutex_unlock(&set_mode_mutex);
    pthread_mutex_unlock(&scripted_mode_mutex);

}

//Method called during the 'SCRIPTED' mode
void HeadProvider::calculateNextJoints() {

    switch(curMode){
    case SCRIPTED:
        pthread_mutex_lock(&scripted_mode_mutex);
        scriptedMode();
        pthread_mutex_unlock(&scripted_mode_mutex);
        break;
    case SET:
        pthread_mutex_lock(&set_mode_mutex);
        setMode();
        pthread_mutex_unlock(&set_mode_mutex);
        break;

    }

    setActive();
}

//Method called during the 'SET' Mode
void HeadProvider::setMode(){
    //Maximum head movement is Rad/motion frame (6 deg/20ms from AL docs)
    const float MAX_HEAD_VEL = 6.0*TO_RAD;

    //Calculate how much we can move toward the goal
    const float yawChangeTarget = clip(yawDest - lastYawDest,
                                       -MAX_HEAD_VEL,
                                       MAX_HEAD_VEL);
    const float pitchChangeTarget = clip(pitchDest - lastPitchDest,
                                         -MAX_HEAD_VEL,
                                         MAX_HEAD_VEL);
#ifdef DEBUG_HEADPROVIDER
     cout << "Last values "<<endl
          <<"   were       (" << lastYawDest <<","<< lastPitchDest <<")"<<endl
          <<"   added      ("<<yawChangeTarget<<","<<pitchChangeTarget<<")"<<endl
          <<"   target was ("<<yawDest<<","<<pitchDest<<")"<<endl;
#endif

    //update memory for next  run
    lastYawDest = lastYawDest+yawChangeTarget;
    lastPitchDest = lastPitchDest +pitchChangeTarget;


    //update the chain angles
    float newHeads[Kinematics::HEAD_JOINTS] = {lastYawDest,lastPitchDest};
    vector<float> newChainAngles  =
        vector<float>(newHeads,newHeads + Kinematics::HEAD_JOINTS);
    setNextChainJoints(HEAD_CHAIN,newChainAngles);
}

void HeadProvider::scriptedMode(){
    if ( currCommand->isDone() )
        setNextHeadCommand();

    if (!currCommand->isDone() ) {
        setNextChainJoints( HEAD_CHAIN, currCommand->getNextJoints(HEAD_CHAIN) );
    }
    else {
        setNextChainJoints( HEAD_CHAIN, getCurrentHeads() );
    }
}

void HeadProvider::setCommand(const SetHeadCommand *command) {
    pthread_mutex_lock(&set_mode_mutex);
    transitionTo(SET);
    yawDest = command->getYaw();
    pitchDest = command->getPitch();
    setActive();
    pthread_mutex_unlock(&set_mode_mutex);
}

void HeadProvider::setCommand(const HeadJointCommand *command) {
	pthread_mutex_lock(&scripted_mode_mutex);
    transitionTo(SCRIPTED);

    headCommandQueue.push(command); //HACK this should probably be mutexed
    setActive();
    pthread_mutex_unlock(&scripted_mode_mutex);
}

void HeadProvider::enqueueSequence(std::vector<HeadJointCommand*> &seq) {
	// Take in vec of commands and enqueue them all
	for (vector<HeadJointCommand*>::iterator i= seq.begin(); i != seq.end(); i++)
		setCommand(*i);
}

void HeadProvider::setNextHeadCommand() {


	if ( !headCommandQueue.empty() ) {
		const HeadJointCommand *command = headCommandQueue.front();
		currCommand = chopper.chopCommand(command);
		headCommandQueue.pop();

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
    const bool setDone = (yawDest == lastYawDest) && (pitchDest == lastPitchDest);
    const bool scriptedDone = currCommand->isDone()  && headCommandQueue.empty();
    switch(curMode){
    case SET:
        return setDone;
        break;
    case SCRIPTED:
        return scriptedDone;
        break;
    default:
        return true;
    }
}
void HeadProvider::stopScripted(){


    while(!headCommandQueue.empty()){
        const HeadJointCommand * cmd = headCommandQueue.front();
        delete cmd;
        headCommandQueue.pop();
    }
}

void HeadProvider::stopSet(){
    //set the target to our current location.
    yawDest = lastYawDest;
    pitchDest = lastPitchDest;
}

void HeadProvider::transitionTo(HeadMode newMode){
//Method to handle special cases when the state changes
    if(newMode != curMode){
        switch(newMode){
        case SCRIPTED:
            stopScripted();
            break;
        case SET:
            //If we need to switch modes, then we may not know what the latest
            //angles are, so lets get them again from sensors
            vector<float> mAngles = sensors->getMotionBodyAngles();
            lastYawDest =mAngles[0];
            lastPitchDest =mAngles[1];
            break;
        }
        curMode = newMode;
#ifdef DEBUG_HEADPROVIDER
    cout << "Transitioned to mode :"<<curMode<<endl;
#endif
    }else{
#ifdef DEBUG_HEADPROVIDER
    cout << "No transition need to get to :"<<curMode<<endl;
#endif
    }

}
