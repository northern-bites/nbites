#ifndef WBEnactor_h
#define WBEnactor_h

#include "Sensors.h"
#include "Transcriber.h"
#include "ThreadedMotionEnactor.h"

#include <webots/robot.h>

class WBEnactor : public MotionEnactor {
public:
    WBEnactor(boost::shared_ptr<Sensors> _sensors,
              boost::shared_ptr<Transcriber> transcriber);
    virtual ~WBEnactor();

    void postSensors();
    void sendCommands();

protected:
    boost::shared_ptr<Sensors> sensors;
    boost::shared_ptr<Transcriber> transcriber;

    static const int MOTION_FRAME_RATE;
    static const float MOTION_FRAME_LENGTH_uS; // in microseconds
    static const float MOTION_FRAME_LENGTH_S; // in seconds

private:
    std::vector<float> motionValues;
    std::vector<WbDeviceTag> jointDevices;
};

#endif
