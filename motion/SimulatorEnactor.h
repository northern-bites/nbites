
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

#ifndef _SimulatorEnactor_h_DEFINED
#define _SimulatorEnactor_h_DEFINED

#include "almotionproxy.h"

#include "MotionEnactor.h"

class SimulatorEnactor : public MotionEnactor {
public:
    SimulatorEnactor(Sensors * s)
        : MotionEnactor(),sensors(s) {};
    virtual ~SimulatorEnactor() { };

    virtual void run();

    virtual void postSensors();

private:
    Sensors * sensors;

    static const int MOTION_FRAME_RATE = 50;
    static const float MOTION_FRAME_LENGTH_uS = // in microseconds
      // 1 second * 1000 ms/s * 1000 us/ms
      1.0f * 1000.0f * 1000.0f / MOTION_FRAME_RATE;
    static const float MOTION_FRAME_LENGTH_S = // in seconds
      1.0f / MOTION_FRAME_RATE;
};

#endif

