
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
 * Provides a wrapper around a ChoppedCommand that generates future
 * joint angles and CoM positions, to allow for dynamic stabilization.
 * This extra computation is fine because the motion engine isn't running
 * when we're executing SweetMoves. ScriptedProvider is the only provider
 * that uses this class, so far.
 *
 * @author Nathan Merritt
 * @date June 2011
 */

#pragma once
#ifndef PREVIEW_CHOPPED_H
#define PREVIEW_CHOPPED_H

#include "COMKinematics.h"
#include "ChoppedCommand.h"
#include "NBMath.h"
#include "dsp.h" // for FifoBuffer

using NBMath::ufvector3;

class PreviewChoppedCommand : public ChoppedCommand {
public:
    PreviewChoppedCommand( ChoppedCommand::ptr choppedCommand );
    ~PreviewChoppedCommand();

    virtual std::vector<float> getNextJoints(int id);
    virtual const std::vector<float> getStiffness(Kinematics::ChainID id) const;
    virtual bool isDone() const;

    const ufvector3 getFutureComPosition();
    const ufvector3 getComDerivative();

private:
    void bufferNextAngles(int chainID);
    void updateComEstimates();

    typedef FifoBuffer<std::vector<float> > VectorFifo;
    std::vector<VectorFifo> jointAngles;

    std::vector<float> thisFramesAngles;

    Boxcar com_x, com_y, com_dx, com_dy;
    ChoppedCommand::ptr alreadyChoppedCommand;
};


#endif
