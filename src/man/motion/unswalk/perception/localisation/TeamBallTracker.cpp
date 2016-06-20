/*
 * TeamBallTracker.cpp
 *
 *  Created on: 06/07/2014
 *      Author: osushkov
 */

#include "TeamBallTracker.hpp"
#include <cmath>

static const double BALL_CERTAINTY_DAMPING = 0.5;
static const double MIN_CERTAINTY_TO_FLIP = 0.75;
static const double MAX_BALL_DISTANCE_MATCH = 1000.0;
static const double MIN_BALL_CENTRE_DISTANCE = 1500.0;

TeammateBallState::TeammateBallState() {
   ballCertainty = 0.0;
   ballPosition = AbsCoord(0.0, 0.0, 0.0);
   lastUpdateAge = 0;
}

TeamBallTracker::TeamBallTracker() {
   
}

void TeamBallTracker::decay(void) {
   for (unsigned i = 0; i < NUM_PLAYERS_ON_TEAM; i++) {
      teammateBallState[i].lastUpdateAge++;
   }
}

void TeamBallTracker::addTeammateObservation(const BroadcastData &broadcastData) {
   int index = broadcastData.playerNum - 1;
   double ballSeenFraction = broadcastData.sharedLocalisationBundle.ballSeenFraction;
   
   teammateBallState[index].ballCertainty = 
         BALL_CERTAINTY_DAMPING * teammateBallState[index].ballCertainty + 
         (1.0 - BALL_CERTAINTY_DAMPING) * ballSeenFraction;
   
   teammateBallState[index].ballPosition = broadcastData.ballPosAbs;
   teammateBallState[index].lastUpdateAge = 0;
}

bool TeamBallTracker::isModeFlipped(const SimpleGaussian &mode) {
   unsigned numRobotsWantToFlip = 0;
   unsigned numRobotsAgree = 0;
   
   AbsCoord myBallPos = mode.getBallPosition();
   AbsCoord flippedBallPos(-myBallPos.x(), -myBallPos.y(), 0.0);
   
   // If the ball is too close to the centre of the field, we cant reliably tell if youre flipped.
   if (distanceBetween(myBallPos, AbsCoord(0.0, 0.0, 0.0)) < MIN_BALL_CENTRE_DISTANCE) {
      return false;
   }
   
   for (unsigned i = 0; i < NUM_PLAYERS_ON_TEAM; i++) {
      if (teammateBallState[i].ballCertainty > MIN_CERTAINTY_TO_FLIP &&
          teammateBallState[i].lastUpdateAge < 60) {
         
         if (distanceBetween(teammateBallState[i].ballPosition, myBallPos) < MAX_BALL_DISTANCE_MATCH) {
            numRobotsAgree += (i == 0) ? 3 : 1; // Goalie counts triple!
         } else if (distanceBetween(teammateBallState[i].ballPosition, flippedBallPos) < MAX_BALL_DISTANCE_MATCH) {
            numRobotsWantToFlip += (i == 0) ? 3 : 1; // Goalie counts triple!
         }

      }
   }
   
   return numRobotsWantToFlip >= 2 && numRobotsWantToFlip >= (2 * numRobotsAgree);
}

double TeamBallTracker::distanceBetween(const AbsCoord &coord0, const AbsCoord &coord1) {
   double dx = coord0.x() - coord1.x();
   double dy = coord0.y() - coord1.y();
   return sqrt(dx*dx + dy*dy);
}

