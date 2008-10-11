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

#include <vector>
using namespace std;

class MotionSwitchboard {
public:
    MotionSwitchboard();
    ~MotionSwitchboard();

    const vector <float> getNextJoints(){return nextJoints; }
    

private:
    vector <float> nextJoints;
};

#endif
