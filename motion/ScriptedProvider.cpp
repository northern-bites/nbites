#include "ScriptedProvider.h"
#include "Kinematics.h"



using namespace Kinematics;


ScriptedProvider::ScriptedProvider(float motionFrameLength,
								   Sensors *s)
	: MotionProvider(),
	  sensors(s),
	  FRAME_LENGTH_S(motionFrameLength),
	  chopper(sensors, FRAME_LENGTH_S),
	  nextJoints(),
	  choppedCommand(),
	  headQueue(HEAD_CHAIN),
	  lArmQueue(LARM_CHAIN),
	  lLegQueue(LLEG_CHAIN),
	  rLegQueue(RLEG_CHAIN),
	  rArmQueue(RARM_CHAIN),
	  commandQueue()
{


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
	if (lArmQueue.empty() && lLegQueue.empty() &&
		rLegQueue.empty() && rArmQueue.empty() ) {
		cout << "They're empty!" << endl;
		setNextCommand();
	}

	// Make sure first that the queues are not empty
	// If they're empty, then add the current joints to be the
	// next joints. If they're not empty, then add the queued
	// joints as the next Chain joints
  	if (lArmQueue.empty() ) {
 		setNextChainJoints( LARM_CHAIN,currentChains.at(LARM_CHAIN) );
  	}else {
		setNextChainJoints( LARM_CHAIN, lArmQueue.front() );
		lArmQueue.pop();
	}

  	if (lLegQueue.empty() ) {
 		setNextChainJoints( LLEG_CHAIN, currentChains.at(LLEG_CHAIN) );
  	}else {
		setNextChainJoints( LLEG_CHAIN, lLegQueue.front() );
		lLegQueue.pop();
	}

  	if (rLegQueue.empty() ){
 		setNextChainJoints( RLEG_CHAIN, currentChains.at(RLEG_CHAIN) );
  	}else {
		setNextChainJoints( RLEG_CHAIN, rLegQueue.front() );
		rLegQueue.pop();
	}

  	if (rArmQueue.empty() ) {
 		setNextChainJoints( RARM_CHAIN, currentChains.at(RARM_CHAIN) );
	}else {
		setNextChainJoints( RARM_CHAIN, rArmQueue.front() );
		rArmQueue.pop();
	}

}

// Adds new command to queue of commands.
// when the chainQueues are all empty,
// a command is popped, locked, and chopped.
void ScriptedProvider::enqueue(const BodyJointCommand *command) {
	commandQueue.push(command);
}

void ScriptedProvider::setNextCommand(){

	const BodyJointCommand *command = commandQueue.front();
	commandQueue.pop();
	choppedCommand = chopper.chopCommand(command);



	//Split command by chops
	vector<vector<vector<float> > >::iterator choppedIter;

	while (!choppedCommand.empty()){
		// Pass each chain to its chainqueue


		headQueue.push(choppedCommand.front().at(HEAD_CHAIN));
		lArmQueue.push(choppedCommand.front().at(LARM_CHAIN));
		lLegQueue.push(choppedCommand.front().at(LLEG_CHAIN));
		rLegQueue.push(choppedCommand.front().at(RLEG_CHAIN));
		rArmQueue.push(choppedCommand.front().at(RARM_CHAIN));
		choppedCommand.pop();
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
