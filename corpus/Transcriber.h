
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

#ifndef _Transcriber_h_DEFINED
#define _Transcriber_h_DEFINED

#include "boost/shared_ptr.hpp"

#include "Sensors.h"

/**
 *  This abstract class serves to copy (transcribe) information from the 
 *  underlying robot platform to our codebase (e.g. copy into the Sensors class)
 *  It has different methods which can be called
 *  from different threads in order to update sensors at different
 *  frequencies, depending on their need.
 *
 *  The advantage of having the transcriber is that similar code does not need
 *  to be duplicated accross Enactors, and we can pull more robot/platform
 *  specific code out of Man
 */


class  Transcriber{
public:
    Transcriber(boost::shared_ptr<Sensors> _sensors):sensors(_sensors){}
    virtual ~Transcriber(){}

    virtual void postMotionSensors() = 0;
    virtual void postVisionSensors() = 0;

protected:
    boost::shared_ptr<Sensors> sensors;
};
#endif
