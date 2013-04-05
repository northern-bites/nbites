#include <Python.h>
#include <cstdlib>
#include <exception>
#include <boost/shared_ptr.hpp>
#include <boost/python/errors.hpp>
#include <iostream>
#include "Common.h"

#include "BehaviorsModule.h"
#include "PyObjects.h"

using namespace boost::python;

extern "C" void initLedCommand_proto();
extern "C" void initGameState_proto();
extern "C" void initVisionField_proto();
extern "C" void initVisionRobot_proto();
extern "C" void initWorldModel_proto();
extern "C" void initRobotLocation_proto();
extern "C" void initBallModel_proto();
extern "C" void initPMotion_proto();
extern "C" void initMotionStatus_proto();
extern "C" void initRobotLocation_proto();
extern "C" void initSonarState_proto();
extern "C" void initFootBumperState_proto();
extern "C" void initinterface();

namespace man {
namespace behaviors {

const char * BRAIN_MODULE = "python.Brain";
static const unsigned int NUM_PYTHON_RESTARTS_MAX = 3;

BehaviorsModule::BehaviorsModule(int teamNum, int playerNum)
    : error_state(false),
      brain_module(NULL),
      brain_instance(NULL),
      do_reload(0),
      pyInterface(),
      ledCommandOut(base()),
      motionRequestOut(base()),
      bodyMotionCommandOut(base()),
      headMotionCommandOut(base())
{
    std::cout << "BehaviorsModule::initializing" << std::endl;

    // Store team and player numbers
    teamNumber = teamNum;
    playerNumber = playerNum;

    // Initialize the PyInterface pointer
    set_interface_ptr(boost::shared_ptr<PyInterface> (&pyInterface));

    // Initialize the interpreter and C python extensions
    initializePython();

    // import noggin.Brain and instantiate a Brain reference
    import_modules();

    std::cout << "  Retrieving Brain.Brain instance" << std::endl;

    // Instantiate a Brain instance
    getBrainInstance();
}

BehaviorsModule::~BehaviorsModule ()
{
    std::cout << "BehaviorsModule destructor" << std::endl;
    Py_XDECREF(brain_instance);
    Py_XDECREF(brain_module);
}

void BehaviorsModule::initializePython()
{
    std::cout << "  Initializing interpreter and extension modules"
              << std::endl;

    Py_Initialize();
    modifySysPath();
    brain_module = NULL;

    c_init_noggin_constants();
    c_init_objects();

    try{
        initLedCommand_proto();
        initGameState_proto();
        initVisionField_proto();
        initVisionRobot_proto();
        initWorldModel_proto();
        initBallModel_proto();
        initRobotLocation_proto();
        initPMotion_proto();
        initMotionStatus_proto();
        initSonarState_proto();
        initFootBumperState_proto();
        initRobotLocation_proto();
        // Init the interface as well
        initinterface();
    } catch (error_already_set) {
        PyErr_Print();
    }
}

bool BehaviorsModule::import_modules ()
{
    // Load Brain module
    //
    if (brain_module == NULL) {
        // Import brain module
        std::cout << "  Importing noggin.Brain" << std::endl;
        brain_module = PyImport_ImportModule(BRAIN_MODULE);
    }

    if (brain_module == NULL) {
        // error, couldn't import noggin.Brain
        std::cout << "Error importing noggin.Brain module" << std::endl;
        if (PyErr_Occurred())
            PyErr_Print();
        else
            std::cout << "  No Python exception information available"
                      << std::endl;
        return false;
    }

    return true;
}

void BehaviorsModule::reload_hard ()
{
    std::cout << "Reloading Python interpreter" << std::endl;
    // finalize and reinitialize the Python interpreter
    Py_Finalize();
    // load C extension modules
    initializePython();
    // import noggin.Brain and instantiate a Brain reference
    import_modules();
    // Instantiate a Brain instance
    getBrainInstance();
}

void BehaviorsModule::getBrainInstance ()
{

    if (brain_module == NULL)
        if (!import_modules())
            return;

    // drop old reference
    Py_XDECREF(brain_instance);
    // Grab instantiate and hold a reference to a new noggin.Brain.Brain()
    PyObject *dict = PyModule_GetDict(brain_module);
    PyObject *brain_class = PyDict_GetItemString(dict, "Brain");
    if (brain_class != NULL)
        brain_instance = PyObject_CallObject(brain_class, Py_BuildValue("ii", teamNumber, playerNumber));
    else
        brain_instance = NULL;

    if (brain_instance == NULL) {
        std::cout << "Error accessing Brain" << std::endl;
        if (PyErr_Occurred())
            PyErr_Print();
        else
            std::cout << "  No error available" << std::endl;
    }

    // Successfully reloaded
    error_state = (brain_instance == NULL);
}

void BehaviorsModule::run_ ()
{
    static unsigned int num_crashed = 0;
    if (error_state && num_crashed < NUM_PYTHON_RESTARTS_MAX) {
        this->reload_hard();
        error_state = false;
        num_crashed++;
    }

    // Latch incoming messages and prepare outgoing messages
    prepareMessages();

    /*PROF_ENTER(P_PYTHON);*/

    // Call main run() method of Brain
    //PROF_ENTER(P_PYRUN);
    if (brain_instance != NULL) {
        PyObject *result = PyObject_CallMethod(brain_instance, "run", NULL);
        if (result == NULL) {
            // set BehaviorsModule in error state
            error_state = true;
            // report error
            std::cout << "Error occurred in Brain.run() method" << std::endl;
            if (PyErr_Occurred()) {
                PyErr_Print();
            } else {
                std::cout << "  No Python exception information available"
                          << std::endl;
            }
        } else {
            Py_DECREF(result);
        }
    }
    //PROF_EXIT(P_PYRUN);

    // PROF_EXIT(P_PYTHON);

    // Send outgoing messages
    sendMessages();
}

void BehaviorsModule::prepareMessages()
{
    // Latch incoming messages
    gameStateIn.latch();
    pyInterface.setGameState_ptr(&gameStateIn.message());

    visionFieldIn.latch();
    pyInterface.setVisionField_ptr(&visionFieldIn.message());

    visionRobotIn.latch();
    pyInterface.setVisionRobot_ptr(&visionRobotIn.message());

    visionObstacleIn.latch();
    pyInterface.setVisionObstacle_ptr(&visionObstacleIn.message());

    filteredBallIn.latch();
    pyInterface.setFilteredBall_ptr(&filteredBallIn.message());

    localizationIn.latch();
    pyInterface.setRobotLocation_ptr(&localizationIn.message());

    for (int i=0; i<NUM_PLAYERS_PER_TEAM; i++) {
        worldModelIn[i].latch();
        pyInterface.setWorldModel_ptr(&worldModelIn[i].message(),i);
    }

    motionStatusIn.latch();
    pyInterface.setMotionStatus_ptr(&motionStatusIn.message());

    odometryIn.latch();
    pyInterface.setOdometry_ptr(&odometryIn.message());

    sonarStateIn.latch();
    pyInterface.setSonarState_ptr(&sonarStateIn.message());

    footBumperStateIn.latch();
    pyInterface.setFootBumperState_ptr(&footBumperStateIn.message());

    jointAnglesIn.latch();
    pyInterface.setJointAngles_ptr(&jointAnglesIn.message());

    ledCommand = portals::Message<messages::LedCommand>(0);
    pyInterface.setLedCommand_ptr(ledCommand.get());

    motionRequest = portals::Message<messages::MotionRequest>(0);
    pyInterface.setMotionRequest_ptr(motionRequest.get());

    bodyMotionCommand = portals::Message<messages::MotionCommand>(0);
    pyInterface.setBodyMotionCommand_ptr(bodyMotionCommand.get());

    headMotionCommand = portals::Message<messages::HeadMotionCommand>(0);
    pyInterface.setHeadMotionCommand_ptr(headMotionCommand.get());
}

void BehaviorsModule::sendMessages()
{
    ledCommandOut.setMessage(ledCommand);

    // Only set motion commands that python has actually used
    if (!bodyMotionCommand.get()->processed_by_motion())
    {
        bodyMotionCommandOut.setMessage(bodyMotionCommand);
    }
    if (!headMotionCommand.get()->processed_by_motion())
    {
        headMotionCommandOut.setMessage(headMotionCommand);
    }
    if (!motionRequest.get()->processed_by_motion())
    {
        motionRequestOut.setMessage(motionRequest);
    }
}

void BehaviorsModule::modifySysPath ()
{
    // Enter the current working directory into the python module path
    const char *cwd = "/home/nao/nbites/lib";

    std::cout << "  Adding " << cwd << " to sys.path" << std::endl;

    PyObject *sys_module = PyImport_ImportModule("sys");
    if (sys_module == NULL) {
        std::cout << "** Error importing sys module: **" << std::endl;
        if (PyErr_Occurred())
            PyErr_Print();
        else
            std::cout << "** No Python exception information available **"
                      << std::endl;
    }
    else
    {
        PyObject *dict = PyModule_GetDict(sys_module);
        PyObject *path = PyDict_GetItemString(dict, "path");
        PyList_Append(path, PyString_FromString(cwd));
        Py_DECREF(sys_module);
    }
}

}
}
