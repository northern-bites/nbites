#include "ChopShop.h"
#include "Kinematics.h"

using namespace Kinematics;

namespace man
{
namespace motion
{

ChopShop::ChopShop()
{
}

// Breaks command into FRAME_LENGTH_S size pieces,
// adds it to the queue
ChoppedCommand::ptr ChopShop::chopCommand(
    const JointCommand::ptr command,
    std::vector<float>& currentJoints
    ) 
{
    ChoppedCommand::ptr chopped;
    int numChops = 1;
    if (command->getDuration() > MOTION_FRAME_LENGTH_S) 
    {
	numChops = static_cast<int>(command->getDuration() 
				    / MOTION_FRAME_LENGTH_S);
    }

    if (command->getInterpolation() == INTERPOLATION_LINEAR) 
    {
	chopped = chopLinear(command, currentJoints, numChops);
    }
    else if (command->getInterpolation() == INTERPOLATION_SMOOTH) 
    {
	chopped = chopSmooth(command, currentJoints, numChops);
    }
    else 
    {
	std::cout << "ILLEGAL INTERPOLATION VALUE. CHOPPING SMOOTHLY" << std::endl;
	chopped = chopSmooth(command, currentJoints, numChops);
    }

    return chopped;
}

//Smooth interpolation motion
ChoppedCommand::ptr ChopShop::chopSmooth(
    const JointCommand::ptr command,
    std::vector<float>& currentJoints, 
    int numChops
    ) 
{
    return ChoppedCommand::ptr(new SmoothChoppedCommand(
				   command,
				   currentJoints,
				   numChops));
}

/*
 * Linear interpolation chopping:
 * Retrieves current joint angles and acquires the differences
 * between the current and the intended final. Send them to
 */
ChoppedCommand::ptr ChopShop::chopLinear(
    const JointCommand::ptr command,
    std::vector<float>& currentJoints,
    int numChops) 
{
    return ChoppedCommand::ptr(new LinearChoppedCommand(
				   command,
				   currentJoints,
				   numChops));
}

} // namespace motion
} // namespace man
