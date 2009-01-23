
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
	virtual ~MotionCommand() { }
	const MotionConstants::MotionType getType() const { return motionType; }
	const std::list<int>* getChainList() const { return &chainList; }

protected:
	std::list<int> chainList;
private:
	virtual void setChainList() = 0;
	const MotionConstants::MotionType motionType;
};

#endif
