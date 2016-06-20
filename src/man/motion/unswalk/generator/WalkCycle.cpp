#include <cmath>
#include <iostream>
#include "WalkCycle.hpp"
 
const float WalkCycle::movFrac = .4;

WalkCycle::WalkCycle() {
   this->useForwardL = 0;
   this->useForwardR = 0;
   this->useLeft = 0;
   this->useTurn = 0;
   this->T = 0.5;
   this->t = 0;
   this->leftPhase = false;

}

WalkCycle::WalkCycle(float useForwardL, float useForwardR, float useLeft,
                     float useTurn, float T, float t) {
   this->useForwardL = useForwardL;
   this->useForwardR = useForwardR;
   this->useLeft = useLeft;
   this->useTurn = useTurn;
   this->T = T;
   this->t = t;
   if (t < T/2.0) {
      this->leftPhase = true;
   } else {
      this->leftPhase = false;
   }
}

void WalkCycle::generateWalk(float &forwardL, float &forwardR,
                             float &leftL, float &leftR,
                             float &turnLR, float &liftL, float &liftR) {
//      float movePercent = 0.5;
//      if(leftSwing){
//         forwardR = moveSin(s_x*(1-movePercent), -s_x_*movePercent, getPhase(t, T));
//         forwardL = moveSin(-s_x*movePercent, s_x_*(1-movePercent), getPhase(t, T));
//         leftR = moveSin(s_y*(1-movePercent), -s_y_*movePercent, getPhase(t, T));
//         leftL = moveSin(-s_y*movePercent, s_y_*(1-movePercent), getPhase(t, T));
//      } else {
//         forwardL = moveSin(s_x*(1-movePercent), -s_x_*movePercent, getPhase(t, T));
//         forwardR = moveSin(-s_x*movePercent, s_x_*(1-movePercent), getPhase(t, T));
//         leftL = moveSin(s_y*(1-movePercent), -s_y_*movePercent, getPhase(t, T));
//         leftR = moveSin(-s_y*movePercent, s_y_*(1-movePercent), getPhase(t, T));
//      }
//      turnLR = moveSin(s_r, s_r_, getPhase(t, T));

   float phaseLeftT = t;
   float phaseTurnT = t;
   forwardL = -useForwardL / 4.0f;
   forwardR = +useForwardR / 4.0f;

   leftL = leftR  = 0.0f;
   if (useLeft > 0.0f) {
   } else {
      phaseLeftT = (t + T/2.0);
      if (phaseLeftT > T) phaseLeftT -= T;
   }
   turnLR = useTurn / 8.0f;
   if (useTurn < 0.0f) {
      phaseTurnT = (t + T/2.0);
      if (phaseTurnT > T) phaseTurnT -=T;
   }

   forwardL -= useForwardL / T * t;
   forwardR -= useForwardR / T * t;
   leftL    -= useLeft / T * phaseLeftT;
   leftR    -= useLeft / T * phaseLeftT;
   // omni-dir movement
   float movT = movFrac * T;
   float startT = ((1.0f - 2.0f * movFrac) / 4.0f) * T;

   //left side
   if (t >= startT) {
      if (t < startT + movT + T / FPS && leftPhase) {  // left leg
         float sinu = sqrt((1.0f - cos((t - startT) * M_PI / movT)) / 2.0f);  // changed
         if (t > startT + movT) sinu = 1.0f;
         forwardL  += useForwardL * sinu;
      } else {
         forwardL  += useForwardL * 1.0;
      }
   }

   float dir = 1;
   if (useTurn < 0) {
      startT = ((1.0f - 2.0f * movFrac) / 4.0f) * T + T / 2.0f;
   } else {
      startT = ((1.0f - 2.0f * movFrac) / 4.0f);
   }
   if (phaseTurnT >= startT) {
      if (phaseTurnT < startT + movT + T / FPS && leftPhase) {  // lefphaseLeftT leg
         float sinu = sqrt((1.0f - cos((phaseTurnT - startT) * M_PI / movT)) / 2.0f);  // changed
         if (phaseTurnT > startT + movT) sinu = 1.0f;
         turnLR     += dir * useTurn * sinu;
      } else {
         turnLR     += dir *  useTurn * 1.0;
      }
   }


   if (useLeft < 0) {
      startT = ((1.0f - 2.0f * movFrac) / 4.0f) * T + T / 2.0f;
   } else {
      startT = ((1.0f - 2.0f * movFrac) / 4.0f);
   }
   if (phaseLeftT >= startT) {
      if (phaseLeftT < startT + movT + T / FPS && leftPhase) {  // lefphaseLeftT leg
         float sinu = sqrt((1.0f - cos((phaseLeftT - startT) * M_PI / movT)) / 2.0f);  // changed
         if (phaseLeftT > startT + movT) sinu = 1.0f;
         leftL     += useLeft * sinu;
      } else {
         leftL     += useLeft * 1.0;
      }
   }

   // ride side
   startT = ((1.0f - 2.0f * movFrac) / 4.0f) * T + T/2.0f;
   if (t >= startT) {
      if (t < startT + movT + T / FPS && !leftPhase) {  // right leg
         float sinu = sqrt((1.0f - cos((t - startT) * M_PI / movT)) / 2.0f);  // changed
         if (t > startT + movT) sinu = 1.0f;
         forwardR  += useForwardR * sinu;
      } else {
         forwardR  += useForwardR * 1.0;
      }
   }

   dir = 1;
   if (useTurn < 0) {
      startT = ((1.0f - 2.0f * movFrac) / 4.0f);
      dir = -1;
   } else {
      startT = ((1.0f - 2.0f * movFrac) / 4.0f) * T + T/2.0f;
      dir = -1;
   }

   if (phaseTurnT >= startT) {
      if (phaseTurnT < startT + movT + T / FPS && !leftPhase) {  // lefphaseLeftT leg
         float sinu = sqrt((1.0f - cos((phaseTurnT - startT) * M_PI / movT)) / 2.0f);  // changed
         if (phaseTurnT > startT + movT) sinu = 1.0f;
         turnLR     += dir * useTurn * sinu;
      } else {
         turnLR     += dir *useTurn * 1.0;
      }
   }
   
   if (useLeft < 0) {
      startT = ((1.0f - 2.0f * movFrac) / 4.0f);
   } else {
      startT = ((1.0f - 2.0f * movFrac) / 4.0f) * T + T/2.0f;
   }
   if (phaseLeftT >= startT) {
      if (phaseLeftT < startT + movT + T / FPS && !leftPhase) {  // right leg
         float sinu = sqrt((1.0f - cos((phaseLeftT - startT) * M_PI / movT)) / 2.0f);  // changed
         if (phaseLeftT > startT + movT) sinu = 1.0f;
         leftR     += useLeft * sinu;
      } else {
         leftR     += useLeft * 1.0;
      }
   }
   // leftL *= useLeft > 0 ? 1 : - 1;
   // leftR *= useLeft > 0 ? 1 : - 1;
}

void WalkCycle::next() {
   t += T / 100.0f;
   if (t > T) {
      t = 0.0f;
   }

   // this could be wrong
   if (t > T / 2) {
      leftPhase = false;
   } else {
      leftPhase = true;
   }
}


bool WalkCycle::isDoubleSupportPhase() {
   float D = T - T * 2 * movFrac;
   if (t <= D/4 || t >= T - D/4 || (t > T/2 - D/4 && t < T/2 + D/4)) return true;
   return false;
}
