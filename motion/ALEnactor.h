#ifndef _ALEnactor_h_DEFINED
#define _ALEnactor_h_DEFINED

#ifdef NAOQI1

#include "almotionproxy.h"
#include "alptr.h"

#include "MotionEnactor.h"
using namespace AL;

class ALEnactor : public MotionEnactor {
public:
    ALEnactor(MotionSwitchboard * _switchboard, ALPtr<ALMotionProxy> _mproxy)
        : MotionEnactor(_switchboard), mproxy(_mproxy) {};
    virtual ~ALEnactor() { };

    virtual void run();

private:
    ALPtr<ALMotionProxy>  mproxy;

    static const int MOTION_FRAME_RATE = 50;
    static const float MOTION_FRAME_LENGTH_uS = // in microseconds
      // 1 second * 1000 ms/s * 1000 us/ms
      1.0f * 1000.0f * 1000.0f / MOTION_FRAME_RATE;
    static const float MOTION_FRAME_LENGTH_S = // in seconds
      1.0f / MOTION_FRAME_RATE;
};

#endif //NAOQI1

#endif//_ALEnactor_h_DEFINED
