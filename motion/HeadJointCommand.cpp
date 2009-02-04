#include "HeadJointCommand.h"

HeadJointCommand::HeadJointCommand(const float time,
				   const std::vector<float> *joints,
				   const Kinematics::InterpolationType _type)
	: JointCommand(HEAD_JOINT, time, _type), headJoints(joints),
	  noJoints(0)
{
	setChainList();
}

HeadJointCommand::HeadJointCommand(const HeadJointCommand &other)
	: JointCommand(HEAD_JOINT, other.getDuration(), other.getInterpolation()),
	  noJoints(0)
{
	setChainList();

	if(other.headJoints)
		headJoints = new std::vector<float>(*other.headJoints);
}

HeadJointCommand::~HeadJointCommand() {
  if (headJoints != NULL)
    delete headJoints;
}

void
HeadJointCommand::setChainList() {
	chainList.insert(chainList.end(),
					 HEAD_JOINT_CHAINS,
					 HEAD_JOINT_CHAINS + HEAD_JOINT_NUM_CHAINS);
}
