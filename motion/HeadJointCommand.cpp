#include "HeadJointCommand.h"

HeadJointCommand::HeadJointCommand(const float time,
				   const std::vector<float> *joints,
				   const AL::ALMotionProxy::INTERPOLATION_TYPE _type)
  : duration(time), headJoints(joints), type(_type) {

}

HeadJointCommand::HeadJointCommand(const HeadJointCommand &other)
  : duration(other.duration),
    type(other.type) {
  if(other.headJoints)
    headJoints = new vector <float>(*other.headJoints);
}

HeadJointCommand::~HeadJointCommand() {
  if (headJoints != NULL)
    delete headJoints;
}
