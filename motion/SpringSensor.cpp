
#include "SpringSensor.h"
#include "MotionConstants.h"

using namespace boost::numeric;
using namespace NBMath;
using namespace std;
//#define DEBUG_SPRING

SpringSensor::SpringSensor(const MetaGait * _gait,
    const SensorAxis _axis):
    gait(_gait),
    axis(_axis),
    K_INDEX(axis == X ?
            WP::SPRING_K_X  :  WP::SPRING_K_Y),
    GAMMA_INDEX(axis == X ?
                WP::GAMMA_X  :  WP::GAMMA_Y),
    MAX_INDEX(axis == X ?
                WP::MAX_ANGLE_X:  WP::MAX_ANGLE_Y),
    b(CoordFrame3D::vector3D(0.0f,0.0f,0.0f)),
    c(CoordFrame3D::rowVector3D(1.0f,0.0f,0.0f)),
    name(axis == X ? "springX" : "springY"),
    lastSensorAngle(0.0f)
{

    //build statematrix
    A = ublas::identity_matrix<float>(3);
    A(0,1) = A(1,2)=MotionConstants::MOTION_FRAME_LENGTH_S;
    A(0,2)= 0.5f*std::pow(MotionConstants::MOTION_FRAME_LENGTH_S,2);
    A(2,2) = A(1,1) = 0.0f;

    //NOTE: might added BETA, a friction coefficient, which damps the velocity:
    //in A(2,1)

    reset();
    updateMatrices();
#ifdef DEBUG_SPRING
    cout << "SpringSensor init finished."<<endl
         << "   A:"<<A<<endl
         << "   b:"<<b<<endl
         << "   max Angle:"<<gait->sensor[MAX_INDEX]<<endl;
    cout << "   max angle index! = "<<MAX_INDEX<<endl;
#endif
}

SpringSensor::~SpringSensor()
{}

void SpringSensor::reset(){
//reset state
    x_k = CoordFrame3D::vector3D(0.0f,0.0f,0.0f);
}

void SpringSensor::tick_sensor(const float sensorAngle){
    updateMatrices();

#ifdef DEBUG_SPRING
    cout << "tick sensor "<<name<<endl
         << "   sensor"<<sensorAngle
         << "   start x_k"<<x_k
         << "   A:"<<A<<endl
         << "   b:"<<b<<endl
         << "   max Angle:"<<gait->sensor[MAX_INDEX]<<endl;
#endif

    const float clippedSensorAngle  = NBMath::clip(sensorAngle,
                                                  gait->sensor[MAX_INDEX]);

    //control
    const float u = sensorAngle -x_k(0);
    const ufvector3 x_hat = prod(A,x_k) + b*u;
    x_k = x_hat;

    //finally clip position: reference x_hat to avoid assignment error
    x_k(0) = NBMath::clip(x_hat(0),gait->sensor[MAX_INDEX]);
    //should potentially clip more aggressively things like:
    //when we are near position zero, clip velocity

#ifdef DEBUG_SPRING
    cout << "   control*GAMMA: "<<b*u<<endl;
    cout << "   pre-clip x_k: "<<x_hat<<endl;
    cout << "   post-clip x_k: "<<x_k<<endl;
#endif

    lastSensorAngle = sensorAngle;//unused
}

const float SpringSensor::getSensorAngle(){
    return x_k(0);
}


void SpringSensor::updateMatrices(){
    //Update state matrix by selecting the appropriate spring const
    A(2,0) = -gait->sensor[K_INDEX];

    //Update control vector by selecting the appropriate spring const
    b(0) = gait->sensor[GAMMA_INDEX];
}
