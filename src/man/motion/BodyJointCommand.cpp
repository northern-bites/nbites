
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

#include "BodyJointCommand.h"
using namespace std;
using namespace MotionConstants;
using namespace Kinematics;

BodyJointCommand::BodyJointCommand
(const float time,
 const vector<float> *bodyJoints,
 const vector<float> *body_stiffness,
 const InterpolationType _type)
    : JointCommand(BODY_JOINT, time, _type, body_stiffness),
      larm_joints(0), lleg_joints(0), rleg_joints(0), rarm_joints(0)
{
    setChainList();
    // bodyJoints must contain joints for the _entire_ body
    // @JGM Throw Exception when wrong?

    vector<float> *joints;
    vector<float>::const_iterator firstMark, endMark;

    // Both iterators start at the beginning
    // then bump them to the end of the head_chain,
    // since BJC do not deal with the head
    firstMark = endMark = bodyJoints->begin();

    // Build joint vectors for each chain
    // from the bodyJoint vector
    for (unsigned int chainID = LARM_CHAIN ; chainID<NUM_CHAINS ; chainID++ ) {
        joints = new vector<float>(chain_lengths[chainID]);

        // Position the end iterator and assign the new vector
        // then move the beginning iterator up to meet the end
        endMark += chain_lengths[chainID];
        joints->assign(firstMark,endMark);
        firstMark = endMark;

        switch (chainID) {
        case LARM_CHAIN:
            larm_joints = joints;
            break;
        case LLEG_CHAIN:
            lleg_joints = joints;
            break;
        case RLEG_CHAIN:
            rleg_joints = joints;
            break;
        case RARM_CHAIN:
            rarm_joints = joints;
            break;

        }
    }
    delete bodyJoints;
}

BodyJointCommand::BodyJointCommand(const float time,
                                   ChainID chainID,
                                   const vector<float> *joints,
                                   const vector<float> *body_stiffness,
                                   const InterpolationType _type)
    : JointCommand(BODY_JOINT, time, _type, body_stiffness), larm_joints(0),
      lleg_joints(0), rleg_joints(0), rarm_joints(0)
{
    setChainList();
    switch (chainID) {
    case LARM_CHAIN:
        larm_joints = joints;
        break;
    case LLEG_CHAIN:
        lleg_joints = joints;
        break;
    case RLEG_CHAIN:
        rleg_joints = joints;
        break;
    case RARM_CHAIN:
        rarm_joints = joints;
        break;
    default:
        assert(false);
    }
}

BodyJointCommand::BodyJointCommand(const float time, const vector<float> *larm,
                                   const vector<float> *lleg,
                                   const vector<float> *rleg,
                                   const vector<float> *rarm,
                                   const vector<float> *body_stiffness,
                                   const InterpolationType _type)
    : JointCommand(BODY_JOINT, time, _type, body_stiffness), larm_joints(larm),
      lleg_joints(lleg), rleg_joints(rleg), rarm_joints(rarm)
{
    setChainList();
}

BodyJointCommand::BodyJointCommand(const BodyJointCommand &other)
    : JointCommand( BODY_JOINT,
                    other.getDuration(),
                    other.getInterpolation(),
                    other.getStiffness() ),
      larm_joints(0), lleg_joints(0), rleg_joints(0), rarm_joints(0)
{
    setChainList();
    if (other.larm_joints)
        larm_joints = new vector<float>(*other.larm_joints);
    if (other.lleg_joints)
        lleg_joints = new vector<float>(*other.lleg_joints);
    if (other.rleg_joints)
        rleg_joints = new vector<float>(*other.rleg_joints);
    if (other.rarm_joints)
        rarm_joints = new vector<float>(*other.rarm_joints);
}

BodyJointCommand::~BodyJointCommand (void)
{
    delete larm_joints;
    delete lleg_joints;
    delete rleg_joints;
    delete rarm_joints;
}

const vector<float>*
BodyJointCommand::getJoints (ChainID chainID) const
{
    switch (chainID) {
    case LARM_CHAIN:
        return larm_joints;
    case LLEG_CHAIN:
        return lleg_joints;
    case RLEG_CHAIN:
        return rleg_joints;
    case RARM_CHAIN:
        return rarm_joints;
    default:
        // bjc do not have other chains
        return 0; 			// HACK.
    }
}

const bool
BodyJointCommand::conflicts (const BodyJointCommand &other) const
{
    if (larm_joints && other.getLArmJoints())
        return true;

    if (lleg_joints && other.getLLegJoints())
        return true;

    if (rleg_joints && other.getRLegJoints())
        return true;

    if (rarm_joints && other.getRArmJoints())
        return true;

    return false;
}

const bool
BodyJointCommand::conflicts (const float chainTimeRemaining[NUM_CHAINS]) const {
    if (larm_joints && chainTimeRemaining[LARM_CHAIN] > 0)
        return true;

    if (lleg_joints && chainTimeRemaining[LLEG_CHAIN] > 0)
        return true;

    if (rleg_joints && chainTimeRemaining[RLEG_CHAIN] > 0)
        return true;

    if (rarm_joints && chainTimeRemaining[RARM_CHAIN] > 0)
        return true;

    return false;
}

void
BodyJointCommand::setChainList() {
    chainList.insert(chainList.end(),
                     BODY_JOINT_CHAINS,
                     BODY_JOINT_CHAINS + BODY_JOINT_NUM_CHAINS);
}
