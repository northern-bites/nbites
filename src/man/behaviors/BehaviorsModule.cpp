#include "BehaviorsModule.h"

static const unsigned int NUM_PYTHON_RESTARTS_MAX = 3;

namespace man {
	namespace behaviors {

		BehaviorsModule::BehaviorsModule(unsigned int player_num, unsigned int team_num)
			: portals::Module(),
			  brain_module(NULL),
			  brain_instance(NULL),
			  num_crashed(0),
			  player_number(player_num),
			  team_number(team_num),
			  in_proto(),
			  in_size(),
			  out_serial(),
			  out_proto(),
			  out_size_t(),
			  out_size(),
			  ledCommandOut(base())
		{
			// Build format string for messages.
			message_format = "(";
			for (unsigned int i=0; i<NUM_IN_MESSAGES; i++)
				message_format += "s#";
			message_format += ")";

			// Initialize python and brain.
			initializePython();
			getBrainInstance();
		}

		void BehaviorsModule::run_()
		{
			// If in error, try restarting automatically.
			// If too many consecutive failures, stop python and stop trying.
			if (num_crashed > NUM_PYTHON_RESTARTS_MAX) {
				return;
			} else if (num_crashed == NUM_PYTHON_RESTARTS_MAX) {
				Py_Finalize();
				num_crashed++;
				return;
			} else if (error_state && num_crashed < NUM_PYTHON_RESTARTS_MAX) {
				this->reload_hard();
				error_state = false;
				num_crashed++;
			}

			// Profiler
			//PROF_ENTER(P_PYTHON);

			//PROF_ENTER(P_PYTHON);

			// Call main run() method of Brain
			if (brain_instance != NULL) {
				// Serialize messages to send into python.
				serializeInMessages();

				// Calls the run method with no args.
				PyObject *result = PyObject_CallMethod(brain_instance,
													   "run",
													   message_format.c_str(),
													   in_proto[0],
													   in_size[0],
													   in_proto[1],
													   in_size[1]);
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
					// Retrieve serialized out messages.
					parseOutMessages(result);
					// Send out messages.
					portals::Message<messages::LedCommand> ledCommand(0);
					ledCommand.ParseFromArray(out_proto[0],out_size[0]);
					ledCommandOut.setMessage(ledCommand);

					Py_DECREF(result);
				}
			}
			// Profiler
			//PROF_EXIT(P_PYRUN);

			//PROF_EXIT(P_PYTHON);
		}

		void BehaviorsModule::serializeInMessages()
		{
			initialStateIn.latch();
			// Size that serialized message will be.
			in_size[0] = initialStateIn.message().ByteSize();
			// Set in_proto to be the serialized message.
			initialStateIn.message().SerializeToArray(in_proto[0],in_size[0]);
		}

		void BehaviorsModule::parseOutMessages(PyObject *tuple)
		{
			PyArg_UnpackTuple(tuple, "name", NUM_OUT_MESSAGES, NUM_OUT_MESSAGES, &out_serial[0], &out_serial[1]);
			for (int i=0; i<NUM_OUT_MESSAGES; i++) {
				PyString_AsStringAndSize(out_serial[i], &out_proto[i], out_size_t[i]);
				out_size[i] = PyLong_AsLong(PyLong_FromSsize_t(*out_size_t[i]));
			}
		}

		void BehaviorsModule::reload_hard()
		{
			printf("Reloading Behaviors Python interpreter\n");
			// finalize and reinitialize the Python interpreter
			Py_Finalize();
			// load C extension modules
			initializePython();
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

			//TODO: remove unnecessary calls here. The rest of them?
			// Initialize low-level modules
			// c_init_speed();
			// c_init_noggin_constants();
			// c_init_objects();
			// c_init_goalie();
		}

		void BehaviorsModule::modifySysPath()
		{
			//TODO: figure out if we still need this
			// Enter the current working directory into the pyton module path
#if defined OFFLINE || defined STRAIGHT
			string dir1 = NBITES_DIR"/build/tool";
			string dir2 = NBITES_DIR"/build/tool/man";
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

		bool BehaviorsModule::import_brain()
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
				if (!import_brain())
					return;

			// drop old reference
			Py_XDECREF(brain_instance);
			// grab instantiate and hold a reference to a new behaviors.Brain.Brain()
			PyObject *dict = PyModule_GetDict(brain_module);
			PyObject *brain_class = PyDict_GetItemString(dict, "Brain");
			if (brain_class != NULL) {
				// Constructs brain object with args
				PyObject *brain_constructor_args = Py_BuildValue("(ii)",player_number,team_number);
				brain_instance = PyObject_CallObject(brain_class, brain_constructor_args);
			} else {
				brain_instance = NULL;
				fprintf(stderr, "Error accessing behaviors.Brain.Brain\n");
				if (PyErr_Occurred())
					PyErr_Print();
				else
					fprintf(stderr, "  No error available\n");
			}

			error_state = (brain_instance == NULL);
		}
	}
}
