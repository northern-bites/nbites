
#ifndef _MotionConstants_h_DEFINED
#define _MotionConstants_h_DEFINED
#include "Kinematics.h"

using namespace Kinematics;

namespace MotionConstants {

	enum MotionType {
		WALK = 0,
		BODY_JOINT,
		HEAD_JOINT
	};

	const static unsigned int WALK_NUM_CHAINS = NUM_BODY_CHAINS;

	const static int WALK_CHAINS[WALK_NUM_CHAINS] = {LARM_CHAIN,
										LLEG_CHAIN,
										RLEG_CHAIN,
										RARM_CHAIN };
	const static unsigned int BODY_JOINT_NUM_CHAINS = NUM_BODY_CHAINS;
	const static int BODY_JOINT_CHAINS[BODY_JOINT_NUM_CHAINS] = {LARM_CHAIN,
																 LLEG_CHAIN,
																 RLEG_CHAIN,
																 RARM_CHAIN };
	const static unsigned int HEAD_JOINT_NUM_CHAINS = 1;
	const static int HEAD_JOINT_CHAINS[HEAD_JOINT_NUM_CHAINS] = {HEAD_CHAIN};

};
#endif
