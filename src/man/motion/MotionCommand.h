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

	    // Sent through to Python (override them for nicer behavior)
	    virtual int framesRemaining() { return framesLeft; }
	    virtual bool isDoneExecuting() { return commandFinished; }
	    virtual float timeRemaining() {
		if (isDoneExecuting())
		    return 0.0f;
		return static_cast<float>(framesRemaining()) * MOTION_FRAME_RATE;
	    }

	    // ONLY CALL THESE FROM PROVIDERS (aren't exposed to Python)
	    void finishedExecuting() { commandFinished = true; }
	    void framesRemaining(int _frames) {
		framesLeft = _frames;

		if (framesLeft > 0){
		    commandFinished = false;
		}
	    }

	    void tick() {
		if (--framesLeft < 0)
		    commandFinished = true;
	    }

	protected:
	    std::list<int> chainList;

	private:
	    virtual void setChainList() = 0;
	    const MotionConstants::MotionType motionType;
	    bool commandFinished;
	    int framesLeft;
	};
    } // namespace motion
} // namespace man
