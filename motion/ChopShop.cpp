using namespace Kinematics;
// Build the Chop Shop!
ChopShop::ChopShop(Sensors *sensors)
	: finalJoints(0),
	  diffPerChop(0),
	  choppedJoints(0)
{

}

// Breaks command into FRAME_LENGTH_S size pieces,
// adds it to the queue
vector<vector<float> > ChopShop::chopCommand(BodyJointCommand *command){
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
void ChopShop::chopSmooth(BodyJointCommand *command){

}

// Linear interpolation motion
void ChopShop::chopLinear(BodyJointCommand *command){
	// Get number of chops according to duration
	float numChops = command->getDuration()/FRAME_LENGTH_S;

	// Get current angles
	const vector<float> *currentJoints = sensors->getBodyAngles();

	// Get all the joints from the Command, even if they're null
	// Start with chain
	for (int chain_id=0; chain_id<NUM_CHAINS;++chain_id){
		vector<float> *nextChain = command->getJoints(chain_id);

		// If the next chain is not queued (empty), add current joints
		if ( nextChain->empty() ){

			switch (chain_id) {
			case HEAD_CHAIN:
				finalJoints.push_back(currentJoints->at(HEAD_YAW));
				finalJoints.push_back(currentJoints->at(HEAD_PITCH));
			case LARM_CHAIN:
				finalJoints.push_back(currentJoints->at(L_SHOULDER_PITCH));
				finalJoints.push_back(currentJoints->at(L_SHOULDER_ROLL));
				finalJoints.push_back(currentJoints->at(L_ELBOW_YAW));
				finalJoints.push_back(currentJoints->at(L_ELBOW_ROLL));
			case LLEG_CHAIN:
				finalJoints.push_back(currentJoints->at(L_HIP_YAW_PITCH));
				finalJoints.push_back(currentJoints->at(L_HIP_ROLL));
				finalJoints.push_back(currentJoints->at(L_HIP_PITCH));
				finalJoints.push_back(currentJoints->at(L_KNEE_PITCH));
				finalJoints.push_back(currentJoints->at(L_ANKLE_PITCH));
				finalJoints.push_back(currentJoints->at(L_ANKLE_ROLL));
			case RLEG_CHAIN:
				finalJoints.push_back(currentJoints->at(R_HIP_YAW_PITCH));
				finalJoints.push_back(currentJoints->at(R_HIP_ROLL));
				finalJoints.push_back(currentJoints->at(R_HIP_PITCH));
				finalJoints.push_back(currentJoints->at(R_KNEE_PITCH));
				finalJoints.push_back(currentJoints->at(R_ANKLE_PITCH));
				finalJoints.push_back(currentJoints->at(R_ANKLE_ROLL);)
			case RARM_CHAIN:
				finalJoints.push_back(currentJoints->at(R_SHOULDER_PITCH));
				finalJoints.push_back(currentJoints->at(R_SHOULDER_ROLL));
				finalJoints.push_back(currentJoints->at(R_ELBOW_YAW));
				finalJoints.push_back(currentJoints->at(R_ELBOW_ROLL));
			}

		}

		// Add each chain of joints to the final joints
		finalJoints.push_back(finalJoints.end(),
							  nextChain.begin(),
							  nextChain.end())

	}


	//Get diff per chop from current to final
	for (int joint_id=0; joint_id<NUM_JOINTS;++joint_id){
		diffPerChop.push_back( (finalJoints.at(joint_id) -
								currentJoints->at(joint_id)) /
							   numChops);
	}


	// @JGM need to add vector for each chain!
	vector<float> nextChopped(0);
	// Make vectors of new joints
	for (int num_chopped = 1; num_chopped<numChops;++num_chopped){

		for (int joint_id=0; joint_id<NUM_JOINTS;++joint_id){

			nextChopped.push_back(currentJoints->at(joint_id) +
									diffPerChop.at(joint_id)*num_chopped);
		}

		choppedJoints.push_back(nextChopped);
		nextChopped.clear();
	}
	return choppedJoints;
}
