
#include <iostream>
#include <algorithm>

#include "PreviewChoppedCommand.h"

using std::vector;

static const int COM_PREVIEW_FRAMES = 20;

PreviewChoppedCommand::PreviewChoppedCommand ( ChoppedCommand::ptr choppedCommand )
    : thisFramesAngles(vector<float>()),
      com_x(COM_PREVIEW_FRAMES),
      com_y(COM_PREVIEW_FRAMES),
      com_dx(COM_PREVIEW_FRAMES),
      com_dy(COM_PREVIEW_FRAMES),
      alreadyChoppedCommand(choppedCommand)
{
    for (unsigned int i = 0; i < Kinematics::NUM_CHAINS; ++i)
	jointAngles.push_back( VectorFifo(COM_PREVIEW_FRAMES) );

    thisFramesAngles.reserve(Kinematics::NUM_JOINTS);

    // Fill the FIFOs with joint angles & the Boxcars with future CoM estimates
    for (int frame = 0; frame < COM_PREVIEW_FRAMES; ++frame) {
	if (alreadyChoppedCommand->isDone()) {
	    printf("command is done, breaking\n");
	    break;
	}

	for (unsigned int chain = 0; chain < Kinematics::NUM_CHAINS; ++chain)
	    bufferNextAngles(chain);
    }
}

PreviewChoppedCommand::~PreviewChoppedCommand() {
}

/**
 * Gets the next chain angles from a chopped command, and puts them onto the
 * back of the appropriate FIFO. Also copies the angles to a storage vector, so
 * that after all the angles at a given motion tick have been queued we can
 * calculate the CoM from pose.
 */
void PreviewChoppedCommand::bufferNextAngles(int chainID) {
    vector<float> chainAngles = alreadyChoppedCommand->getNextJoints(chainID);

    thisFramesAngles.insert(thisFramesAngles.end(),
			    chainAngles.begin(), chainAngles.end());

    jointAngles[chainID].Write(chainAngles);

    //std::cout << " queue size after write is: " << jointAngles[chainID].Count()
    //<< " with " << jointAngles[chainID].Error() << " errors"
    //<< std::endl;

    // updates CoM estimates
    if (chainID == Kinematics::NUM_CHAINS - 1) {
	ufvector4 com_c = Kinematics::getCOMc(thisFramesAngles);

	double lastX = com_x.Y();
	double lastY = com_y.Y();

	// put the CoM into the filters
	com_x.X(com_c(0));
	com_y.X(com_c(1));

	// update the dx filters
	com_dx.X(lastX - com_x.Y());
	com_dy.X(lastY - com_y.Y());

	std::cout //<< "Saw last chain, updating CoM...angles vector size: "
	    //<< thisFramesAngles.size()
		  << " CoM this frame: " << com_c
		  << " CoM_X filter: " << com_x.Y()
		  << " d/dt CoM_X: " << com_dx.Y()
		  << " CoM_Y filter: " << com_y.Y()
		  << " d/dt CoM_Y: " << com_dy.Y()
		  << std::endl;

	thisFramesAngles.clear();
    }
}

/**
 * If all the upcoming angles have been pushed onto the FIFO, then just pop
 * the oldest one off and return it. Otherwise, add the next angle from the
 * command pointer onto the back of the FIFO and update the CoM estimate for it.
 *
 * @return next joint angles for this id
 */
vector<float> PreviewChoppedCommand::getNextJoints( int chainID ) {
    if (jointAngles[chainID].Empty()) {
	std::cout << "Went off the end of a PreviewChoppedCommand, chain: "
		  << chainID << " size=" << jointAngles[chainID].Count()
		  << std::endl;
	return vector<float>(0);
    }

    vector<float> next = jointAngles[chainID].Read();

    if (!alreadyChoppedCommand->isDone())
 	bufferNextAngles(chainID);

    return next;
}

const vector<float>
PreviewChoppedCommand::getStiffness (Kinematics::ChainID id) const {
    return alreadyChoppedCommand->getStiffness(id);
}

// Done when all the joint angles have been fetched from the FIFOs and there
// aren't any more left to replace them
bool PreviewChoppedCommand::isDone() const {
    for (unsigned int i = 0; i < Kinematics::NUM_CHAINS; ++i) {
	if (!jointAngles[i].Empty())
	    return false;
    }
    return true;
}

const ufvector4 PreviewChoppedCommand::getFutureComPosition() {
    ufvector4 filteredCom = CoordFrame4D::vector4D(static_cast<float>(com_x.Y()),
						   static_cast<float>(com_y.Y()),
						   0.0f);
    return filteredCom;
}

const ufvector4 PreviewChoppedCommand::getComDerivative() {
    ufvector4 comDerivative = CoordFrame4D::vector4D(static_cast<float>(com_dx.Y()),
						     static_cast<float>(com_dy.Y()),
						     0.0f);
    return comDerivative;
}
