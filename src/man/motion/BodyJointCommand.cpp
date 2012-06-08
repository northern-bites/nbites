
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
 const vector<float>& bodyJoints,
 const vector<float>& body_stiffness,
 const InterpolationType _type)
    : JointCommand(BODY_JOINT, time, _type, body_stiffness),
      larm_joints(0), lleg_joints(0), rleg_joints(0), rarm_joints(0)
{
    setChainList();
    // bodyJoints must contain joints for the _entire_ body
    vector<float> *joints;
    vector<float>::const_iterator firstMark, endMark;

    // Both iterators start at the beginning
    // then bump them to the end of the head_chain,
    // since BJC do not deal with the head
    firstMark = endMark = bodyJoints.begin();

    // Build joint vectors for each chain
    // from the bodyJoint vector
    list<int>::iterator chainID = chainList.begin();
    for ( ; chainID != chainList.end() ; chainID++ ) {

        joints = getJoints(static_cast<ChainID>(*chainID));

        // Position the end iterator and assign the new vector
        // then move the beginning iterator up to meet the end
        endMark += chain_lengths[*chainID];
        joints->assign(firstMark,endMark);
        firstMark = endMark;
    }
}

BodyJointCommand::BodyJointCommand(const float time,
                                   ChainID chainID,
                                   const vector<float>& joints,
                                   const vector<float>& body_stiffness,
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

BodyJointCommand::BodyJointCommand(const float time,
                                   const vector<float>& larm,
                                   const vector<float>& lleg,
                                   const vector<float>& rleg,
                                   const vector<float>& rarm,
                                   const vector<float>& body_stiffness,
                                   const InterpolationType _type)
    : JointCommand(BODY_JOINT, time, _type, body_stiffness), larm_joints(larm),
      lleg_joints(lleg), rleg_joints(rleg), rarm_joints(rarm)
{
    setChainList();
}

BodyJointCommand::~BodyJointCommand (void)
{
}

const vector<float>&
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
        cout << "SHOULD NOT ASK FOR THIS TYPE OF CHAIN FROM BODYJOINTCOMMAND"
             << endl;
        static vector<float> empty;
        return empty;
    }
}

vector<float>*
BodyJointCommand::getJoints (ChainID chainID)
{
    switch (chainID) {
    case LARM_CHAIN:
        return &larm_joints;
    case LLEG_CHAIN:
        return &lleg_joints;
    case RLEG_CHAIN:
        return &rleg_joints;
    case RARM_CHAIN:
        return &rarm_joints;
    default:
        return NULL;
    }}

const bool
BodyJointCommand::conflicts (const BodyJointCommand &other) const
{
    return ((!larm_joints.empty() && !other.getLArmJoints().empty()) ||
            (!lleg_joints.empty() && !other.getLLegJoints().empty()) ||
            (!rleg_joints.empty() && !other.getRLegJoints().empty()) ||
            (!rarm_joints.empty() && !other.getRArmJoints().empty())  );
}

const bool
BodyJointCommand::conflicts (const float chainTimeRemaining[NUM_CHAINS]) const {
    return ((!larm_joints.empty() && chainTimeRemaining[LARM_CHAIN] > 0) ||
            (!lleg_joints.empty() && chainTimeRemaining[LLEG_CHAIN] > 0) ||
            (!rleg_joints.empty() && chainTimeRemaining[RLEG_CHAIN] > 0) ||
            (!rarm_joints.empty() && chainTimeRemaining[RARM_CHAIN] > 0) );
}

void
BodyJointCommand::setChainList() {
    chainList.insert(chainList.end(),
                     BODY_JOINT_CHAINS,
                     BODY_JOINT_CHAINS + BODY_JOINT_NUM_CHAINS);
}
