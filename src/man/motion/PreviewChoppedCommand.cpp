
#include "PreviewChoppedCommand.h"

static const int COM_PREVIEW_FRAMES = 20;

PreviewChoppedCommand::PreviewChoppedCommand ( ChoppedCommand::ptr choppedCommand )
    : com_x(COM_PREVIEW_FRAMES),
      com_y(COM_PREVIEW_FRAMES),
      com_dx(COM_PREVIEW_FRAMES),
      com_dy(COM_PREVIEW_FRAMES),
      alreadyChoppedCommand(choppedCommand)
{
    // on construction, fill the Boxcars with our future CoM estimates
}

PreviewChoppedCommand::~PreviewChoppedCommand() {
    // nothing to do here, no dynamically allocated storage
}

bool PreviewChoppedCommand::isDone() {
    return alreadyChoppedCommand->isDone();
}

std::vector<float> PreviewChoppedCommand::getNextJoints( int id ) {
    return alreadyChoppedCommand->getNextJoints(id);
}

const std::vector<float>
PreviewChoppedCommand::getStiffness (Kinematics::ChainID id) const {
    return alreadyChoppedCommand->getStiffness(id);
}

const ufvector4 PreviewChoppedCommand::getFutureComPosition() {
    ufvector4 filteredCom = CoordFrame4D::vector4D(static_cast<float>(com_x.Y()),
						   static_cast<float>(com_y.Y()),
						   0.0f);
    return filteredCom;
}

const ufvector4 PreviewChoppedCommand::getComDerivative() {
    ufvector4 comDerivative = CoordFrame4D::vector4D(static_cast<float>(com_x.Y()),
						     static_cast<float>(com_y.Y()),
						     0.0f);
    return comDerivative;
}
