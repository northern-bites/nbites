#include "MotionSwitchboard.h"
MotionSwitchboard::MotionSwitchboard(Sensors *s)
    : sensors(s),
      walkProvider(),
	  scriptedProvider(1/50.,sensors), // HOW SHOULD WE PASS FRAME_LENGTH??? FILE?
      nextJoints(Kinematics::NUM_JOINTS, 0.0),
	  running(false)
{

    //Allow safe access to the next joints
    pthread_mutex_init(&next_joints_mutex, NULL);
    pthread_cond_init(&calc_new_joints_cond,NULL);

	bodyJoints = new vector<float>(20,90.0f);
	bodyJoints2 = new vector<float>(20,-90.0f);
	command = new BodyJointCommand(100.0f,
								   bodyJoints,
							   Kinematics::INTERPOLATION_LINEAR);

	command2 = new BodyJointCommand(100.0f,
								   bodyJoints2,
							   Kinematics::INTERPOLATION_LINEAR);

}

MotionSwitchboard::~MotionSwitchboard() {
    pthread_mutex_destroy(&next_joints_mutex);
}


void MotionSwitchboard::start() {

#ifdef DEBUG_INITIALIZATION
    cout << "Switchboard::initializing" << endl;
    cout << "  creating threads" << endl;
#endif
    fflush(stdout);

	scriptedProvider.enqueue(command);
	cout << "enqueued 1" << endl;
	scriptedProvider.enqueue(command2);
	cout << "enqueued 2" << endl;
	delete bodyJoints;
	delete command;
//	delete bodyJoints2;
//	delete command2;


    running = true;

    // set thread attributes
    pthread_attr_t attr;
    pthread_attr_init (&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    // create & start thread.
    pthread_create(&switchboard_thread, &attr, runThread, (void *)this);
    // destroy the used attributes
    pthread_attr_destroy(&attr);
}


void MotionSwitchboard::stop() {
    running = false;
    //signal to end waiting in the run method,
    pthread_mutex_lock(&next_joints_mutex);
    pthread_cond_signal(&calc_new_joints_cond);
    pthread_mutex_unlock(&next_joints_mutex);
}


/**
 * The switchboard run method is continuously looping. At each iteration
 * it grabs the appropriate joints from the designated provider, and
 * then copies them into place so an enactor can send them to the low level.
 * This threaed then ``hangs'' until the enactor signals it has read the current
 * values. (This signaling is actually done in the getNextJoints method in
 * this class)
 *
 * Potential problems: If the processing for the next joints
 * takes too long, the enactor will send old joints.
 */
void MotionSwitchboard::run() {
    static int fcount = 0;
    while(running) {

        if(fcount == 1){
            //hack to help keep from falling over in the simulator
            usleep(2*1000*1000);
        }

        // Calculate the next joints and get them
        scriptedProvider.calculateNextJoints();

        vector <float > llegJoints = scriptedProvider.getChainJoints(LLEG_CHAIN);
        vector <float > rlegJoints = scriptedProvider.getChainJoints(RLEG_CHAIN);

		vector <float > rarmJoints = scriptedProvider.getChainJoints(RARM_CHAIN);
		vector <float > larmJoints= scriptedProvider.getChainJoints(LARM_CHAIN);


        //Copy the new values into place, and wait to be signaled.
        pthread_mutex_lock(&next_joints_mutex);

        for(unsigned int i = 0; i < LEG_JOINTS; i ++){
            nextJoints[L_HIP_YAW_PITCH + i] = llegJoints.at(i);
        }
        for(unsigned int i = 0; i < LEG_JOINTS; i ++){
            nextJoints[R_HIP_YAW_PITCH + i] = rlegJoints.at(i);
        }
        for(unsigned int i = 0; i < ARM_JOINTS; i ++){
            nextJoints[L_SHOULDER_PITCH + i] = larmJoints.at(i);
        }
        for(unsigned int i = 0; i < ARM_JOINTS; i ++){
            nextJoints[R_SHOULDER_PITCH + i] = rarmJoints.at(i);
       }


        sensors->setBodyAngles(nextJoints);
        pthread_cond_wait(&calc_new_joints_cond, &next_joints_mutex);
        pthread_mutex_unlock(&next_joints_mutex);
        fcount++;

    }

}


const vector <float> MotionSwitchboard::getNextJoints() {
    //grab the latest values, and signal
    pthread_mutex_lock(&next_joints_mutex);

    const vector <float> vec(nextJoints);

    pthread_cond_signal(&calc_new_joints_cond);

    pthread_mutex_unlock(&next_joints_mutex);


    return vec;
}

