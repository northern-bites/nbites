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
 * This class moves an arm to automatically balance based on a CoM preview
 * calculated somewhere else.
 *
 * @author Nathan Merritt
 * @date June 2011
 * @see WalkingArm, PreviewChoppedCommand
 */

#pragma once
#ifndef BALANCING_ARM_H
#define BALANCING_ARM_H

#include <boost/shared_ptr.hpp>

#include "NBMath.h"
#include "NBMatrixMath.h"

#include "Kinematics.h"
#include "COMPreview.h"

class BalancingArm {
public:
    typedef boost::shared_ptr<BalancingArm> ptr;

    enum ArmJoint {
	SHOULDER_PITCH = 0,
	SHOULDER_ROLL,
	ELBOW_YAW,
	ELBOW_ROLL,
    };

    BalancingArm( Kinematics::ChainID id, COMPreview::ptr com );
    ~BalancingArm();

    std::vector<float> getNextJoints();

private:
    float getJoint(int id);

    float shoulderPitch();
    float shoulderRoll();
    float elbowYaw();
    float elbowRoll();

    bool leftArm;
    Kinematics::ChainID chain;
    COMPreview::ptr comPreview;
};


#endif
