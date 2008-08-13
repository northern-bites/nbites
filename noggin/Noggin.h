
#ifndef _Noggin_h_DEFINED
#define _Noggin_h_DEFINED

#include <Python.h>
#include <exception>

#include "Sensors.h"
#include "Profiler.h"
#include "PyVision.h"

class Noggin
{
  public:
    Noggin(Sensors *s, Profiler *p, Vision *v);
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
    void initializeVision(Vision *v);
    // Load sys module and add current working directory to the module path
    void modifySysPath();
    // Import the util.module_helper and noggin.Brain modules
    bool import_modules();
    // Instantiate a Brain instance
    void getBrainInstance();

  private:
    Sensors *sensors;
    Profiler *profiler;
    PyVision *vision;

    bool error_state;
    PyObject *module_helper;
    PyObject *brain_module;
    PyObject *brain_instance;
};


#endif
