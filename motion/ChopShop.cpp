#include "ChopShop.h"
#include <algorithm>

ChopShop::ChopShop (Sensors *s, float motionFrameLength)
	: finalJoints(),
	  diffPerChop(22),
	  sensors(s),
	  FRAME_LENGTH_S(motionFrameLength),
	  choppedJoints(),
	  nextChain()


{

}

// Breaks command into FRAME_LENGTH_S size pieces,
// adds it to the queue
queue<vector<vector<float> > > ChopShop::chopCommand(const BodyJointCommand *command){
	// Checks type of body command and sends to
	// appropriate function
//  if (command->getType() == INTERPOLATION_LINEAR){
	cout << "Chopping command!!!" << endl;
	return chopLinear(command);
//  }

//  else if (command->getType() == INTERPOLATION_SMOOTH){
	//  return chopSmooth(command);
//  }


}

// Smooth interpolation motion
queue<vector<vector<float> > > ChopShop::chopSmooth(const BodyJointCommand *command){
	queue<vector<vector<float> > > a;
	return a;
}

// Linear interpolation motion
// Maybe allocate stuff on heap?
queue<vector<vector<float> > > ChopShop::chopLinear(const BodyJointCommand *command){


 	while (!choppedJoints.empty()){
 		choppedJoints.pop();
 	}
	diffPerChop.clear();

	// Get number of chops according to duration
	float numChops = command->getDuration()/FRAME_LENGTH_S;
	// Get current angles



	vector<float> currentJoints = sensors->getBodyAngles();
	vector<float> currentJointErrors = sensors->getBodyAngleErrors();

	for (int i=0; i<NUM_JOINTS ; i++){
		currentJoints[i] = currentJoints[i]-currentJointErrors[i];
	}

	// Add final joints for all chains
	addFinalJoints(command, HEAD_CHAIN, &currentJoints);
	addFinalJoints(command, LARM_CHAIN, &currentJoints);
	addFinalJoints(command, LLEG_CHAIN, &currentJoints);
	addFinalJoints(command, RLEG_CHAIN, &currentJoints);
	addFinalJoints(command, RARM_CHAIN, &currentJoints);

	//Get diff per chop from current to final
	for ( int joint_id=0; joint_id < NUM_JOINTS ;++joint_id){
		diffPerChop.push_back( (finalJoints.at(joint_id) -
								currentJoints.at(joint_id)) /
							   numChops);
	}
	finalJoints.clear();
	// @JGM need to add vector for each chain!
    // Make vectors of new joints



	chopThat(numChops, &currentJoints);
	return choppedJoints;
}

void ChopShop::chopThat(float numChops, vector<float> *currentJoints) {
	cout << "chopthatshit" << endl;
	float nextVal(0);

	for (int num_chopped=1; num_chopped<=numChops; num_chopped++ ) {
		vector<vector<float> > nextChopped(5);

		int lastChainJoint = 0;
		for (int chain=0,joint=0; chain<NUM_CHAINS; chain++) {
			lastChainJoint += chain_lengths[chain];

			for ( ; joint < lastChainJoint ; joint++){
				nextVal = currentJoints->at(joint)+ diffPerChop.at(joint)*num_chopped;
//				cout << "nextVal for joint " << joint << " is " << nextVal << endl;
				nextChopped.at(chain).push_back(nextVal);
			}
		}
		choppedJoints.push(nextChopped);
	}


} // END CHOP THAT


void ChopShop::addFinalJoints(const BodyJointCommand *command,
							  ChainID id,
							  vector<float>* currentJoints) {


	// First, get chain joints from command
	nextChain = command->getJoints(id);


	vector<float>::const_iterator currentStart = currentJoints->begin();
	vector<float>::const_iterator currentEnd = currentJoints->begin();

	// Then, copy into end of new vector


		// If the next chain is not queued (empty), add current joints
	if ( nextChain == 0 ||
		 nextChain->empty()){
		cout << "nextChain is empty" << endl;
		switch (id) {

		case HEAD_CHAIN:
			// Set the iterators to encompass the head chain
			currentEnd += chain_lengths[HEAD_CHAIN];

			// Add the head to the final joints

			finalJoints.insert(finalJoints.end(), currentStart, currentEnd );

			break;

		case LARM_CHAIN:
			// Set the iterators around the larm chain
			currentStart += chain_lengths[HEAD_CHAIN];
			currentEnd += (chain_lengths[HEAD_CHAIN] +
						   chain_lengths[LARM_CHAIN]);

			// Add the larm chain to the final joints
			finalJoints.insert(finalJoints.end(), currentStart, currentEnd );
			break;

		case LLEG_CHAIN:
			// Set the iterators around the lleg
			currentStart += (chain_lengths[HEAD_CHAIN] +
							 chain_lengths[LARM_CHAIN]);

			currentEnd += (chain_lengths[HEAD_CHAIN] +
						   chain_lengths[LARM_CHAIN] +
						   chain_lengths[LLEG_CHAIN]);

			// Add lleg to final joints
			finalJoints.insert(finalJoints.end(), currentStart, currentEnd );
			break;

		case RLEG_CHAIN:
			// Set the iterators around the rleg
			currentStart += (chain_lengths[HEAD_CHAIN] +
							 chain_lengths[LARM_CHAIN] +
							 chain_lengths[LLEG_CHAIN] );

			currentEnd += (chain_lengths[HEAD_CHAIN] +
						   chain_lengths[LARM_CHAIN] +
						   chain_lengths[LLEG_CHAIN] +
						   chain_lengths[RLEG_CHAIN]);

			// FinalJoints. rleg to final joints
			finalJoints.insert(finalJoints.end(), currentStart, currentEnd );
			break;

		case RARM_CHAIN:
			// Set the iterators from rarm beginning to end of currentJoints
			currentStart += (chain_lengths[HEAD_CHAIN] +
							 chain_lengths[LARM_CHAIN] +
							 chain_lengths[LLEG_CHAIN] +
							 chain_lengths[RLEG_CHAIN] );
			currentEnd = currentJoints->end();

			// Copy rarm to final joints
			finalJoints.insert(finalJoints.end(), currentStart, currentEnd ) ;
			break;
		}


	}

	else{
		cout << "nextChain is NOT empty" << endl;

// Add each chain of joints to the final joints


		finalJoints.insert( finalJoints.end(),
							nextChain->begin(),
							nextChain->end() );


	}



}
