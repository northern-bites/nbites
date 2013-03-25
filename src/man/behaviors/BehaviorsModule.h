#pragma once

#include <Python.h>
#include <boost/shared_ptr.hpp>

//#include "Profiler.h"

#include "PyConstants.h"
#include "RoboGrams.h"

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

    bool error_state;
    PyObject *module_helper;
    PyObject *brain_module;
    PyObject *brain_instance;

    // Reload specifiers
    int do_reload;
    std::vector<std::string> module_list;
};

}
}
