
#ifndef _Noggin_h_DEFINED
#define _Noggin_h_DEFINED

#include <Python.h>
#include <boost/shared_ptr.hpp>

#include "MotionInterface.h"
#include "Profiler.h"
#include "PyVision.h"
#include "MCL.h"
#include "LocEKF.h"
#include "BallEKF.h"
#include "Comm.h"
#include "GameController.h"

#include "RoboGuardian.h"
#include "ClickableButton.h"


class Noggin
{
public:
    Noggin(boost::shared_ptr<Profiler> p, boost::shared_ptr<Vision> v,
           boost::shared_ptr<Comm> c, boost::shared_ptr<RoboGuardian> rbg,
           MotionInterface * _minterface);
    virtual ~Noggin();

    // reload Brain module
    void reload ();
    // reload specified modules
    void reload (std::string modules);
    // run behavioral step
    void runStep();

    // current Noggin error status
    bool inErrorState() { return error_state; }

private:
    // Initialize the interpreter, the vision module, and PyVision instance
    void initializeVision(boost::shared_ptr<Vision> v);
    // Import the util.module_helper and noggin.Brain modules
    bool import_modules();
    // Instantiate a Brain instance
    void getBrainInstance();
    // Initialize the localization system
    void initializeLocalization();
    // Run the localization update; performed at every run step
    void updateLocalization();
    //Process button  clicks that pertain to GameController manipulation
    void processGCButtonClicks();

private:
    boost::shared_ptr<Profiler> profiler;
    boost::shared_ptr<Vision> vision;
    boost::shared_ptr<Comm> comm;
    boost::shared_ptr<GameController> gc;

    boost::shared_ptr<ClickableButton> chestButton;
    boost::shared_ptr<ClickableButton> leftFootButton;
    boost::shared_ptr<ClickableButton> rightFootButton;

    PyVision* pyvision;
    bool error_state;
    PyObject *module_helper;
    PyObject *brain_module;
    PyObject *brain_instance;
    MotionInterface * motion_interface;

    //GC stuff
    bool registeredGCReset;

// Public members
public:
    boost::shared_ptr<LocSystem> loc;
    boost::shared_ptr<BallEKF> ballEKF;
    boost::shared_ptr<LocEKF> locEKF;
};


#endif
