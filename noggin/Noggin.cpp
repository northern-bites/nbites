

#include <Python.h>
#include <exception>
#include <boost/shared_ptr.hpp>

#include "Noggin.h"
#include "_ledsmodule.h"

#include "nogginconfig.h"

using namespace std;
using namespace boost;

const char * BRAIN_MODULE = "man.noggin.Brain";

Noggin::Noggin (shared_ptr<Profiler> p, shared_ptr<Vision> v)
  : error_state(false), brain_module(NULL), brain_instance(NULL), mcl()
{
#ifdef DEBUG_NOGGIN_INITIALIZATION
    printf("Noggin::initializing\n");
#endif

    // Initialize the interpreter, the vision module, and PyVision instance
    initializeVision(v);

    // import noggin.Brain and instantiate a Brain reference
    import_modules();

#ifdef DEBUG_NOGGIN_INITIALIZATION
    printf("  Retrieving Brain.Brain instance\n");
#endif

    // Instantiate a Brain instance
    getBrainInstance();

#ifdef DEBUG_NOGGIN_INITIALIZATION
    printf("  DONE!\n");
#endif

}

Noggin::~Noggin ()
{
    Py_XDECREF(brain_instance);
    Py_XDECREF(brain_module);
}

void Noggin::initializeVision(shared_ptr<Vision> v)
{
#ifdef DEBUG_NOGGIN_INITIALIZATION
    printf("  Initializing interpreter and extension modules\n");
#endif

    // Initialize PyVision module
    vision = v;
    MODULE_INIT(vision) ();

    // Initialize and insert the vision wrapper into the module
    PyObject *result = PyVision_new(v.get());
    if (result == NULL) {
        cerr << "** Noggin extension could not initialize PyVision object **" <<
            endl;
        assert(false);
    }
    vision_addToModule(result, MODULE_HEAD);
    pyvision = reinterpret_cast<PyVision*>(result);

    init_leds();
}

bool Noggin::import_modules ()
{
#ifdef  DEBUG_NOGGIN_INITIALIZATION
    printf("  Importing noggin.Brain\n");
#endif

    // Load Brain module
    //
    if (brain_module == NULL)
        // Import brain module
        brain_module = PyImport_ImportModule(BRAIN_MODULE);

    if (brain_module == NULL) {
        // error, couldn't import noggin.Brain
        fprintf(stderr, "Error importing noggin.Brain module\n");
        if (PyErr_Occurred())
            PyErr_Print();
        else
            fprintf(stderr, "  No Python exception information available\n");
        return false;
    }

    return true;
}

void Noggin::reload ()
{
    if (brain_module == NULL)
        if (!import_modules())
            return;

    // reload Brain module
    PyImport_ReloadModule(brain_module);
    // Instantiate a Brain instance
    getBrainInstance();
}

void
Noggin::reload(std::string modules)
{
    if (brain_module == NULL)
        if (!import_modules())
            return;

    /* Not currently implemented */

    // reload Brain module
    PyImport_ReloadModule(brain_module);
    // Instantiate a Brain instance
    getBrainInstance();
}

void Noggin::getBrainInstance ()
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
        brain_instance = PyObject_CallObject(brain_class, NULL);
    else
        brain_instance = NULL;

    if (brain_instance == NULL) {
        fprintf(stderr, "Error accessing noggin.Brain.Brain\n");
        if (PyErr_Occurred())
            PyErr_Print();
        else
            fprintf(stderr, "  No error available\n");
    }

    // Successfully reloaded
    error_state = (brain_instance == NULL);
}

void Noggin::runStep ()
{
#ifdef USE_NOGGIN_AUTO_HALT
    // don't bother doing anything if there's a Python error and we
    // haven't reloaded
    if (error_state)
        return;
#endif

    PROF_ENTER(profiler, P_PYTHON);

    // Update vision information for Python
    PROF_ENTER(profiler, P_PYUPDATE);
    PyVision_update(pyvision);
    PROF_EXIT(profiler, P_PYUPDATE);

    // Update localization information
    PROF_ENTER(profiler, P_LOC);
    updateLocalization();
    PROF_EXIT(profiler, P_LOC);

    // Call main run() method of Brain
    PROF_ENTER(profiler, P_PYRUN);
    if (brain_instance != NULL) {
        PyObject *result = PyObject_CallMethod(brain_instance, "run", NULL);
        if (result == NULL) {
            // set Noggin in error state
            error_state = true;
            // report error
            fprintf(stderr, "Error occurred in noggin.Brain.run() method\n");
            if (PyErr_Occurred()) {
                PyErr_Print();
            } else {
                fprintf(stderr,
                        "  No Python exception information available\n");
            }
        } else {
            Py_DECREF(result);
        }
    }
    PROF_EXIT(profiler, P_PYRUN);

    PROF_EXIT(profiler, P_PYTHON);
}

void Noggin::updateLocalization()
{
    // Self Localization
    MotionModel odometery(0.0f, 0.0f, 0.0f);

    // Build the observations from vision data
    vector<Observation> observations;
    // FieldObjects
    VisualFieldObject fo = *(vision->bgrp);

    if(fo.getDistance() > 0) {
        observations.push_back(fo);
        cout << "Saw bgrp at distance" << fo.getDistance() << endl;
    }

    fo = *(vision->bglp);
    if(fo.getDistance() > 0) {
        observations.push_back(fo);
        cout << "Saw bglp at distance" << fo.getDistance() << endl;
    }

    fo = *(vision->ygrp);
    if(fo.getDistance() > 0) {
        observations.push_back(fo);
        cout << "Saw ygrp at distance" << fo.getDistance() << endl;
    }

    fo = *(vision->yglp);
    if(fo.getDistance() > 0) {
        observations.push_back(fo);
        cout << "Saw yglp at distance" << fo.getDistance() << endl;
    }
    // Corners

    // Lines

    // Process the information
    PROF_ENTER(profiler, P_MCL);
    mcl.updateLocalization(odometery, observations, true);
    PROF_EXIT(profiler, P_MCL);
    //cout << mcl << endl;

    // Ball Tracking


    // Opponent Tracking
}
