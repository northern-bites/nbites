#ifndef _ALEnactor_h_DEFINED
#define _ALEnactor_h_DEFINED

#ifdef NAOQI1

#include "almotionproxy.h"
#include "alptr.h"
#include "albroker.h"

#include "Sensors.h"
#include "MotionEnactor.h"


class ALEnactor : public MotionEnactor {
public:
    ALEnactor(AL::ALPtr<AL::ALBroker> _pbroker, Sensors * s)
        : MotionEnactor(),broker(_pbroker),mproxy(broker->getMotionProxy()),
          sensors(s) {};
    virtual ~ALEnactor() { };

    virtual void run();

    virtual void postSensors();
private:
    AL::ALPtr<AL::ALBroker> broker;
    AL::ALPtr<AL::ALMotionProxy>  mproxy;
    Sensors *sensors;
    static const int MOTION_FRAME_RATE;
    static const float MOTION_FRAME_LENGTH_uS; // in microseconds
    static const float MOTION_FRAME_LENGTH_S; // in seconds
};

#endif //NAOQI1

#endif//_ALEnactor_h_DEFINED
