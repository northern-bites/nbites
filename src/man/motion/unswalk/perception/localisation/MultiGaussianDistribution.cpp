/*
Copyright 2013 The University of New South Wales (UNSW).

This file is part of the 2010 team rUNSWift RoboCup entry. You may
redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version as
modified below. As the original licensors, we add the following
conditions to that license:

In paragraph 2.b), the phrase "distribute or publish" should be
interpreted to include entry into a competition, and hence the source
of any derived work entered into a competition must be made available
to all parties involved in that competition under the terms of this
license.

In addition, if the authors of a derived work publish any conference
proceedings, journal articles or other academic papers describing that
derived work, then appropriate academic citations to the original work
must be included in that publication.

This rUNSWift source is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License along
with this source code; if not, write to the Free Software Foundation,
Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include "MultiGaussianDistribution.hpp"
#include "LocalisationConstantsProvider.hpp"
#include "SimpleGaussian.hpp"
#include "VisionUpdateBundle.hpp"
#include "types/Odometry.hpp"
#include "LocalisationUtils.hpp"
#include "utils/Logger.hpp"
#include "utils/speech.hpp"

#include <cassert>
#include <vector>
#include <algorithm>

#include "Eigen/Geometry"
#include "Eigen/LU"

using namespace Eigen;

static const LocalisationConstantsProvider& constantsProvider(
      LocalisationConstantsProvider::instance());

/**
 * Comparison functor for sorting the list of Gaussians into descending order by weight.
 */
struct GuassianSortFunction {
   bool operator()(const SimpleGaussian *lhs, const SimpleGaussian *rhs) {
      return lhs->getWeight() > rhs->getWeight();
   }
};

MultiGaussianDistribution::MultiGaussianDistribution(unsigned maxGaussians, int playerNumber) :
      maxGaussians(maxGaussians), playerNumber(playerNumber) {
   MY_ASSERT(maxGaussians > 0, "invalid number of maxGaussians");
   resetDistributionToPenalisedPose();
   lastObservationLikelyhood = 0.00001;
   
   isInReadyMode = false;
   haveSeenLandmarks = false;
   numVisionUpdatesInReady = 0;
}

MultiGaussianDistribution::~MultiGaussianDistribution() {
   for (unsigned i = 0; i < modes.size(); i++) {
      delete modes[i];
   }
}

void MultiGaussianDistribution::resetDistributionToPenalisedPose(void) {
   Eigen::MatrixXd mean(MAIN_DIM, 1);
   // Standing on the left field edge in our half looking inward.
   if (modes.size() == 0) {
      mean << -2.0*FIELD_LENGTH/6.0, FIELD_WIDTH/2.0, -M_PI/2.0,
              0.0, 0.0,
              0.0, 0.0,
              // TODO: this may not be the optimal thing to do for teammate poses. Maybe copy over
              // the pose from the top Gaussian if it is available?
              0.0, 0.0, 0.0, // Teammates robot poses.
              0.0, 0.0, 0.0,
              0.0, 0.0, 0.0,
              0.0, 0.0, 0.0;
   } else {
      mean = modes.front()->getMean();
      mean(0, 0) = -FIELD_LENGTH/4.0;
      mean(1, 0) = FIELD_WIDTH/2.0;
      mean(2, 0) = -M_PI/2.0;
   }

   Eigen::MatrixXd diagonalVariance(MAIN_DIM, 1);
   diagonalVariance << get95CF(FULL_FIELD_LENGTH/4.0), get95CF(FULL_FIELD_WIDTH/4.0), get95CF(M_PI / 4.0),
                       get95CF(10.0*FULL_FIELD_LENGTH), get95CF(10.0*FULL_FIELD_WIDTH),
                       get95CF(10000.0), get95CF(10000.0),
                       // Teammates
                       get95CF(FULL_FIELD_LENGTH), get95CF(FULL_FIELD_WIDTH), get95CF(M_PI),
                       get95CF(FULL_FIELD_LENGTH), get95CF(FULL_FIELD_WIDTH), get95CF(M_PI),
                       get95CF(FULL_FIELD_LENGTH), get95CF(FULL_FIELD_WIDTH), get95CF(M_PI),
                       get95CF(FULL_FIELD_LENGTH), get95CF(FULL_FIELD_WIDTH), get95CF(M_PI);

   modes.clear();
   modes.push_back(new SimpleGaussian(MAIN_DIM, 1.0/4.0, mean, diagonalVariance));
   
   mean(1, 0) *= -1.0;
   mean(2, 0) *= -1.0;
   modes.push_back(new SimpleGaussian(MAIN_DIM, 1.0/4.0, mean, diagonalVariance));
   
   
   mean(0, 0) += FULL_FIELD_LENGTH/6.0;
   modes.push_back(new SimpleGaussian(MAIN_DIM, 1.0/4.0, mean, diagonalVariance));
   
   mean(1, 0) *= -1.0;
   mean(2, 0) *= -1.0;
   modes.push_back(new SimpleGaussian(MAIN_DIM, 1.0/4.0, mean, diagonalVariance));


   mean(0, 0) = -800;
   mean(1, 0) = 0;
   mean(2, 0) = 0;
   modes.push_back(new SimpleGaussian(MAIN_DIM, 1.0/4.0, mean, diagonalVariance));

   mean(0, 0) = -3500;
   mean(1, 0) = 0;
   mean(2, 0) = 0;
   modes.push_back(new SimpleGaussian(MAIN_DIM, 1.0/4.0, mean, diagonalVariance));
}

void MultiGaussianDistribution::resetToPenaltyShootout(void) {
   Eigen::MatrixXd mean(MAIN_DIM, 1);
   // Standing on the left field edge in our half looking inward.
   if (playerNumber == 1) { 
      mean << -FIELD_LENGTH/2.0, 0.0, 0.0, // If youre the goalie, in goalbox facing opponent
              0.0, 0.0,
              0.0, 0.0,
              
              0.0, 0.0, 0.0, // Teammates robot poses.
              0.0, 0.0, 0.0,
              0.0, 0.0, 0.0,
              0.0, 0.0, 0.0;
   } else {
      mean << 2200.0, 0.0, 0.0, // centre of the field, facing opposition goal.
              3200.0, 0.0, // 2m in front of you for the ball.
              0.0, 0.0,
              
              0.0, 0.0, 0.0, // Teammates robot poses.
              0.0, 0.0, 0.0,
              0.0, 0.0, 0.0,
              0.0, 0.0, 0.0;
   }

   Eigen::MatrixXd diagonalVariance(MAIN_DIM, 1);
   diagonalVariance << get95CF(FULL_FIELD_WIDTH/4.0), get95CF(FULL_FIELD_WIDTH/4.0), get95CF(M_PI / 8.0),
                       get95CF(10.0*FULL_FIELD_WIDTH), get95CF(10.0*FULL_FIELD_WIDTH),
                       get95CF(10000.0), get95CF(10000.0),
                       // Teammates
                       get95CF(FULL_FIELD_LENGTH), get95CF(FULL_FIELD_WIDTH), get95CF(M_PI),
                       get95CF(FULL_FIELD_LENGTH), get95CF(FULL_FIELD_WIDTH), get95CF(M_PI),
                       get95CF(FULL_FIELD_LENGTH), get95CF(FULL_FIELD_WIDTH), get95CF(M_PI),
                       get95CF(FULL_FIELD_LENGTH), get95CF(FULL_FIELD_WIDTH), get95CF(M_PI);

   modes.clear();
   modes.push_back(new SimpleGaussian(MAIN_DIM, 1.0, mean, diagonalVariance));
}

void MultiGaussianDistribution::setLineUpMode(bool enabled) {
   for (unsigned i = 0; i < modes.size(); i++) {
      modes[i]->setLineUpMode(enabled);
   }
}

void MultiGaussianDistribution::setReadyMode(bool enabled) {  
   isInReadyMode = enabled;
   if (!isInReadyMode) {
      numVisionUpdatesInReady = 0;
      haveSeenLandmarks = false;
   }
   
   for (unsigned i = 0; i < modes.size(); i++) {
      modes[i]->setReadyMode(enabled);
   }
}

void MultiGaussianDistribution::startOfPlayReset(void) {
   // All other modes other than the best mode that are in the opponent half are zeroed out.
   for (unsigned i = 1; i < modes.size(); i++) {
      if (modes[i]->getRobotPose().x() > 0.0) {
         modes[i]->setWeight(0.0);
      }
   }
   
   AbsCoord robotPose = modes.front()->getRobotPose();
   
   // If we are way inside the opponent half or we are just inside the opponent half and think we
   // are facing our own goal, then we are symmetrically flipped. NOTE: we dont want to blindly
   // zero out all modes that are in the opposing half because it is possible that we are 
   // actually in our own half but due to noise the state estimation places us just over the
   // halfway line. This is quite possible for robots standing near the halfway line.
   if (robotPose.x() > 1000.0 || (robotPose.x() > 0.0 && fabs(robotPose.theta()) > M_PI/2.0)) {
      SimpleGaussian *flippedMode = modes.front()->createSymmetricGaussian();
      modes.front()->setWeight(0.0);
      modes.push_back(flippedMode);
   }
   
   fixupDistribution();
}

void MultiGaussianDistribution::processUpdate(const Odometry &odometry, const double dTimeSeconds,
      const bool canSeeBall) {
   MY_ASSERT(checkValidDistribution(modes), "invalid distribution @ processUpdate start");
   for (unsigned i = 0; i < modes.size(); i++) {
      modes[i]->processUpdate(odometry, dTimeSeconds, canSeeBall);
   }
   MY_ASSERT(checkValidDistribution(modes), "invalid distribution @ processUpdate end");
}

void MultiGaussianDistribution::visionUpdate(const VisionUpdateBundle &visionBundle) {
   MY_ASSERT(checkValidDistribution(modes), "invalid distribution @ visionUpdate start");
   
   teamBallTracker.decay();
   haveSeenLandmarks = visionBundle.posts.size() > 0;
   
   if (isInReadyMode) {
      numVisionUpdatesInReady++;
   }

   lastObservationLikelyhood = -1.0;
   
   std::vector<SimpleGaussian*> allNewModes;
   for (unsigned i = 0; i < modes.size(); i++) {
      std::vector<SimpleGaussian*> newModes = modes[i]->visionUpdate(visionBundle);
      allNewModes.insert(allNewModes.end(), newModes.begin(), newModes.end());

      if (i == 0 && (visionBundle.fieldFeatures.size() > 0 || visionBundle.posts.size() > 0 ||
            visionBundle.visibleBalls.size() > 0)) {
         for (unsigned j = 0; j < newModes.size(); j++) {
            double weightAdjustment = newModes[j]->getWeight();
            if (weightAdjustment > lastObservationLikelyhood) {
               lastObservationLikelyhood = weightAdjustment;
            }
         }
      }
   }
   
   modes.insert(modes.end(), allNewModes.begin(), allNewModes.end());
   
   // This is a bit of a dodgy hack that performs ICP if we are in "initial state". Initial state refers
   // to the first few frames after booting up. We want to do this to better disambiguate which side of the
   // field we are on so we dont flip sides during the very first ready state.
   if (isInInitialState() && haveSeenLandmarks) {
      for (unsigned i = 0; i < modes.size(); i++) {
         if (modes[i]->getHaveLastVisionUpdate()) {
            modes[i]->doICPUpdate(visionBundle, true);
         }
      }
   }
   
   // Make sure that the goalie is always in its own half.
   if (playerNumber == 1) {
      for (unsigned i = 0; i < modes.size(); i++) {
         if (modes[i]->getRobotPose().x() > 0.0) {
            modes[i]->weight *= 0.00001;
         }
      }
   }

   fixupDistribution();
   
   // Perform ICP on the top mode. We perform the ICP update and see how well it matches the current
   // state by checking the weight of the update. If the weight is below some threshold, then the 
   // ICP update is probably invalid and it is rejected. 
   // NOTE: we do this last bit only because we cannot afford to perform ICP on all modes. If we could,
   // then we would simply perform ICP on all modes with their weights being updated.
   if (!isInInitialState()) {
      if (!modes.empty() && modes.front()->getHaveLastVisionUpdate()) {
         SimpleGaussian *noICPMode = modes.front()->createSplitGaussian();
         double invalidICPProbability = 
               constantsProvider.get(LocalisationConstantsProvider::INVALID_ICP_PROBABILITY);         

         double icpWeight = modes.front()->doICPUpdate(visionBundle, false);
         if (icpWeight < invalidICPProbability) {
            modes.front()->setWeight(0.0);
            modes.push_back(noICPMode);
         } else {
            delete noICPMode;
         }
         
         fixupDistribution();
      }
   }
   
   if (modes.empty()) {
      std::cout << "Distribution is empty! This is bad, resetting to initial state to recover" << std::endl;
      resetDistributionToPenalisedPose();
   }

   MY_ASSERT(checkValidDistribution(modes), "invalid distribution @ visionUpdate end");
}

void MultiGaussianDistribution::applyRemoteUpdate(
      const BroadcastData &broadcastData, int teammateIndex, bool isFromGoalie) {
   MY_ASSERT(checkValidDistribution(modes), "invalid distribution @ remoteUpdate start");
   
   if (broadcastData.sharedLocalisationBundle.haveBallUpdates) {
      addSymmetricMode(modes);
   }

   std::vector<SimpleGaussian*> allNewModes;
   bool amIGoalie = (playerNumber == 1);
   for(unsigned i = 0; i < modes.size(); i++) {
      std::vector<SimpleGaussian*> newModes = modes[i]->applyRemoteUpdate(
            broadcastData.sharedLocalisationBundle, teammateIndex, amIGoalie);
      
      allNewModes.insert(allNewModes.end(), newModes.begin(), newModes.end());
   }
   modes.insert(modes.end(), allNewModes.begin(), allNewModes.end());
   
   if (!amIGoalie) { // We dont want teammates to try flipping the goalie.
      teamBallTracker.addTeammateObservation(broadcastData);
      for (unsigned i = 0; i < modes.size(); i++) {
         if (teamBallTracker.isModeFlipped(*modes[i])) {
            if (i == 0) {
               SAY("Teammates flipping me");
            }
            
            double weightScale = constantsProvider.get(LocalisationConstantsProvider::TEAMMATE_FLIP_WEIGHT);
            modes[i]->setWeight(modes[i]->getWeight() * weightScale);
         }
      }
   }
   
   fixupDistribution();
   MY_ASSERT(checkValidDistribution(modes), "invalid distribution @ remoteUpdate end");
}

const SimpleGaussian* MultiGaussianDistribution::getTopGaussian(void) const {
   MY_ASSERT(checkValidDistribution(modes), "invalid distribution @ getTopGaussian");
   return modes[0];
}

double MultiGaussianDistribution::getLastObservationLikelyhood(void) const {
   return lastObservationLikelyhood;
}

void MultiGaussianDistribution::doTeammateRobotVisionUpdate(const VisionUpdateBundle &visionBundle) {
   std::vector<SimpleGaussian*> allNewModes;
   for (unsigned i = 0; i < modes.size(); i++) {
      std::vector<SimpleGaussian*> newModes = modes[i]->visionTeammateRobotsUpdate(visionBundle);
      allNewModes.insert(allNewModes.end(), newModes.begin(), newModes.end());
   }
   
   modes.insert(modes.end(), allNewModes.begin(), allNewModes.end());
   fixupDistribution();
}

bool MultiGaussianDistribution::isInInitialState(void) {
   return isInReadyMode;// && numVisionUpdatesInReady < 120;
}

void MultiGaussianDistribution::fixupDistribution(void) {
   std::sort(modes.begin(), modes.end(), GuassianSortFunction());
   normaliseDistribution(modes);
   mergeSimilarModes(modes);
   std::sort(modes.begin(), modes.end(), GuassianSortFunction());
   normaliseDistribution(modes);
   removeUnlikelyModes(modes);
   removeExcessModes(modes, maxGaussians);
   normaliseDistribution(modes);
}

void MultiGaussianDistribution::normaliseDistribution(const std::vector<SimpleGaussian*> &distribution) {
   double weightSum = 0.0;
   for (unsigned i = 0; i < distribution.size(); i++) {
      weightSum += distribution[i]->getWeight();
   }

   // Make sure we dont divide by zero.
   double scale = 1.0 / weightSum;
   for (unsigned i = 0; i < distribution.size(); i++) {
      distribution[i]->setWeight(distribution[i]->getWeight() * scale);
   }
}

void MultiGaussianDistribution::mergeSimilarModes(const std::vector<SimpleGaussian*> &distribution) {
   for (unsigned i = 0; i < distribution.size() - 1; i++) {
      if (distribution[i]->getWeight() > 0.0) {
         std::vector<SimpleGaussian*> similarGaussians;
         
         for (unsigned j = i + 1; j < distribution.size(); j++) {
            if (distribution[i]->isSimilarTo(*distribution[j])) {
               similarGaussians.push_back(distribution[j]);
            }
         }
         
         for (unsigned j = 0; j < similarGaussians.size(); j++) {
            distribution[i]->mergeWith(*similarGaussians[j]);
         }
      }
   }
}

void MultiGaussianDistribution::removeUnlikelyModes(std::vector<SimpleGaussian*> &distribution) {
   MY_ASSERT(checkValidDistribution(distribution), "invalid distribution @ removeUnlikelyModes");

   double minWeight = constantsProvider.get(LocalisationConstantsProvider::MIN_MODE_WEIGHT);
   while (distribution.back()->getWeight() < minWeight && distribution.size() > 1) {
      delete distribution.back();
      distribution.pop_back();
   }
}

void MultiGaussianDistribution::removeExcessModes(
      std::vector<SimpleGaussian*> &distribution, unsigned maxGaussians) {
   MY_ASSERT(checkValidDistribution(distribution), "invalid distribution @ removeExcessModes");

   while (distribution.size() > maxGaussians) {
      delete distribution.back();
      distribution.pop_back();
   }
}

void MultiGaussianDistribution::addBaselineModes(std::vector<SimpleGaussian*> &distribution) {
   std::vector<SimpleGaussian*> baseline = SimpleGaussian::createBaselineGaussians();
   distribution.insert(distribution.end(), baseline.begin(), baseline.end());
}

void MultiGaussianDistribution::addSymmetricMode(std::vector<SimpleGaussian*> &distribution) {
   AbsCoord ballPos = distribution.front()->getBallPosition();
   double fromCentre = sqrt(ballPos.x()*ballPos.x() + ballPos.y()*ballPos.y());
   
   // If the ball is close to the centre then a symmetric mode will be too close to the original mode
   // and hence provide spurious matches potentially.
   if (fromCentre >= 1300.0) { // TODO: make this a constant in the ConstantsProvider
      SimpleGaussian *symmetric = distribution.front()->createSymmetricGaussian();
      distribution.push_back(symmetric);
   }
}

bool MultiGaussianDistribution::checkValidDistribution(
      const std::vector<SimpleGaussian*> &distribution) {  
   if (distribution.size() == 0) {
      std::cout << "Invalid distribution, empty" << std::endl;
      return false;
   }

   for (unsigned i = 0; i < distribution.size(); i++) {
      if (distribution[i]->getWeight() < 0.0 || distribution[i]->getWeight() > 1.0) {
         std::cout << "Invalid distribution, weight invalid: " << distribution[i]->getWeight() << std::endl;
         return false;
      }

      // Assert that the modes are ordered in descending order of weight.
      if (i < (distribution.size() - 1) &&
          distribution[i]->getWeight() < distribution[i + 1]->getWeight()) {
         std::cout << "Invalid distribution, unordered" << std::endl;
         return false;
      }
   }

   return true;
}

void MultiGaussianDistribution::printModes() const {

   std::vector<SimpleGaussian*>::const_iterator it;
   int i = 0;
   for (it = modes.begin(); it != modes.end(); ++it) {
      std::cout << i << " : " << (*it)->getWeight() << " | ";
   }
   std::cout << std::endl;
}
