
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
#include <string>
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

    void executeShutdownAction()const;
    void executeStartupAction()const;
    void speakIPAddress()const;

    //getters
    bool isRobotFalling()const { return falling; }
    bool isRobotFallen()const { return fallen; }
    int getNumChestClicks()const;

//private: // Since this feature is not really production ready
// from George: I removed the private tag, so that Python can call this method
    //setters
    void enableFallProtection(bool _useFallProtection)const //off by default
        { useFallProtection = _useFallProtection; };

public:
    static const int NO_CLICKS;

private:
    void checkFallProtection();
    void checkBatteryLevels();
    void checkTemperatures();
    void checkButtonPushes();
    void executeClickAction(int);
    void shutoffGains();
    void resetWifiConnection();
    //helpers
    std::string getHostName()const;
    void playFile(std::string filePath)const; //non-blocking
private:

    boost::shared_ptr<Sensors> sensors;
    AL::ALPtr<AL::ALBroker> broker;
    MotionInterface * motion_interface;
    std::vector<float> lastTemps;
    float lastBatteryCharge;
    int buttonOnCounter;
    int buttonOffCounter;
    int lastButtonOnCounter;
    int lastButtonOffCounter;
    int buttonClicks; //Stores how many clicks we think we may have gotten
    Inertial lastInertial;
    int fallingFrames,notFallingFrames,fallenCounter;
    mutable int numClicks;

    bool registeredClickThisTime,registeredShutdown;

    bool falling, fallen;
    mutable bool useFallProtection;

    mutable pthread_mutex_t click_mutex;
    static const int GUARDIAN_FRAME_RATE;
    static const float GUARDIAN_FRAME_LENGTH_uS;

};

#endif
