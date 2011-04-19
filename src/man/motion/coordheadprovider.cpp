
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
#include "NaoPose.h"

using namespace std;

using namespace Kinematics;
using boost::shared_ptr;

//#define DEBUG_HEADPROVIDER

CoordHeadProvider::CoordHeadProvider(shared_ptr<Sensors> s, shared_ptr<Profiler> p, shared_ptr<NaoPose> _pose)
	: MotionProvider(HEAD_PROVIDER, p),
	  sensors(s),
	  pose(_pose),
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
    pthread_mutex_init (&coord_head_provider_mutex, NULL);
}


// Motion Provider Methods
CoordHeadProvider::~CoordHeadProvider() {
    pthread_mutex_destroy(&coord_head_provider_mutex);
    // remove all remaining values from chain queues
}

void HeadProvider::requestStopFirstInstance() {
    // Finish motion or stop immediately?
    //For the head, we will stop immediately:
    pthread_mutex_lock(&coord_head_provider_mutex);
    stopScripted();
    stopSet();
    setActive();
    pthread_mutex_unlock(&coord_head_provider_mutex);

}

void HeadProvider::hardReset(){
    pthread_mutex_lock(&coord_head_provider_mutex);
    stopScripted();
    stopSet();
    setActive();
    pthread_mutex_unlock(&coord_head_provider_mutex);
}


//Method called from MotionSwitchboard
void HeadProvider::calculateNextJointsAndStiffnesses() {
	PROF_ENTER(profiler,P_HEAD);
	pthread_mutex_lock(&coord_head_provider_mutex);
	coordMode();
    }
    setActive();
	pthread_mutex_unlock(&coord_head_provider_mutex);
	PROF_EXIT(profiler,P_HEAD);
}

//Method called for coord mode
void HeadProvider::coordMode(){
  //Maximum head movement is deg/motion frame (6 deg/20ms from AL docs)
  const float MAX_HEAD_VEL = 6.0f*TO_RAD;

  //Calculate how much we can move toward the goal
  const float yawChangeTarget = NBMath::clip(yawDest - lastYawDest, -yawMaxSpeed, yawMaxSpeed);
  const float pitchChangeTarget = NBMath::clip(pitchDest - lastPitchDest, -pitchMaxSpeed, pitchMaxSpeed);

  //update memory for next run
  lastYawDest = lastYawDest+yawChangeTarget;
  lastPitchDest = lastPitchDest+pitchChangeTarget;

  //update the chain angles
  float newHeads[Kinematics::HEAD_JOINTS] = {lastYawDest,lastPitchDest};
  vector<float> newChainAngles  =
    vector<float>(newHeads,newHeads + Kinematics::HEAD_JOINTS);
  setNextChainJoints(HEAD_CHAIN,newChainAngles);
  
  vector<float> head_gains(HEAD_JOINTS, headSetStiffness);
  //Return the stiffnesses for each joint
  setNextChainStiffnesses(HEAD_CHAIN,head_gains);
}

void HeadProvider::setCommand(const CoordHeadCommand *command) {
    pthread_mutex_lock(&coord_head_provider_mutex);

    yawDest = atan(command->getRelX()/command->getRelY());

    float dist = sqrt(pow(command->getRelX(),2) + pow(command->getRelY(),2));
    pitchDest = atan((pose->getFocalPointInWorldFrameZ()-command->getRelZ())/dist)-40.0f;

    yawMaxSpeed = command->getMaxSpeedYaw();
    pitchMaxSpeed = command->getMaxSpeedPitch();

    setActive();
    pthread_mutex_unlock(&coord_head_provider_mutex);
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
    return ((yawDest == lastYawDest) && (pitchDest == lastPitchDest));
}

void HeadProvider::stopSet(){
    //set the target to our current location.
    yawDest = lastYawDest;
    pitchDest = lastPitchDest;
}

void HeadProvider::transitionTo(HeadMode newMode){
//Method to handle special cases when the state changes

  //If we need to switch modes, then we may not know what the latest
  //angles are, so lets get them again from sensors
  vector<float> mAngles = sensors->getMotionBodyAngles();
  lastYawDest =mAngles[0];
  lastPitchDest =mAngles[1];

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
