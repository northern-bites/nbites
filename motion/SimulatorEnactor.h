#ifndef _SimulatorEnactor_h_DEFINED
#define _SimulatorEnactor_h_DEFINED

#include "MotionEnactor.h"

class SimulatorEnactor : public MotionEnactor {
public:
    SimulatorEnactor(MotionSwitchboard * _switchboard)
        : MotionEnactor(_switchboard) {};
    virtual ~SimulatorEnactor() { };

    virtual void run();
};

#endif

