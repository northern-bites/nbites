#include "MotionSwitchboard.h"

MotionSwitchboard::MotionSwitchboard()
  : walkProvider(),
    nextJoints(Kinematics::NUM_CHAINS, 0.0)
{

    pthread_mutex_init(&next_joints_mutex, NULL);

}

MotionSwitchboard::~MotionSwitchboard() {
    pthread_mutex_destroy(&next_joints_mutex);
}

const vector <float> MotionSwitchboard::getNextJoints() {
    pthread_mutex_lock(&next_joints_mutex);

    const vector <float> vec(nextJoints);

    pthread_mutex_unlock(&next_joints_mutex);
    return vec;
}
