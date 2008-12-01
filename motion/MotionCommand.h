
#ifndef _MotionCommand_h_DEFINED
#define _MotionCommand_h_DEFINED

/**
 * Overarching MotionCommand class for motion.
 *
 * It's only ability is to keep track of what kind of motion
 * it implements.
 */
#include <list>

class MotionCommand
{
public:
	MotionCommand(MotionConstants::MotionType type)
		: chainList(), motionType(type) { }

	MotionConstants::MotionType getType() { return motionType; }
	std::list<int> getChainList() { return chainList; }

protected:
	std::list<int> chainList;
private:
	virtual void setChainList() = 0;
	MotionConstants::MotionType motionType;
};

#endif
