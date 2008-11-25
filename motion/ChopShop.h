#include "Sensors.h"
#include <vector>
#include "BodyJointCommand.h"

using namespace std;

class ChopShop {
public:
	ChopShop(Sensors *sensors);
	virtual ~ChopShop();

	vector<vector<float> > chopCommand(BodyJointCommand *command);

private:
	void chopSmooth(BodyJointCommand *command);
	void chopLinear(BodyJointCommand *command);
	vector<float> finalJoints;
	vector<float> diffPerChop;
	

	// Inside most vector: joint values for a chain
	// Next: vector for each choppped move (holding each chain,
	// instead of just holding the joint values in a row)
	// Outside: vector to hold all the chopped moves
	vector<vector<vector<float> > > choppedJoints;

}
