#pragma once
#include <iostream>
#include <list>
#include "WalkCycle.hpp"
#include "types/JointValues.hpp"
#include "types/Odometry.hpp"
#include "types/XYZ_Coord.hpp"
#include "perception/kinematics/Kinematics.hpp"

#include "PendulumModel.hpp"

class BodyModel {
   public:
      BodyModel();
      void update(Odometry *odometry, const SensorValues &sensors);

      inline float getPressureL() {
         return pressureL;
      }

      inline float getPressureR() {
         return pressureR;
      }

      inline float getFilHighZMPF() {
         return filHighZMPF;
      }

      inline float getFilLowZMPF() {
         return filLowZMPF;
      }

      inline float getFilAccX() {
         return filAccX;
      }

      inline float getFilAccY() {
         return filAccY;
      }

      inline float getFilZMPL() {
         return filZMPL;
      }

      inline float getLastZMPL() {
         return lastZMPL;
      }

      inline XYZ_Coord getCoM() {
         return centreOfMass;
      }

      inline float getSagittalAcceleration() {
         return sagittalAcceleration;
      }

      inline float getFilteredTotalPressure() {
         return filteredTotalPressure;
      }

      inline void setForwardL(float forwardL) {
         this->forwardL = forwardL;
      }

      inline void setForwardR(float forwardR) {
         this->forwardR = forwardR;
      }

      inline void setIsBetweenSteps(bool isBetweenSteps) {
         if (this->isBetweenSteps == false && isBetweenSteps) {
            updateStepFrequency();
         }
         this->isBetweenSteps = isBetweenSteps;
      }

      inline bool getIsBetweenSteps() {
         return this->isBetweenSteps;
      }

      inline void setIsPaused(bool isPaused) {
         this->isPaused = isPaused;
      }

      inline bool getIsPaused() {
         return isPaused;
      }
      inline void updateSagittalAcceleration(float stepSize) {
         sagittalAcceleration += ((stepSize * 0.5) - sagittalAcceleration) * .3;
      }

      inline void setTurn(float turnL, float turnR) {
         this->turnL = turnL;
         this->turnR = turnR;
      }
      inline void updateStepFrequency() {
         lastStepFrequencyDifferences.push_back(stepCounter);
         if (lastStepFrequencyDifferences.size() > 2) {
            lastStepFrequencyDifferences.pop_front();
         }
         stepCounter = 0;
      }
      inline float getRealStepFrequency() {
         if (lastStepFrequencyDifferences.size() == 0) {
            return stepCounter;
         }
         float sum = 0;
         std::list<float>::iterator it;
         for (it = lastStepFrequencyDifferences.begin();
              it != lastStepFrequencyDifferences.end(); ++it) {
            sum += *it;
         }
         return (sum / (lastStepFrequencyDifferences.size())) * 2 / 100.0f;
      }
      void setIsLeftPhase(bool b) {
         isLeftPhase = b;
      }
      void setWalkCycle(WalkCycle w) {
         this->walkCycle = w;
      }

      WalkCycle getWalkCycle() {
         return walkCycle;
      }

      void setIsStopped(bool b) {
         isStopped = b;
      }

      bool getIsStopped() {
         return isStopped;
      }

      PendulumModel getPendulumModel() {
         return pendulumModel;
      }

      float getFootZMP(bool isLeft, const SensorValues &sensors);
      float getHorizontalFootZMP(bool isLeft, const SensorValues &sensors);

      bool isFootOnGround(const SensorValues &sensors);

      void processUpdate(Odometry *odometry, const SensorValues &sensors);
      void observationUpdate(Odometry *odometry, const SensorValues &sensors);
      void simulationUpdate();

      Kinematics *kinematics;
      PendulumModel pendulumModel;
      PendulumModel lastPendulumModel;


   private:
      WalkCycle walkCycle;

      bool isStopped;
      float currentfrequency;
      int stepCounter;
      std::list<float> lastStepFrequencyDifferences;
      bool isBetweenSteps;
      bool isPaused;
      float sagittalAcceleration;
      float forwardL, forwardR, turnL, turnR;
      float pressureL, pressureR;
      float filAccX;
      float filAccY;
      float filteredTotalPressure;
      float ZMPF;
      float filHighZMPF;
      float filLowZMPF;
      float ZMPL;
      float filZMPL;
      float lastZMPL;
      float fsLfr, fsLfl, fsLrr, fsLrl;                       // Maximum foot sensor reading during latest run
      float fsRfr, fsRfl, fsRrr, fsRrl;                       // ... used to scale each foot sensor to read in similar range

      XYZ_Coord centreOfMass;
      XYZ_Coord centreOfMassOther; // other foot

      static const float COM_HEIGHT;
      static const float MOTION_DT;
      static const float FOOT_LENGTH;
      static const float L_RATE;

      bool isLeftPhase;
      bool isOnFront;
      bool lastIsLeftPhase;

      // real body model
      float t;

      bool isOnFrontOfFoot(const SensorValues &sensor);
};

