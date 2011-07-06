
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

#include "HeadJointCommand.h"
using namespace MotionConstants;
using namespace Kinematics;

HeadJointCommand::HeadJointCommand(const float time,
                                   const std::vector<float>& joints,
                                   const std::vector<float>& head_stiffness,
                                   const Kinematics::InterpolationType _type)
    : JointCommand(HEAD_JOINT, time, _type, head_stiffness),
      headJoints(joints), noJoints(0)
{
    setChainList();
}

HeadJointCommand::~HeadJointCommand() {

}
void
HeadJointCommand::setChainList() {
    chainList.insert(chainList.end(),
                     HEAD_JOINT_CHAINS,
                     HEAD_JOINT_CHAINS + HEAD_JOINT_NUM_CHAINS);
}
