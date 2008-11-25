#include "ChainQueue.h"

ChainQueue::ChainQueue (ChainID newChainID, float motionFrameLength)
	: chainID(newChainID), FRAME_LENGTH_S(motionFrameLength)
{

}

// Takes vector, sends to chopCommand,
void ChainQueue::add(vector<vector<float> > nextJoints){


}


// Removes all values from the queue
void ChainQueue::clear(){

}
