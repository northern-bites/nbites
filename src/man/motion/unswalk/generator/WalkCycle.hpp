#pragma once


/* Give period of walk cycle and the current part of walk cycle,
   in addition to, forward, left, turn, leftPhase.

   Determine the position of the legs for this t in the walk cycle
*/
#define FPS 100.0f
class WalkCycle {
   public:
      WalkCycle();
      WalkCycle(float useForwardL, float useForwardR, float useLeft,
                float useTurn, float T, float t);

      // parameters needed to determine the whole walk cycle
      float useForwardL, useForwardR, useLeft, useTurn;
      float T;

      // parameters needed to tell us an instant of the walk cycle
      float t;
      bool leftPhase;
      static const float movFrac;


      void generateWalk(float &forwardL, float &forwardR,
                        float &leftL, float &leftR,
                        float &turnLR, float &liftL, float &liftR);

      bool isDoubleSupportPhase();

      // a predictive method for the A* planner. Just moves t forward
      // not the same as normal walk cycle as we usually weight until
      // foot sensors sense weight has shifted which will offset t by
      // a small fraction
      void next();
};

