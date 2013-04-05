#pragma once

#include <Python.h>
#include <boost/shared_ptr.hpp>

//#include "Profiler.h"

#include "PyConstants.h"
#include "RoboGrams.h"
#include "PyInterface.h"
#include "Common.h"

#include "GameState.pb.h"
#include "BallModel.pb.h"
#include "LedCommand.pb.h"
#include "WorldModel.pb.h"
#include "PMotion.pb.h"
#include "MotionStatus.pb.h"
#include "SonarState.pb.h"
#include "VisionField.pb.h"
#include "VisionRobot.pb.h"
#include "FootBumperState.pb.h"

/**
 *
 * @brief Class to control the main thread function of all reasoning and
 * planning components of the robot.
 */

namespace man {
namespace behaviors {

class BehaviorsModule : public portals::Module
{
public:
    BehaviorsModule(int teamNum, int playerNum);
    virtual ~BehaviorsModule();

    // reinitialize and reload the Python interpreter
    void reload_hard ();

    // current Noggin error status
    bool inErrorState() { return error_state; }

    // Runs the module
    virtual void run_();

private:
    // Initialize the interpreter and C Python extensions
    void initializePython();
    // Insert the man directory in the system path
    void modifySysPath();
    // Import the util.module_helper and noggin.Brain modules
    bool import_modules();
    // Instantiate a Brain instance
    void getBrainInstance();
    // Latch new messages and prep out messages
    void prepareMessages();
    // Send out messages
    void sendMessages();

    int teamNumber;
    int playerNumber;

    bool error_state;
    PyObject *module_helper;
    PyObject *brain_module;
    PyObject *brain_instance;

    // Reload specifiers
    int do_reload;
    std::vector<std::string> module_list;

    // Portals and interface
    PyInterface pyInterface;
public:
    portals::InPortal<messages::GameState> gameStateIn;
    portals::InPortal<messages::WorldModel> worldModelIn[NUM_PLAYERS_PER_TEAM];
    portals::InPortal<messages::MotionStatus> motionStatusIn;
    portals::InPortal<messages::SonarState> sonarStateIn;
    portals::InPortal<messages::VisionBall> visionBallIn;
    portals::InPortal<messages::VisionField> visionFieldIn;
    portals::InPortal<messages::VisionRobot> visionRobotIn;
    portals::InPortal<messages::FootBumperState> footBumperStateIn;
    portals::InPortal<messages::RobotLocation> localizationIn;
    portals::InPortal<messages::FilteredBall> filteredBallIn;
    portals::OutPortal<messages::LedCommand> ledCommandOut;
    portals::OutPortal<messages::MotionCommand> motionCommandOut;
private:
    portals::Message<messages::LedCommand> ledCommand;
    portals::Message<messages::MotionCommand> motionCommand;
};

}
}
