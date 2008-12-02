
#ifndef _JointCommand_h_DEFINED
#define _JointCommand_h_DEFINED

/*
 * General Scripted motion class.
 * Implements MotionCommand.
 *
 */

#include "MotionConstants.h"

class JointCommand : public MotionCommand
{
public:
	JointCommand(MotionConstants::MotionType type)
		: MotionCommand(type) { }

};
#endif
