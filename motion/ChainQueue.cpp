#include "ChainQueue.h"

ChainQueue::ChainQueue (ChainID newChainID)
	: queue<vector<float> >(),
	  chainID(newChainID)
{

}

// Removes all values from the queue
void ChainQueue::clear(){
	while ( !empty() ){
		pop();
	}
}
