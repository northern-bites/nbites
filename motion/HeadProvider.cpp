#include "HeadProvider.h"

HeadProvider::HeadProvider(float motionFrameLength,
						   Sensors *s)
	: MotionProvider(),
	  sensors(s),
	  FRAME_LENGTH_S(motionFrameLength),
	  chopper(sensors, FRAME_LENGTH_S),
	  nextJoints(),
	  choppedHeadCommand(),
	  headCommandQueue()

{
	pthread_mutex_init (&head_mutex, NULL);
}

HeadProvider::~HeadProvider() {
	// remove all remaining values from chain queues
}

void HeadProvider::requestStop() {
	// Finish motion or stop immediately?
}

void HeadProvider::calculateNextJoints() {

}
void HeadProvider::enqueue(const HeadJointCommand *command) {
	headCommandQueue.push(command);
}

void HeadProvider::enqueueSequence(std::vector<HeadJointCommand*> &seq) {
	// Take in vec of commands and enqueue them all
	pthread_mutex_lock(&head_mutex);
	for (vector<HeadJointCommand*>::iterator i= seq.begin(); i != seq.end(); i++)
		enqueue(*i);
	pthread_mutex_unlock(&head_mutex);
}


void HeadProvider::setNextHeadCommand() {

	if ( !headCommandQueue.empty() ) {
		const HeadJointCommand *command = headCommandQueue.front();
		headCommandQueue.pop();
		choppedHeadCommand = chopper.chopCommand(command);
		delete command;

		while (!choppedHeadCommand.empty()){
			// Pass each chain to its chainqueue
			chainQueues.at(HEAD_CHAIN).push(choppedHeadCommand.front().at(HEAD_CHAIN));
			choppedHeadCommand.pop();

		}



	}

}

vector<float> HeadProvider::getCurrentHeads() {
	vector<float> currentHeads(HEAD_JOINTS);

	for (int i=0; i<HEAD_JOINTS ; i++) {
		currentHeads[i] = sensors->getBodyAngle(i) -
			sensors->getBodyAngleError(i);
	}

	return currentHeads;
}
