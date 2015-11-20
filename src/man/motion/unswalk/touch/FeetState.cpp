#include "motion/touch/FeetState.hpp"

using namespace boost::numeric::ublas;

FeetState::FeetState(){
   for(int i = 0; i < 2; i++){
      groundContact[i] = true;
      ZMP[i] = 0;
      CoP[i][0] = CoP[i][1] = 0;
      footPos[i][0] = footPos[i][1] = footPos[i][2] = 0;
   }
}

void FeetState::update(const SensorValues &sensors, Kinematics &kinematics){
   float footSensor[2][4];
   footSensor[0][0] = sensors.sensors[Sensors::LFoot_FSR_FrontLeft];
   footSensor[0][1] = sensors.sensors[Sensors::LFoot_FSR_FrontRight];
   footSensor[1][0] = sensors.sensors[Sensors::RFoot_FSR_FrontLeft];
   footSensor[1][1] = sensors.sensors[Sensors::RFoot_FSR_FrontRight];
   footSensor[0][2] = sensors.sensors[Sensors::LFoot_FSR_RearLeft];
   footSensor[0][3] = sensors.sensors[Sensors::LFoot_FSR_RearRight];
   footSensor[1][2] = sensors.sensors[Sensors::RFoot_FSR_RearLeft];
   footSensor[1][3] = sensors.sensors[Sensors::RFoot_FSR_RearRight];

   groundContact[0] = true;
   groundContact[1] = true;
   //trust 2 out of 4
   int count = 0;
   for(int i = 0; i < 2; i++){
      count = 0;
      for(int j = 0; j < 4; j++){
         count += (footSensor[i][j]*1000.0 > 150);
      }
      if(count < 2) groundContact[i] = false;
   }

   matrix<float> leftFootChain =
      kinematics.evaluateDHChain(
            Kinematics::FOOT,
            Kinematics::BODY,
            Kinematics::LEFT_CHAIN);
   matrix<float> rightFootChain =
      kinematics.evaluateDHChain(
            Kinematics::FOOT,
            Kinematics::BODY,
            Kinematics::RIGHT_CHAIN);
   matrix<float> origin = vec4<float>(0, 0, 0, 1);
   matrix<float> lf = prod(leftFootChain, origin);
   matrix<float> rf = prod(rightFootChain, origin);
   for(int i = 0; i < 3; i++){
      footPos[0][i] = -lf(i, 0);
      footPos[1][i] = -rf(i, 0);
   }

   float total[2][2] = {{0,0}, {0,0}};
   float curCoP[2][2] = {{0,0}, {0,0}};
   float curZMP[2] = {0,0};

   for(int l = 0; l < 2; l++){       //l or r foot
      for(int i = 0; i < 4; i++){    //sensor pos
         for(int j = 0; j < 2; j++){ //x/y axis
            curCoP[l][j] += Sensors::FSR_POS[l][i][j] * footSensor[l][i];
            total[l][j] += footSensor[l][i];
            curZMP[j] += (footPos[l][j] + Sensors::FSR_POS[l][i][j]) * footSensor[l][i];
         }
      }
   }

   for(int l = 0; l < 2; l++){
      for(int j = 0; j < 2; j++){
         if(total[l][j] > 0.0001){
            curCoP[l][j] /= total[l][j];
         } else {
            curCoP[l][j] = 0;
         }
         CoP[l][j] += 0.5 * (curCoP[l][j] - CoP[l][j]);
      }
   }

   for(int i = 0; i < 2; i++){
      if((total[0][i] + total[1][i]) > 0.00001){
         curZMP[i] /= (total[0][i] + total[1][i]);
      } else {
         curZMP[i] = 0;
      }
      ZMP[i] += 1.0 * (curZMP[i] - ZMP[i]);
   }
}
