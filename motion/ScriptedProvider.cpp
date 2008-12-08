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
	  rArmQueue(RARM_CHAIN)
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
	cout << "Calculating next joints!" << endl;
	// Sets the next joints for all the joints
	if (!lArmQueue.empty() ||
		!rArmQueue.empty() ||
		!lLegQueue.empty() ||
		!rLegQueue.empty() ){
		cout << "adding new joints" << endl;
		setNextChainJoints( LARM_CHAIN, lArmQueue.front() );
		setNextChainJoints( LLEG_CHAIN, lLegQueue.front() );
		setNextChainJoints( RLEG_CHAIN, rLegQueue.front() );
		setNextChainJoints( RARM_CHAIN, rArmQueue.front() );
		cout << "added new joints" << endl;
		lArmQueue.pop();
		lLegQueue.pop();
		rLegQueue.pop();
		rArmQueue.pop();
	}

	cout << "calculated next joints" << endl;
}

void ScriptedProvider::enqueue(const BodyJointCommand *command) {
	// Chop command
	choppedCommand = chopper.chopCommand(command);
	cout << "ENQUEING COMMAND" << endl;
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
