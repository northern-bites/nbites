#ifndef _ChopShop_h_DEFINED
#define _ChopShop_h_DEFINED

#include <vector>
#include <queue>

#include "Sensors.h"
#include "BodyJointCommand.h"
#include "Kinematics.h"
#include "JointCommand.h"

using namespace std;
using namespace Kinematics;

class ChopShop
{
public:
 	ChopShop(Sensors *s, float motionFrameLength);

	queue<vector<vector<float> > >* chopCommand(const JointCommand *command);
private:
	// Inside most vector: joint values for a chain
	// Next: vector for each choppped move (holding each chain,
	// instead of just holding the joint values in a row)
	// Outside: vector to hold all the chopped moves
	Sensors* sensors;
	float FRAME_LENGTH_S;

	queue<vector<vector<float> > >* chopSmooth(const JointCommand *command);
	queue<vector<vector<float> > >* chopLinear(const JointCommand *command);

	vector<float> getCurrentJoints();
	vector<float> getFinalJoints(const JointCommand *command,
								 vector<float> *currentJoints);
	vector<float> getDiffPerChop(int numChops,
								 vector<float> *current,
								 vector<float> *final);
	void vectorToRad(vector<float> *vect);

	queue<vector<vector<float> > >* buildChops(float numChops,
					vector<float> *currentJoints,
					vector<float> *diffPerChop,
					const JointCommand *command);
};

#endif
