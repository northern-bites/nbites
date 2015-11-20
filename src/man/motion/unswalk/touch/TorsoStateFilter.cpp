#include <cmath>
#include "TorsoStateFilter.hpp"
#include "utils/angles.hpp"
#include "utils/Timer.hpp"
#include "motion/touch/FilteredTouch.hpp"

#define STATE_DIM 2
//assumption of a straight body here is not good enough, high process variances
#define PROCESS_ANGLE_SD 0.01
#define PROCESS_VEL_SD 0.1
#define SIDE_ANGLE_SD 0.01
#define SIDE_VEL_SD 0.1
#define H 245

TorsoStateFilter::TorsoStateFilter(){
   est = matrix<float>(STATE_DIM, 1);
   estBar = matrix<float>(STATE_DIM, 1);
   for(int i = 0; i < STATE_DIM; i++){
      est(i, 0) = estBar(i, 0) = 0;
   }

   covEst = matrix<float>(STATE_DIM, STATE_DIM);
   covEstBar = matrix<float>(STATE_DIM, STATE_DIM);

   covR = matrix<float>(STATE_DIM, STATE_DIM);
   covQ = matrix<float>(STATE_DIM, STATE_DIM);

   for(int i = 0; i < STATE_DIM; i++){
      for(int j = 0; j < STATE_DIM; j++){
         covEst(i, j) = covEst(i, j) = covR(i, j) = covQ(i, j) = 0;
      }
   }

   covR(0, 0) = PROCESS_ANGLE_SD * PROCESS_ANGLE_SD;
   covR(1, 1) = PROCESS_VEL_SD * PROCESS_VEL_SD;
}

void TorsoStateFilter::init(float dt, float obsAngleSD, float obsVelSD, bool frontal){
   TorsoStateFilter::dt = dt;
   TorsoStateFilter::frontal = frontal;
   covQ(0, 0) = obsAngleSD * obsAngleSD;
   covQ(0, 1) = covQ(1, 0) = 0;
   covQ(1, 1) = obsVelSD * obsVelSD;
   if(!frontal){
      covR(0, 0) = SIDE_ANGLE_SD*SIDE_ANGLE_SD;
      covR(1, 1) = SIDE_VEL_SD*SIDE_VEL_SD;
   }
}

TorsoStateFilter::~TorsoStateFilter(){}

//shortcut for 2by2 matrix
matrix<float> invert(matrix<float> &m){
   float temp = m(1, 1);
   m(1, 1) = m(0, 0);
   m(0, 0) = temp;
   m(0, 1) *= -1.0;
   m(1, 0) *= -1.0;
   float det = m(0, 0) * m(1, 1) - m(1, 0) * m(0, 1);
   for(int i = 0; i < 2; i++){
      for(int j = 0; j < 2; j++){
         m(i, j) /= det;
      }
   }
   return m;
}

float TorsoStateFilter::getFulcrum(matrix<float> obs, FeetState feetState){
#define BACK_LIM -55
#define FRONT_LIM 90
#define COP_OFFSET 0
   float fulcrum = 0.0;
   if(frontal){  //frontal plane pendulum
      //these are just estimates and empirically tuned
      fulcrum = RAD2DEG(est(0, 0)) * 6.5;
      if(est(0,0) < 0) fulcrum *= 1.6;
      if(fulcrum > FRONT_LIM) fulcrum = FRONT_LIM;
      if(fulcrum < BACK_LIM) fulcrum = BACK_LIM;
      if(feetState.groundContact[0] && feetState.groundContact[1]){
         fulcrum += (feetState.footPos[0][0] + feetState.footPos[1][0])/2.0;
      } else {
         fulcrum += (feetState.footPos[0][0]) * feetState.groundContact[0]
                  + (feetState.footPos[1][0]) * feetState.groundContact[1];
      }
   } else {
      fulcrum = -feetState.ZMP[1];
   }

   return fulcrum;
}

matrix<float> TorsoStateFilter::update(matrix<float> obs, FeetState feetState) {
#define g 9810.0
   float fulcrum = getFulcrum(obs, feetState);

   //process update
   float alpha;                        //angle between COM height and pendulum radius
   alpha = atan(fulcrum / H);

   float l = sqrt(fulcrum * fulcrum + H * H);           //pendulum radius

   matrix<float> A(STATE_DIM, STATE_DIM);
   A(0, 0) = 1.0 + dt*dt*g/(2.0*l);   A(0, 1) = dt;
   A(1, 0) = dt * g/l;                A(1, 1) = 1.0;

   matrix<float> u(STATE_DIM, 1);
   u(0, 0) = -dt*dt*g/(2.0*l) * alpha;
   u(1, 0) = -dt * g/l * alpha;

   estBar = prod(A, est) + u;

   matrix<float> temp = prod(A, covEst);
   covEstBar = prod(temp, trans(A)) + covR;

   //observation update
   temp = covEstBar + covQ;
   matrix<float> inverse = invert(temp);
   matrix<float> k = prod(covEstBar, inverse);

   est = estBar + prod(k, obs - estBar);

   identity_matrix<float> I(STATE_DIM);
   covEst = prod(I - k, covEstBar);

   return matrix<float>(est);
}
