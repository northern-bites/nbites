#ifndef _RoboGuardian_h
#define _RoboGuardian_h

#include <vector>
#include <string>
#include <string.h>
#include <boost/shared_ptr.hpp>

#include "ClickableButton.h"

#include "RoboGrams.h"

namespace man{
namespace guardian{

class GuardianModule : public portals::Module
{
public:
    GuardianModule();
    virtual ~GuardianModule();

    void run_();

    void executeShutdownAction()const;
    void executeStartupAction()const;

    //getters
    bool isRobotFalling()const { return useFallProtection && falling; }
    bool isRobotFallen()const { return useFallProtection && fallen; }
    bool isFeetOnGround()const { return useFallProtection && feetOnGround; }

    void enableFallProtection(bool _useFallProtection)
        { useFallProtection = _useFallProtection; };

private:
    void checkFalling();
    void checkFallen();
    void checkFeetOnGround();
    void checkBatteryLevels();
    void checkTemperatures();
    void countButtonPushes();
    void processFallingProtection();
    void processChestButtonPushes();
    bool executeChestClickAction(int);
    void executeFallProtection();
    void shutoffGains();
    void enableGains();
    void ifUpDown();
    void playFile(std::string filePath)const; //non-blocking
    void reloadMan();

    static const int NO_CLICKS;
    static const int GUARDIAN_FRAME_RATE;
    static const int GUARDIAN_FRAME_LENGTH_uS;
    static const unsigned long long int TIME_BETWEEN_HEAT_WARNINGS =
        MICROS_PER_SECOND * 60;

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

    bool falling, fallen, feetOnGround;
    bool useFallProtection;

    unsigned long long int lastHeatAudioWarning, lastHeatPrintWarning;
};

}
}

#endif
