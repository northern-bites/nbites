
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
#include <string.h>
#include <boost/shared_ptr.hpp>

#include "Sensors.h"
#include "MotionInterface.h"
#include "ClickableButton.h"

#include "synchro/synchro.h"
#include "guardian/WifiAngel.h"

//TODO: move this to the guardian folder

enum  ButtonID {
    CHEST_BUTTON = 0,
    LEFT_FOOT_BUTTON,
    RIGHT_FOOT_BUTTON
};

class RoboGuardian : public Thread {
public:
    RoboGuardian(boost::shared_ptr<Sensors>);
    virtual ~RoboGuardian();

    void run();

    void executeShutdownAction()const;
    void executeStartupAction()const;
    void speakIPAddress()const;

    //getters
    bool isRobotFalling()const { return useFallProtection && falling; }
    bool isRobotFallen()const { return useFallProtection && fallen; }
	bool isFeetOnGround()const { return useFallProtection && feetOnGround; }

    boost::shared_ptr<ClickableButton> getButton(ButtonID)const;

    //this should be mutex locked - if you set the pointer to NULL it might
    //after the guardian thread checked to see if the pointer was NULL
    //(and it wasn't) it might result in a segfault - Octavian
    void setMotionInterface(MotionInterface* minterface)
        { motion_interface = minterface; }

    void enableFallProtection(bool _useFallProtection) const
        { useFallProtection = _useFallProtection; };


    const std::string discoverIP() const;

public:
    static const int NO_CLICKS;

private:
    void checkFalling();
    void checkFallen();
	void checkFeetOnGround();
    void checkBatteryLevels();
    void checkTemperatures();
    bool checkConnection();
    void countButtonPushes();
    void processFallingProtection();
    void processChestButtonPushes();
    bool executeChestClickAction(int);
    void executeFallProtection();
    void shutoffGains();
    void enableGains();
    void ifUpDown();
    //helpers
    std::string getHostName()const;
    void playFile(std::string filePath)const; //non-blocking
    void reloadMan();

public:
    static const int GUARDIAN_FRAME_RATE;
    static const int CONNECTION_CHECK_RATE;
    static const int GUARDIAN_FRAME_LENGTH_uS;
    static const unsigned long long int TIME_BETWEEN_HEAT_WARNINGS =
        MICROS_PER_SECOND * 60;

private:

    boost::shared_ptr<Sensors> sensors;
    MotionInterface* motion_interface;
    std::vector<float> lastTemps;
    float lastBatteryCharge;

    boost::shared_ptr<ClickableButton> chestButton,
        leftFootButton,
        rightFootButton;

    unsigned int frameCount;

    Inertial lastInertial;
    int fallingFrames,notFallingFrames,fallenCounter,groundOnCounter,groundOffCounter;
    bool registeredFalling;
    bool registeredShutdown;

    int wifiReconnectTimeout;

    bool falling, fallen, feetOnGround;
    mutable bool useFallProtection;

    unsigned long long int lastHeatAudioWarning, lastHeatPrintWarning;

    man::corpus::guardian::WifiAngel wifiAngel;

    mutable pthread_mutex_t click_mutex;
};

#endif
