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

	command = new BodyJointCommand(100.0f,
								   bodyJoints,
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
	cout << "larmsize" << command->getJoints(Kinematics::LARM_CHAIN)->size() << endl;
	scriptedProvider.enqueue(command);
	delete bodyJoints;
	delete command;

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
	cout << "STOPPING SWITCHBOARD" << endl;
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
<<<<<<< HEAD:motion/MotionSwitchboard.cpp

=======
		
>>>>>>> Probably not a commit with value. Just want to be able to push this to my github. There is a memory leak in ChopShop that I am unsure of its location.:motion/MotionSwitchboard.cpp
		vector <float > rarmJoints = scriptedProvider.getChainJoints(RARM_CHAIN);
		vector <float > larmJoints= scriptedProvider.getChainJoints(LARM_CHAIN);


        //Copy the new values into place, and wait to be signaled.
        pthread_mutex_lock(&next_joints_mutex);

        for(unsigned int i = 0; i < LEG_JOINTS; i ++){
            nextJoints[L_HIP_YAW_PITCH + i] = llegJoints[i];
        }
        for(unsigned int i = 0; i < LEG_JOINTS; i ++){
            nextJoints[R_HIP_YAW_PITCH + i] = rlegJoints[i];
        }
        for(unsigned int i = 0; i < ARM_JOINTS; i ++){
            nextJoints[L_SHOULDER_PITCH + i] = larmJoints[i];
        }
        for(unsigned int i = 0; i < ARM_JOINTS; i ++){
            nextJoints[R_SHOULDER_PITCH + i] = rarmJoints[i];
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

