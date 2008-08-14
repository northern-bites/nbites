

#include "Noggin.h"
#include "potentialfieldmodule.h"
#include "_matrixmodule.h"
#include "_ledsmodule.h"

#include "nogginconfig.h"

using namespace std;


Noggin::Noggin (Sensors *s, Profiler *p, Vision *v)
  : sensors(s), profiler(p), vision(NULL),
    error_state(false),
    brain_module(NULL), brain_instance(NULL)
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

void
Noggin::initializeVision(Vision *v)
{
#ifdef DEBUG_NOGGIN_INITIALIZATION
  printf("  Initializing interpreter and extension modules\n");
#endif

  // Initialize PyVision module
  MODULE_INIT(vision) ();
  // Initialize and insert the vision wrapper into the module
  vision = (PyVision*)PyVision_new(v);
  if (vision == NULL)
    cerr << "** Noggin extension could not initialize PyVision object **" <<
        endl;
  vision_addToModule(reinterpret_cast<PyObject*>(vision), "inst");

  initpotentialfield();
  init_matrix();
  init_leds();
}

bool
Noggin::import_modules ()
{
#ifdef  DEBUG_NOGGIN_INITIALIZATION
  printf("  Importing util.module_helper and noggin.Brain\n");
#endif

  // Ensure module_helper is loaded
  //
  if (module_helper == NULL)
    module_helper = PyImport_ImportModule("util.module_helper");

  if (module_helper == NULL) {
    // error, couldn't import util.module_helper
    fprintf(stderr, "Error importing util.module_helper\n");
    if (PyErr_Occurred())
      PyErr_Print();
    else
      fprintf(stderr, "  No Python exception information available\n");
    return false;
  }

  // Load/Reload Brain module
  //
  if (brain_module == NULL)
    // Import brain module
    brain_module = PyImport_ImportModule("noggin.Brain");

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

void
Noggin::reload ()
{
  if (module_helper == NULL || brain_module == NULL)
    if (!import_modules())
      return;

  PyObject *result;

  // Reload all modules via module_helper
  PyObject *dict = PyModule_GetDict(module_helper);
  PyObject *reloadAll = PyDict_GetItemString(dict, "reloadAll");
  if (reloadAll == NULL || 
      (result = PyObject_CallObject(reloadAll, NULL)) == NULL) {
    fprintf(stderr, "Error reloading modules via module_helper.reloadAll()\n");
    if (PyErr_Occurred())
      PyErr_Print();
    else
      fprintf(stderr, "  No python exception information available\n");
    return;
  }
  Py_DECREF(result);

  // reload Brain module
  PyImport_ReloadModule(brain_module);
  // Instantiate a Brain instance
  getBrainInstance();
}

void
Noggin::reload(std::string modules)
{
  if (module_helper == NULL || brain_module == NULL)
    if (!import_modules())
      return;

  // Reload all modules via module_helper
  //
  // retrieve function from module
  PyObject *dict = PyModule_GetDict(module_helper);
  PyObject *_reload = PyDict_GetItemString(dict, "reload");
  // build arguments
  PyObject *args = PyTuple_New(0);
  PyObject *kwds = PyDict_New();
  PyObject *mod_arg = PyString_FromString(modules.c_str());
  PyDict_SetItemString(kwds, "modules", mod_arg);
  // call function
  PyObject *result = PyObject_Call(_reload, args, kwds);
  if (result == NULL) {
    fprintf(stderr, "Error reloading modules via module_helper.reload()\n");
    if (PyErr_Occurred())
      PyErr_Print();
    else
      fprintf(stderr, "  No python exception information available\n");
  }else
    Py_DECREF(result);
  // decrease references to release temporary objects
  Py_DECREF(args);
  Py_DECREF(kwds);
  Py_DECREF(mod_arg);

  // reload Brain module
  PyImport_ReloadModule(brain_module);
  // Instantiate a Brain instance
  getBrainInstance();
}

void
Noggin::getBrainInstance ()
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

void
Noggin::runStep ()
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
  PyVision_update(vision);
  PROF_EXIT(profiler, P_PYUPDATE);

  // Call main run() method of Brain
  PROF_ENTER(profiler, P_PYRUN);
  if (brain_instance != NULL) {
    PyObject *result = PyObject_CallMethod(brain_instance, "run", NULL);
    if (result == NULL) {
      // set Noggin in error state
      error_state = true;
      // report error
      fprintf(stderr, "Error occurred in noggin.Brain.run() method\n");
      if (PyErr_Occurred())
        PyErr_Print();
      else
        fprintf(stderr, "  No Python exception information available\n");
    }else
      Py_DECREF(result);
  }
  PROF_EXIT(profiler, P_PYRUN);

  PROF_EXIT(profiler, P_PYTHON);
}
