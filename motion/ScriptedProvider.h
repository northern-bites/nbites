#ifndef _ScriptedProvider_h_DEFINED
#define _ScriptedProvider_h_DEFINED

#include <vector>
#include <queue>

#include "MotionProvider.h"
#include "ChainQueue.h"
#include "BodyJointCommand.h"
#include "Sensors.h"
#include "ChopShop.h"
#include "Kinematics.h"

using namespace std;
using namespace Kinematics;

class ScriptedProvider : public MotionProvider {
public:
    ScriptedProvider(float motionFrameLength,
					 Sensors *s);
    virtual ~ScriptedProvider();

    void requestStop();
    void calculateNextJoints();

	void enqueue(const BodyJointCommand *command);
	void enqueueSequence(std::vector<const BodyJointCommand*> &seq);

private:
	Sensors *sensors;
	float FRAME_LENGTH_S;
	ChopShop chopper;
	vector<vector<float> > nextJoints;

	// ChainQueues
	vector<ChainQueue> chainQueues;
	queue<const BodyJointCommand*> bodyCommandQueue;

	pthread_mutex_t scripted_mutex;

	vector <vector <float> > getCurrentChains();
	void setNextBodyCommand();
    void setActive();
	bool isDone();
};

#endif
