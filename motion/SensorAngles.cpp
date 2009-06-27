#include "SensorAngles.h"
#include "MotionConstants.h"
#include "NBMath.h"
using boost::shared_ptr;

SensorAngles::SensorAngles(shared_ptr<Sensors> s,
    const MetaGait * _gait):
    sensors(s),
    gait(_gait),
    sensorAngleX(0.0f),sensorAngleY(0.0f),
    lastSensorAngleX(0.0f),lastSensorAngleY(0.0f)
{}


SensorAngles::~SensorAngles(){}


void SensorAngles::tick_sensors(){
    const float MAX_SENSOR_ANGLE_X = gait->sensor[WP::MAX_ANGLE_X];
    const float MAX_SENSOR_ANGLE_Y = gait->sensor[WP::MAX_ANGLE_Y];

    const float MAX_SENSOR_VEL = gait->sensor[WP::MAX_ANGLE_VEL]*
        MotionConstants::MOTION_FRAME_LENGTH_S;

    //calculate the new angles, take into account gait angles already
    Inertial inertial = sensors->getInertial();
    const float angleScale = gait->sensor[WP::ANGLE_SCALE];

    // const float desiredSensorAngleX =
    //     std::pow(inertial.angleX,2)*std::sqrt(angleScale);
    // const float desiredSensorAngleY =
    //     std::pow(inertial.angleY-gait->stance[WP::BODY_ROT_Y],2)
    //     *std::sqrt(angleScale);
    const float desiredSensorAngleX =
        inertial.angleX*angleScale;
    const float desiredSensorAngleY =
        (inertial.angleY-gait->stance[WP::BODY_ROT_Y])*angleScale;

    //Clip the velocities, and max. limits
    sensorAngleX =
        NBMath::clip(
            NBMath::clip(desiredSensorAngleX,
                         desiredSensorAngleX - MAX_SENSOR_VEL,
                         desiredSensorAngleX + MAX_SENSOR_VEL),
            MAX_SENSOR_ANGLE_X);
    sensorAngleY =
        NBMath::clip(
            NBMath::clip(desiredSensorAngleY,
                         desiredSensorAngleY - MAX_SENSOR_VEL,
                         desiredSensorAngleY + MAX_SENSOR_VEL),
            MAX_SENSOR_ANGLE_Y);

    lastSensorAngleX = sensorAngleX;
    lastSensorAngleY = sensorAngleY;


}
void SensorAngles::reset(){
    lastSensorAngleX = lastSensorAngleY = 0.0f;
    sensorAngleX=  sensorAngleY = 0.0f;
}


/*
 * Get the sensor based adjustment to the body's rotation
 *
 */
const boost::tuple<const float, const float> SensorAngles::getAngles() const {

    return boost::tuple<const float, const float> (sensorAngleX,
                                                   sensorAngleY);
}
