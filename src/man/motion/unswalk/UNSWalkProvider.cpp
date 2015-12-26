#include "UNSWalkProvider.h"
#include "Profiler.h"
#include "Common.h"
#include "NaoPaths.h"

#include <cassert>
#include <string>
#include <iostream>

#include "generator/Walk2014Generator.hpp"
#include <cmath>
#include "utils/angles.hpp"
#include "utils/body.hpp"
#include "utils/Logger.hpp"
#include "utils/basic_maths.hpp"
#include "utils/speech.hpp"

namespace man 
{
namespace motion 
{

const float UNSWalkProvider::INITIAL_BODY_POSE_ANGLES[] = {
	0.f,0.f,0.f,0.f,0.f,0.f,0.f,
	0.f,0.f,0.f,0.f,0.f,0.f,0.f,
	0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
};

/**
*NBites joint order. commented out joints are the ones that UNSW has but we dont use
*/
static const Joints::JointCode nb_joint_order[] {
		Joints::HeadYaw,
      	Joints::HeadPitch,
      	Joints::LShoulderPitch,
		Joints::LShoulderRoll,
		Joints::LElbowYaw,
		Joints::LElbowRoll,
		Joints::LHipYawPitch,
		Joints::LHipRoll,
		Joints::LHipPitch,
		Joints::LKneePitch,
		Joints::LAnklePitch,
		Joints::LAnkleRoll,
		Joints::RHipRoll,
		Joints::RHipPitch,
		Joints::RKneePitch,
		Joints::RAnklePitch,
		Joints::RAnkleRoll,
		Joints::RShoulderPitch,
		Joints::RShoulderRoll,
		Joints::RElbowYaw,
		Joints::RElbowRoll,
		Joints::LWristYaw,
		//LHand,
		//RWristYaw,
		//RHand,
  };

UNSWalkProvider::UNSWalkProvider() : MotionProvider(WALK_PROVIDER), 
 									 requestedToStop(false), tryingToWalk(false) 
{
	walk2014gen = new Walk2014Generator;
	resetAll();
}

UNSWalkProvider::~UNSWalkProvider()
{
	delete walk2014gen;
}

void UNSWalkProvider::resetAll() {
	inactive();

	//reset odometry through walk2014gen
	//reset other parts of walk2014gen
}

bool UNSWalkProvider::calibrated() const {
	return NULL;
}

bool UNSWalkProvider::upright() const {
	return NULL;
}

float UNSWalkProvider::leftHandSpeed() const {
	return NULL;
}

float UNSWalkProvider::rightHandSpeed() const {
	return NULL;
}

void UNSWalkProvider::requestStopFirstInstance() {

}

void UNSWalkProvider::calculateNextJointsAndStiffnesses(
	std::vector<float>& 			sensorAngles,
	std::vector<float>& 			sensorCurrents,
	const messages::InertialState&  sensorInertials,
	const messages::FSR& 			sensorFSRs
	) 
{
	
}

void UNSWalkProvider::hardReset() {

}

void UNSWalkProvider::resetOdometry() {

}

void UNSWalkProvider::setCommand(const WalkCommand::ptr command) {

}

void UNSWalkProvider::setCommand(const DestinationCommand::ptr command) {

}

void UNSWalkProvider::setCommand(const StepCommand::ptr command) {

}

void UNSWalkProvider::getOdometryUpdate(portals::OutPortal<messages::RobotLocation>& out) const {

}

bool UNSWalkProvider::isStanding() const {
	return NULL;
}

bool UNSWalkProvider::isWalkActive() const {

}

void UNSWalkProvider::stand() {

}


// void UNSWalkProvider::restingStand()
// {

// }

// void UNSWalkProvider::resetOdometry() {
// 	//rest odo
// }

}

}