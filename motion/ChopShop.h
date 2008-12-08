#ifndef _ChopShop_h_DEFINED
#define _ChopShop_h_DEFINED

#include <vector>

#include "Sensors.h"
#include "BodyJointCommand.h"
#include "Kinematics.h"

using namespace std;
using namespace Kinematics;

class ChopShop
{
public:
	ChopShop(Sensors *s, float motionFrameLength);

	queue<vector<vector<float> > > chopCommand(const BodyJointCommand *command);

private:
	// Class objects
	vector<float> finalJoints;
	vector<float> diffPerChop;

	// Inside most vector: joint values for a chain
	// Next: vector for each choppped move (holding each chain,
	// instead of just holding the joint values in a row)
	// Outside: vector to hold all the chopped moves


	Sensors* sensors;
	float FRAME_LENGTH_S;
	queue<vector<vector<float> > > choppedJoints;
	const vector<float> *nextChain;

	queue<vector<vector<float> > > chopSmooth(const BodyJointCommand *command);
	queue<vector<vector<float> > > chopLinear(const BodyJointCommand *command);




};

#endif
