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
	virtual ~ChopShop();

	vector<vector<vector<float> > > chopCommand(const BodyJointCommand *command);

private:
	vector<vector<vector<float> > > chopSmooth(const BodyJointCommand *command);
	vector<vector<vector<float> > > chopLinear(const BodyJointCommand *command);

	// Class objects
	vector<float> finalJoints;
	vector<float> diffPerChop;

	// Inside most vector: joint values for a chain
	// Next: vector for each choppped move (holding each chain,
	// instead of just holding the joint values in a row)
	// Outside: vector to hold all the chopped moves
	vector<vector<vector<float> > > choppedJoints;
	Sensors* sensors;
	float FRAME_LENGTH_S;



};
