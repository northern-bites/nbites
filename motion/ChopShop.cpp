#include "ChopShop.h"

ChopShop::ChopShop (Sensors *s, float motionFrameLength)
	: finalJoints(22),
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

	// Get number of chops according to duration
	float numChops = command->getDuration()/FRAME_LENGTH_S;
	// Get current angles
	vector<float> currentJoints = sensors->getBodyAngles();
	vector<float> currentJointErrors = sensors->getBodyAngleErrors(); // FINISH THIS LINE!!

	for (int i=0; i<NUM_JOINTS ; i++){
		currentJoints[i] = currentJoints[i]-currentJointErrors[i];
	}

  	vector<vector<float> > nextChopped(5);
	vector<float> nextHead(2);
	vector<float> nextLArm(2);
	vector<float> nextRArm(2);
	vector<float> nextLLeg(2);
	vector<float> nextRLeg(2);


	// Get all the joints from the Command
	// Start with chain
	for ( int chain_id=LARM_CHAIN; chain_id<NUM_CHAINS;++chain_id) {
		cout << "chain_id, finalJoints.size()=" << chain_id << " " << finalJoints.size() << endl;
		cout << "commandjointsize = " << command->getJoints(LARM_CHAIN)->size() << endl;
		switch (chain_id){
		case LARM_CHAIN:
			cout << "nope, i'm here" << endl;
			nextChain = command->getJoints(LARM_CHAIN);
			break;

		case LLEG_CHAIN:
			nextChain = command->getJoints(LLEG_CHAIN);
			break;

		case RLEG_CHAIN:
			nextChain = command->getJoints(RLEG_CHAIN);
			break;

		case RARM_CHAIN:
			nextChain = command->getJoints(RARM_CHAIN);
			break;
		}

		cout << "nextchain.size= " << nextChain->size() << endl;


		// If the next chain is not queued (empty), add current joints
		if ( nextChain==0 ||
			 nextChain->empty()){

			switch (chain_id) {

			case LARM_CHAIN:

				finalJoints.assign(HEAD_YAW, currentJoints.at(HEAD_YAW));
				finalJoints.assign(HEAD_PITCH, currentJoints.at(HEAD_PITCH));

				finalJoints.assign(L_SHOULDER_PITCH, currentJoints.at(L_SHOULDER_PITCH));
				finalJoints.assign(L_SHOULDER_ROLL, currentJoints.at(L_SHOULDER_ROLL));
				finalJoints.assign(L_ELBOW_YAW, currentJoints.at(L_ELBOW_YAW));
				finalJoints.assign(L_ELBOW_ROLL, currentJoints.at(L_ELBOW_ROLL));
				break;

			case LLEG_CHAIN:
				finalJoints.assign(L_HIP_YAW_PITCH, currentJoints.at(L_HIP_YAW_PITCH));
				finalJoints.assign(L_HIP_ROLL, currentJoints.at(L_HIP_ROLL));
				finalJoints.assign(L_HIP_PITCH, currentJoints.at(L_HIP_PITCH));
				finalJoints.assign(L_KNEE_PITCH, currentJoints.at(L_KNEE_PITCH));
				finalJoints.assign(L_ANKLE_PITCH, currentJoints.at(L_ANKLE_PITCH));
				finalJoints.assign(L_ANKLE_ROLL, currentJoints.at(L_ANKLE_ROLL));

				break;

			case RLEG_CHAIN:
				finalJoints.assign(R_HIP_YAW_PITCH, currentJoints.at(R_HIP_YAW_PITCH));
				finalJoints.assign(R_HIP_ROLL, currentJoints.at(R_HIP_ROLL));
				finalJoints.assign(R_HIP_PITCH, currentJoints.at(R_HIP_PITCH));
				finalJoints.assign(R_KNEE_PITCH, currentJoints.at(R_KNEE_PITCH));
				finalJoints.assign(R_ANKLE_PITCH, currentJoints.at(R_ANKLE_PITCH));
				finalJoints.assign(R_ANKLE_ROLL, currentJoints.at(R_ANKLE_ROLL));

				break;

			case RARM_CHAIN:
				finalJoints.assign(R_SHOULDER_PITCH, currentJoints.at(R_SHOULDER_PITCH));
				finalJoints.assign(R_SHOULDER_ROLL, currentJoints.at(R_SHOULDER_ROLL));
				finalJoints.assign(R_ELBOW_YAW, currentJoints.at(R_ELBOW_YAW));
				finalJoints.assign(R_ELBOW_ROLL, currentJoints.at(R_ELBOW_ROLL));
				break;
			}

		}

		else{
			// Add each chain of joints to the final joints
			cout << "nextchain not empty" << endl;
				finalJoints.insert(finalJoints.end(),
								   nextChain->begin(),
								   nextChain->end());
		}
	}


	//Get diff per chop from current to final
	for ( int joint_id=0; joint_id < NUM_JOINTS ;++joint_id){
		diffPerChop.push_back( (finalJoints.at(joint_id) -
								currentJoints.at(joint_id)) /
							   numChops);
	}
	finalJoints.clear();
	// @JGM need to add vector for each chain!
    // Make vectors of new joints

    for (int num_chopped = 1; num_chopped<=numChops;++num_chopped){
		// For each chop, insert all new joint vals into chain vectors
		for ( int joint_id=0; joint_id<NUM_JOINTS; joint_id++){
			switch (joint_id){
			case HEAD_YAW:
			case HEAD_PITCH:
				nextHead.push_back(currentJoints.at(joint_id) + diffPerChop.at(joint_id)*num_chopped);
				break;
				// LARM:
			case L_SHOULDER_PITCH:
			case L_SHOULDER_ROLL:
			case L_ELBOW_YAW:
			case L_ELBOW_ROLL:
				nextLArm.push_back(currentJoints.at(joint_id) + diffPerChop.at(joint_id)*num_chopped);
				break;

				// LLEG:
			case L_HIP_YAW_PITCH:
			case L_HIP_ROLL:
			case L_HIP_PITCH:
			case L_KNEE_PITCH:
			case L_ANKLE_PITCH:
			case L_ANKLE_ROLL:
				nextLLeg.push_back(currentJoints.at(joint_id) + diffPerChop.at(joint_id)*num_chopped);
				break;

				// RLEG:
			case R_HIP_YAW_PITCH:
			case R_HIP_ROLL:
			case R_HIP_PITCH:
			case R_KNEE_PITCH:
			case R_ANKLE_PITCH:
			case R_ANKLE_ROLL:
				nextRLeg.push_back(currentJoints.at(joint_id) + diffPerChop.at(joint_id)*num_chopped);
				break;

				// RARM:
			case R_SHOULDER_PITCH:
			case R_SHOULDER_ROLL:
			case R_ELBOW_YAW:
			case R_ELBOW_ROLL:
				nextRArm.push_back(currentJoints.at(joint_id) + diffPerChop.at(joint_id)*num_chopped);
				break;
			} // END SWITCH

//			cout << "joint_id= " << joint_id << " diffPerChop*numChopped = " << diffPerChop.at(joint_id)*num_chopped << endl;
		} // END JOINT LOOP

		// Add the new chain vectors to the move vector
		nextChopped.push_back(nextHead);
		nextChopped.push_back(nextLArm);
		nextChopped.push_back(nextLLeg);
		nextChopped.push_back(nextRLeg);
		nextChopped.push_back(nextRArm);

		// Add the vector of next moves to the vector of all chopped moves
		choppedJoints.push(nextChopped);
		nextChopped.clear();

		nextHead.clear();
		nextLArm.clear();
		nextLLeg.clear();
		nextRLeg.clear();
		nextRArm.clear();

	} // END NUM CHOPS LOOP
	diffPerChop.clear();
	return choppedJoints;
} // END CHOP LINEAR


