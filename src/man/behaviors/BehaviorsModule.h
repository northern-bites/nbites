#pragma once

#include "RoboGrams.h"
#include <Python.h>
//#include <boost/shared_ptr.hpp>
//#include "Profiler.h"
//#include "PyConstants.h"

#include "LedCommand.pb.h"

namespace man {
	namespace behaviors {

		class BehaviorsModule : public portals::Module {

		public:
			BehaviorsModule();
			virtual ~BehaviorsModule() {}

			// reinitialize and reload the Python interpreter
			void reload_hard();
			// current Behaviors error status
			bool inErrorState() { return error_state; }

			//placeholder in portal
			//portals::InPortal<essages::Ball> ballMessageIn;

		protected:
			virtual void run_();

		private:
			// Initialize the interpreter and C Python extensions
			void initializePython();
			// Insert the man directory in the system path
			void modifySysPath();
			// Import the util.module_helper and noggin.Brain modules
			// Note: these are not "real" modules like Motion or Sensors
			bool import_modules();
			// Instantiate a Brain instance
			void getBrainInstance();

			bool error_state;
			PyObject *module_helper;//how is this used currently?
			PyObject *brain_module;
			PyObject *brain_instance;
			unsigned int num_crashed;
		};
	}
}
