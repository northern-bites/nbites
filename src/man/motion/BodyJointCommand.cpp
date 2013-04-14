#include "BodyJointCommand.h"
using namespace Kinematics;

namespace man
{
namespace motion
{
BodyJointCommand::BodyJointCommand
(const float time,
 const std::vector<float>& bodyJoints,
 const std::vector<float>& body_stiffness,
 const InterpolationType _type)
    : JointCommand(MotionConstants::BODY_JOINT, time, _type, body_stiffness),
      larm_joints(0), lleg_joints(0), rleg_joints(0), rarm_joints(0)
{
    setChainList();
    // bodyJoints must contain joints for the _entire_ body
    std::vector<float> *joints;
    std::vector<float>::const_iterator firstMark, endMark;

    // Both iterators start at the beginning
    // then bump them to the end of the head_chain,
    // since BJC do not deal with the head
    firstMark = endMark = bodyJoints.begin();

    // Build joint vectors for each chain
    // from the bodyJoint vector
    std::list<int>::iterator chainID = chainList.begin();
    for ( ; chainID != chainList.end() ; chainID++ ) {

        joints = getJoints(static_cast<ChainID>(*chainID));

        // Position the end iterator and assign the new vector
        // then move the beginning iterator up to meet the end
        endMark += chain_lengths[*chainID];
        joints->assign(firstMark,endMark);
        firstMark = endMark;
    }
}

BodyJointCommand::BodyJointCommand(const float time,
                                   ChainID chainID,
                                   const std::vector<float>& joints,
                                   const std::vector<float>& body_stiffness,
                                   const InterpolationType _type)
    : JointCommand(MotionConstants::BODY_JOINT, time, _type, body_stiffness), larm_joints(0),
      lleg_joints(0), rleg_joints(0), rarm_joints(0)
{
    setChainList();
    switch (chainID) {
    case LARM_CHAIN:
        larm_joints = joints;
        break;
    case LLEG_CHAIN:
        lleg_joints = joints;
        break;
    case RLEG_CHAIN:
        rleg_joints = joints;
        break;
    case RARM_CHAIN:
        rarm_joints = joints;
        break;
    default:
        assert(false);
    }
}

BodyJointCommand::BodyJointCommand(const float time,
                                   const std::vector<float>& larm,
                                   const std::vector<float>& lleg,
                                   const std::vector<float>& rleg,
                                   const std::vector<float>& rarm,
                                   const std::vector<float>& body_stiffness,
                                   const InterpolationType _type)
    : JointCommand(MotionConstants::BODY_JOINT, time, _type, body_stiffness), larm_joints(larm),
      lleg_joints(lleg), rleg_joints(rleg), rarm_joints(rarm)
{
    setChainList();
}

BodyJointCommand::~BodyJointCommand (void)
{
}

    const std::vector<float>&
BodyJointCommand::getJoints (ChainID chainID) const
{
    switch (chainID) {
    case LARM_CHAIN:
        return larm_joints;
    case LLEG_CHAIN:
        return lleg_joints;
    case RLEG_CHAIN:
        return rleg_joints;
    case RARM_CHAIN:
        return rarm_joints;
    default:
        static std::vector<float> empty;
        return empty;
    }
}

    std::vector<float>*
BodyJointCommand::getJoints (ChainID chainID)
{
    switch (chainID) {
    case LARM_CHAIN:
        return &larm_joints;
    case LLEG_CHAIN:
        return &lleg_joints;
    case RLEG_CHAIN:
        return &rleg_joints;
    case RARM_CHAIN:
        return &rarm_joints;
    default:
        return NULL;
    }}

const bool
BodyJointCommand::conflicts (const BodyJointCommand &other) const
{
    return ((!larm_joints.empty() && !other.getLArmJoints().empty()) ||
            (!lleg_joints.empty() && !other.getLLegJoints().empty()) ||
            (!rleg_joints.empty() && !other.getRLegJoints().empty()) ||
            (!rarm_joints.empty() && !other.getRArmJoints().empty())  );
}

const bool
BodyJointCommand::conflicts (const float chainTimeRemaining[NUM_CHAINS]) const {
    return ((!larm_joints.empty() && chainTimeRemaining[LARM_CHAIN] > 0) ||
            (!lleg_joints.empty() && chainTimeRemaining[LLEG_CHAIN] > 0) ||
            (!rleg_joints.empty() && chainTimeRemaining[RLEG_CHAIN] > 0) ||
            (!rarm_joints.empty() && chainTimeRemaining[RARM_CHAIN] > 0) );
}

void
BodyJointCommand::setChainList() {
    chainList.insert(chainList.end(),
                     MotionConstants::BODY_JOINT_CHAINS,
                     MotionConstants::BODY_JOINT_CHAINS +
		     MotionConstants::BODY_JOINT_NUM_CHAINS);
}

} // namespace motion
} // namespace man
