#ifndef _RoboGuardian_h
#define _RoboGuardian_h

#include <vector>
#include <string>
#include <string.h>
#include <boost/shared_ptr.hpp>

#include "Common.h"

#include "ClickableButton.h"

#include "RoboGrams.h"
#include "PMotion.pb.h"
#include "ButtonState.pb.h"
#include "FootBumperState.pb.h"
#include "InertialState.pb.h"
#include "StiffnessControl.pb.h"
#include "InitialState.pb.h"
#include "FeetOnGround.pb.h"
#include "FSR.pb.h"
#include "FallStatus.pb.h"
#include "BatteryState.pb.h"
#include "AudioCommand.pb.h"

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

    void executeShutdownAction();

    void enableFallProtection(bool _useFallProtection)
        { useFallProtection = _useFallProtection; };

    portals::OutPortal<messages::StiffnessControl> stiffnessControlOutput;
    portals::OutPortal<messages::InitialState>     initialStateOutput;
    portals::OutPortal<messages::FeetOnGround>     feetOnGroundOutput;
    portals::OutPortal<messages::FallStatus>       fallStatusOutput;
    portals::OutPortal<messages::AudioCommand>     audioOutput;

    portals::InPortal<messages::JointAngles>     temperaturesInput;
    portals::InPortal<messages::ButtonState>     chestButtonInput;
    portals::InPortal<messages::FootBumperState> footBumperInput;
    portals::InPortal<messages::InertialState>   inertialInput;
    portals::InPortal<messages::FSR>             fsrInput;
    portals::InPortal<messages::BatteryState>    batteryInput;

private:
    void checkFalling();
    bool isFalling(float angle_pos, float angle_vel);
    void checkFallen();
    void checkFeetOnGround();
    void checkBatteryLevels();
    void checkTemperatures();
    std::vector<float> vectorizeTemperatures(const messages::JointAngles& temps);
    void countButtonPushes();
    void processFallingProtection();
    void processChestButtonPushes();
    bool executeChestClickAction(int);
    void executeFallProtection();
    void shutoffGains();
    void enableGains();
    void playFile(std::string filePath);
    void checkAudio();
    void reloadMan();
    void initialState();

    static const int NO_CLICKS;
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

    bool lastInitial;

    bool sentAudio;

    unsigned long long int lastHeatAudioWarning, lastHeatPrintWarning;
};

}
}

#endif
