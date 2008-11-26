#include "ChopShop.h"

ChopShop::ChopShop (Sensors *s, float motionFrameLength)
	: finalJoints(0),
	  diffPerChop(0),
	  choppedJoints(0),
	  sensors(s),
	  FRAME_LENGTH_S(motionFrameLength)
{

}

// Breaks command into FRAME_LENGTH_S size pieces,
// adds it to the queue
vector<vector<vector<float> > > ChopShop::chopCommand(BodyJointCommand *command){
  // Checks type of body command and sends to
  // appropriate function
//  if (command->getType() == INTERPOLATION_LINEAR){
	return chopLinear(command);
//  }

//  else if (command->getType() == INTERPOLATION_SMOOTH){
  //  return chopSmooth(command);
//  }


}

// Smooth interpolation motion
vector<vector<vector<float> > > ChopShop::chopSmooth(BodyJointCommand *command){
	vector<vector<vector<float> > > a;
	a = vector<vector<vector<float> > >(0);
	return a;
}

// Linear interpolation motion
vector<vector<vector<float> > > ChopShop::chopLinear(BodyJointCommand *command){
  // Get number of chops according to duration
  float numChops = command->getDuration()/FRAME_LENGTH_S;

  // Get current angles
  vector<float> currentJoints = sensors->getBodyAngles();

  // Get all the joints from the Command, even if they're null
  // Start with chain
  for ( int chain_id=0; chain_id<NUM_CHAINS;++chain_id){
	  const vector<float> *nextChain(0);

	  switch (chain_id){
	  case HEAD_CHAIN:
		  nextChain = command->getJoints(HEAD_CHAIN);
	  case LARM_CHAIN:
		  nextChain = command->getJoints(LARM_CHAIN);
	  case LLEG_CHAIN:
		  nextChain = command->getJoints(LLEG_CHAIN);
	  case RLEG_CHAIN:
		  nextChain = command->getJoints(RLEG_CHAIN);
	  case RARM_CHAIN:
		  nextChain = command->getJoints(RARM_CHAIN);
	  }


    // If the next chain is not queued (empty), add current joints
    if ( nextChain->empty() ){

      switch (chain_id) {
      case HEAD_CHAIN:
	finalJoints.push_back(currentJoints.at(HEAD_YAW));
	finalJoints.push_back(currentJoints.at(HEAD_PITCH));
      case LARM_CHAIN:
	finalJoints.push_back(currentJoints.at(L_SHOULDER_PITCH));
	finalJoints.push_back(currentJoints.at(L_SHOULDER_ROLL));
	finalJoints.push_back(currentJoints.at(L_ELBOW_YAW));
	finalJoints.push_back(currentJoints.at(L_ELBOW_ROLL));
      case LLEG_CHAIN:
	finalJoints.push_back(currentJoints.at(L_HIP_YAW_PITCH));
	finalJoints.push_back(currentJoints.at(L_HIP_ROLL));
	finalJoints.push_back(currentJoints.at(L_HIP_PITCH));
	finalJoints.push_back(currentJoints.at(L_KNEE_PITCH));
	finalJoints.push_back(currentJoints.at(L_ANKLE_PITCH));
	finalJoints.push_back(currentJoints.at(L_ANKLE_ROLL));
      case RLEG_CHAIN:
	finalJoints.push_back(currentJoints.at(R_HIP_YAW_PITCH));
	finalJoints.push_back(currentJoints.at(R_HIP_ROLL));
	finalJoints.push_back(currentJoints.at(R_HIP_PITCH));
	finalJoints.push_back(currentJoints.at(R_KNEE_PITCH));
	finalJoints.push_back(currentJoints.at(R_ANKLE_PITCH));
	finalJoints.push_back(currentJoints.at(R_ANKLE_ROLL));
      case RARM_CHAIN:
	  finalJoints.push_back(currentJoints.at(R_SHOULDER_PITCH));
	finalJoints.push_back(currentJoints.at(R_SHOULDER_ROLL));
	finalJoints.push_back(currentJoints.at(R_ELBOW_YAW));
	finalJoints.push_back(currentJoints.at(R_ELBOW_ROLL));
      }

    }

    // Add each chain of joints to the final joints
    finalJoints.insert(finalJoints.end(),
					   nextChain->begin(),
					   nextChain->end());

  }


  //Get diff per chop from current to final
  for ( int joint_id=0; joint_id<NUM_JOINTS;++joint_id){
    diffPerChop.push_back( (finalJoints.at(joint_id) -
			    currentJoints.at(joint_id)) /
			   numChops);
  }
  	vector<vector<float> > nextChopped(0);
	vector<float> nextHead(0);
	vector<float> nextLArm(0);
	vector<float> nextRArm(0);
	vector<float> nextLLeg(0);
	vector<float> nextRLeg(0);

  // @JGM need to add vector for each chain!
    // Make vectors of new joints
    for (int num_chopped = 1; num_chopped<numChops;++num_chopped){

		// For each chop, insert all new joint vals into chain vectors
		for ( int joint_id=0; joint_id<NUM_JOINTS; joint_id++){
			switch (joint_id){
			case HEAD_YAW:
			case HEAD_PITCH:
				nextHead.push_back(currentJoints.at(joint_id) + diffPerChop.at(joint_id)*num_chopped);

				// LARM:
			case L_SHOULDER_PITCH:
			case L_SHOULDER_ROLL:
			case L_ELBOW_YAW:
			case L_ELBOW_ROLL:
				nextLArm.push_back(currentJoints.at(joint_id) + diffPerChop.at(joint_id)*num_chopped);

				// LLEG:
			case L_HIP_YAW_PITCH:
			case L_HIP_ROLL:
			case L_HIP_PITCH:
			case L_KNEE_PITCH:
			case L_ANKLE_PITCH:
			case L_ANKLE_ROLL:
				nextLLeg.push_back(currentJoints.at(joint_id) + diffPerChop.at(joint_id)*num_chopped);

				// RLEG:
			case R_HIP_YAW_PITCH:
			case R_HIP_ROLL:
			case R_HIP_PITCH:
			case R_KNEE_PITCH:
			case R_ANKLE_PITCH:
			case R_ANKLE_ROLL:
				nextRLeg.push_back(currentJoints.at(joint_id) + diffPerChop.at(joint_id)*num_chopped);

				// RARM:
			case R_SHOULDER_PITCH:
			case R_SHOULDER_ROLL:
			case R_ELBOW_YAW:
			case R_ELBOW_ROLL:
				nextRArm.push_back(currentJoints.at(joint_id) + diffPerChop.at(joint_id)*num_chopped);
			} // END SWITCH
		} // END JOINT LOOP

		// Add the new chain vectors to the move vector
		nextChopped.push_back(nextHead);
		nextChopped.push_back(nextLArm);
		nextChopped.push_back(nextLLeg);
		nextChopped.push_back(nextRLeg);
		nextChopped.push_back(nextRArm);

		// Add the vector of next moves to the vector of all chopped moves
		choppedJoints.push_back(nextChopped);
		nextChopped.clear();

	} // END NUM CHOPS LOOP
	return choppedJoints;
} // END CHOP LINEAR


