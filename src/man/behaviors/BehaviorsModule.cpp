#include "BehaviorsModule.h"

static const unsigned int NUM_PYTHON_RESTARTS_MAX = 3;

namespace man {
	namespace behaviors {

		BehaviorsModule::BehaviorsModule()
			: Module(),
			  error_state(false)
		{}

		void BehaviorsModule::run_()
		{
			// If in error, try restarting automatically.
			static unsigned int num_crashed = 0;
			if (error_state && num_crashed < NUM_PYTHON_RESTARTS_MAX) {
				this->reload_hard();
				error_state = false;
				num_crashed++;
			}

			// Call main run() method of Brain
			if (brain_instance != NULL) {
				PyObject *result = PyObject_CallMethod(brain_instance, "run", NULL);
				if (result == NULL) {
					// set Behaviors in error state
					error_state = true;
					// report error
					fprintf(stderr, "Error occurred in behaviors.Brain.run() method\n");
					if (PyErr_Occurred()) {
						PyErr_Print();
					} else {
						fprintf(stderr, "  No Python exception information available\n");
					}
				} else {
					Py_DECREF(result);
				}
			}
		}

		void BehaviorsModule::reload_hard()
		{
			printf("Reloading Behaviors Python interpreter\n");
			// finalize and reinitialize the Python interpreter
			Py_Finalize();
			// load C extension modules
			initializePython();
			// import behaviors.Brain and instantiate a Brain reference
			import_modules();
			// instantiate a Brain instance
			getBrainInstance();
		}

		void BehaviorsModule::reload_brain()
		{
			if (brain_module == NULL)
				if (!import_modules())
					return;

			// reload the Brain module
			PyImport_ReloadModule(brain_module);
			// instantiate a Brain instance
			getBrainInstance();
		}

		void BehaviorsModule::initializePython()
		{
# ifdef DEBUG_BEHAVIORS_INITIALIZATION
			printf("  Initializing interpreter and extension modules\n");
# endif

			Py_Initialize();
			modifySysPath();
			brain_module = NULL;

			//TODO: remove unnecessary calls here. All of them?
			// Initialize low-level modules
			// c_init_sensors();
			// c_init_lights();
			// c_init_speed();
			// c_init_roboguardian();
			// c_init_motion();
			// c_init_comm();
			// c_init_logging();
			// c_init_vision();
			// c_init_noggin_constants();
			// c_init_objects();
			// c_init_goalie();
		}

		void BehaviorsModule::modifySysPath()
		{
			//TODO: figure out if we still need this
			// Enter the current working directory into the pyton module path
#if defined OFFLINE || defined STRAIGHT
			string dir1 = NBITES_DIR"/build/qtool";
			string dir2 = NBITES_DIR"/build/qtool/man";
			const char* cwd = "";
#else
			const char* cwd = "/home/nao/nbites/lib";
#endif

#ifdef DEBUG_BEHAVIORS_INITIALIZATION
			printf("  Adding %s to sys.path\n", cwd);
#endif

			PyObject *sys_module = PyImport_ImportModule("sys");
			if (sys_module == NULL) {
				fprintf(stderr, "** Error importing sys module: **");
				if (PyErr_Occurred())
					PyErr_Print();
				else
					fprintf(stderr, "** No Python exception information available **");
			}else {
				PyObject *dict = PyModule_GetDict(sys_module);
				PyObject *path = PyDict_GetItemString(dict, "path");
#ifdef OFFLINE
				PyList_Append(path, PyString_FromString(dir1.c_str()));
				PyList_Append(path, PyString_FromString(dir2.c_str()));
#else
				PyList_Append(path, PyString_FromString(cwd));
#endif
				Py_DECREF(sys_module);
			}
		}

		bool BehaviorsModule::import_modules()
		{
			// Load Brain module
			if (brain_module == NULL) {
				// Import brain module
#ifdef DEBUG_BEHAVIORS_INITIALIZATION
				printf("  Importing behaviors.Brain\n");
#endif
				brain_module = PyImport_ImportModule("man.behaviors.Brain");
			}

			if (brain_module == NULL) {
				// error, couldn't import behaviors.Brain
				fprintf(stderr, "Error importing behaviors.Brain module\n");
				if (PyErr_Occurred())
					PyErr_Print();
				else
					fprintf(stderr, "  No Python exception information available\n");
				return false;
			}

			return true;
		}

		void BehaviorsModule::getBrainInstance()
		{
			if (brain_module == NULL)
				if (!import_modules())
					return;

			// drop old reference
			Py_XDECREF(brain_instance);
			// grab instantiate and hold a reference to a new behaviors.Brain.Brain()
			PyObject *dict = PyModule_GetDict(brain_module);
			PyObject *brain_class = PyDict_GetItemString(dict, "Brain");
			if (brain_class != NULL)
				brain_instance = PyObject_CallObject(brain_class, NULL);
			else
				brain_instance = NULL;

			if (brain_instance == NULL) {
				fprintf(stderr, "Error accessing behaviors.Brain.Brain\n");
				if (PyErr_Occurred())
					PyErr_Print();
				else
					fprintf(stderr, "  No error available\n");
			}

			// Successfully reloaded
			error_state = (brain_instance == NULL);
		}
	}
}
