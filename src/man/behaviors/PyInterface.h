#pragma once

#include <Python.h>
#include <boost/shared_ptr.hpp>

#include "GameState.pb.h"
#include "BallModel.pb.h"
#include "LedCommand.pb.h"

namespace man {
	namespace behaviors {


		class PyInterface
		{
		public:
			const messages::GameState* gameState_ptr;
			const messages::FilteredBall* filteredBall_ptr;
			const messages::LedCommand* ledCommand_ptr;

			void setGameState_ptr(const messages::GameState* msg)
				{
					gameState_ptr = msg;
				}
			void setFilteredBall_ptr(const messages::FilteredBall* msg)
				{
					filteredBall_ptr = msg;
				}
			void setLedCommand_ptr(const messages::LedCommand* msg)
				{
					ledCommand_ptr = msg;
				}

		};

		void set_interface_ptr(boost::shared_ptr<PyInterface> ptr);
	}
}
