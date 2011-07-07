
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

/**
 * All PyMotion commands inherit from here, so we can define a unified
 * callback interface from the motion engine.
 *
 * @author Nathan Merritt
 * @date June 2011
 */

#pragma once
#ifndef PY_MOTION_COMMAND_H
#define PY_MOTION_COMMAND_H

#include <boost/shared_ptr.hpp>

#include "AbstractCommand.h"

class PyMotionCommand {
public:
    typedef boost::shared_ptr<PyMotionCommand> ptr;

    virtual ~PyMotionCommand() {}

    // Generic, exposed to Python for all commands

    // NOTE: this returns motion frames remaining, not Python frames
    int framesRemaining() const { return command->framesRemaining(); }
    bool isDone() const { return command->isDoneExecuting(); }
    float timeRemaining() const { return command->timeRemaining(); }

protected:
    PyMotionCommand() {} // only derived classes can be instantiated
    AbstractCommand::ptr command;
};


#endif
