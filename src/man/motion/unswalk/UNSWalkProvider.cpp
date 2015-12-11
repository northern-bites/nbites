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

/**
*NBites joint order. commented out joints are the ones that UNSW has but we
*dont use
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

/*void UNSWalkProvider::calculateNextJointsAndStiffnesses(
	std::vector<float>& 			sensorAngles,
	std::vector<float>& 			sensorCurrents,
	const messages::IntertialState& sensorIntertials,
	const messages::FSR& 			sensorFSRs
	) 
{
	
} */

// void UNSWalkProvider::restingStand()
// {

// }

// void UNSWalkProvider::resetOdometry() {
// 	//rest odo
// }

}

}