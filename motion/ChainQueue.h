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
	ChainQueue(ChainID newChainID, float motionFrameLength);
	void add(BodyJointCommand *command);
	int getChainID();
	void clear();


private:
	ChainID chainID;
	void chopCommand(BodyJointCommand *command);
	void chopLinear(BodyJointCommand *command);
	void chopSmooth(BodyJointCommand *command);
	// Motion frame length in SECONDS
	float FRAME_LENGTH_S;


};

#endif
