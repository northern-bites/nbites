
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

    WalkCommand(float _x_mms, float _y_mms, float _theta_rads)
        : MotionCommand(MotionConstants::WALK),
          x_mms(_x_mms),y_mms(_y_mms),theta_rads(_theta_rads)
        { setChainList(); }

    virtual ~WalkCommand() {}
public:
//    WalkParameters params;
    const float x_mms;    //mm/second
    const float y_mms;    //mm/second
    const float theta_rads; //rad/second
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
                     << w.x_mms << "," << w.y_mms << "," << w.theta_rads
                     << ") ";
        }

};

#endif
