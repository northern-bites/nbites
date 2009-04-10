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

#ifndef _MotionEnactor_h_DEFINED
#define _MotionEnactor_h_DEFINED

#include "MotionSwitchboard.h"

class MotionEnactor {
public:
    MotionEnactor()
        : switchboard(NULL),switchboardSet(false){};
    virtual ~MotionEnactor() { }

    virtual void sendCommands() = 0;
    virtual void postSensors() = 0;

    void setSwitchboard(MotionSwitchboard * s){
        switchboard = s;
        switchboardSet = true;
    }

protected:
    MotionSwitchboard *switchboard;
    bool switchboardSet; //Only true once the switchboard is set.
                         //Helps generate valid error messages when
                         //switchboard is deconstructed

};

#endif
