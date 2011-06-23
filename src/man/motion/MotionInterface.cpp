
// This file is part of Man, a robotic perception, locomotion, and
// team strategy application created by the Northern Bites RoboCup
// team of Bowdoin College in Brunswick, Maine, for the Aldebaran
// Nao robot.
//
// Man is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Man is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser Public License for more details.
//
// You should have received a copy of the GNU General Public License
// and the GNU Lesser Public License along with Man.  If not, see
// <http://www.gnu.org/licenses/>.

#include "MotionInterface.h"

void MotionInterface::setNextWalkCommand(const WalkCommand *command){
    switchboard->sendMotionCommand(command);
}
void
MotionInterface::sendStepCommand(const boost::shared_ptr<StepCommand> command){
    switchboard->sendMotionCommand(command);
}
void MotionInterface::enqueue(const BodyJointCommand::ptr command){
    switchboard->sendMotionCommand(command);
}

void MotionInterface::enqueue(const HeadJointCommand::ptr command){
    switchboard->sendMotionCommand(command);
}

void MotionInterface::setHead(const SetHeadCommand::ptr command){
    switchboard->sendMotionCommand(command);
}

void MotionInterface::coordHead(const CoordHeadCommand *command){
    switchboard->sendMotionCommand(command);
}

void MotionInterface::sendFreezeCommand(const FreezeCommand::ptr command){
    switchboard->sendMotionCommand(command);
}
void MotionInterface::sendFreezeCommand(const UnfreezeCommand::ptr command){
    switchboard->sendMotionCommand(command);
}


void MotionInterface::stopBodyMoves() {
	switchboard->stopBodyMoves();
}

void MotionInterface::stopHeadMoves() {
    switchboard->stopHeadMoves();
}

void MotionInterface::resetWalkProvider(){
    switchboard->resetWalkProvider();
}

void MotionInterface::resetScriptedProvider(){
    switchboard->resetScriptedProvider();
}

float MotionInterface::getHeadSpeed() const {
    return DUMMY_F;
}

void MotionInterface::setWalkConfig ( float pMaxStepLength, float pMaxStepHeight,
				      float pMaxStepSide, float pMaxStepTurn,
				      float pZmpOffsetX, float pZmpOffsetY) const {
}

void MotionInterface::setWalkArmsConfig ( float pShoulderMedian,
					  float pShoulderApmlitude,
					  float pElbowMedian,
					  float pElbowAmplitude) const {
}

void MotionInterface::setWalkExtraConfig( float pLHipRollBacklashCompensator,
					  float pRHipRollBacklashCompensator,
					  float pHipHeight,
					  float pTorsoYOrientation) const {
}

void MotionInterface::setGait(const Gait::ptr command){
    switchboard->sendMotionCommand(command);
}

void MotionInterface::setSupportMode(int pSupportMode) const {
}

int MotionInterface::getSupportMode() const {
    return DUMMY_I;
}


void MotionInterface::setBalanceMode(int pBalanceMode) const {
}

int MotionInterface::getBalanceMode() const {
    return DUMMY_I;
}
