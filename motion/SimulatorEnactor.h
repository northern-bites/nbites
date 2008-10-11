#ifndef _SimulatorEnactor_h_DEFINED
#define _SimulatorEnactor_h_DEFINED

#include "MotionEnactor.h"

class SimulatorEnactor : MotionEnactor {
public:
    SimulatorEnactor(MotionSwitchboard * _switchboard)
        : MotionEnactor(_switchboard) {};
    virtual ~SimulatorEnactor() { }
    virtual void start() { }

};

#endif
