 
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
// newVision.h -- Soon to be the new Vision Module Class file header

// This file does nothing as yet. When completed, this file will
//   be responsible for the following:
  
//   - Initializing and managing the vision blackboard
//   - Initializing all detectors and their tools
//   - Managing inputs from ImageAcquisition and Sensor's portal
//   - Managing outputs to appropriate portals
//   - NOT BEING CONFUSING
***************************************************************/
#ifndef NEW_VIS_H
#define NEW_VIS_H

#include "VisionDef.h"
#include "Blackboard.h"
#include "RLE/Encoder.h"

#include <iostream>
#include <stdlib.h>

namespace man {
namespace newVis {

class newVision {

 public:
  newVision();
  ~newVision();


  void visionLoop(); //this will be the "main" function of vision

 public:
  Blackboard *memory;
  Encoder *rle_machine;

};
}
}

#endif
