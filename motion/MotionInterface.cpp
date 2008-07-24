
#include "MotionInterface.h"

// This method immediately forwards the WalkCommand to MotionCore
void MotionInterface::setNextWalkCommand(const WalkCommand *command){
  core->setNextWalkCommand(command);
}

void MotionInterface::enqueue(const BodyJointCommand *command){
  core->enqueue(command);
}

void MotionInterface::enqueue(const HeadJointCommand *command){
  core->enqueue(command);
}

void MotionInterface::enqueue(const HeadScanCommand *command){
  core->enqueue(command);
}

void MotionInterface::stopBodyMoves() {
  core->stopBodyMoves();
}

void MotionInterface::stopHeadMoves() {
  core->stopHeadMoves();
}

float MotionInterface::getHeadSpeed() {
  return core->getHeadSpeed();
}

void MotionInterface::setWalkConfig ( float pMaxStepLength, float pMaxStepHeight,
				      float pMaxStepSide, float pMaxStepTurn,
				      float pZmpOffsetX, float pZmpOffsetY) {
  core->setWalkConfig(pMaxStepLength, pMaxStepHeight,
		      pMaxStepSide, pMaxStepTurn,
		      pZmpOffsetX, pZmpOffsetY);
}

void MotionInterface::setWalkArmsConfig ( float pShoulderMedian,
					  float pShoulderAmplitude,
					  float pElbowMedian,
					  float pElbowAmplitude) {
  core->setWalkArmsConfig(pShoulderMedian,
			  pShoulderAmplitude,
			  pElbowMedian,
			  pElbowAmplitude);
}

void MotionInterface::setWalkExtraConfig( float pLHipRollBacklashCompensator,
					  float pRHipRollBacklashCompensator,
					  float pHipHeight,
					  float pTorsoYOrientation) {
  core->setWalkExtraConfig(pLHipRollBacklashCompensator,
			   pRHipRollBacklashCompensator,
			   pHipHeight,
			   pTorsoYOrientation);
}

void MotionInterface::setWalkParameters( const WalkParameters& params) {
  core->setWalkParameters(params);
}


void MotionInterface::setSupportMode(int pSupportMode) {
  core->setSupportMode( pSupportMode );
}

int MotionInterface::getSupportMode() {
  return core->getSupportMode();
}


void MotionInterface::setBalanceMode(int pBalanceMode) {
  core->setBalanceMode( pBalanceMode );
}

int MotionInterface::getBalanceMode() {
  return core->getBalanceMode();
}



/*
 * Send off the contents of the action and joint queues over to MotionCore.
 * We currently have no use for this method as MotionInterface is a thin
 * layer between python and MotionCore
 */
/*
void MotionInterface::sendToMotion() {
  for (unsigned int i = 0, numCommands = actionCommands.size();
       i < numCommands; i++) {
    const WalkCommand *command = actionCommands.front();
    core->enqueueAction(command);
    actionCommands.pop();
  }

  for (unsigned int i = 0, numCommands = jointCommands.size();
       i < numCommands; i++) {
    const BodyJointCommand *command = jointCommands.front();
    core->enqueueJoints(command);
    jointCommands.pop();
  }
}
*/
