#include "ChopShop.h"

ChopShop::ChopShop (Sensors *s, float motionFrameLength)
	: finalJoints(0),
	  diffPerChop(22),
	  sensors(s),
	  FRAME_LENGTH_S(motionFrameLength),
	  choppedJoints(),
	  nextChain()
{

}

// Breaks command into FRAME_LENGTH_S size pieces,
// adds it to the queue
queue<vector<vector<float> > > ChopShop::chopCommand(const BodyJointCommand *command) {

	if (command->getType() == INTERPOLATION_LINEAR) {
		cout << "interpolate linearly" << endl;
		return chopLinear(command);
	}

	else if (command->getType() == INTERPOLATION_SMOOTH) {
		return chopSmooth(command);
	}


}

queue<vector<vector<float> > > ChopShop::chopCommand(const HeadJointCommand *command) {
	return chopLinear(command);
}

// Smooth interpolation motion
queue<vector<vector<float> > > ChopShop::chopSmooth(const BodyJointCommand *command) {
	// PLACE HOLDER
	queue<vector<vector<float> > > a;
	return a;
}

queue<vector<vector<float> > > ChopShop::chopSmooth(const HeadJointCommand *command) {
	// PLACE HOLDER
	queue<vector<vector<float> > > a;
	return a;
}

/*
 * Linear interpolation chopping:
 * Retrieves current joint angels and acquiries the differences
 * between the current and the intended final. Send them to 
 * 
 *
 *
 */
queue<vector<vector<float> > > ChopShop::chopLinear(const BodyJointCommand *command) {

	// Clear previous queue and vectors
 	while (!choppedJoints.empty())
 		choppedJoints.pop();
	diffPerChop.clear();

	// Get number of chops according to duration
	float numChops = command->getDuration()/FRAME_LENGTH_S;

	// Get current angles&errors from sensors
	vector<float> currentJoints = sensors->getBodyAngles();
	vector<float> currentJointErrors = sensors->getBodyAngleErrors();

	// Adjust to real currentJoints, not Sensors'
	for (int i=0; i<NUM_JOINTS ; i++) {
		currentJoints[i] = currentJoints[i]-currentJointErrors[i];
	}

	// Add final joints for all chains
	addFinalJoints(command, &currentJoints);

	//Get diff per chop from current to final
	for ( int joint_id=0; joint_id < NUM_JOINTS ;++joint_id) {
		diffPerChop.push_back( (finalJoints.at(joint_id) -
								currentJoints.at(joint_id)) /
							   numChops);
	}
	finalJoints.clear();

	chopThat(numChops, &currentJoints);
	return choppedJoints;
}



// Takes final joint values and
void ChopShop::chopThat(float numChops, vector<float> *currentJoints) {
	float nextVal(0);

	for (int num_chopped=1; num_chopped<=numChops; num_chopped++ ) {
		vector<vector<float> > nextChopped(5);

		int lastChainJoint = 0;
		for (int chain=0,joint=0; chain<NUM_CHAINS; chain++) {
			lastChainJoint += chain_lengths[chain];
			for ( ; joint < lastChainJoint ; joint++) {
				nextVal = currentJoints->at(joint)+ diffPerChop.at(joint)*num_chopped;
				nextChopped.at(chain).push_back(nextVal);
			}
		}
		choppedJoints.push(nextChopped);
	}


} // END CHOP THAT


void ChopShop::addFinalJoints(const BodyJointCommand *command,
								  vector<float>* currentJoints) {
	vector<float>::iterator currentStart = currentJoints->begin();
	vector<float>::iterator currentEnd = currentJoints->begin();

	for (int chain=0; chain < NUM_CHAINS;chain++) {
		// First, get chain joints from command
		nextChain = command->getJoints((ChainID)chain);

		// Set the end iterator
		currentEnd += chain_lengths[chain];

		// If the next chain is not queued (empty), add current joints
		if ( nextChain == 0 ||
			 nextChain->empty()) {
			finalJoints.insert(finalJoints.end(), currentStart, currentEnd );
		}else {
			// Add each chain of joints to the final joints
			finalJoints.insert( finalJoints.end(),
								nextChain->begin(),
								nextChain->end() );
		}
		// Set the start iterator into the right position for the
		// next chain
		currentStart += chain_lengths[chain];

	}
	finalJointsToRad();
}

void ChopShop::finalJointsToRad() {
	vector<float>::iterator i;
	i = finalJoints.begin();
	i++;
	while ( i != finalJoints.end() ) {
		// Convert joints to radians from degrees
		*i = *i / 57.29577;
		i++;
	}
}
