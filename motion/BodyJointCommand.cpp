
#include <vector>

#include "BodyJointCommand.h"

using namespace std;

BodyJointCommand::BodyJointCommand
(const float time,
 const vector<float> *bodyJoints,
 const Kinematics::InterpolationType _type)
 : duration(time), type(_type)
{

	unsigned int i = 0, j;
	vector<float> *joints;
	vector<float>::const_iterator ite;
	vector<float>::const_iterator iteEnd;
	cout <<"here" << endl;

	joints = new vector<float>(chain_lengths[LARM_CHAIN]);

	// Both iterators start at the beginning
	ite = bodyJoints->begin();
	iteEnd = bodyJoints->begin();

	// Put the beginning limit at the end of the start of
	// the LARM_CHAIN joints
	ite += chain_lengths[HEAD_CHAIN];

	
	// Put the end limit at the end of the LARM_CHAIN joints,
	// aka at the beginning plus its length
	iteEnd = ite;
	iteEnd += chain_lengths[LARM_CHAIN];

	// Copy the values out of the bodyJoints
	// into joints for the LARM_CHAIN
	joints->assign(ite,iteEnd);
	larm_joints = joints;

	// Move the limiters to LLEG_CHAIN
	// asign lleg_joints
	joints = new vector<float>(chain_lengths[LLEG_CHAIN]);
	ite += chain_lengths[LARM_CHAIN];
	iteEnd += chain_lengths[LLEG_CHAIN];
	joints->assign(ite,iteEnd);
	lleg_joints = joints;

	// Repeat above for RLEG_CHAIN and RARM_CHAIN
	joints = new vector<float>(chain_lengths[RLEG_CHAIN]);
	ite += chain_lengths[LLEG_CHAIN];
	iteEnd += chain_lengths[RLEG_CHAIN];
	joints->assign(ite,iteEnd);
	rleg_joints = joints;

	joints = new vector<float>(chain_lengths[RARM_CHAIN]);
	ite += chain_lengths[RLEG_CHAIN];
	iteEnd += chain_lengths[RARM_CHAIN];
	joints->assign(ite,iteEnd);
	rarm_joints = joints;

	delete bodyJoints;
}

BodyJointCommand::BodyJointCommand(const float time, ChainID chainID,
				   const vector<float> *joints,
				   const Kinematics::InterpolationType _type)
  : duration(time), type(_type), larm_joints(0),
    lleg_joints(0), rleg_joints(0), rarm_joints(0)
{
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
				   const Kinematics::InterpolationType _type)
  : duration(time), type(_type), larm_joints(larm),
    lleg_joints(lleg), rleg_joints(rleg), rarm_joints(rarm) 
{
}

BodyJointCommand::BodyJointCommand(const BodyJointCommand &other) 
  : duration(other.duration), type(other.type),
    larm_joints(0), lleg_joints(0), rleg_joints(0), rarm_joints(0)
 {

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
  if (larm_joints)
    delete larm_joints;
  if (lleg_joints)
    delete lleg_joints;
  if (rleg_joints)
    delete rleg_joints;
  if (rarm_joints)
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
    // we don't have head joints
    return 0;
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
