
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

#ifndef _StiffnessCommand_h_DEFINED
#define _StiffnessCommand_h_DEFINED

#include "MotionConstants.h"
#include "MotionCommand.h"

class StiffnessCommand : public MotionCommand
{
public:
    //Default constructor is private and just inits the vector* to NULL
    virtual ~StiffnessCommand();
    StiffnessCommand();
    StiffnessCommand(const float stiffness);
    StiffnessCommand(const Kinematics::ChainID chainID,
                     const float stiffness);

    void setChainStiffness(const Kinematics::ChainID chainID,
                           const float chainStiffness);

    const  std::vector<float>*
    getChainStiffness(const Kinematics::ChainID chainID) const;
    const  float
    getJointStiffness(const Kinematics::JointNames joint) const;
    const std::vector<float>*
    getHeadStiffness(void) const { return stiffnesses[Kinematics::HEAD_CHAIN]; }
    const std::vector<float>*
    getLArmStiffness(void) const { return stiffnesses[Kinematics::LARM_CHAIN]; }
    const std::vector<float>*
    getLLegStiffness(void) const { return stiffnesses[Kinematics::LLEG_CHAIN]; }
    const std::vector<float>*
    getRLegStiffness(void) const { return stiffnesses[Kinematics::RLEG_CHAIN]; }
    const std::vector<float>*
    getRArmStiffness(void) const { return stiffnesses[Kinematics::RARM_CHAIN]; }

public:
    static const float NOT_SET;
    static const float DECOUPLED;

private:
    virtual void setChainList() {
        chainList.insert(chainList.end(),
                         MotionConstants::STIFFNESS_CHAINS,
                         MotionConstants::STIFFNESS_CHAINS
                         + MotionConstants::STIFFNESS_NUM_CHAINS);
    }

    void init(){
        setChainList();
        for(int i = Kinematics::HEAD_CHAIN; i <= Kinematics::RARM_CHAIN; i++){
            stiffnesses[i] = NULL;
        }
    }

private:
    const  std::vector<float> * stiffnesses[Kinematics::NUM_CHAINS];

};

#endif
