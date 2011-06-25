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

/**
 * This class extends PreviewChoppedCommand to control two BalancingArms
 * which use the CoM preview calculations. All this should be transparent
 * to ScriptedProvider
 *
 * @author Nathan Merritt
 * @date June 2011
 */

#pragma once
#ifndef BALANCING_CHOPPED_COMMAND
#define BALANCING_CHOPPED_COMMAND

#include "PreviewChoppedCommand.h"
#include "BalancingArm.h"

class BalancingChoppedCommand : public PreviewChoppedCommand {
public:
    BalancingChoppedCommand( ChoppedCommand::ptr choppedCommand );
    ~BalancingChoppedCommand();

    virtual std::vector<float> getNextJoints(int id);
    virtual const std::vector<float> getStiffness(Kinematics::ChainID id) const;

private:
    BalancingArm::ptr leftArm, rightArm;

};

#endif
