#pragma once

#include <Python.h>
#include <boost/shared_ptr.hpp>
//#include "Profiler.h"
//#include "PyConstants.h"

#include "LedCommand.ph.h"

namespace man {
	namespace noggin {

		class NogginModule : public portals::Module {

		public:
			NogginModule();
			virtual ~NogginModule() {}

			// reinitialize and reload the Python interpreter
			void reload_hard();
			// reload Brain module
			void reload_brain();
			// current Noggin error status
			bool inErrorState() { return error_state; }

			//placeholder in portal
			portals::InPortal<messages::Ball> ballMessageIn;

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
		};
	}
}
