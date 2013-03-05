#pragma once

/**
 * Overarching MotionCommand class for motion.
 *
 * Keeps track of what kind of motion it implements.
 *
 */
#include <list>

#include "Common.h"
#include "MotionConstants.h"

namespace man
{
    namespace motion
    {
	class MotionCommand
	{
	public:
	    typedef boost::shared_ptr<MotionCommand> ptr;

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
    } // namespace motion
} // namespace man
