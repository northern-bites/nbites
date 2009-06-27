#ifndef SensorAngles_h_DEFINED
#define SensorAngles_h_DEFINED

#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include "Sensors.h"
#include "MetaGait.h"


class SensorAngles{
public:
    SensorAngles(boost::shared_ptr<Sensors> s, const MetaGait * _gait);
    ~SensorAngles();


    void tick_sensors();
    void reset();

    //tuple indices
    enum SensorAxis{
        X = 0,
        Y
    };

    const boost::tuple<const float, const float>
    getAngles(const float scale = 1.0f) const ;

private:
    void basic_sensor_feedback();
    void spring_sensor_feedback();

private:
    boost::shared_ptr<Sensors> sensors;
    const MetaGait * gait;

    //store what will be returned by getAngles
    float sensorAngleX, sensorAngleY;


    //OLD
    //State info
    //sensor feedback stuff
    float lastSensorAngleX,lastSensorAngleY;

};


#endif
