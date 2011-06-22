
#include "BalancingChoppedCommand.h"

using std::vector;

using namespace Kinematics;

BalancingChoppedCommand::BalancingChoppedCommand(ChoppedCommand::ptr choppedCommand)
    : PreviewChoppedCommand(choppedCommand)
{
    // TODO: initialize left/right arms
}

BalancingChoppedCommand::~BalancingChoppedCommand() {

}

vector<float> BalancingChoppedCommand::getNextJoints(int id) {
    if (id == LARM_CHAIN || id == RARM_CHAIN)
	return PreviewChoppedCommand::getNextJoints(id); // temporary
    else
	return PreviewChoppedCommand::getNextJoints(id);
}

const vector<float>
BalancingChoppedCommand::getStiffness(Kinematics::ChainID id) const {
    if (id == LARM_CHAIN || id == RARM_CHAIN)
	return PreviewChoppedCommand::getStiffness(id); // temporary
    else
	return PreviewChoppedCommand::getStiffness(id);
}
