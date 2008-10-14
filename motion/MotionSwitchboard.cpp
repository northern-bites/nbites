#include "MotionSwitchboard.h"
MotionSwitchboard::MotionSwitchboard()
  : walkProvider(),
    nextJoints(Kinematics::NUM_JOINTS, 0.0),
    running(false)
{

    //Allow safe access to the next joints
    pthread_mutex_init(&next_joints_mutex, NULL);
    pthread_cond_init(&calc_new_joints_cond,NULL);
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

    running = true;

    // set thread attributes
    pthread_attr_t attr;
    pthread_attr_init (&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    // create & start thread.
    pthread_create(&switchboard_thread, &attr, runThread, (void *)this);
    // destroy the used attributes
    pthread_attr_destroy(&attr);
}


void MotionSwitchboard::stop() {
    running = false;
}


void* MotionSwitchboard::runThread(void *switchboard) {
    ((MotionSwitchboard*)switchboard)->run();
    pthread_exit(NULL);
}


void MotionSwitchboard::run() {
    while(running) {
        walkProvider.calculateNextJoints();

        vector <float > llegJoints = walkProvider.getChainJoints(LLEG_CHAIN);
        vector <float > rlegJoints = walkProvider.getChainJoints(RLEG_CHAIN);

        //copy the new values into place, and wait to be signaled.
        pthread_mutex_lock(&next_joints_mutex);
        for(unsigned int i = 0; i < LEG_JOINTS; i ++){
            nextJoints[L_HIP_YAW_PITCH + i] = llegJoints[i];
        }
        for(unsigned int i = 0; i < LEG_JOINTS; i ++){
            nextJoints[R_HIP_YAW_PITCH + i] = rlegJoints[i];
        }
        pthread_cond_wait(&calc_new_joints_cond, &next_joints_mutex);
        pthread_mutex_unlock(&next_joints_mutex);

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

