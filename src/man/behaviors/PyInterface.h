#pragma once

#include <Python.h>
#include <boost/shared_ptr.hpp>

#include "GameState.pb.h"

namespace man {
	namespace behaviors {


		class PyInterface
		{
		public:
			const messages::GameState* gameState_ptr;

			void setGameState_ptr(const messages::GameState* msg)
				{
					gameState_ptr = msg;
				}

		};

		void set_interface_ptr(boost::shared_ptr<PyInterface> ptr);
	}
}
