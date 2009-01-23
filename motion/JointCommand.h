
#ifndef _JointCommand_h_DEFINED
#define _JointCommand_h_DEFINED

/*
 * General Scripted motion class.
 * Implements MotionCommand.
 *
 */

#include "MotionConstants.h"
#include "MotionCommand.h"
#include "Kinematics.h"

class JointCommand : public MotionCommand {
public:
	JointCommand(const MotionConstants::MotionType motionType,
				 const float _duration,
				 const Kinematics::InterpolationType _type)
		: MotionCommand(motionType), duration(_duration), type(_type) { }

	virtual ~JointCommand() { };

/* Attributes for JointCommand
 * Duration
 * InterpolationType
 */
	const float getDuration() const { return duration; }
	const Kinematics::InterpolationType getInterpolation() const {return type;}
	virtual const std::vector<float>* getJoints(Kinematics::ChainID chain) const = 0;

protected:
	const float duration;
	const Kinematics::InterpolationType type;
};
#endif
