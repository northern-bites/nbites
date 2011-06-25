
// This file is part of Man, a robotic perception, locomotion, and
// team strategy application created by the Northern Bites RoboCup
// team of Bowdoin College in Brunswick, Maine, for the Aldebaran
// Nao robot.
//
// Man is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Man is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser Public License for more details.
//
// You should have received a copy of the GNU General Public License
// and the GNU Lesser Public License along with Man.  If not, see
// <http://www.gnu.org/licenses/>.

#ifndef _JointCommand_h_DEFINED
#define _JointCommand_h_DEFINED

/*
 * General Scripted motion class.
 * Implements MotionCommand.
 *
 */

#include <boost/shared_ptr.hpp>

#include "MotionConstants.h"
#include "MotionCommand.h"
#include "Kinematics.h"

class JointCommand : public MotionCommand {
public:
    typedef boost::shared_ptr<JointCommand> ptr;

    JointCommand(const MotionConstants::MotionType motionType,
		 const float _duration,
		 const Kinematics::InterpolationType _type,
		 const std::vector<float>* _stiffness)
	: MotionCommand(motionType),
	  duration(_duration),
	  type(_type),
	  stiffness(_stiffness)
	{ }

    virtual ~JointCommand() { delete stiffness; };

    const float getDuration() const { return duration; }
    const Kinematics::InterpolationType getInterpolation() const {return type;}
    const std::vector<float>* getStiffness() const{ return stiffness; }
    virtual const std::vector<float>* getJoints(Kinematics::ChainID chain) const = 0;

protected:
    const float duration;
    const Kinematics::InterpolationType type;

private:
    const std::vector<float> *stiffness;
};
#endif
