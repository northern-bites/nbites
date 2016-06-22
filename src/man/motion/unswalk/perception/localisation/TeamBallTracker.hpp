/*
 * TeamBallTracker.hpp
 *
 *  Created on: 06/07/2014
 *      Author: osushkov
 */

#pragma once

#include "types/BroadcastData.hpp"
#include "types/AbsCoord.hpp"
#include "SimpleGaussian.hpp"

#define NUM_PLAYERS_ON_TEAM 4

struct TeammateBallState {
   TeammateBallState();
   
   double ballCertainty; // between 0 and 1 and how often it has seen the ball as a ratio of frames.
   AbsCoord ballPosition;
   unsigned lastUpdateAge; // in frames.
};

/**
 * This class is meant for tracking where each teammate thinks the ball is on the field.
 * The purpose of this is to explicitly flip a robot if its tracked ball position is 
 * symmetrically opposite where the majority of teammates think the ball is located.
 */
class TeamBallTracker {
public:
   TeamBallTracker();

   void decay(void);
   void addTeammateObservation(const BroadcastData &broadcastData);
   bool isModeFlipped(const SimpleGaussian &mode);
   
private:
   
   TeammateBallState teammateBallState[NUM_PLAYERS_ON_TEAM]; 
   
   double distanceBetween(const AbsCoord &coord0, const AbsCoord &coord1);
};
