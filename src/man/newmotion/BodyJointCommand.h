
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

#ifndef _BodyJointCommand_h_DEFINED
#define _BodyJointCommand_h_DEFINED

/**
 * BodyBodyJointCommand is a class that holds a single command for a subset of
 * all body joints (excluding the head). All joints and time must be set during
 * instantiation and can't be changed after that. No setters in here.
 */

#include <vector>

#include <boost/shared_ptr.hpp>

#include "Kinematics.h"
#include "JointCommand.h"
#include "MotionConstants.h"

namespace man
{
namespace motion
{
class BodyJointCommand : public JointCommand {
public:
    typedef boost::shared_ptr<BodyJointCommand> ptr;

    BodyJointCommand(const float time,
                     const std::vector<float>& body_joints,
                     const std::vector<float>& body_stiffness,
                     const Kinematics::InterpolationType _type);

    BodyJointCommand(const float time,
                     Kinematics::ChainID chainId,
                     const std::vector<float>& joints,
                     const std::vector<float>& body_stiffness,
                     const Kinematics::InterpolationType _type);
    BodyJointCommand(const float time,
                     const std::vector<float>& larm,
                     const std::vector<float>& lleg,
                     const std::vector<float>& rleg,
                     const std::vector<float>& rarm,
                     const std::vector<float>& body_stiffness,
                     const Kinematics::InterpolationType _type);

    virtual ~BodyJointCommand(void);

    virtual const std::vector<float>&
    getJoints(Kinematics::ChainID chainID) const;

    const std::vector<float>& getLArmJoints(void) const { return larm_joints; }
    const std::vector<float>& getLLegJoints(void) const { return lleg_joints; }
    const std::vector<float>& getRLegJoints(void) const { return rleg_joints; }
    const std::vector<float>& getRArmJoints(void) const { return rarm_joints; }

    const bool conflicts(const BodyJointCommand &other) const;

    const bool
    conflicts(const float chainTimeRemaining[Kinematics::NUM_CHAINS]) const;

private:
    BodyJointCommand(const BodyJointCommand &other);
    virtual void setChainList();

    std::vector<float>*
    getJoints(Kinematics::ChainID chainID);

    std::vector<float> larm_joints;
    std::vector<float> lleg_joints;
    std::vector<float> rleg_joints;
    std::vector<float> rarm_joints;
};

}
}
#endif
