#pragma once

#include "RoboGrams.h"
#include <Python.h>
//#include <boost/shared_ptr.hpp>
//#include "Profiler.h"

#include "LedCommand.pb.h"
#include "GameState.pb.h"
#include "WorldModel.pb.h"
#include "BallModel.pb.h"

namespace man {
	namespace behaviors {

		class BehaviorsModule : public portals::Module {

		public:
			BehaviorsModule();//unsigned int player_num, unsigned int team_num);
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
			bool import_brain();
			// Instantiate a Brain instance
			void getBrainInstance();

			bool error_state;
			PyObject *brain_module;
			PyObject *brain_instance;
			unsigned int num_crashed;
			unsigned int player_number;
			unsigned int team_number;

			// Latch protobufs and serialize them
			void serializeInMessages();
			// Parse python protobufs
			void parseOutMessages(PyObject *tuple);

			// message variables
			static const unsigned int NUM_IN_MESSAGES = 3;
			static const unsigned int NUM_OUT_MESSAGES = 2;
			std::string message_format;
			// initialize arrays to 0, null, etc
			char *in_proto [NUM_IN_MESSAGES];
			unsigned int in_size [NUM_IN_MESSAGES];
			PyObject *out_serial [NUM_OUT_MESSAGES];
			char *out_proto [NUM_OUT_MESSAGES];
			Py_ssize_t *out_size_t [NUM_OUT_MESSAGES];
			long out_size [NUM_OUT_MESSAGES];

		public:
			// portals
			// note: order of portals matches message array indicies
			portals::InPortal<messages::GameState> gameStateIn;
			portals::InPortal<messages::WorldModel> worldModelIn;
			portals::InPortal<messages::FilteredBall> filteredBallIn;
			portals::OutPortal<messages::LedCommand> ledCommandOut;
		};
	}
}
