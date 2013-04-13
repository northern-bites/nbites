#include "HeadJointCommand.h"

using namespace Kinematics;

namespace man
{
namespace motion
{

HeadJointCommand::HeadJointCommand(const float time,
                                   const std::vector<float>& joints,
                                   const std::vector<float>& head_stiffness,
                                   const Kinematics::InterpolationType _type)
    : JointCommand(MotionConstants::HEAD_JOINT,
                   time,
                   _type,
                   head_stiffness),
      headJoints(joints), noJoints(0)
{
    setChainList();
}

HeadJointCommand::~HeadJointCommand() {

}
void
HeadJointCommand::setChainList() {
    chainList.insert(chainList.end(),
                     MotionConstants::HEAD_JOINT_CHAINS,
                     MotionConstants::HEAD_JOINT_CHAINS
                     + MotionConstants::HEAD_JOINT_NUM_CHAINS);
}

} // namespace motion
} // namespace man
