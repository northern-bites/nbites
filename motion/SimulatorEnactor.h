#ifndef _SimulatorEnactor_h_DEFINED
#define _SimulatorEnactor_h_DEFINED

#include "almotionproxy.h"

#include "MotionEnactor.h"

class SimulatorEnactor : public MotionEnactor {
public:
    SimulatorEnactor(MotionSwitchboard * _switchboard)
        : MotionEnactor(_switchboard) {};
    virtual ~SimulatorEnactor() { };

    virtual void run();

private:
    static const int MOTION_FRAME_RATE = 50;
    static const float MOTION_FRAME_LENGTH_uS = // in microseconds
      // 1 second * 1000 ms/s * 1000 us/ms
      1.0f * 1000.0f * 1000.0f / MOTION_FRAME_RATE;
    static const float MOTION_FRAME_LENGTH_S = // in seconds
      1.0f / MOTION_FRAME_RATE;
};

#endif

