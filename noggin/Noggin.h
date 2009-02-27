
#ifndef _Noggin_h_DEFINED
#define _Noggin_h_DEFINED

#include <Python.h>
#include <boost/shared_ptr.hpp>

#include "MotionInterface.h"
#include "Profiler.h"
#include "PyVision.h"
#include "MCL.h"
#include "BallEKF.h"

class Noggin
{
public:
    Noggin(boost::shared_ptr<Profiler> p, boost::shared_ptr<Vision> v,
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

private:
    boost::shared_ptr<Profiler> profiler;
    boost::shared_ptr<Vision> vision;
    PyVision* pyvision;
    bool error_state;
    PyObject *module_helper;
    PyObject *brain_module;
    PyObject *brain_instance;
    MotionInterface * motion_interface;
// Public members
public:
    boost::shared_ptr<MCL> mcl;
    boost::shared_ptr<BallEKF> ballEKF;
};


#endif
