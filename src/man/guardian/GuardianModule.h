#ifndef _RoboGuardian_h
#define _RoboGuardian_h

#include <vector>
#include <queue>
#include <string>
#include <string.h>
#include <boost/shared_ptr.hpp>

#include "Common.h"

#include "ClickableButton.h"
#include "DebugConfig.h"

#include "RoboGrams.h"
#include "PMotion.pb.h"
#include "MotionStatus.pb.h"
#include "ButtonState.pb.h"
#include "InertialState.pb.h"
#include "StiffnessControl.pb.h"
#include "Toggle.pb.h"
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
    portals::OutPortal<messages::Toggle>           initialStateOutput;
    portals::OutPortal<messages::Toggle>           advanceStateOutput;
    portals::OutPortal<messages::Toggle>           printJointsOutput;
    portals::OutPortal<messages::Toggle>           switchTeamOutput;
    portals::OutPortal<messages::Toggle>           switchKickOffOutput;
    portals::OutPortal<messages::FeetOnGround>     feetOnGroundOutput;
    portals::OutPortal<messages::FallStatus>       fallStatusOutput;
    portals::OutPortal<messages::AudioCommand>     audioOutput;

    portals::InPortal<messages::JointAngles>     temperaturesInput;
    portals::InPortal<messages::ButtonState>     chestButtonInput;
    portals::InPortal<messages::FootBumperState> footBumperInput;
    portals::InPortal<messages::InertialState>   inertialInput;
    portals::InPortal<messages::FSR>             fsrInput;
    portals::InPortal<messages::BatteryState>    batteryInput;
    portals::InPortal<messages::MotionStatus>    motionStatusIn;

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
    void processFootBumperPushes();
    bool executeChestClickAction(int);
    bool executeLeftFootClickAction(int);
    bool executeRightFootClickAction(int);
    void shutoffGains();
    void enableGains();
    void playFile(std::string filePath);
    void checkAudio();
    void reloadMan();
    void initialState();
    void advanceState();
    void printJointAngles();
    void sayBatteryLevel();
    void switchTeams();
    void switchKickOff();

    static const int NO_CLICKS;
    static const unsigned long long int TIME_BETWEEN_HEAT_WARNINGS =
        MICROS_PER_SECOND * 60;

    std::vector<float> lastTemps;
    float lastBatteryCharge;

    boost::shared_ptr<ClickableButton> chestButton,
        leftFootButton,
        rightFootButton;

    unsigned int frameCount;
    unsigned int framesInBHWalk;

    struct Inertial lastInertial;
    int fallingFrames,notFallingFrames,fallenCounter,groundOnCounter,groundOffCounter;
    bool registeredFalling;
    bool registeredShutdown;

    bool falling, fallen, feetOnGround;
    bool useFallProtection;

    bool lastInitial;
    bool lastAdvance;
    bool lastPrint;

    bool lastTeamSwitch;
    bool lastKickOffSwitch;

    std::queue<std::string> audioQueue;

    unsigned long long int lastHeatAudioWarning, lastHeatPrintWarning;

    /*  */
    void doPickupDetection();

    static const int PICKUP_FRAMES = 50;
    static float PICKUP_NOT_SET;

    float pickupFrames[PICKUP_FRAMES];
    bool pickupCalibrated;
    float pickupZCalibration;
    int pickupIndex;

    enum PickupStatus {
        PICKED_UP = 1,
        NONE = 0,
        PUT_DOWN = -1
    };

    PickupStatus latestPickupStatus;
};

}
}

#endif
