 
// This file is part of Man, a robotic perception, locomotion, and
// team strategy application created by the Northern Bites RoboCup
// team of Bowdoin College in Brunswick, Maine, for the Aldebaran
// Nao robot.
//
// Man is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Man is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser Public License for more details.
//
// You should have received a copy of the GNU General Public License
// and the GNU Lesser Public License along with Man.  If not, see
// <http://www.gnu.org/licenses/>.



/**************************************************************
// newVision.cpp -- Soon to be the new Vision Module Class file

// This file does nothing as yet. When completed, this file will
//   be responsible for the following:
  
//   - Initializing and managing the vision blackboard
//   - Initializing all detectors and their tools
//   - Managing inputs from ImageAcquisition and Sensor's portal
//   - Managing outputs to appropriate portals
//   - NOT BEING CONFUSING
***************************************************************/

#include "newVision.h"

#include <boost/shared_ptr.hpp>

using namespace std;

namespace man {
namespace newVis {

newVision::newVision() {

  memory = new Blackboard();
  rle_machine = new Encoder(memory);

}


void newVision::visionLoop() {

  if (memory->isEmpty()) {
    cout << "There is no image in vision memory" << endl;
    return;
  }

  cout << "Good times\n";

  memory->setTopCamera(true); //begin by processing the top camera data
  
  rle_machine->make_runs(); 
    

}

}
}
