
#include "MotionInterface.h"

void MotionInterface::setNextWalkCommand(const WalkCommand *command){
    switchboard->sendMotionCommand(command);
}

void MotionInterface::enqueue(const BodyJointCommand *command){
    switchboard->sendMotionCommand(command);
}

void MotionInterface::enqueue(const HeadJointCommand *command){
    switchboard->sendMotionCommand(command);
}

void MotionInterface::setHead(const SetHeadCommand *command){
    switchboard->sendMotionCommand(command);
}

void MotionInterface::stopBodyMoves() {
}

void MotionInterface::stopHeadMoves() {
}

float MotionInterface::getHeadSpeed() {
    return DUMMY_F;
}

void MotionInterface::setWalkConfig ( float pMaxStepLength, float pMaxStepHeight,
				      float pMaxStepSide, float pMaxStepTurn,
				      float pZmpOffsetX, float pZmpOffsetY) {
}

void MotionInterface::setWalkArmsConfig ( float pShoulderMedian,
					  float pShoulderApmlitude,
					  float pElbowMedian,
					  float pElbowAmplitude) {
}

void MotionInterface::setWalkExtraConfig( float pLHipRollBacklashCompensator,
					  float pRHipRollBacklashCompensator,
					  float pHipHeight,
					  float pTorsoYOrientation) {
}

void MotionInterface::setGait(const GaitCommand * command){
    switchboard->sendMotionCommand(command);
}

void MotionInterface::setSupportMode(int pSupportMode) {
}

int MotionInterface::getSupportMode() {
    return DUMMY_I;
}


void MotionInterface::setBalanceMode(int pBalanceMode) {
}

int MotionInterface::getBalanceMode() {
    return DUMMY_I;
}
