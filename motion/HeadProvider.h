#ifndef _HeadProvider_h_DEFINED
#define _HeadProvider_h_DEFINED

#include <vector>
#include <queue>

#include "MotionProvider.h"
#include "ChainQueue.h"
#include "HeadJointCommand.h"
#include "Sensors.h"
#include "ChopShop.h"
#include "Kinematics.h"

using namespace std;
using namespace Kinematics;

class HeadProvider : public MotionProvider {
public:
	HeadProvider(float motionFrameLength,
				 Sensors *s);
	virtual ~HeadProvider();

	void requestStop();
	void calculateNextJoints();

	void enqueue(const HeadJointCommand *command);
	void enqueueSequence(vector<const HeadJointCommand*> &seq);

private:
    void setActive();
    bool isDone();

	Sensors *sensors;
	float FRAME_LENGTH_S;
	ChopShop chopper;
	vector<vector<float> > nextJoints;

	ChainQueue headQueue;
	// Queue of all future commands
	queue<const HeadJointCommand*> headCommandQueue;

	pthread_mutex_t head_mutex;

	vector<float> getCurrentHeads();
	void setNextHeadCommand();
};

#endif
