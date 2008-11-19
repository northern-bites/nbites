#include "ChainQueue.h"

ChainQueue::ChainQueue (ChainID newChainID, float motionFrameLength)
	: chainID(newChainID), FRAME_LENGTH_S(motionFrameLength)
{

}

// Takes vector, sends to chopCommand,
void ChainQueue::add(BodyJointCommand *command){
	chopCommand(command);

}

// Breaks command into FRAME_LENGTH_S size pieces,
// adds it to the queue
void ChainQueue::chopCommand(BodyJointCommand *command){
	// Checks type of body command and sends to
	// appropriate function
	if (command->getType() == INTERPOLATION_LINEAR){
		chopLinear(command);
	}

	else if (command->getType() == INTERPOLATION_SMOOTH){
		chopSmooth(command);
	}

	else{
		// ERROR!
	}
}

// Smooth interpolation motion
void ChainQueue::chopSmooth(BodyJointCommand *command){

}

// Linear interpolation motion
void ChainQueue::chopLinear(BodyJointCommand *command){
	// Get number of chops according to duration
	float numChops = command->getDuration()/FRAME_LENGTH_S;

	// Final joints
	const vector<float> *finalJoints = command->getJoints(chainID);

	// Get current angles ??????
	// NEEDS SENSORS OBJECT
	//const vector<float> *currentJoints = sensors->getJoints(chainID);

	//Get diff from current to final

	for (int i=0; i<finalJoints.size();++i){
		vector<float> diff = finalJoints[i]-currentJoints[i];
	}

	// split diff by numChops


}

// Removes all values from the queue
void ChainQueue::clear(){

}
