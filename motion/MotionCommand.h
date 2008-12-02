
#ifndef _MotionCommand_h_DEFINED
#define _MotionCommand_h_DEFINED

/**
 * Overarching MotionCommand class for motion.
 *
 * It's only ability is to keep track of what kind of motion
 * it implements.
 */

#include "MotionConstants.h";

class MotionCommand
{
public:
	MotionCommand(MotionConstants::MotionType type) { motionType = _type; }

	virtual ~MotionCommand() = 0;

	MotionConstants::MotionType getType() {return motionType;}

private:
	MotionConstants::MotionType motionType;

};

#endif
