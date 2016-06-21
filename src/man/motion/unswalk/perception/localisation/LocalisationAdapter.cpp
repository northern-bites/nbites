#include <vector>
#include <algorithm>
#include <fstream>
#include "Localiser.hpp"
#include "LocalisationAdapter.hpp"
#include "LocalisationDefs.hpp"

#include "blackboard/Blackboard.hpp"
#include "utils/Logger.hpp"
#include "utils/incapacitated.hpp"
#include "utils/Timer.hpp"
#include "types/BroadcastData.hpp"
#include "types/BallInfo.hpp"
#include "gamecontroller/RoboCupGameControlData.hpp"
#include "perception/localisation/robotfilter/types/RobotFilterUpdate.hpp"

using namespace std;

LocalisationAdapter::LocalisationAdapter(Blackboard *bb) : Adapter(bb) {
   firstCycle = true;
   playerNumber = readFrom(gameController, player_number);
   prevTimestamp = 0;
   numFramesSinceSeen = 0;
   prevGameState = 0;
   isInPenaltyShootout = false;

   L = new Localiser(playerNumber);
   robotFilter = new RobotFilter();
   
   if (LOCALISATION_DEBUG) {
      logStream.open("localisation.log", std::ios::binary);
      logStream.rdbuf()->pubsetbuf(0,0);
   }
}

LocalisationAdapter::~LocalisationAdapter() {
   delete L;
   delete robotFilter;
   
   if (LOCALISATION_DEBUG) {
      logStream.close();
   }
}

unsigned totalFrames = 0;
unsigned framesBuckets[] = {0, 0, 0, 0, 0, 0}; // <10fps, <15fps, <20fps, <25fps, <30fps, >30fps

void logFPS(unsigned fps) {
   totalFrames++;
   if (fps <= 10) {
      framesBuckets[0]++;
   } else if (fps <= 15) {
      framesBuckets[1]++;
   } else if (fps <= 20) {
      framesBuckets[2]++;
   } else if (fps <=25) {
      framesBuckets[3]++;
   } else if (fps <= 30) {
      framesBuckets[4]++;
   } else if (fps > 30) {
      framesBuckets[5]++;
   }
   
   if (totalFrames % 60 == 0) {
      std::cout << std::endl;
      std::cout << "< 10: \t" << (100 * framesBuckets[0])/totalFrames << std::endl;
      std::cout << "< 15: \t" << (100 * framesBuckets[1])/totalFrames << std::endl;
      std::cout << "< 20: \t" << (100 * framesBuckets[2])/totalFrames << std::endl;
      std::cout << "< 25: \t" << (100 * framesBuckets[3])/totalFrames << std::endl;
      std::cout << "< 30: \t" << (100 * framesBuckets[4])/totalFrames << std::endl;
      std::cout << "> 30: \t" << (100 * framesBuckets[5])/totalFrames << std::endl;
   }
}

void LocalisationAdapter::tick() {
   llog(DEBUG1) << "Localisation.. ticking away" << endl;
   const ActionCommand::All active = readFrom(motion, active);

   /* calculate odometry */
   //const Odometry &newOdometry = readFrom(vision, dualOdometry);
   const Odometry &newOdometry = readFrom(motion, odometry);

   Odometry odometryDiff;
   if (!firstCycle) {
      odometryDiff = (newOdometry - prevOdometry);
   }
   prevOdometry = newOdometry;
   
   int64_t timestamp = readFrom(vision, timestamp);
   double dTimeSeconds = 0.0;
   if (!firstCycle) {
      // timestamp is in micro-seconds, hence the 1e6.
      dTimeSeconds = (timestamp - prevTimestamp) / 1000000.0;
      
      //unsigned fps = 1.0 / dTimeSeconds;
      //logFPS(fps);
   }
   prevTimestamp = timestamp;
   
   UNSWSensorValues values = readFrom(kinematics, sensorsLagged);
   VisionUpdateBundle visionUpdateBundle(
         readFrom(vision, fieldEdges),
         readFrom(vision, fieldFeatures),
         readFrom(vision, posts),
         readFrom(vision, robots),
         getTeammatePoses(),
         readFrom(gameController, team_red),
         readFrom(vision, awayGoalProb),
         values.joints.angles[Joints::HeadYaw],
         readFrom(vision, balls),
         !amTurningHead(active),
         !amWalking(active));
   
   if (haveTransitionedSetToPlay() && readFrom(gameController, data).secondaryState != STATE2_PENALTYSHOOT) {
      L->startOfPlayReset();
   }
   
   bool doingBallLineUp = readFrom(behaviour, behaviourSharedData).doingBallLineUp;
   L->setLineUpMode(doingBallLineUp);
   
   bool isInReadyMode = readFrom(behaviour, behaviourSharedData).isInReadyMode;
   L->setReadyMode(isInReadyMode);
   
   //handleObservationMeasurements(visionUpdateBundle);
   
   bool isPenalised = 
         readFrom(gameController, our_team).players[playerNumber - 1].penalty != PENALTY_NONE;

   handleMySharedDistribution();
   
   /* update our own localisation */
   LocaliserBundle localiserBundle(
         odometryDiff,
         visionUpdateBundle,
         active,
         isPenalised,
         readFrom(gameController, data).state,
         readFrom(gameController, data).secondaryState,
         dTimeSeconds);

   if (LOCALISATION_DEBUG) {
      logStream << localiserBundle;
      logStream.flush();
   }

   if (isPenalised) {
      if (isInPenaltyShootout) {
         L->resetToPenaltyShootout();
      } else {
         L->setReset();
      }
   } else if (canLocaliseInState(readFrom(gameController, data).state, readFrom(behaviour, skill))) {
      L->localise(localiserBundle, canDoObservations());
   }

   if (canLocaliseInState(readFrom(gameController, data).state, readFrom(behaviour, skill)) &&
       !isInPenaltyShootout) {
      handleIncomingSharedUpdate();
   }
   RobotFilterUpdate update;
   update.visualRobots = readFrom(vision, robots);
   update.robotPos = readFrom(localisation, robotPos);
   update.headYaw = readFrom(motion, sensors).joints.angles[Joints::HeadYaw];
   update.odometryDiff = odometryDiff;
   update.isIncapacitated = isIncapacitated(active.body.actionType);

   robotFilter->update(update);

   writeResultToBlackboard();
   
   firstCycle = false;
   
   if (haveTransitionedIntoSet() && readFrom(gameController, data).secondaryState == STATE2_PENALTYSHOOT) {
      isInPenaltyShootout = true;
      L->resetToPenaltyShootout();
   }

//   if (LOCALISATION_DEBUG) {
//      logStream << localiserBundle;
//   }
   
   prevGameState = readFrom(gameController, data).state;
}

bool LocalisationAdapter::canLocaliseInState(uint8_t state, std::string skill) {
   if (skill == "GameController") {
      return state == STATE_READY || state == STATE_SET || state == STATE_PLAYING;
   } else {
      return true;
   }
}

bool LocalisationAdapter::canDoObservations(void) {
   ActionCommand::Body::ActionType currentAction = readFrom(motion, active).body.actionType;
   
   bool isPickedUp = (currentAction == ActionCommand::Body::REF_PICKUP);
   bool isDiving = (currentAction == ActionCommand::Body::GOALIE_DIVE_LEFT || currentAction == ActionCommand::Body::GOALIE_DIVE_RIGHT);
   bool isDead = (currentAction == ActionCommand::Body::DEAD);
   bool isGettingUp = 
         (currentAction == ActionCommand::Body::GETUP_FRONT) || (currentAction == ActionCommand::Body::GETUP_BACK);
   
   // TODO: is there a "I am falling" state?
   return !isPickedUp && !isDiving && !isGettingUp && !isDead;
}

bool LocalisationAdapter::haveTransitionedSetToPlay(void) {
   uint8_t curGameState = readFrom(gameController, data).state;
   return prevGameState == STATE_SET && curGameState == STATE_PLAYING;
}

bool LocalisationAdapter::haveTransitionedIntoSet(void) {
   uint8_t curGameState = readFrom(gameController, data).state;
   return prevGameState != STATE_SET && curGameState == STATE_SET;
}

std::vector<AbsCoord> LocalisationAdapter::getTeammatePoses(void) {
   const BroadcastData *teamData = readFrom(receiver, data);
   const bool *incapacitated = readFrom(receiver, incapacitated);
   
   std::vector<AbsCoord> result;
   for (unsigned i = 0; i < ROBOTS_PER_TEAM; i++) {
      if (i != (playerNumber - 1) && 
          !incapacitated[i] &&
          teamData[i].uptime > 1.0f &&
          teamData[i].acB != Body::DEAD && 
          teamData[i].acB != Body::REF_PICKUP) {
         result.push_back(teamData[i].robotPos);
      }
   }
   return result;
}

bool LocalisationAdapter::amWalking(const ActionCommand::All &commands) {
   return abs(commands.body.forward) > 0 || abs(commands.body.left) > 0 || abs(commands.body.turn) > 0;
}

bool LocalisationAdapter::amTurningHead(const ActionCommand::All &commands) {
   return abs(commands.head.yawSpeed) > 0.1;
}

void LocalisationAdapter::writeResultToBlackboard(void) {
   AbsCoord robotPos = L->getRobotPose();
   AbsCoord ballPosition = L->getBallPosition();
   RRCoord ballPosRR = ballPosition.convertToRobotRelative(robotPos);
   AbsCoord ballPosRRC = ballPosition.convertToRobotRelativeCartesian(robotPos);
   AbsCoord ballVelocity = L->getBallVelocity();
   
   double robotPosUncertainty = L->getRobotPosUncertainty();
   double robotHeadingUncertainty = L->getRobotHeadingUncertainty();
   double ballPosUncertainty = L->getBallPosUncertainty();
   double ballVelEigenvalue = L->getBallVelocityUncertainty();
   
   AbsCoord nextBall(ballPosition.x() + ballVelocity.x(), ballPosition.y() + ballVelocity.y(), 0.0f);
   AbsCoord nextBallRRC = nextBall.convertToRobotRelativeCartesian(robotPos);
   AbsCoord ballVelRRC(nextBallRRC.x() - ballPosRRC.x(), nextBallRRC.y() - ballPosRRC.y(), 0.0f);
   
   Pose pose = readFrom(motion, pose);
   XYZ_Coord ballNeckRelative = pose.robotRelativeToNeckCoord(ballPosRR, BALL_RADIUS);
   
   uint32_t ballLostCount = L->getBallLostCount();
   
   SharedLocalisationUpdateBundle sharedLocalisationBundle = L->getSharedUpdateData();

   acquireLock(serialization);
   writeTo(localisation, robotPos, robotPos);
   writeTo(localisation, ballLostCount, ballLostCount);
   writeTo(localisation, ballPos, ballPosition);
   writeTo(localisation, ballPosRR, ballPosRR);
   writeTo(localisation, ballPosRRC, ballPosRRC);
   writeTo(localisation, ballVelRRC, ballVelRRC);
   writeTo(localisation, ballVel, ballVelocity);
   writeTo(localisation, robotPosUncertainty, robotPosUncertainty);
   writeTo(localisation, robotHeadingUncertainty, robotHeadingUncertainty);
   writeTo(localisation, ballPosUncertainty, ballPosUncertainty);
   writeTo(localisation, ballVelEigenvalue, ballVelEigenvalue);
   writeTo(localisation, teamBall, TeamBallInfo()); // TODO: make this a different value?
   writeTo(localisation, ballNeckRelative, ballNeckRelative);
   writeTo(localisation, sharedLocalisationBundle, sharedLocalisationBundle);
   writeTo(localisation, havePendingOutgoingSharedBundle, true);
   writeTo(localisation, havePendingIncomingSharedBundle, std::vector<bool>(5, false));
   writeTo(localisation, robotObstacles, robotFilter->filteredRobots);
   releaseLock(serialization);
}

void LocalisationAdapter::handleMySharedDistribution(void) {
   if (!readFrom(localisation, havePendingOutgoingSharedBundle)) {
      L->resetSharedUpdateData();
   }
}

void LocalisationAdapter::handleIncomingSharedUpdate(void) {
   std::vector<bool> incomingSharedUpdates = readFrom(localisation, havePendingIncomingSharedBundle);
   for (unsigned i = 0; i < incomingSharedUpdates.size(); i++) {
      if (incomingSharedUpdates[i]) {
         BroadcastData incomingData = readFrom(receiver, data)[i];
         L->applyRemoteUpdate(incomingData, incomingData.playerNum);
      }
   }
}

/**
 * All of the code below is used for gathering observation variance data. We basically have the 
 * robot stand and observe say a ball, average out the observations, and calculate the variance.
 * We then cover the robots vision, move the ball, and uncover the vision. This indicates a new
 * round and we do the same thing but now with a different ball distance/heading. This data allows
 * us to use an offline util to fit a function that maps observation distance to variance.
 */
// TODO(sushkov): more detailed explanation of the below code.
void LocalisationAdapter::handleObservationMeasurements(const VisionUpdateBundle &visionUpdateBundle) {
   if (visionUpdateBundle.visibleBalls.size() == 0) {
      numFramesSinceSeen++;
      if (numFramesSinceSeen > 150) {
         if (distanceObservations.size() > 100) {
            outputVariances();
         }
         
         numFramesSinceSeen = 0;
         distanceObservations.clear();
         headingObservations.clear();
      }
   } else {
      numFramesSinceSeen = 0;
      distanceObservations.push_back(visionUpdateBundle.visibleBalls[0].rr.distance());
      headingObservations.push_back(visionUpdateBundle.visibleBalls[0].rr.heading());
   }
}

void LocalisationAdapter::outputVariances(void) {
   std::sort(distanceObservations.begin(), distanceObservations.end());
   std::sort(headingObservations.begin(), headingObservations.end());
   
   double outliersRatio = 0.1;
   
   double distanceMean = getMean(distanceObservations, outliersRatio);
   double distanceVariances = getVariance(distanceObservations, distanceMean, outliersRatio);
   
   double headingMean = getMean(headingObservations, outliersRatio);
   double headingVariance = getVariance(headingObservations, headingMean, outliersRatio);
   
   std::cout << "variance obs: " << distanceMean << " " << distanceVariances << " " 
         << headingMean << " " << headingVariance << std::endl;
}

double LocalisationAdapter::getMean(const std::vector<double> &vals, double outliersRatio) {
   int outliers = vals.size() * outliersRatio;
   double sum = 0.0;
   int count = 0;
   
   for (unsigned i = outliers; i < vals.size() - outliers; i++) {
      count++;
      sum += vals[i];
   }
   
   return sum / count;
}

double LocalisationAdapter::getVariance(const std::vector<double> &vals, double mean,
      double outliersRatio) {
   int outliers = vals.size() * outliersRatio;
   double sum = 0.0;
   int count = 0;
   
   for (unsigned i = outliers; i < vals.size() - outliers; i++) {
      count++;
      double diff = vals[i] - mean;
      sum += diff * diff;
   }
   
   return sum / count;
}
