
#include "CKF.hpp"
#include "utils/basic_maths.hpp"


static const float angle_var = SQUARE(DEG2RAD(1.f)); //1
static const float velocity_var = SQUARE(DEG2RAD(5.f)); //1 

static const float dt = 0.01f; // 10ms time intervals

static const float acc_var = SQUARE(DEG2RAD(50.f)); //50
static const float gyro_var = SQUARE(DEG2RAD(1.f));  


CKF::CKF(){
   resetFilter();
}

void CKF::resetFilter(){
   state = Eigen::Vector4f::Zero(); 
   
   Q = Eigen::Matrix<float, 4, 4>::Zero();
   Q(0,0) = Q(1,1) = angle_var;
   Q(2,2) = Q(3,3) = velocity_var;
   var = Q;

   R = Eigen::Matrix<float, 4, 4>::Zero();
   R(0,0) = R(1,1) = acc_var;
   R(2,2) = R(3,3) = gyro_var;
}



void CKF::update(const SensorValues &sensorValues){

   // Process matrix   
   Eigen::Matrix<float, 4, 4> F = Eigen::Matrix<float, 4, 4>::Identity();
   F(0,2) = cos(state(2))*dt;
   F(1,2) = sin(state(2))*tan(state(1))*dt;
   F(1,3) = dt;
   
   // Process update
   state = F*state;
   var = F*var*F.transpose()+Q;

   // Observation update
   Eigen::Vector4f obs;
    
   // Calculate pitch and roll angle from accelerometers
   float Ax = sensorValues.sensors[Sensors::InertialSensor_AccX];
   float Ay = sensorValues.sensors[Sensors::InertialSensor_AccY];
   float Az = sensorValues.sensors[Sensors::InertialSensor_AccZ];

   obs(0) = atan(Ay/Az); // Roll
   obs(1) = atan(-Ax/Az)*cos(obs(0)); // Pitch
   obs(2) = sensorValues.sensors[Sensors::InertialSensor_GyrX]; // Roll velocity
   obs(3) = sensorValues.sensors[Sensors::InertialSensor_GyrY]; // Pitch velocity

   //std::cout << "Forward Lean / Side Lean : Obs = " << RAD2DEG(obs(1)) << ", " << RAD2DEG(obs(2)) << "\n";

   Eigen::Matrix<float, 4, 4> K = var * (var + R).inverse();
   state = state + K*(obs-state);
   var = var - K*var;

   //std::cout << "Roll / Pitch: " << RAD2DEG(state(0)) << ", " << RAD2DEG(state(1)) << "\n";

}


float CKF::getForwardLean(){
   return state(1);
}


float CKF::getSideLean(){
   return state(0);
}
