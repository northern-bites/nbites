#pragma once

#include <Python.h>
#include <boost/shared_ptr.hpp>

//#include "Profiler.h"

#include "PyConstants.h"
#include "RoboGrams.h"
#include "PyInterface.h"

#include "GameState.pb.h"
#include "BallModel.pb.h"
#include "LedCommand.pb.h"

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
    BehaviorsModule();
    virtual ~BehaviorsModule();

    // reinitialize and reload the Python interpreter
    void reload_hard ();

    // run behavioral step
    void runStep();

    // current Noggin error status
    bool inErrorState() { return error_state; }

protected:
    virtual void run_() { runStep(); }

private:
    // Initialize the interpreter and C Python extensions
    void initializePython();
    // Insert the man directory in the system path
    void modifySysPath();
    // Import the util.module_helper and noggin.Brain modules
    bool import_modules();
    // Instantiate a Brain instance
    void getBrainInstance();
	// Latch new messages
	void latchMessages();
	// Send out messages
	void sendMessages();

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
	portals::InPortal<messages::FilteredBall> filteredBallIn;
	portals::OutPortal<messages::LedCommand> ledCommandOut;
private:
	portals::Message<messages::LedCommand> ledCommand;
};

}
}
