#pragma once
#include <vector>
#include <utility>
#include <string>
#include "perception/kinematics/Parameters.hpp"
#include "blackboard/Adapter.hpp"
#include "types/RRCoord.hpp"
#include "utils/Timer.hpp"
#include "types/BehaviourRequest.hpp"
#include "FADBAD++/fadiff.h"

#define MIN_ALPHA 0.004f
#define MAX_ALPHA (MIN_ALPHA * 2)
#define STABALIZE_FRAMES  25

class KinematicsCalibrationSkill : Adapter {
   public:
      KinematicsCalibrationSkill(Blackboard *blackboard);
      ~KinematicsCalibrationSkill();

      BehaviourRequest execute();
      static std::string printParams(Parameters<float>
                                     &parameters);
   private:
      Timer timer;

      Parameters<fadbad::F<float> > lastParams;
      fadbad::F<float> objectiveFunction(
         Parameters<fadbad::F<float> > &parameters);
      bool gradientDescent(
         Parameters<fadbad::F<float> > &parameters,
         Parameters<fadbad::F<float> > gradients);
      static std::string printParams(Parameters<fadbad::F<float> >
                                     &parameters);
      void resetGradients();
      void updateGradient();
      std::vector<std::pair<float, float> > points;
      unsigned int currentWaypoint;
      int beenAtFrameFor;
      bool takenReading;
      int n;
      float value;
      float alpha;
      bool isTop;
      Parameters<fadbad::F<float> > gradients;
};

