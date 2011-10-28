/**
 * This class is an abstraction to send commands from our robot code to the
 * underlying robot code, depending on the platform. The idea is to try
 * to remove as much platfrom dependant code from our code base and conncentrate
 * it in files which are 'hot swappable' so if we change from simmulation to
 * the robot, we can easily do this by creating a new Enactor/Transcriber pair.
 *
 * Note that this file was originally created only to send joint information
 * from the switchboard to ALMotion/Webots, so this transformation is not
 * complete yet. Also, there is still no enactor for Webots.
 *
 * See also Transcriber, ThreadedEnactor, NaoEnactor (DCM)
 * and ALEnactor(ALMotion).
 *
 * Future improvements. The interface could be more generalize by making
 * the switchboard private, and require lower classes to call methods in this
 * class to access information from the switchboard. At this point,
 * there is no need for that extra effort however. Also this class should be
 * named "Enactor" not MotionEnactor
 *
 * Johannes Strom April 2009
 */

#ifndef _MotionEnactor_h_DEFINED
#define _MotionEnactor_h_DEFINED

#include "MotionSwitchboard.h"

class MotionEnactor {
public:
    MotionEnactor()
        : switchboard(MotionSwitchboardInterface::NullInstance()) {}
    virtual ~MotionEnactor() { }

    virtual void sendCommands() = 0;
    virtual void postSensors() = 0;

    void resetSwitchboard() {
        this->setSwitchboard(MotionSwitchboardInterface::NullInstance());
    }

    void setSwitchboard(MotionSwitchboardInterface * s){
        assert(s != NULL);
        if (MotionSwitchboardInterface::isTheNullInstance(s)) {
            std::cout << "Switchboard reset to the Null Instance" << std::endl;
        } else {
            std::cout << "Switchboard set" <<std::endl;
        }
        switchboard = s;
    }

protected:
    MotionSwitchboardInterface *switchboard;
    bool switchboardSet; //Only true once the switchboard is set.
                         //Helps generate valid error messages when
                         //switchboard is deconstructed

};

#endif
