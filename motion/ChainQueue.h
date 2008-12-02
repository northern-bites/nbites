#ifndef _ChainQueue_h_DEFINED
#define _ChainQueue_h_DEFINED

#include "Sensors.h"
#include "BodyJointCommand.h"
#include "HeadJointCommand.h"
#include "HeadScanCommand.h"
#include "WalkCommand.h"
#include "Kinematics.h"

#include <queue>
#include <vector>
using namespace std;
using namespace Kinematics;

class ChainQueue : public queue<vector<float> > {
public:
	ChainQueue(ChainID newChainID);
	void add(vector<vector<float> > nextJoints);
	ChainID getChainID() {return chainID;};
	void clear();


private:
	ChainID chainID;

};

#endif
