#ifndef _ScriptedProvider_h_DEFINED
#define _ScriptedProvider_h_DEFINED

#include <vector>
using namespace std;

#include "MotionProvider.h"
#include "ChainQueue.h"


class ScriptedProvider : public MotionProvider {
public:
    ScriptedProvider(float motionFrameLength);
    virtual ~ScriptedProvider();

    void requestStop();
    void calculateNextJoints();


	void enqueue(const BodyJointCommand *command);
	void enqueueSequence(std::vector<BodyJointCommand*> &seq);

private:
	float FRAME_LENGTH_S;
};

#endif

