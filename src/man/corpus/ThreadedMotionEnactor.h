/**
 * This class is an abstraction of the connection between the Switchboard and
 * the lower level access to robot hardware. Since this connection may change
 * often, and is different for the simulator and the robot, this class provides
 * a contract to fulfill to provide that connection.
 *
 * Each MotionEnactor is in charge of passing joint commands to the low-level.
 * Typically, this will be done by starting a high-priority thread which recurs
 * close to the timestep (20 ms) as possible.
 * Each enactor must call getNextJoints on the switchboard, and relay that
 * information correctly.
 */

#ifndef _ThreadedMotionEnactor_h_DEFINED
#define _ThreadedMotionEnactor_h_DEFINED

#include "MotionEnactor.h"
#include "synchro/synchro.h"

class ThreadedMotionEnactor : public MotionEnactor , public Thread{
public:
    ThreadedMotionEnactor(std::string name)
        : MotionEnactor(), Thread(name){};
    virtual ~ThreadedMotionEnactor() { }

};

#endif
