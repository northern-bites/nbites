
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

#ifndef _RoboGuardian_h
#define _RoboGuardian_h

#include <vector>
#include <boost/shared_ptr.hpp>

#include "albroker.h"
#include "alptr.h"

#include  "synchro.h"
#include  "Sensors.h"
#include  "MotionInterface.h"

class RoboGuardian : public Thread {
public:
    RoboGuardian(boost::shared_ptr<Synchro>,
                 boost::shared_ptr<Sensors>,
                 AL::ALPtr<AL::ALBroker>,
                 MotionInterface *);
    virtual ~RoboGuardian();

    void run();

    void executeShutdownAction();
    void executeStartupAction();
    void speakIPAddress();
private:
    void checkTemperatures();
    void checkButtonPushes();
    void executeClickAction(int);
private:

    boost::shared_ptr<Sensors> sensors;
    AL::ALPtr<AL::ALBroker> broker;
    MotionInterface * motion_interface;
    std::vector<float> lastTemps;
    int buttonOnCounter;
    int buttonOffCounter;
    int lastButtonOnCounter;
    int lastButtonOffCounter;
    int buttonClicks; //Stores how many clicks we think we may have gotten

    bool registeredClickThisTime;

    static const int GUARDIAN_FRAME_RATE;
    static const float GUARDIAN_FRAME_LENGTH_uS;

};

#endif
