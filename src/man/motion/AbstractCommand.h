
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
 * Defines the interface that will end up being exposed to Python for any
 * type of command we send in, including Walk Commands, Gait Commands, etc...
 *
 * @author Nathan Merritt
 * @date June 2011
 */

#pragma once
#ifndef ABSTRACT_COMMAND_H
#define ABSTRACT_COMMAND_H

#include <boost/shared_ptr.hpp>
#include <iostream>

#include "Common.h"

class AbstractCommand {
public:
    // all derived classes need at least this, many will override
    typedef boost::shared_ptr<AbstractCommand> ptr;

    AbstractCommand()
        : commandFinished(false), framesLeft(-1)
        {}
    virtual ~AbstractCommand() {}

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
    void framesRemaining(int _frames) { framesLeft = _frames; }

    void tick() {
        if (--framesLeft < 0)
            commandFinished = true;
    }

private:
    bool commandFinished;
    int framesLeft;
};


#endif
