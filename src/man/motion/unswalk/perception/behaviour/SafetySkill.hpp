#pragma once

// #define MIN_STANDING_WEIGHT 0.75f
#define MIN_STANDING_WEIGHT 0.55f
#define FALLEN 9
#define FALLING 8
#define FALLEN_ANG 70
#define FALLING_ANG 45

#include "types/BehaviourRequest.hpp"
#include "types/UNSWSensorValues.hpp"

class SafetySkill {
   public:
      SafetySkill();
      ~SafetySkill();
      BehaviourRequest wrapRequest(const BehaviourRequest &request, const UNSWSensorValues &s);
   private:
      float filtered_fsr_sum;
      int blink;
};
