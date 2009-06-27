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

    const boost::tuple<const float, const float> getAngles() const ;

private:
    boost::shared_ptr<Sensors> sensors;
    const MetaGait * gait;

    float sensorAngleX, sensorAngleY;

    //State info
    //sensor feedback stuff
    float lastSensorAngleX,lastSensorAngleY;

};


#endif
