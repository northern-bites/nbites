#ifndef _ScriptedProvider_h_DEFINED
#define _ScriptedProvider_h_DEFINED

#include <vector>

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
	void enqueueSequence(std::vector<HeadJointCommand*> &seq);

private:
	Sensors *sensors;
	float FRAME_LENGTH_S;
	ChopShop chopper;
	vector<vector<float> > nextJoints;
	queue<vector<vector<float> > > choppedHeadCommand;


	ChainQueue headQueue;
	// Queue of all future commands
	queue<const HeadJointCommand*> headCommandQueue;

	void setNextHeadCommand();
	vector<float> getCurrentHeads();

	pthread_mutex_t head_mutex;
}

#endif
