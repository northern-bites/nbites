/**
 * The switchboard is literally a switching mechanism to select between the
 * different ways that we can publish joint values to the robot (called
 * MotionProviders). For example, the walk engine and the queueing system both
 * provide such functionality.
 *
 * The appropriate MotionEnactor will then take the nextJoints and pass them
 * down to the robot/simulator correctly.
 */

#ifndef _MotionSwitchboard_h_DEFINED
#define _MotionSwitchboard_h_DEFINED

#include <pthread.h>
#include <vector>
using namespace std;

#include "Kinematics.h"
#include "WalkProvider.h"
#include "Sensors.h"

class MotionSwitchboard {
public:
    MotionSwitchboard(Sensors *s);
    ~MotionSwitchboard();

    void start();
    void stop();
    static void* runThread(void *switchboard);
    void run();

    const vector <float> getNextJoints();

private:
    Sensors * sensors;
    WalkProvider walkProvider;
    vector <float> nextJoints;

    bool running;
    pthread_t       switchboard_thread;
    pthread_cond_t  calc_new_joints_cond;
    pthread_mutex_t next_joints_mutex;
};

#endif
