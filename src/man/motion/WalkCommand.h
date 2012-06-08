
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
 * Walk commands take parameters in the following units:
 *   - Centimeter/second for Distance, Degrees/second for rotation.
 */
#ifndef _WalkCommand_h_DEFINED
#define _WalkCommand_h_DEFINED

#include <boost/shared_ptr.hpp>

// Ours
#include "MotionConstants.h"
#include "MotionCommand.h"

class WalkCommand : public MotionCommand
{
public:
    typedef boost::shared_ptr<WalkCommand> ptr;

    WalkCommand(float x_percent, float y_percent, float theta_percent)
        : MotionCommand(MotionConstants::WALK),
          x_percent(x_percent),y_percent(y_percent),theta_percent(theta_percent)
        { setChainList(); }

    virtual ~WalkCommand() {}
public:
//    WalkParameters params;
    const float x_percent;    //0 - 1 magnitude
    const float y_percent;    //0 - 1 magnitude
    const float theta_percent; //0 - 1 magnitude
protected:
	virtual void setChainList() {
        chainList.assign(MotionConstants::WALK_CHAINS,
                         MotionConstants::WALK_CHAINS +
                         MotionConstants::WALK_NUM_CHAINS);
    }

public:
    friend std::ostream& operator<< (std::ostream &o, const WalkCommand &w)
        {
            return o << "WalkCommand("
                     << w.x_percent << "," << w.y_percent << "," << w.theta_percent
                     << ") ";
        }

};

#endif
