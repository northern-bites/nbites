#include "MotionSwitchboard.h"

MotionSwitchboard::MotionSwitchboard()
  : walkProvider(),
    nextJoints(Kinematics::NUM_JOINTS, 0.0),
    running(false)
{

    pthread_mutex_init(&next_joints_mutex, NULL);

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

        //pthread_cond_wait (&
    }
}


const vector <float> MotionSwitchboard::getNextJoints() {
    pthread_mutex_lock(&next_joints_mutex);

    const vector <float> vec(nextJoints);

    pthread_mutex_unlock(&next_joints_mutex);

    //pthread_cond_broadcast (&

    return vec;
}

