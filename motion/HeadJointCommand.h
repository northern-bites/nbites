#ifndef _HeadJointCommand_h_DEFINED
#define _HeadJointCommand_h_DEFINED

/**
 * Class to store joint commands specifically for the head.
 * Pass in a vector with target angles and the amount of time the robot needs
 * to take to get there. If the vector pointer is NULL, then that means we
 * are queueing up idle time on the head.
 */

#include <vector>
#include <string>

//#include "WalkCommand.h"
#include "Kinematics.h"
#include "JointCommand.h"
#include "MotionConstants.h"

using namespace MotionConstants;
using namespace Kinematics;

class HeadJointCommand : public JointCommand {
public:
	HeadJointCommand(const float time, const std::vector<float> *joints,
					 const Kinematics::InterpolationType _type);
	HeadJointCommand(const HeadJointCommand &other);
	virtual ~HeadJointCommand();

	// TODO @JGM NEED A CHECK ON THE CHAINID REQUEST
	virtual const std::vector<float>* getJoints(ChainID chain) const {
		if (chain == HEAD_CHAIN) {
			return headJoints;
		} else {
			return &noJoints;
		}
	}
private:
	//const skew_function skew_func;
	const std::vector<float> *headJoints;
	virtual void setChainList();
	const std::vector<float> noJoints;
};

#endif
