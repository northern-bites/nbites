
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
 * Destination commands take parameters in the following units:
 * X/Y mm relative to our current position to walk to, rads to turn to
 */
#ifndef _DestinationCommand_h_DEFINED
#define _DestinationCommand_h_DEFINED

#include "MotionConstants.h"
#include "MotionCommand.h"

class DestinationCommand : public MotionCommand
{
public:
    typedef boost::shared_ptr<DestinationCommand> ptr;

    DestinationCommand(float _x_mm, float _y_mm, float _theta_rads, float _gain=1.0f)
	: MotionCommand (MotionConstants::DESTINATION),
          x_mm(_x_mm),y_mm(_y_mm),theta_rads(_theta_rads), gain(_gain) {
	setChainList();
    }

    virtual ~DestinationCommand() {}
public:
    // DestinationCommand parameters
    const float x_mm;
    const float y_mm;
    const float theta_rads;
    const float gain;
protected:
    virtual void setChainList() {
        chainList.assign(MotionConstants::WALK_CHAINS,
                         MotionConstants::WALK_CHAINS +
                         MotionConstants::WALK_NUM_CHAINS);
    }

public:
    friend std::ostream& operator<< (std::ostream &o, const DestinationCommand &w)
        {
            return o << "DestinationCommand("
                     << w.x_mm << "," << w.y_mm << "," << w.theta_rads
                     << ") at: " << w.gain*100 << "%";
        }

};

#endif
