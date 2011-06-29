
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

using namespace std;

using namespace Kinematics;
using boost::shared_ptr;

//#define DEBUG_HEADPROVIDER

HeadProvider::HeadProvider(shared_ptr<Sensors> s,shared_ptr<Profiler> p)
    : MotionProvider(HEAD_PROVIDER, p),
      sensors(s),
      chopper(sensors),
      nextJoints(),
      currCommand(new ChoppedCommand() ),
      headCommandQueue(),
      curMode(SCRIPTED),
      yawDest(0.0f), pitchDest(0.0f),
      lastYawDest(0.0f), lastPitchDest(0.0f),
      pitchMaxSpeed(0.0f), yawMaxSpeed(0.0f),
      headSetStiffness(0.6f)
{
    pthread_mutex_init (&head_provider_mutex, NULL);
}


// Motion Provider Methods
HeadProvider::~HeadProvider() {
    pthread_mutex_destroy(&head_provider_mutex);
}

void HeadProvider::requestStopFirstInstance() {
    // Finish motion or stop immediately?
    //For the head, we will stop immediately:
    pthread_mutex_lock(&head_provider_mutex);
    stopScripted();
    stopSet();
    setActive();
    pthread_mutex_unlock(&head_provider_mutex);

}

void HeadProvider::hardReset(){
    pthread_mutex_lock(&head_provider_mutex);
    stopScripted();
    stopSet();
    setActive();
    pthread_mutex_unlock(&head_provider_mutex);
}


//Method called from MotionSwitchboard
void HeadProvider::calculateNextJointsAndStiffnesses() {
    PROF_ENTER(profiler,P_HEAD);
    pthread_mutex_lock(&head_provider_mutex);
    switch(curMode){
    case SCRIPTED:
        scriptedMode();
        break;
    case SET:
        setMode();
        break;
	case COORD:
		//dummy case to remove unnecessary cmake warning
		break;
    }
    setActive();
    pthread_mutex_unlock(&head_provider_mutex);
    PROF_EXIT(profiler,P_HEAD);
}

//Method called during the 'SET' Mode
void HeadProvider::setMode(){
    //Maximum head movement is Rad/motion frame (6 deg/20ms from AL docs)
    const float MAX_HEAD_VEL = 6.0f*TO_RAD;/* ** *///we don't use this...

    //Calculate how much we can move toward the goal
    const float yawChangeTarget = NBMath::clip(yawDest - lastYawDest,
                                               - yawMaxSpeed,
                                               yawMaxSpeed);
    const float pitchChangeTarget = NBMath::clip(pitchDest - lastPitchDest,
                                                 -pitchMaxSpeed,
                                                 pitchMaxSpeed);
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
    vector<float> newChainAngles;
	newChainAngles.push_back(lastYawDest);
	newChainAngles.push_back(lastPitchDest);
    setNextChainJoints(HEAD_CHAIN,newChainAngles);

    vector<float> head_gains(HEAD_JOINTS, headSetStiffness);
    //Return the stiffnesses for each joint
    setNextChainStiffnesses(HEAD_CHAIN,head_gains);
}

void HeadProvider::scriptedMode(){
    if ( currCommand->isDone() )
        setNextHeadCommand();

    if (!currCommand->isDone() ) {
        setNextChainJoints( HEAD_CHAIN,
			    currCommand->getNextJoints(HEAD_CHAIN) );
	setNextChainStiffnesses( Kinematics::HEAD_CHAIN,
				 currCommand->getStiffness(
				     Kinematics::HEAD_CHAIN) );

    }
    else {
        setNextChainJoints( HEAD_CHAIN, getCurrentHeads() );
	setNextChainStiffnesses( Kinematics::HEAD_CHAIN,
				 vector<float>(HEAD_JOINTS, 0.0f) );
    }


}

void HeadProvider::setCommand(const SetHeadCommand::ptr command) {
    pthread_mutex_lock(&head_provider_mutex);

    transitionTo(SET);
    yawDest = command->getYaw();
    pitchDest = command->getPitch();
	yawMaxSpeed = command->getMaxSpeedYaw();
	pitchMaxSpeed = command->getMaxSpeedPitch();

    /* ** *///debugging speed clipping (should probably stay in some form)
	yawMaxSpeed = clip(yawMaxSpeed,0,Kinematics::jointsMaxVelNominal[Kinematics::HEAD_YAW]*.1);
	pitchMaxSpeed = clip(pitchMaxSpeed,0,Kinematics::jointsMaxVelNominal[Kinematics::HEAD_PITCH]*.1);

    setActive();

    pthread_mutex_unlock(&head_provider_mutex);
}

void HeadProvider::setCommand(const HeadJointCommand::ptr command) {
    pthread_mutex_lock(&head_provider_mutex);

    transitionTo(SCRIPTED);
    headCommandQueue.push(command); //HACK this should probably be mutexed
    setActive();

    pthread_mutex_unlock(&head_provider_mutex);
}

void HeadProvider::enqueueSequence(std::vector<HeadJointCommand::ptr> &seq) {
    // Take in vec of commands and enqueue them all
    vector<HeadJointCommand::ptr>::iterator i;
    for (i = seq.begin(); i != seq.end(); ++i)
	setCommand(*i);
}

void HeadProvider::setNextHeadCommand() {
    if ( !headCommandQueue.empty() ) {
	currCommand = chopper.chopCommand(headCommandQueue.front());
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
    const bool setDone = ((yawDest == lastYawDest) && (pitchDest == lastPitchDest));
    const bool scriptedDone = (currCommand->isDone()  && headCommandQueue.empty());
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
    while(!headCommandQueue.empty())
        headCommandQueue.pop();

    currCommand = ChoppedCommand::ptr(new ChoppedCommand());
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
        case SET:{
            //If we need to switch modes, then we may not know what the latest
            //angles are, so lets get them again from sensors
            vector<float> mAngles = sensors->getMotionBodyAngles();
            lastYawDest =mAngles[0];
            lastPitchDest =mAngles[1];
            break;
		}
		case COORD:
			//dummy case to remove unnecessary cmake warning
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
