#include "HeadProvider.h"

HeadProvider::HeadProvider(float motionFrameLength,
						   Sensors *s)
	: MotionProvider(),
	  sensors(s),
	  FRAME_LENGTH_S(motionFrameLength),
	  chopper(sensors, FRAME_LENGTH_S),
	  headCommandQueue(),
	  headQueue(HEAD_CHAIN)
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

void HeadProvider::enqueue(const HeadJointCommand *command) {
	headCommandQueue.push(command);
}

void HeadProvider::enqueueSequence(std::vector<const HeadJointCommand*> &seq) {
	// Take in vec of commands and enqueue them all
	pthread_mutex_lock(&head_mutex);
	for (vector<const HeadJointCommand*>::iterator i= seq.begin(); i != seq.end(); i++)
		enqueue(*i);
	pthread_mutex_unlock(&head_mutex);
}

void HeadProvider::setNextHeadCommand() {

	if ( !headCommandQueue.empty() ) {
		const HeadJointCommand *command = headCommandQueue.front();
		queue<vector<vector<float> > >*	choppedHeadCommand = chopper.chopCommand(command);
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

	for (int i=0; i<HEAD_JOINTS ; i++) {
		currentHeads[i] = sensors->getBodyAngle(i) - sensors->getBodyAngleError(i);
	}

	return currentHeads;
}
