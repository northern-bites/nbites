
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
 * This class is an abstraction to send commands from our robot code to the
 * underlying robot code, depending on the platform. The idea is to try
 * to remove as much platfrom dependant code from our code base and conncentrate
 * it in files which are 'hot swappable' so if we change from simmulation to
 * the robot, we can easily do this by creating a new Enactor/Transcriber pair.
 *
 * Note that this file was originally created only to send joint information
 * from the switchboard to ALMotion/Webots, so this transformation is not
 * complete yet. Also, there is still no enactor for Webots.
 *
 * See also Transcriber, ThreadedEnactor, NaoEnactor (DCM)
 * and ALEnactor(ALMotion).
 *
 * Future improvements. The interface could be more generalize by making
 * the switchboard private, and require lower classes to call methods in this
 * class to access information from the switchboard. At this point,
 * there is no need for that extra effort however. Also this class should be
 * named "Enactor" not MotionEnactor
 *
 * Johannes Strom April 2009
 */

#ifndef _MotionEnactor_h_DEFINED
#define _MotionEnactor_h_DEFINED

#include "MotionSwitchboard.h"

class MotionEnactor {
public:
    MotionEnactor()
        : switchboard(MotionSwitchboardInterface::NullInstance()) {}
    virtual ~MotionEnactor() { }

    virtual void sendCommands() = 0;
    virtual void postSensors() = 0;

    void resetSwitchboard() {
        this->setSwitchboard(MotionSwitchboardInterface::NullInstance());
    }

    void setSwitchboard(MotionSwitchboardInterface * s){
        assert(s != NULL);
        if (MotionSwitchboardInterface::isTheNullInstance(s)) {
            std::cout << "Switchboard reset to the Null Instance" << std::endl;
        } else {
            std::cout << "Switchboard set" <<std::endl;
        }
        switchboard = s;
    }

protected:
    MotionSwitchboardInterface *switchboard;
    bool switchboardSet; //Only true once the switchboard is set.
                         //Helps generate valid error messages when
                         //switchboard is deconstructed

};

#endif
