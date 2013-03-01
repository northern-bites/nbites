#ifndef _RoboGuardian_h
#define _RoboGuardian_h

#include <vector>
#include <string>
#include <string.h>
#include <boost/shared_ptr.hpp>

#include "ClickableButton.h"

#include "RoboGrams.h"
#include "JointAngles.pb.h"
#include "ButtonState.pb.h"
#include "FootBumperState.pb.h"
#include "InertialState.pb.h"
#include "StiffnessControl.pb.h"

namespace man{
namespace guardian{

struct Inertial
{
    float angleX;
    float angleY;
};


class GuardianModule : public portals::Module
{
public:
    GuardianModule();
    virtual ~GuardianModule();

    void run_();

    void executeShutdownAction()const;

    //getters
    bool isRobotFalling()const { return useFallProtection && falling; }
    bool isRobotFallen()const { return useFallProtection && fallen; }
    bool isFeetOnGround()const { return useFallProtection && feetOnGround; }

    void enableFallProtection(bool _useFallProtection)
        { useFallProtection = _useFallProtection; };

    portals::OutPortal<messages::StiffnessControl> stiffnessControlOutput;

    portals::InPortal<messages::JointAngles>     jointsInput;
    portals::InPortal<messages::ButtonState>     chestButtonInput;
    portals::InPortal<messages::FootBumperState> footBumperInput;
    portals::InPortal<messages::InertialState>   intertialInput;

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

    struct Inertial lastInertial;
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
