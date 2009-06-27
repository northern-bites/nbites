#ifndef SpringSensor_h_DEFINED
#define SpringSensor_h_DEFINED
//WORK IN PROGRESS:
//IDEA IS TO USE A SPRING MODEL TO RESTORE SENSORANGLE BACK TO ZERO over time
class SpringSensor {
public:
    SpringSensor(const MetaGait * _gait);
    ~SpringSensor();

    void tick_sensor();
    const float getSensorAngle();

private:
    NBMath::ufvector3 x_k;
    NBMath::ufmatrix3 A;
    NBMath::ufvector3 b;

};
#endif
