#include "ChainQueue.h"

ChainQueue::ChainQueue (ChainID id)
	: queue<vector<float> >(),
	  chainID(id)
{

}

// Removes all values from the queue
void ChainQueue::clear(){
	while ( !empty() ){
		pop();
	}
}
