#include "NogginModule.h"

using boost::shared_ptr;

namespace man {
	namespace noggin {

		NogginModule::NogginModule()
			: Module(),
			  error_state(false);
		{}

		void NogginModule::run_()
		{
			//latch in portals
		}

		void NogginModule::reload_hard()
		{
			printf("Reloading Noggin Python interpreter\n");
			// finalize and reinitialize the Python interpreter
			Py_Finalize();
			// load C extension modules
			initializePython();
			// import noggin.Brain and instantiate a Brain reference
			import_modules();
			// instantiate a Brain instance
			getBrainInstance();
		}

		void NogginModule::reload_brain()
		{
			if (brain_module == NULL)
				if (!import_modules())
					return;

			// reload the Brain module
			PyImport_ReloadModule(brain_module);
			// instantiate a Brain instance
			getBrainInstance();
		}

		void NogginModule::initializePython()
		{
# ifdef DEBUG_NOGGIN_INITIALIZATION
			printf("  Initializing interpreter and extension modules\n");
# endif

			Py_Initialize();
			modifySysPath();
			brain_module = NULL;

			//TODO: remove unnecessary calls here
			// Initialize low-level modules
			c_init_sensors();
			c_init_lights();
			c_init_speed();
			c_init_roboguardian();
			c_init_motion();
			c_init_comm();
			c_init_logging();
			c_init_vision();
			c_init_noggin_constants();
			c_init_objects();
			c_init_goalie();
		}

		void NogginModule::modifySysPath()
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

#ifdef DEBUG_NOGGIN_INITIALIZATION
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

		bool NogginModule::import_modules()
		{
			// Load Brain module
			if (brain_module == NULL) {
				// Import brain module
#ifdef DEBUG_NOGGIN_INITIALIZATION
				printf("  Importing noggin.Brain\n");
#endif
				brain_module = PyImport_ImportModule(BRAIN_MODULE);
			}

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

		void NogginModule::getBrainInstance()
		{
			if (brain_module == NULL)
				if (!import_modules())
					return;

			// drop old reference
			Py_XDECREF(brain_instance);
			// grab instantiate and hold a reference to a new noggin.Brain.Brain()
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
					fprintf(strderr, "  No error available\n");
			}

			// Successfully reloaded
			error_state = (brain_instance == NULL);
		}
	}
}
