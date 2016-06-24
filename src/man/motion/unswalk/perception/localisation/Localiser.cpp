#include <cassert>

#include "Localiser.hpp"
#include "LocalisationUtils.hpp"
#include "utils/Logger.hpp"
#include "utils/incapacitated.hpp"
#include "utils/speech.hpp"

static const int MAX_GAUSSIANS = 8;

Localiser::Localiser(int playerNumber) {
   this->myPlayerNumber = playerNumber;
   ballLostCount = 0;
   worldDistribution = new MultiGaussianDistribution(MAX_GAUSSIANS, playerNumber);
   sharedDistribution = new SharedDistribution();
}

Localiser::~Localiser() {
   delete worldDistribution;
   delete sharedDistribution;
}

void Localiser::setReset(void) {
   worldDistribution->resetDistributionToPenalisedPose();
   resetSharedUpdateData();
}

void Localiser::resetToPenaltyShootout(void) {
   worldDistribution->resetToPenaltyShootout();
}

void Localiser::setLineUpMode(bool enabled) {
   worldDistribution->setLineUpMode(enabled);
}

void Localiser::setReadyMode(bool enabled) {
   LocalisationConstantsProvider& constantsProvider(LocalisationConstantsProvider::instance());
   constantsProvider.setReadyMode(enabled);
   
   worldDistribution->setReadyMode(enabled);
   sharedDistribution->setReadyMode(enabled);
}

void Localiser::startOfPlayReset(void) {
   worldDistribution->startOfPlayReset();
   resetSharedUpdateData();
}

AbsCoord Localiser::getRobotPose(void) {
   assert(worldDistribution != NULL);
   return worldDistribution->getTopGaussian()->getRobotPose();
}

AbsCoord Localiser::getBallPosition(void) {
   assert(worldDistribution != NULL);
   return worldDistribution->getTopGaussian()->getBallPosition();
}

AbsCoord Localiser::getBallVelocity(void) {
   assert(worldDistribution != NULL);
   return worldDistribution->getTopGaussian()->getBallVelocity();
}

unsigned Localiser::getBallLostCount(void) {
   return ballLostCount;
}

double Localiser::getRobotPosUncertainty(void) const {
   return worldDistribution->getTopGaussian()->getRobotPosUncertainty();
}

double Localiser::getRobotHeadingUncertainty(void) const {
   return worldDistribution->getTopGaussian()->getRobotHeadingUncertainty();
}

double Localiser::getBallPosUncertainty(void) const {
   return worldDistribution->getTopGaussian()->getBallPosUncertainty();
}

double Localiser::getBallVelocityUncertainty(void) const {
   return worldDistribution->getTopGaussian()->getBallVelocityUncertainty();
}

SharedLocalisationUpdateBundle Localiser::getSharedUpdateData(void) const {
   return sharedDistribution->getBroadcastData();
}

void Localiser::resetSharedUpdateData(void) {
   const SimpleGaussian *topGaussian = worldDistribution->getTopGaussian();
   sharedDistribution->reset(topGaussian);
}

void Localiser::localise(const LocaliserBundle &lb, const bool canDoObservations) {
   if (lb.visionUpdateBundle.visibleBalls.size() > 1) {
      std::cout << "MULTIBALL!!" << std::endl;
   }
   
   bool canSeeBall = lb.visionUpdateBundle.visibleBalls.size() > 0;
   
   worldDistribution->processUpdate(lb.odometry, lb.dTimeSeconds, canSeeBall);
   if (canDoObservations) {
      worldDistribution->visionUpdate(lb.visionUpdateBundle);
   }
   
   sharedDistribution->processUpdate(lb.odometry, lb.dTimeSeconds, canSeeBall);
   if (canDoObservations && worldDistribution->getTopGaussian()->getHaveLastVisionUpdate()) {
      sharedDistribution->visionUpdate(
            worldDistribution->getTopGaussian()->getLastAppliedICPUpdate(),
            worldDistribution->getTopGaussian()->getLastAppliedVisionUpdate());
   }

   if (lb.visionUpdateBundle.visibleBalls.size() > 0 && canDoObservations) {
      ballLostCount = 0;
   } else {
      ballLostCount++;
   }
}

void Localiser::applyRemoteUpdate(const BroadcastData &broadcastData, int playerNumber) {
   if (playerNumber == myPlayerNumber || !broadcastData.sharedLocalisationBundle.isUpdateValid) {
      return;
   }

   MY_ASSERT_2(playerNumber >= 1 && playerNumber <= 5);
   
   // Convert the player number to a teammate index. There are two cases to handle, if the player number
   // greater than our own, or if it is less than our own.
   int teammateIndex = 0;
   if (playerNumber < myPlayerNumber) {
      teammateIndex = playerNumber - 1;
   } else {
      teammateIndex = playerNumber - 2;
   }
   
   MY_ASSERT(teammateIndex >= 0 && teammateIndex <= 3, "Invalid teammateIndex");
   worldDistribution->applyRemoteUpdate(broadcastData, teammateIndex, playerNumber == 1);
}

double Localiser::getLastObservationLikelyhood(void) const {
   return worldDistribution->getLastObservationLikelyhood();
}
