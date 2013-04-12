#pragma once

#include <vector>
#include <boost/shared_ptr.hpp>

#include "BodyJointCommand.h"

#include "JointCommand.h"
#include "ChoppedCommand.h"
#include "LinearChoppedCommand.h"
#include "SmoothChoppedCommand.h"

namespace man
{
namespace motion
{

class ChopShop
{
public:
    ChopShop();

    ChoppedCommand::ptr chopCommand(const JointCommand::ptr command,
	                            std::vector<float>& currentJoints);

private:
    float FRAME_LENGTH_S;

    ChoppedCommand::ptr chopLinear(const JointCommand::ptr command,
				   std::vector<float>& currentJoints,
				   int numChops);

    ChoppedCommand::ptr chopSmooth(const JointCommand::ptr command,
				   std::vector<float>& currentJoints,
				   int numChops);

};

} // namespace motion
} // namespace man
