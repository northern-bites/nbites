#include "ChopShop.h"

ChopShop::ChopShop (Sensors *s, float motionFrameLength)
	: sensors(s),
	  FRAME_LENGTH_S(motionFrameLength)
{

}

// Breaks command into FRAME_LENGTH_S size pieces,
// adds it to the queue
queue<vector<vector<float> > >* ChopShop::chopCommand(const JointCommand *command) {
	// It's a BJC so it deals with 4 chains

	if (command->getInterpolation() == INTERPOLATION_LINEAR) {
		return chopLinear(command);
	}

	else if (command->getInterpolation() == INTERPOLATION_SMOOTH) {
		return chopSmooth(command);
	}

	else {
		cout << "ILLEGAL INTERPOLATION VALUE. CHOPPING LINEARLY" << endl;
		return chopLinear(command);
	}
}

// Smooth interpolation motion
queue<vector<vector<float> > >*
ChopShop::chopSmooth(const JointCommand *command) {

	// PLACE HOLDER
	queue<vector<vector<float> > >* a = new queue<vector<vector<float> > >();
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
queue<vector<vector<float> > >*
ChopShop::chopLinear(const JointCommand *command) {
	// Get number of chops according to duration
	int numChops = (int)(command->getDuration()/FRAME_LENGTH_S);

	vector<float> currentJoints = getCurrentJoints();
//     cout << "Current joints in CHOPCHOP" <<endl;
//     for (int i =0; i<22; i++)
//         cout <<"  [" <<i<<"] "<< currentJoints.at(i) <<endl;
	// Add final joints for all chains
	vector<float> finalJoints = getFinalJoints(command, &currentJoints);
//     cout << "Dest joints in CHOPCHOP" <<endl;
//     for (int i =0; i<22; i++)
//         cout <<"  [" <<i<<"] "<< finalJoints.at(i) <<endl;

	//Get diff per chop from current to final

	vector<float> diffPerChop = getDiffPerChop(numChops,
											   &currentJoints,
											   &finalJoints);


	return buildChops(numChops, &currentJoints, &diffPerChop, command);
}

vector<float> ChopShop::getCurrentJoints() {

	// Get current angles&errors from sensors
	vector<float> currentJoints = sensors->getBodyAngles();
	vector<float> currentJointErrors = sensors->getBodyAngleErrors();

	// Adjust to real currentJoints, not Sensors'
	for (unsigned int i=0; i<NUM_JOINTS ; i++)
		currentJoints[i] = currentJoints[i]-currentJointErrors[i];

	return currentJoints;
}



vector<float> ChopShop::getFinalJoints(const JointCommand *command,
									   vector<float>* currentJoints) {
	vector<float> finalJoints;
	vector<float>::iterator currentStart = currentJoints->begin();
	vector<float>::iterator currentEnd = currentJoints->begin();

	for (unsigned int chain=0; chain < NUM_CHAINS;chain++) {
		// First, get chain joints from command
		const vector<float> *nextChain = command->getJoints((ChainID)chain);

		// Set the end iterator
		currentEnd += chain_lengths[chain];

		// If the next chain is not queued (empty), add current joints
		if ( nextChain == 0 ||
			 nextChain->size() == 0 ||
			 nextChain->empty() ) {
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
	return finalJoints;

}
vector<float> ChopShop::getDiffPerChop(int numChops,
									   vector<float> *current,
									   vector<float> *final) {
	vector<float> diffPerChop;

	for (unsigned int joint_id=0; joint_id < NUM_JOINTS ;++joint_id) {
		diffPerChop.push_back( (final->at(joint_id) -
								current->at(joint_id)) / (float)numChops);
	}

	return diffPerChop;
}

// Takes final joint values and
queue<vector<vector<float> > >* ChopShop::buildChops(int numChops,
													 vector<float> *currentJoints,
													 vector<float> *diffPerChop,
													 const JointCommand *command) {
	queue<vector<vector<float> > >* choppedJoints = new queue<vector<vector<float> > >();
	float nextVal;

	for (int num_chopped=1; num_chopped<=numChops; num_chopped++ ) {
		const list<int>* chainList = command->getChainList();
		vector<vector<float> > nextChopped(NUM_CHAINS);



		int lastChainJoint,joint = 0;
		list<int>::const_iterator chain;
		chain = chainList->begin();

		for ( ; chain != chainList->end() ; chain++) {
			lastChainJoint = chain_last_joint[*chain];
			joint = chain_first_joint[*chain];

			for ( ; joint <= lastChainJoint; joint++) {
				nextVal = currentJoints->at(joint)+ diffPerChop->at(joint)*(float)num_chopped;
				nextChopped.at(*chain).push_back(nextVal);
//				if (*chain == 0)
//					cout << "nextVal is" << nextVal << " for chain " << *chain << endl;
			}
		}
		choppedJoints->push(nextChopped);
	}
	return choppedJoints;

} // END CHOP THAT



void ChopShop::vectorToRad(vector<float> *vect) {
	vector<float>::iterator i;
	i = vect->begin();
	while ( i != vect->end() ) {
		// Convert joints to radians from degrees
		*i = *i * TO_RAD;
		i++;
	}
}
