/*
 * BehaviourSharedData.hpp
 *
 *  Created on: 15/05/2014
 *      Author: osushkov
 */

#pragma once

#include "types/boostSerializationEigenTypes.hpp"
#include "perception/behaviour/ReadySkillPositionAllocation.hpp"

class BehaviourSharedData {
public:

   BehaviourSharedData() :
      goalieAttacking(false),
      timeToReachBall(10000.0f),
      timeToReachUpfielder(10000.0f),
      timeToReachMidfielder(10000.0f),
      timeToReachDefender(10000.0f),
      currentRole(0),
      doingBallLineUp(false),
      isInReadyMode(false) {}
   
   bool goalieAttacking;
   
   // The estimated time (in seconds) that this robot needs to get to the ball. This is 
   // the primary method that we use to decide which robot should be the Striker.
   float timeToReachBall;

   // Times to reach particular positions - used to role assignment
   float timeToReachUpfielder;
   float timeToReachMidfielder;
   float timeToReachDefender;
   
   // The encoded enum value for the robots current role.
   int currentRole;
   
   // Whether the robot is in the process of lining up with the ball to kick it.
   bool doingBallLineUp;

   bool isInReadyMode;
   
   // What this robot thinks the ready position allocation should be.
   ReadySkillPositionAllocation readyPositionAllocation;

   
   template<class Archive>
   void serialize(Archive &ar, const unsigned int file_version) {
      ar & goalieAttacking;
      ar & timeToReachBall;
      ar & timeToReachUpfielder;
      ar & timeToReachMidfielder;
      ar & timeToReachDefender;
      ar & currentRole;
      ar & doingBallLineUp;
      ar & isInReadyMode;
      ar & readyPositionAllocation;
   }
};
