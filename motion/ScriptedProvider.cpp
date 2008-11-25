#include "ScriptedProvider.h"
#include <vector>
#include "Kinematics.h"
#include "ChainQueue.h"


using namespace Kinematics;


ScriptedProvider::ScriptedProvider(float motionFrameLength,
								   Sensors *s)
    : MotionProvider(),FRAME_LENGTH_S(motionFrameLength),
	  chopper(s),nextJoints()//Create the queues!
{


    // Create mutexes
    // (?) Need one mutex per queue (?)
    pthread_mutex_init (&scripted_mutex, NULL);
}

ScriptedProvider::~ScriptedProvider() {
    // remove all remaining values from chain queues
}

void ScriptedProvider::requestStop() {
    // Finish motion or stop immediately?

}

void ScriptedProvider::calculateNextJoints() {
    // Sets the next joints for all the joints

}

void ScriptedProvider::enqueue(const BodyJointCommand *command) {
	// Make new BodyJointCommand for each chain, from *command
	// Add to each chain
	


    }

}

void ScriptedProvider::enqueueSequence(std::vector<BodyJointCommand*> &seq) {
    // Take in vec of commands and enqueue them all
    pthread_mutex_lock(&scripted_mutex);
    for (vector<BodyJointCommand*>::iterator i= seq.begin(); i != seq.end(); i++)
	enqueue(*i);
    pthread_mutex_unlock(&scripted_mutex);

}
