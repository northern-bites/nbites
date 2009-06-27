#include "SensorAngles.h"
#include "MotionConstants.h"
#include "NBMath.h"
using boost::shared_ptr;

//NOTE/DANGER gait constants which work well with one model may
//suck horibly with the other mode!!
#define USE_SPRING

SensorAngles::SensorAngles(shared_ptr<Sensors> s,
    const MetaGait * _gait):
    sensors(s),
    gait(_gait),
    sensorAngleX(0.0f),sensorAngleY(0.0f),
    springX(gait,SpringSensor::X),
    springY(gait,SpringSensor::Y),
    lastSensorAngleX(0.0f),lastSensorAngleY(0.0f)
{}


SensorAngles::~SensorAngles(){}


void SensorAngles::tick_sensors(){
    if(gait->sensor[WP::FEEDBACK_TYPE] == 1.0f)
        spring_sensor_feedback();
    else if(gait->sensor[WP::FEEDBACK_TYPE] == 0.0f)
        basic_sensor_feedback();
    else{
        spring_sensor_feedback();
        // std::cout << gait->sensor[WP::FEEDBACK_TYPE]
        //           <<" is not a valid sensor feedback type"<<std::endl;
    }
}


void SensorAngles::spring_sensor_feedback(){
    const Inertial inertial = sensors->getInertial();
    //std::cout << "AngleX/Y  = ("<<inertial.angleX<<","<<inertial.angleY<<")"<<std::endl;
    springX.tick_sensor(inertial.angleX);
    springY.tick_sensor(inertial.angleY-gait->stance[WP::BODY_ROT_Y]);

    sensorAngleX = springX.getSensorAngle();
    sensorAngleY = springY.getSensorAngle();
}

void SensorAngles::basic_sensor_feedback(){
    const float MAX_SENSOR_ANGLE_X = gait->sensor[WP::MAX_ANGLE_X];
    const float MAX_SENSOR_ANGLE_Y = gait->sensor[WP::MAX_ANGLE_Y];

    const float MAX_SENSOR_VEL = gait->sensor[WP::MAX_ANGLE_VEL]*
        MotionConstants::MOTION_FRAME_LENGTH_S;

    //calculate the new angles, take into account gait angles already
    Inertial inertial = sensors->getInertial();

    const float desiredSensorAngleX =
        inertial.angleX*gait->sensor[WP::GAMMA_X];
    const float desiredSensorAngleY =
        (inertial.angleY-gait->stance[WP::BODY_ROT_Y])
        *gait->sensor[WP::GAMMA_X];

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
const boost::tuple<const float, const float>
SensorAngles::getAngles(const float scale) const {

    return boost::tuple<const float, const float> (sensorAngleX*scale,
                                                   sensorAngleY*scale);
}
