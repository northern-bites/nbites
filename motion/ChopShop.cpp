#include "ChopShop.h"

ChopShop::ChopShop (Sensors *s, float motionFrameLength)
	: sensors(s),
	  FRAME_LENGTH_S(motionFrameLength),
	  choppedJoints(),
	  nextChain()
{

}

// Breaks command into FRAME_LENGTH_S size pieces,
// adds it to the queue
queue<vector<vector<float> > > ChopShop::chopCommand(const BodyJointCommand *command) {
	// It's a BJC so it deals with 4 chains
	chainList = new vector<int>(0);
	chainList->push_back(1);
	chainList->push_back(2);
	chainList->push_back(3);
	chainList->push_back(4);

	if (command->getType() == INTERPOLATION_LINEAR) {
		return chopLinear(command);
	}

	else if (command->getType() == INTERPOLATION_SMOOTH) {
		return chopSmooth(command);
	}


}

// Smooth interpolation motion
queue<vector<vector<float> > >
ChopShop::chopSmooth(const BodyJointCommand *command) {

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
queue<vector<vector<float> > >
ChopShop::chopLinear(const BodyJointCommand *command) {

	// Clear previous queue and vectors
 	while (!choppedJoints.empty())
 		choppedJoints.pop();

	// Get number of chops according to duration
	float numChops = command->getDuration()/FRAME_LENGTH_S;

	vector<float> currentJoints = getCurrentJoints();
	// Add final joints for all chains
	vector<float> finalJoints = getFinalJoints(command, &currentJoints);

	//Get diff per chop from current to final

	vector<float> diffPerChop = getDiffPerChop(numChops,
											   &currentJoints,
											   &finalJoints);

	buildChops(numChops, &currentJoints, &diffPerChop);
	return choppedJoints;
}

vector<float> ChopShop::getCurrentJoints() {

	// Get current angles&errors from sensors
	vector<float> currentJoints = sensors->getBodyAngles();
	vector<float> currentJointErrors = sensors->getBodyAngleErrors();

	// Adjust to real currentJoints, not Sensors'
	for (int i=0; i<NUM_JOINTS ; i++)
		currentJoints[i] = currentJoints[i]-currentJointErrors[i];

	return currentJoints;
}

vector<float> ChopShop::getDiffPerChop(int numChops,
									   vector<float> *current,
									   vector<float> *final) {
	vector<float> diffPerChop;

	for ( int joint_id=0; joint_id < NUM_JOINTS ;++joint_id)
		diffPerChop.push_back( (final->at(joint_id) -
								current->at(joint_id)) / numChops);

	return diffPerChop;
}

vector<float> ChopShop::getFinalJoints(const BodyJointCommand *command,
								  vector<float>* currentJoints) {
	vector<float> finalJoints;
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
	vectorToRad(&finalJoints);
	return finalJoints;
}

// Takes final joint values and
void ChopShop::buildChops(float numChops, 
						  vector<float> *currentJoints,
						  vector<float> *diffPerChop) {
	float nextVal;

	for (int num_chopped=1; num_chopped<=numChops; num_chopped++ ) {
		vector<vector<float> > nextChopped(NUM_CHAINS);

		// NEEDS BETTER CHAIN LOGIC, MORE GENERAL
		int lastChainJoint,joint = 0;
		vector<int>::iterator chain;
		chain = chainList->begin();

		for ( ; chain != chainList->end() ; chain++) {
			lastChainJoint = chain_last_joint[*chain];
			joint = chain_first_joint[*chain];

			for ( ; joint <= lastChainJoint; joint++) {
				nextVal = currentJoints->at(joint)+ diffPerChop->at(joint)*num_chopped;
				nextChopped.at(*chain).push_back(nextVal);
			}
		}
		choppedJoints.push(nextChopped);
	}


} // END CHOP THAT



void ChopShop::vectorToRad(vector<float> *vect) {
	vector<float>::iterator i;
	i = vect->begin();
	i++;
	while ( i != vect->end() ) {
		// Convert joints to radians from degrees
		*i = *i * TO_RAD;
		i++;
	}
}
