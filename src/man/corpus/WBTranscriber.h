#ifndef WBTranscriber_h
#define WBTranscriber_H


#include <webots/servo.h>
#include <boost/tuple/tuple.hpp>

#include "Transcriber.h"
#include "AccEKF.h"
#include "AngleEKF.h"

class WBTranscriber : public Transcriber{
public:
    WBTranscriber(boost::shared_ptr<Sensors> s);
    virtual ~WBTranscriber();

    void postMotionSensors();
    void postVisionSensors();

private:
    const boost::tuple<const float, const float>
    angleWrapper(const float accX, const float accY,
                                const float gyroX,const float gyroY);
private:
    AccEKF accEKF;
    std::vector<float> jointValues;
    std::vector<WbDeviceTag> jointDevices;
    std::vector<float> fsrValues;
    std::vector<WbDeviceTag> fsrDevices;
    WbDeviceTag acc;
    WbDeviceTag gyro;
    WbDeviceTag us1,us2,us3,us4;
    float prevAngleX, prevAngleY;
    AngleEKF angleEKF;

};

#endif
