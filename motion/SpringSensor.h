#ifndef SpringSensor_h_DEFINED
#define SpringSensor_h_DEFINED
//WORK IN PROGRESS:
//IDEA IS TO USE A SPRING MODEL TO RESTORE SENSORANGLE BACK TO ZERO over time
#include "MetaGait.h"
#include "CoordFrame.h"

class SpringSensor {
public:
    //tuple indices
    enum SensorAxis{
        X = 0,
        Y
    };

    SpringSensor(const MetaGait * _gait, const SensorAxis _axis);
    ~SpringSensor();

    //Takes input the sensed angle of the robot MINUS the expected rotation
    void tick_sensor(const float sensorAngle);

    //returns a compensation to body Rot X or Y to keep robot level
    const float getSensorAngle();
    void reset();

private:
    void updateMatrices();

private:
    const MetaGait * gait;
    const SensorAxis axis;
    const unsigned int K_INDEX;
    const unsigned int GAMMA_INDEX;
    const unsigned int MAX_INDEX;

    NBMath::ufvector3 x_k;
    NBMath::ufmatrix3 A;
    NBMath::ufvector3 b;
    NBMath::ufrowVector3 c;

    std::string name;
};
#endif
