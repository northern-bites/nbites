
#include "BalancingChoppedCommand.h"

using std::vector;

using namespace Kinematics;

BalancingChoppedCommand::BalancingChoppedCommand(ChoppedCommand::ptr choppedCommand)
    : PreviewChoppedCommand(choppedCommand),
      leftArm(new BalancingArm(LARM_CHAIN, previewStruct)),
      rightArm(new BalancingArm(RARM_CHAIN, previewStruct))
{
}

BalancingChoppedCommand::~BalancingChoppedCommand() {

}

vector<float> BalancingChoppedCommand::getNextJoints(int id) {
    // we get all the chains so that they get dequed properly in Preview
    vector<float> joints = PreviewChoppedCommand::getNextJoints(id);

    if (id == LARM_CHAIN)
	return leftArm->getNextJoints();
    else if (id == RARM_CHAIN)
	return rightArm->getNextJoints();
    else
	return joints;
}

const vector<float>
BalancingChoppedCommand::getStiffness(Kinematics::ChainID id) const {
    if (id == LARM_CHAIN || id == RARM_CHAIN)
	return PreviewChoppedCommand::getStiffness(id); // temporary
    else
	return PreviewChoppedCommand::getStiffness(id);
}
