#include "SimpleGaussian.hpp"

#include "Eigen/Geometry"
#include "Eigen/LU"

#include "LocalisationConstantsProvider.hpp"
#include "LocalisationUtils.hpp"
#include "LocalisationDefs.hpp"
#include "SharedLocalisationUpdateBundle.hpp"
#include "VarianceProvider.hpp"
#include "utils/basic_maths.hpp"
#include "utils/Logger.hpp"
#include "utils/SPLDefs.hpp"
#include "utils/angles.hpp"

#include <cmath>
#include <iomanip>
#include <ios>
#include <cstdio>

using namespace Eigen;

// TODO: either move this into a utilities header or use a common epsilon value.
static const double EPSILON = 0.0001;
static const double MAX_BALL_VELOCITY = 1000.0;

// The max measurement dimension. This corresponds to seeing two goal posts, a ball, and the centre 
// circle, with each contributing a heading and a distance.
static const int MAX_MEASUREMENT_DIM = 13;

static const LocalisationConstantsProvider& constantsProvider(
      LocalisationConstantsProvider::instance());

// These are the dimension indices for the robot and ball coordinates in the mean vector.
static const int ROBOT_X_DIM = 0;
static const int ROBOT_Y_DIM = 1;
static const int ROBOT_H_DIM = 2;

static const int BALL_X_DIM = 3;
static const int BALL_Y_DIM = 4;
static const int BALL_DX_DIM = 5;
static const int BALL_DY_DIM = 6;

static const int TEAM_INDEX_OFFSET = 7;
static const int NUM_TEAMMATES = 4;

static double complexMagnitude(double real, double imaginary) {
   return sqrt(real*real + imaginary*imaginary);
}

static MatrixXd getResetDiagonalVariance(const unsigned dim) {
   MatrixXd diagonalVariance(dim, 1);
   if (dim == MAIN_DIM) {
      diagonalVariance << get95CF(FULL_FIELD_LENGTH), get95CF(FULL_FIELD_WIDTH), get95CF(M_PI),
                          get95CF(10.0*FULL_FIELD_LENGTH), get95CF(10.0*FULL_FIELD_WIDTH),
                          get95CF(10000.0), get95CF(10000.0),
                          // Teammates
                          get95CF(FULL_FIELD_LENGTH), get95CF(FULL_FIELD_WIDTH), get95CF(M_PI),
                          get95CF(FULL_FIELD_LENGTH), get95CF(FULL_FIELD_WIDTH), get95CF(M_PI),
                          get95CF(FULL_FIELD_LENGTH), get95CF(FULL_FIELD_WIDTH), get95CF(M_PI),
                          get95CF(FULL_FIELD_LENGTH), get95CF(FULL_FIELD_WIDTH), get95CF(M_PI);
   } else if (dim == SHARED_DIM) {
      diagonalVariance << get95CF(FULL_FIELD_LENGTH), get95CF(FULL_FIELD_WIDTH), get95CF(M_PI),
                          get95CF(10.0*FULL_FIELD_LENGTH), get95CF(10.0*FULL_FIELD_WIDTH),
                          get95CF(10000.0), get95CF(10000.0);
   } else {
      MY_ASSERT(false, "Invalid reset dimension");
   }
   return diagonalVariance;
}

static int getTeammateIndex(const int teammateIndex, const int poseDim) {
   MY_ASSERT(teammateIndex >= 0 && teammateIndex <= 3, "unexpected teammate index");
   MY_ASSERT(poseDim >= ROBOT_X_DIM && poseDim <= ROBOT_H_DIM, "invalid poseDim");
   return TEAM_INDEX_OFFSET + teammateIndex*3 + poseDim;
}


UniModalVisionUpdate::UniModalVisionUpdate(const VisionUpdateBundle &visionBundle):
      visionBundle(visionBundle),
      postTypes(std::vector<PostType>()) {}

UniModalVisionUpdate::UniModalVisionUpdate(const VisionUpdateBundle &visionBundle,
      const PostType &postType) :
            visionBundle(visionBundle) {
   this->postTypes.push_back(postType);
}

UniModalVisionUpdate::UniModalVisionUpdate(const VisionUpdateBundle &visionBundle,
      const std::vector<PostType> &postTypes) :
            visionBundle(visionBundle),
            postTypes(postTypes) {}

UniModalTeammateUpdate::UniModalTeammateUpdate(const VisionUpdateBundle &visionBundle, 
      const std::vector<UNSWRobotInfo> &teammateRobots, const AbsCoord &teammatePosition) :
            visionBundle(visionBundle), teammateRobots(teammateRobots) {
   this->teammatePositions.push_back(teammatePosition);
}

UniModalTeammateUpdate::UniModalTeammateUpdate(const VisionUpdateBundle &visionBundle, 
      const std::vector<UNSWRobotInfo> &teammateRobots, const std::vector<AbsCoord> &teammatePositions) :
            visionBundle(visionBundle), teammateRobots(teammateRobots), teammatePositions(teammatePositions) {}


std::vector<SimpleGaussian*> SimpleGaussian::createBaselineGaussians(void) {
   MatrixXd mean(MAIN_DIM, 1);
   
   // NOTE: the reason we set the ball pos to 100 is because things break if the ball pos is identical
   // to the robot pos.
   mean << 0.0, 0.0, M_PI/2.0,  // initialise the robot position to the centre of the field.
           100.0, 0.0,          // initialise the ball position to near the centre of the field.
           0.0, 0.0,            // initialise the ball velocity to 0.
           0.0, 0.0, 0.0,       // Teammates robot poses, (x, y, theta) for each.
           0.0, 0.0, 0.0,
           0.0, 0.0, 0.0,
           0.0, 0.0, 0.0;
   
   double baselineWeight = 0.0;
//      LocalisationConstantsProvider::instance().get(LocalisationConstantsProvider::INVALID_OBSERVATION_PROBABILITY) *
//      LocalisationConstantsProvider::instance().get(LocalisationConstantsProvider::MIN_MODE_WEIGHT) * 2.0;

   std::vector<SimpleGaussian*> result;
   result.push_back(new SimpleGaussian(MAIN_DIM, baselineWeight, mean, getResetDiagonalVariance(MAIN_DIM)));

   mean(ROBOT_H_DIM, 0) = -M_PI/2.0;
   result.push_back(new SimpleGaussian(MAIN_DIM, baselineWeight, mean, getResetDiagonalVariance(MAIN_DIM)));
   
   return result;
}

SimpleGaussian* SimpleGaussian::createBaselineSharedGaussian(void) {
   MatrixXd mean(SHARED_DIM, 1);
   // NOTE: the reason we set the ball pos to 100 is because things break if the ball pos is identical
   // to the robot pos.
   mean << 0.0, 0.0, M_PI/2.0, // initialise the robot position to the centre of the field.
           100.0, 0.0,         // initialise the ball position to near the centre of the field.
           0.0, 0.0;           // initialise the ball velocity to 0.

   return new SimpleGaussian(SHARED_DIM, 1.0, mean, getResetDiagonalVariance(SHARED_DIM));
}

void SimpleGaussian::sanityCheck(void) const {
   if (LOCALISATION_DEBUG && !isStateValid()) {
      std::cout << "SANITY CHECK FAILED" << std::endl;
   }
}

SimpleGaussian::SimpleGaussian(
      const unsigned dim,
      const double weight,
      const Eigen::MatrixXd &mean,
      const Eigen::MatrixXd &diagonalVariance) :
            DIM(dim),
            identity(MatrixXd::Identity(dim, dim)),
            weight(weight), 
            mean(mean),
            covariance(MatrixXd::Identity(dim, dim)),
            doingBallLineUp(false),
            isInReadyMode(false),
            haveLastVisionUpdate(false),
            observedPostsHistory(5) {
   // Insert the desired variance into the main diagonal of the covariance matrix.
   covariance.setZero();
   for (unsigned i = 0; i < dim; i++) {
      covariance(i, i) = diagonalVariance(i, 0);
   }
}

SimpleGaussian::SimpleGaussian(
      const unsigned dim,
      const double weight,
      const Eigen::MatrixXd &mean,
      const Eigen::MatrixXd &covariance,
      bool doingBallLineUp,
      bool isInReadyMode,
      const ObservedPostsHistory &observedPostsHistory) :
            DIM(dim),
            identity(MatrixXd::Identity(dim, dim)),
            weight(weight), 
            mean(mean), 
            covariance(covariance), 
            doingBallLineUp(doingBallLineUp),
            isInReadyMode(isInReadyMode),
            haveLastVisionUpdate(true),
            observedPostsHistory(observedPostsHistory) {}

void SimpleGaussian::resetMean(const Eigen::MatrixXd &src) {
   MY_ASSERT(src.rows() >= DIM, "resetMean() called with incompatible src");
   for (unsigned i = 0; i < DIM; i++) {
      mean(i, 0) = src(i, 0);
   }
}

void SimpleGaussian::resetCovariance(const Eigen::MatrixXd &src) {
   MY_ASSERT(src.rows() >= DIM && src.cols() >= DIM, "resetCovariance() called with incompatible src");
   for (unsigned i = 0; i < DIM; i++) {
      for (unsigned j = 0; j < DIM; j++) {
         covariance(i, j) = src(i, j);
      }
   }
}

void SimpleGaussian::setLineUpMode(bool enabled) {
   doingBallLineUp = enabled;
}

void SimpleGaussian::setReadyMode(bool enabled) {
   isInReadyMode = enabled;
}

OdometryUpdateResult SimpleGaussian::processUpdate(const Odometry &odometry, const double dTimeSeconds,
      const bool canSeeBall) {
   OdometryUpdateResult odometryUpdateResult;

   updateMeanVectorWithOdometry(odometry, dTimeSeconds, odometryUpdateResult);
   updateCovarianceWithOdometry(odometry, dTimeSeconds, canSeeBall, odometryUpdateResult);

   clipToField(mean);
   clipBallOutOfRobot(mean);
   
   return odometryUpdateResult;
}

std::vector<SimpleGaussian*> SimpleGaussian::visionUpdate(const VisionUpdateBundle &visionBundle) {
   std::vector<SimpleGaussian*> newModes;
   
   if (visionBundle.posts.size() == 0) {
      addNoPostsObservationMode(visionBundle, newModes);
   } else if (visionBundle.posts.size() == 1) {
      addOnePostObservationModes(visionBundle, newModes);
   } else if (visionBundle.posts.size() == 2) {
      addTwoPostObservationModes(visionBundle, newModes);
   }

   this->weight *= LocalisationConstantsProvider::instance().get(
         LocalisationConstantsProvider::INVALID_OBSERVATION_PROBABILITY);
   observedPostsHistory.addNoObservedPosts();
   
   haveLastVisionUpdate = false;
   
   return newModes;
}

std::vector<SimpleGaussian*> SimpleGaussian::visionTeammateRobotsUpdate(const VisionUpdateBundle &visionBundle) {
   std::vector<SimpleGaussian*> newModes;
   
   std::vector<UNSWRobotInfo> teammateRobots;
   for (unsigned i = 0; i < visionBundle.robots.size(); i++) {
      if ((visionBundle.amIOnRedTeam && visionBundle.robots[i].type == UNSWRobotInfo::rRed) ||
          (!visionBundle.amIOnRedTeam && visionBundle.robots[i].type == UNSWRobotInfo::rBlue)) {
         teammateRobots.push_back(visionBundle.robots[i]);
      }
   }
   
   // only bother with updates if we see either 1 or 2 robots.
   if (teammateRobots.size() > 0 && teammateRobots.size() <= 2) {
      if (teammateRobots.size() == 1) {
         for (unsigned i = 0; i < visionBundle.teammatePositions.size(); i++) {
            UniModalTeammateUpdate teammateUpdate(visionBundle, teammateRobots, visionBundle.teammatePositions[i]);
            SimpleGaussian *splitGaussian = createSplitGaussian();
            splitGaussian->uniModalTeammateRobotVisionUpdate(teammateUpdate);
            newModes.push_back(splitGaussian);
         }
      } else if (teammateRobots.size() == 2) {
         for (unsigned i = 0; i < visionBundle.teammatePositions.size(); i++) {
            for (unsigned j = 0; j < visionBundle.teammatePositions.size(); j++) {
               if (i == j) {
                  continue;
               }

               std::vector<AbsCoord> chosenPositions;
               chosenPositions.push_back(visionBundle.teammatePositions[i]);
               chosenPositions.push_back(visionBundle.teammatePositions[j]);
               
               UniModalTeammateUpdate teammateUpdate(visionBundle, teammateRobots, chosenPositions);
               SimpleGaussian *splitGaussian = createSplitGaussian();
               splitGaussian->uniModalTeammateRobotVisionUpdate(teammateUpdate);
               newModes.push_back(splitGaussian);
            }
         }
      }

      this->weight *= LocalisationConstantsProvider::instance().get(
            LocalisationConstantsProvider::INVALID_TEAMMATE_ROBOT_OBSERVATION_PROBABILITY);
   }

   return newModes;
}

int SimpleGaussian::uniModalVisionUpdate(const UniModalVisionUpdate &vu) {  
   Eigen::MatrixXd innovation(MAX_MEASUREMENT_DIM, 1);
   Eigen::MatrixXd jacobian(MAX_MEASUREMENT_DIM, DIM);
   Eigen::MatrixXd observationVariance(MAX_MEASUREMENT_DIM, MAX_MEASUREMENT_DIM);
   innovation.setZero();
   jacobian.setZero();
   observationVariance.setZero();

   int currentDimension = 0;
   double unreliableDistanceCutoff = LocalisationConstantsProvider::instance().get(
         LocalisationConstantsProvider::UNRELIABLE_DISTANCE_CUTOFF_FACTOR);
   
   if (vu.postTypes.size() == 0) {
      observedPostsHistory.addNoObservedPosts();
   } else {
      observedPostsHistory.addObservedPost(vu.postTypes[0]);
   }

   MY_ASSERT(vu.postTypes.size() <= vu.visionBundle.posts.size(), "post types more than visible posts");
   for (unsigned i = 0; i < vu.postTypes.size(); i++) {
      AbsCoord postPosition = getGoalpostPosition(vu.postTypes[i]);
      double maxDistance = LocalisationConstantsProvider::instance().get(
            LocalisationConstantsProvider::GOALPOST_MAX_DISTANCE_OBSERVATION);
      if (!vu.visionBundle.isDistanceReliable) {
         maxDistance *= unreliableDistanceCutoff;
      }
      
//      if (distanceTo(postPosition) < 0.66*maxDistance && vu.visionBundle.posts[i].rr.distance() < 0.0) {
//         weight *= 0.5;
//      }
      
      bool useDistance = vu.visionBundle.posts[i].rr.distance() < maxDistance &&
            vu.visionBundle.posts[i].trustDistance;

      currentDimension = addGoalPostMeasurement(
            vu.visionBundle,
            vu.visionBundle.posts[i].rr,
            postPosition,
            useDistance,
            innovation,
            jacobian,
            observationVariance,
            currentDimension);
   }

   for (unsigned i = 0; i < vu.visionBundle.fieldFeatures.size(); i++) {
      if (vu.visionBundle.fieldFeatures[i].type == FieldFeatureInfo::fCentreCircle) {
         double maxDistance = LocalisationConstantsProvider::instance().get(
               LocalisationConstantsProvider::CENTRE_CIRCLE_MAX_DISTANCE_OBSERVATION);
         if (!vu.visionBundle.isDistanceReliable) {
            maxDistance *= unreliableDistanceCutoff;
         }
         
         bool useDistance = vu.visionBundle.fieldFeatures[i].rr.distance() < maxDistance &&
               vu.visionBundle.fieldFeatures[i].rr.distance() > 0.0;
         currentDimension = addFieldFeatureMeasurement(
               vu.visionBundle,
               AbsCoord(0.0, 0.0, 0.0),
               vu.visionBundle.fieldFeatures[i].rr,
               useDistance,
               innovation,
               jacobian,
               observationVariance,
               currentDimension);
         break;
      }
   }
   
   for (unsigned i = 0; i < vu.visionBundle.fieldFeatures.size(); i++) {
      if (vu.visionBundle.fieldFeatures[i].type == FieldFeatureInfo::fPenaltySpot &&
          vu.visionBundle.fieldFeatures[i].rr.distance() < 2000.0 &&
          canObservePenaltySpot()) {

         currentDimension = addFieldFeatureMeasurement(
               vu.visionBundle,
               getPenaltySpotPosition(),
               vu.visionBundle.fieldFeatures[i].rr,
               true,
               innovation,
               jacobian,
               observationVariance,
               currentDimension);
         break;
      }
   }

   // Note: we assume there are not balls on the field (and no point tracking them if they are) if
   // we are in Ready mode. So we ignore obsered balls in that case.
   if (vu.visionBundle.visibleBalls.size() == 1 && !isInReadyMode) {
      double maxDistance = LocalisationConstantsProvider::instance().get(
            LocalisationConstantsProvider::BALL_MAX_DISTANCE_OBSERVATION);
      if (!vu.visionBundle.isDistanceReliable) {
         maxDistance *= unreliableDistanceCutoff;
      }
      
      bool useDistance = vu.visionBundle.visibleBalls[0].rr.distance() < maxDistance &&
            vu.visionBundle.visibleBalls[0].rr.distance() > 0.0;
      currentDimension = addBallMeasurement(
            vu.visionBundle,
            vu.visionBundle.visibleBalls[0].rr,
            useDistance,
            innovation,
            jacobian,
            observationVariance,
            currentDimension);
   }
   
   applyObservation(currentDimension, innovation, jacobian, observationVariance, true);

   // Adjust the weight of the current mode according to the probability that the observed post is
   // our own or the opponent's. This is using vision cues from the background.
//   if (vu.postTypes.size() > 0) {
//      MY_ASSERT(currentDimension > 0, "post types non zero but current dimension zero");
//      if (vu.postTypes[0] == MY_LEFT || vu.postTypes[0] == MY_RIGHT) {
//         weight *= (1.0 - vu.visionBundle.awayGoalProb);
//      } else if (vu.postTypes[0] == OPPONENT_LEFT || vu.postTypes[0] == OPPONENT_RIGHT) {
//         weight *= vu.visionBundle.awayGoalProb;
//      }
//   }
   
   haveLastVisionUpdate = true;
   lastVisionUpdate = vu;
   
   return currentDimension;
}

void SimpleGaussian::uniModalTeammateRobotVisionUpdate(const UniModalTeammateUpdate &vu) {
   Eigen::MatrixXd innovation(MAX_MEASUREMENT_DIM, 1);
   Eigen::MatrixXd jacobian(MAX_MEASUREMENT_DIM, DIM);
   Eigen::MatrixXd observationVariance(MAX_MEASUREMENT_DIM, MAX_MEASUREMENT_DIM);
   innovation.setZero();
   jacobian.setZero();
   observationVariance.setZero();

   int currentDimension = 0;
   double unreliableDistanceCutoff = LocalisationConstantsProvider::instance().get(
         LocalisationConstantsProvider::UNRELIABLE_DISTANCE_CUTOFF_FACTOR);
   
   double maxDistance = LocalisationConstantsProvider::instance().get(
         LocalisationConstantsProvider::TEAMMATE_ROBOT_MAX_DISTANCE_OBSERVATION);
   if (!vu.visionBundle.isDistanceReliable) {
      maxDistance *= unreliableDistanceCutoff;
   }
   
   for (unsigned i = 0; i < vu.teammateRobots.size(); i++) {     
      bool useDistance = vu.teammateRobots[i].rr.distance() < maxDistance;
      currentDimension = addTeammateMeasurement(
            vu.visionBundle,
            vu.teammateRobots[i].rr,
            vu.teammatePositions[i],
            useDistance,
            innovation,
            jacobian,
            observationVariance,
            currentDimension);
   }
   
   applyObservation(currentDimension, innovation, jacobian, observationVariance, true);
}

double SimpleGaussian::applyObservation(int obsDimension, const Eigen::MatrixXd &innovation, 
      const Eigen::MatrixXd &jacobian, const Eigen::MatrixXd &observationVariance, const bool updateWeight) {
   if (obsDimension > 0) {
      Eigen::MatrixXd fittedJacobian;
      if (jacobian.cols() < DIM) {
         fittedJacobian.setZero(jacobian.rows(), DIM);
         for (int i = 0; i < jacobian.rows(); i++) {
            for (int j = 0; j < jacobian.cols(); j++) {
               fittedJacobian(i, j) = jacobian(i, j);
            }
         }
      } else if (jacobian.cols() > DIM) {
         fittedJacobian = jacobian.block(0, 0, jacobian.rows(), DIM);
      } else {
         fittedJacobian = jacobian;
      }
      
      return performKalmanUpdate(obsDimension, innovation, fittedJacobian, observationVariance, updateWeight);
   }
   
   return 1.0;
}

double SimpleGaussian::doICPUpdate(const VisionUpdateBundle &visionBundle, const bool updateWeight) {
   Eigen::MatrixXd innovation(3, 1);
   Eigen::MatrixXd jacobian(3, DIM);
   Eigen::MatrixXd observationVariance(3, 3);
   innovation.setZero();
   jacobian.setZero();
   observationVariance.setZero();
   
   // Check if we have any corners of T junctions or circles. Basically anything but a single
   // line. This is because we often match a single line to the wrong field line which is perpendicular
   // to the correct match. This strongly throws off our heading and wrecks our localisation.
   bool haveKeyICPFeatures = getICPQualityPosts(visionBundle).size() > 0;
   
   if (!haveKeyICPFeatures) {
      for (unsigned i = 0; i < visionBundle.fieldFeatures.size(); i++) {
         if (visionBundle.fieldFeatures[i].type == FieldFeatureInfo::fCorner ||
             visionBundle.fieldFeatures[i].type == FieldFeatureInfo::fTJunction ||
             visionBundle.fieldFeatures[i].type == FieldFeatureInfo::fCentreCircle ||
             visionBundle.fieldFeatures[i].type == FieldFeatureInfo::fXJunction ||
             visionBundle.fieldFeatures[i].type == FieldFeatureInfo::fParallelLines ||
             visionBundle.fieldFeatures[i].type == FieldFeatureInfo::fPenaltySpot) {
            if (visionBundle.fieldFeatures[i].distance() < 2500.0) {
               haveKeyICPFeatures = true;
            }
         }
      }
   }
   
   // Update weight in this case really means "are we in initial state where every mode perform ICP?".
   // In we are in that mode, then we want to perform ICP regardless of what features we have. If we
   // are not in that mode, ie: during normal gameplay, then we want to perform ICP only when there
   // are more features than a single line.
//   if (!isInReadyMode && !haveKeyICPFeature) {
//      return;
//   }
   
   int currentDimension = performICPUpdate(
         visionBundle,
         innovation,
         jacobian,
         observationVariance,
         0, haveKeyICPFeatures);
   MY_ASSERT(currentDimension >= 0 && currentDimension <= 3, "icp dimension unexpected");
   
   storedICPUpdate = StoredICPUpdate(currentDimension, innovation, jacobian, observationVariance);
   double weightUpdate = applyObservation(currentDimension, innovation, jacobian, observationVariance, updateWeight);
   
   // If the ICP update failed, ie: we had features but didnt find a match, then this indicated
   // the current mode is wrong.
   if (currentDimension == 0 && updateWeight && visionBundle.fieldFeatures.size() > 0) {
      weightUpdate *= 0.5;
      weight *= 0.5;
   }
   
   return weightUpdate;
}

SimpleGaussian* SimpleGaussian::createSymmetricGaussian(void) {
   double symmetryWeight = LocalisationConstantsProvider::instance().get(
         LocalisationConstantsProvider::SYMMETRIC_MODE_WEIGHT);
   
   Eigen::MatrixXd reflectedMean = mean;
   reflectedMean(ROBOT_X_DIM, 0) *= -1.0;
   reflectedMean(ROBOT_Y_DIM, 0) *= -1.0;
   reflectedMean(ROBOT_H_DIM, 0) = normaliseTheta(mean(ROBOT_H_DIM, 0) + M_PI);
   reflectedMean(BALL_X_DIM, 0) *= -1.0;
   reflectedMean(BALL_Y_DIM, 0) *= -1.0;
   reflectedMean(BALL_DX_DIM, 0) *= -1.0;
   reflectedMean(BALL_DY_DIM, 0) *= -1.0;
   
   return new SimpleGaussian(DIM, symmetryWeight * weight, reflectedMean, covariance, 
         doingBallLineUp, isInReadyMode, observedPostsHistory.createSymmetricHistory());
}

std::vector<SimpleGaussian*> SimpleGaussian::applyRemoteUpdate(
      const SharedLocalisationUpdateBundle &updateBundle, int teammateIndex, bool amGoalie) {

   MY_ASSERT(DIM == MAIN_DIM, "dim no equal to main dim in apply remote");
   MY_ASSERT(teammateIndex >= 0 && teammateIndex <= 3, "apply remote invalid teammate index");
   
   updateMeanVectorWithRemoteOdometry(updateBundle, teammateIndex);
   updateCovarianceWithRemoteOdometry(updateBundle, teammateIndex);
   
   if (!updateBundle.haveVisionUpdates || doingBallLineUp || isBallTooCloseForRemoteUpdate()) {
      return std::vector<SimpleGaussian*>();
   }
   
   // Create a mode that doesnt have the observation applied.
   std::vector<SimpleGaussian*> newModes;
   SimpleGaussian *splitGaussian = createSplitGaussian();
   
   double remoteUpdateInvalidProbability = 0.0;
   if (amGoalie) {
      remoteUpdateInvalidProbability = LocalisationConstantsProvider::instance().get(
            LocalisationConstantsProvider::INVALID_REMOTE_OBSERVATION_PROBABILITY_GOALIE);
   } else {
      remoteUpdateInvalidProbability = LocalisationConstantsProvider::instance().get(
            LocalisationConstantsProvider::INVALID_REMOTE_OBSERVATION_PROBABILITY);
   }
   splitGaussian->weight *= remoteUpdateInvalidProbability;
   newModes.push_back(splitGaussian);
   
   // Do the direct update part now.
   Eigen::MatrixXd jacobian(SHARED_DIM, DIM);
   jacobian.setZero();
   
   // The ball part is a direct relationship
   jacobian(BALL_X_DIM, BALL_X_DIM) = 1.0;
   jacobian(BALL_Y_DIM, BALL_Y_DIM) = 1.0;
   jacobian(BALL_DX_DIM, BALL_DX_DIM) = 1.0;
   jacobian(BALL_DY_DIM, BALL_DY_DIM) = 1.0;
   
   // The robot pose part is shifted according to the teammate index.
   const unsigned poseXIndex = getTeammateIndex(teammateIndex, ROBOT_X_DIM);
   const unsigned poseYIndex = getTeammateIndex(teammateIndex, ROBOT_Y_DIM);
   const unsigned poseHIndex = getTeammateIndex(teammateIndex, ROBOT_H_DIM);
   jacobian(ROBOT_X_DIM, poseXIndex) = 1.0;
   jacobian(ROBOT_Y_DIM, poseYIndex) = 1.0;
   jacobian(ROBOT_H_DIM, poseHIndex) = 1.0;
   
   MatrixXd innovation(SHARED_DIM, 1);
   innovation(ROBOT_X_DIM, 0) = updateBundle.sharedUpdateMean(ROBOT_X_DIM, 0) - mean(poseXIndex, 0);
   innovation(ROBOT_Y_DIM, 0) = updateBundle.sharedUpdateMean(ROBOT_Y_DIM, 0) - mean(poseYIndex, 0);
   innovation(ROBOT_H_DIM, 0) = normaliseTheta(updateBundle.sharedUpdateMean(ROBOT_H_DIM, 0) - mean(poseHIndex, 0));
   innovation(BALL_X_DIM, 0) = updateBundle.sharedUpdateMean(BALL_X_DIM, 0) - mean(BALL_X_DIM, 0);
   innovation(BALL_Y_DIM, 0) = updateBundle.sharedUpdateMean(BALL_Y_DIM, 0) - mean(BALL_Y_DIM, 0);
   innovation(BALL_DX_DIM, 0) = updateBundle.sharedUpdateMean(BALL_DX_DIM, 0) - mean(BALL_DX_DIM, 0);
   innovation(BALL_DY_DIM, 0) = updateBundle.sharedUpdateMean(BALL_DY_DIM, 0) - mean(BALL_DY_DIM, 0);
   
   double uncertaintyFactor = constantsProvider.get(
         LocalisationConstantsProvider::REMOTE_OBSERVATION_UNCERTAINTY_FACTOR);
   MatrixXd observationVariance(SHARED_DIM, SHARED_DIM);
   for (unsigned i = 0; i < SHARED_DIM; i++) {
      for(unsigned j = 0; j < SHARED_DIM; j++) {
         observationVariance(i, j) = updateBundle.sharedUpdateCovariance(i, j) * uncertaintyFactor;
      }
   }

   double lastWeightAdjustment =
         performTrimmedKalmanUpdate(innovation, jacobian, observationVariance, true, true);
   
   // If the remote update did not go through (its weight is too small) then scale up that teammates
   // pose covariance, since we are less sure of our own idea of it.
   // TODO: investigate whether 2.0 is a good scale.
   if (lastWeightAdjustment < remoteUpdateInvalidProbability) {
      covariance(poseXIndex, poseXIndex) *= 2.0;
      covariance(poseYIndex, poseYIndex) *= 2.0;
      covariance(poseHIndex, poseHIndex) *= 2.0;
   }
   
   sanityCheck();
   return newModes;
}

AbsCoord SimpleGaussian::getRobotPose(void) const {
   AbsCoord result = AbsCoord(mean(ROBOT_X_DIM, 0), mean(ROBOT_Y_DIM, 0), mean(ROBOT_H_DIM, 0));
   for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
         result.var(i, j) = covariance(i, j);
      }
   }
   return result;
}
   
AbsCoord SimpleGaussian::getBallPosition(void) const {
   AbsCoord result = AbsCoord(mean(BALL_X_DIM, 0), mean(BALL_Y_DIM, 0), 0);
   for (int i = 0; i < 2; i++) {
      for (int j = 0; j < 2; j++) {
         result.var(i, j) = covariance(BALL_X_DIM + i, BALL_X_DIM + j);
      }
   }
   return result;
}
   
AbsCoord SimpleGaussian::getBallVelocity(void) const {
   AbsCoord result = AbsCoord(mean(BALL_DX_DIM, 0), mean(BALL_DY_DIM, 0), 0);
   for (int i = 0; i < 2; i++) {
      for (int j = 0; j < 2; j++) {
         result.var(i, j) = covariance(BALL_DX_DIM + i, BALL_DX_DIM + j);
      }
   }
   return result;
}

double SimpleGaussian::getRobotPosUncertainty(void) const {
   Eigen::MatrixXd positionCovariance = covariance.block(ROBOT_X_DIM, ROBOT_X_DIM, 2, 2);
   EigenSolver<MatrixXd> es(positionCovariance);
   
   double value0 = sqrt(complexMagnitude(es.eigenvalues()[0].real(), es.eigenvalues()[0].imag()));
   double value1 = sqrt(complexMagnitude(es.eigenvalues()[1].real(), es.eigenvalues()[1].imag()));
   return value0 * value1;
}

double SimpleGaussian::getRobotHeadingUncertainty(void) const {
   return sqrt(covariance(ROBOT_H_DIM, ROBOT_H_DIM));
}

double SimpleGaussian::getBallPosUncertainty(void) const {
   Eigen::MatrixXd positionCovariance = covariance.block(BALL_X_DIM, BALL_X_DIM, 2, 2);
   EigenSolver<MatrixXd> es(positionCovariance);
   
   double value0 = sqrt(complexMagnitude(es.eigenvalues()[0].real(), es.eigenvalues()[0].imag()));
   double value1 = sqrt(complexMagnitude(es.eigenvalues()[1].real(), es.eigenvalues()[1].imag()));
   return value0 * value1;
}

double SimpleGaussian::getBallVelocityUncertainty(void) const {
   Eigen::MatrixXd velocityCovariance = covariance.block(BALL_DX_DIM, BALL_DX_DIM, 2, 2);
   EigenSolver<MatrixXd> es(velocityCovariance);
   
   double value0 = sqrt(complexMagnitude(es.eigenvalues()[0].real(), es.eigenvalues()[0].imag()));
   double value1 = sqrt(complexMagnitude(es.eigenvalues()[1].real(), es.eigenvalues()[1].imag()));
   return MAX(value0, value1);
}

bool SimpleGaussian::isSimilarTo(const SimpleGaussian &other) const {
   if (!observedPostsHistory.isSimilarTo(other.observedPostsHistory)) {
      return false;
   }
   
   // First compare the means. We only consider the robot pose as part of the similarity
   // calculation.

   double xDiff = mean(ROBOT_X_DIM, 0) - other.mean(ROBOT_X_DIM, 0);
   double yDiff = mean(ROBOT_Y_DIM, 0) - other.mean(ROBOT_Y_DIM, 0);
   double headingDiff = MIN_THETA_DIFF(mean(ROBOT_H_DIM, 0), other.mean(ROBOT_H_DIM, 0));
   double posDiff = sqrt(xDiff*xDiff + yDiff*yDiff);
   
   for (int teammateIndex = 0; teammateIndex < NUM_TEAMMATES; teammateIndex++) {
      const unsigned poseXIndex = getTeammateIndex(teammateIndex, ROBOT_X_DIM);
      const unsigned poseYIndex = getTeammateIndex(teammateIndex, ROBOT_Y_DIM);
      const unsigned poseHIndex = getTeammateIndex(teammateIndex, ROBOT_H_DIM);
      
      xDiff = mean(poseXIndex, 0) - other.mean(poseXIndex, 0);
      yDiff = mean(poseYIndex, 0) - other.mean(poseYIndex, 0);
      
      headingDiff = MAX(headingDiff, MIN_THETA_DIFF(mean(poseHIndex, 0), other.mean(poseHIndex, 0)));
      posDiff = MAX(posDiff, sqrt(xDiff*xDiff + yDiff*yDiff));
   }
   
   xDiff = mean(BALL_X_DIM, 0) - other.mean(BALL_X_DIM, 0);
   yDiff = mean(BALL_Y_DIM, 0) - other.mean(BALL_Y_DIM, 0);
   posDiff = MAX(posDiff, sqrt(xDiff*xDiff + yDiff*yDiff));
   
   double headingThreshold = constantsProvider.get(
         LocalisationConstantsProvider::MEAN_HEADING_SIMILARITY_THRESHOLD);
   double posThreshold = constantsProvider.get(
         LocalisationConstantsProvider::MEAN_DIST_SIMILARITY_THRESHOLD);
   
   bool amNearOwnGoals = fabs(mean(ROBOT_Y_DIM, 0)) < 1700.0 && 
         mean(ROBOT_X_DIM, 0) < (-FIELD_LENGTH / 2.0 + 1700.0);
   bool amNearOpponentGoals = fabs(mean(ROBOT_Y_DIM, 0)) < 1700.0 && 
         mean(ROBOT_X_DIM, 0) > (FIELD_LENGTH / 2.0 - 1700.0);
   
   if (amNearOwnGoals || amNearOpponentGoals) {
      headingThreshold = constantsProvider.get(
            LocalisationConstantsProvider::MEAN_HEADING_SIMILARITY_THRESHOLD_NEAR_GOALS);
      posThreshold = constantsProvider.get(
            LocalisationConstantsProvider::MEAN_DIST_SIMILARITY_THRESHOLD_NEAR_GOALS);
   }
         
   if (headingDiff < headingThreshold && posDiff < posThreshold) {
      // Calculate the Euclidean distance between the covariance matrix elements corresponding to
      // the robot position and heading.
      double covDist = 0.0;
      for (int row = 0; row < 3; row++) {
         for (int col = 0; col < 3; col++) {
            covDist += fabs(covariance(row, col) - other.covariance(row, col));
         }
      }
     
      return covDist < constantsProvider.get(
            LocalisationConstantsProvider::COVARIANCE_DIST_SIMILARITY_THRESHOLD);
   }

   // NOTE: we are only comparing the robots position components to decide similarity, but when we
   // merge similar Gaussians, we merge the entire state. The assumption is that if the robot self
   // state is similar, the other components (ball position,velocity and teammate poses) will also
   // be similar.

   return false;
}

void SimpleGaussian::mergeWith(SimpleGaussian &other) {
   double sumWeights = weight + other.weight;
   if (sumWeights < EPSILON) {
      weight = 0.0;
      other.weight = 0.0;
      return;
   }
   
   double thisRatio = weight / sumWeights;
   double otherRatio = other.weight / sumWeights;

   // Merge the mean vectors.
   for (int row = 0; row < mean.rows(); row++) {
      if (isHeadingRow(row)) {
         double toOther = normaliseTheta(other.mean(row, 0) - mean(row, 0));
         mean(row, 0) = normaliseTheta(mean(row, 0) + otherRatio * toOther);
      } else {
         mean(row, 0) = thisRatio * mean(row, 0) + otherRatio * other.mean(row, 0);
      }
   }

   // Merge the covariance matrices;
   covariance *= thisRatio;
   other.covariance *= otherRatio;
   covariance += other.covariance;

   other.weight = 0.0;
   weight = sumWeights;
   
   if (!haveLastVisionUpdate) {
      lastVisionUpdate = other.lastVisionUpdate;
   }

   haveLastVisionUpdate |= other.haveLastVisionUpdate;
   
   if (weight >= other.weight) {
      observedPostsHistory = observedPostsHistory.mergedWith(other.observedPostsHistory);
   } else {
      observedPostsHistory = other.observedPostsHistory.mergedWith(observedPostsHistory);
   }
}

double SimpleGaussian::getWeight(void) const {
   return weight;
}

void SimpleGaussian::setWeight(double weight) {
   MY_ASSERT(weight >= 0.0 && weight <= 1.0, "invalid weight set");
   this->weight = weight;
}

bool SimpleGaussian::getHaveLastVisionUpdate(void) const {
   return haveLastVisionUpdate;
}

StoredICPUpdate SimpleGaussian::getLastAppliedICPUpdate(void) const {
   return storedICPUpdate;
}

UniModalVisionUpdate SimpleGaussian::getLastAppliedVisionUpdate(void) const {
   return lastVisionUpdate; 
}

Eigen::MatrixXd SimpleGaussian::getMean(void) const {
   return mean;
}

Eigen::MatrixXd SimpleGaussian::getCovariance(void) const {
   return covariance;
}

std::vector<PostInfo> SimpleGaussian::getICPQualityPosts(const VisionUpdateBundle &visionBundle) {
   std::vector<PostInfo> result;
   
   for (unsigned i = 0; i < visionBundle.posts.size(); i++) {
      if (visionBundle.posts[i].trustDistance && visionBundle.posts[i].rr.distance() < 2000.0) {
         result.push_back(visionBundle.posts[i]);
      }
   }
   
   // include posts in ICP only if all observed posts are good.
   if (result.size() != visionBundle.posts.size()) {
      result.clear();
   }

   return result;
}

bool SimpleGaussian::canUsePostType(PostType type, double postDistance) {
   // First we want some temporal hysteresis on whose goalposts we see. We cant have a
   // situation where in one frame we see our own goal posts and in the next the opponents.
   if (!observedPostsHistory.canObservePostType(type)) {
      return false;
   }
   
   // Now this is a bit of a hack, but we want to prevent treating far off goal posts as nearby
   // ones with only the heading being used. This is an issue for the goalie for example.
   
   AbsCoord postPos = getGoalpostPosition(type);
   
   // Find the smallest distance to one of these goal posts.
   double dx = mean(ROBOT_X_DIM, 0) - postPos.x();
   double dy = mean(ROBOT_Y_DIM, 0) - postPos.y();
   double expectedDistance = sqrt(dx*dx + dy*dy);
   
   // TODO: take our position uncertainty into account.
   double postCutoffDistance = LocalisationConstantsProvider::instance().get(
         LocalisationConstantsProvider::GOALPOST_MAX_DISTANCE_OBSERVATION);
   
   // If the post is expected to be close, but its observed very far.
   if (expectedDistance < 2000.0 && 
       postDistance > postCutoffDistance &&
       expectedDistance < 0.5 * postDistance) {
      return false;
   }
   
   // If we expect the post to be very far, but it is observed very close.
   if (postDistance < 2000.0 && 
       expectedDistance > postCutoffDistance &&
       postDistance < 0.5 * expectedDistance) {
      return false;
   }
   
   return true;
}

bool SimpleGaussian::canUsePostType(PostType type0, double distance0, PostType type1, double distance1) {
   return canUsePostType(type0, distance0) && canUsePostType(type1, distance1);
}

double SimpleGaussian::distanceTo(const AbsCoord &coord) {
   double dx = mean(ROBOT_X_DIM, 0) - coord.x();
   double dy = mean(ROBOT_Y_DIM, 0) - coord.y();
   return sqrt(dx*dx + dy*dy);
}

SimpleGaussian* SimpleGaussian::createSplitGaussian(void) {
   return new SimpleGaussian(DIM, weight, mean, covariance, 
         doingBallLineUp, isInReadyMode, observedPostsHistory);
}

bool SimpleGaussian::isHeadingRow(int row) {
   // TODO: when we add teammate robots to the mean, add their heading row indices here.
   if (DIM == MAIN_DIM) {
      return row == 2 || row == 9 || row == 12 || row == 15 || row == 18;
   } else {
      return row == 2;
   }
}

double SimpleGaussian::normaliseTheta(double theta) {
   double r = fmod(theta - M_PI, 2.0*M_PI);
   if (r > 0) {
      return r - M_PI;
   } else {
      return r + M_PI;
   }
}

void SimpleGaussian::addNoPostsObservationMode(const VisionUpdateBundle &visionBundle,
      std::vector<SimpleGaussian*> &outModes) {
   SimpleGaussian *newMode = createSplitGaussian();
   newMode->uniModalVisionUpdate(UniModalVisionUpdate(visionBundle));
   outModes.push_back(newMode);
}

// TODO(sushkov): special case the goalkeeper since we know that it would only see opponent
// goals in the distance and own goal close up.
void SimpleGaussian::addOnePostObservationModes(const VisionUpdateBundle &visionBundle,
      std::vector<SimpleGaussian*> &outModes) {
   MY_ASSERT(visionBundle.posts.size() == 1, "unexpected number of posts");
   
   SimpleGaussian *baseSplitGaussian = createSplitGaussian();
   if (visionBundle.posts[0].type & PostInfo::pLeft) {
      // We know this post is a LEFT post, so it can be either my own or the opponent's
      if (canUsePostType(MY_LEFT, visionBundle.posts[0].rr.distance())) {
         SimpleGaussian *myMode = baseSplitGaussian->createSplitGaussian();
         myMode->uniModalVisionUpdate(UniModalVisionUpdate(visionBundle, MY_LEFT));
         outModes.push_back(myMode);
      }
      
      if (canUsePostType(OPPONENT_LEFT, visionBundle.posts[0].rr.distance())) {
         SimpleGaussian *awayMode = baseSplitGaussian->createSplitGaussian();
         awayMode->uniModalVisionUpdate(UniModalVisionUpdate(visionBundle, OPPONENT_LEFT));
         outModes.push_back(awayMode);
      }
   } else if (visionBundle.posts[0].type & PostInfo::pRight) {
      // We know this post is a RIGHT post, so it can be either my own or the opponent's
      if (canUsePostType(MY_RIGHT, visionBundle.posts[0].rr.distance())) {
         SimpleGaussian *myMode = baseSplitGaussian->createSplitGaussian();
         myMode->uniModalVisionUpdate(UniModalVisionUpdate(visionBundle, MY_RIGHT));
         outModes.push_back(myMode);
      }
      
      if (canUsePostType(OPPONENT_RIGHT, visionBundle.posts[0].rr.distance())) {
         SimpleGaussian *awayMode = baseSplitGaussian->createSplitGaussian();
         awayMode->uniModalVisionUpdate(UniModalVisionUpdate(visionBundle, OPPONENT_RIGHT));
         outModes.push_back(awayMode);
      }
   } else {
      // We don't know if this is a LEFT or RIGHT post. so it can be one of 4 posts.
      for (int i = 0; i < 4; i++) {
         if (canUsePostType(static_cast<PostType>(i), visionBundle.posts[0].rr.distance())) {
            std::vector<PostType> postType;
            postType.push_back(static_cast<PostType>(i));
            
            SimpleGaussian *newMode = baseSplitGaussian->createSplitGaussian();
            newMode->uniModalVisionUpdate(UniModalVisionUpdate(visionBundle, postType));
            outModes.push_back(newMode);
         }
      }
   }
   
   if (outModes.size() == 0) {
      baseSplitGaussian->uniModalVisionUpdate(UniModalVisionUpdate(visionBundle));
      outModes.push_back(baseSplitGaussian);
   } else {
      delete baseSplitGaussian;
   }
}

void SimpleGaussian::addTwoPostObservationModes(const VisionUpdateBundle &visionBundle,
      std::vector<SimpleGaussian*> &outModes) {
   MY_ASSERT(visionBundle.posts.size() == 2, "unexpected number of posts");
   
   SimpleGaussian *baseSplitGaussian = createSplitGaussian();

   // The two observed posts can either both be home or away goals.
         
   // Case: both are home goals.
   std::vector<PostType> postTypes;
   
   if ((visionBundle.posts[0].type & PostInfo::pLeft) || 
       visionBundle.posts[0].rr.heading() > visionBundle.posts[1].rr.heading()) {
      if (canUsePostType(MY_LEFT, visionBundle.posts[0].rr.distance(), 
                         MY_RIGHT, visionBundle.posts[1].rr.distance())) {
         postTypes.push_back(MY_LEFT);
         postTypes.push_back(MY_RIGHT);
      }
   } else {
      if (canUsePostType(MY_RIGHT, visionBundle.posts[0].rr.distance(),
                         MY_LEFT, visionBundle.posts[1].rr.distance())) {
         postTypes.push_back(MY_RIGHT);
         postTypes.push_back(MY_LEFT);
      }
   }
   
   if (postTypes.size() == 2) {
      SimpleGaussian *homeMode = baseSplitGaussian->createSplitGaussian();
      homeMode->uniModalVisionUpdate(UniModalVisionUpdate(visionBundle, postTypes));
      outModes.push_back(homeMode);
   }
   
   
   // Case: both are away goals.
   postTypes.clear();
   
   if ((visionBundle.posts[0].type & PostInfo::pLeft) || 
       visionBundle.posts[0].rr.heading() > visionBundle.posts[1].rr.heading()) {
      if (canUsePostType(OPPONENT_LEFT, visionBundle.posts[0].rr.distance(),
                         OPPONENT_RIGHT, visionBundle.posts[1].rr.distance())) {
         postTypes.push_back(OPPONENT_LEFT);
         postTypes.push_back(OPPONENT_RIGHT);
      }
   } else {
      if (canUsePostType(OPPONENT_RIGHT, visionBundle.posts[0].rr.distance(),
                         OPPONENT_LEFT, visionBundle.posts[1].rr.distance())) {
         postTypes.push_back(OPPONENT_RIGHT);
         postTypes.push_back(OPPONENT_LEFT);
      }
   }
        
   if (postTypes.size() == 2) {
      SimpleGaussian *awayMode = baseSplitGaussian->createSplitGaussian();
      awayMode->uniModalVisionUpdate(UniModalVisionUpdate(visionBundle, postTypes));
      outModes.push_back(awayMode);
   }
   
   if (outModes.size() == 0) {
      baseSplitGaussian->uniModalVisionUpdate(UniModalVisionUpdate(visionBundle));
      outModes.push_back(baseSplitGaussian);
   } else {
      delete baseSplitGaussian;
   }
}

double SimpleGaussian::performKalmanUpdate(
      const int observationDim,
      const Eigen::MatrixXd &innovation,
      const Eigen::MatrixXd &jacobian,
      const Eigen::MatrixXd &observationVariance,
      const bool updateWeight) {
   MY_ASSERT(observationDim <= MAX_MEASUREMENT_DIM, "observation dim greater than max measurement dim");
   MY_ASSERT(observationDim > 0, "observation dim is 0");
   
   Eigen::MatrixXd trimmedInnovation = innovation.block(0, 0, observationDim, 1);
   Eigen::MatrixXd trimmedJacobian = jacobian.block(0, 0, observationDim, DIM);
   Eigen::MatrixXd trimmedObservationVariance = 
         observationVariance.block(0, 0, observationDim, observationDim);
   
   return performTrimmedKalmanUpdate(trimmedInnovation, trimmedJacobian, trimmedObservationVariance, false, updateWeight);
}

double SimpleGaussian::performTrimmedKalmanUpdate(
      const Eigen::MatrixXd &innovation,
      const Eigen::MatrixXd &jacobian,
      const Eigen::MatrixXd &observationVariance,
      const bool isSharedUpdate,
      const bool updateWeight) {
   
   const Eigen::MatrixXd jacobianT = jacobian.transpose();
   MY_ASSERT(jacobianT.rows() == (int) DIM, "jacobianT rows unexpected");

   Eigen::MatrixXd cjt;
   if (isSharedUpdate) {
      cjt = covariance * jacobianT;
   } else {
      cjt = sparseMultiplication(covariance, jacobianT, 0, 0, 5, jacobianT.cols());
   }
   
   Eigen::MatrixXd combinedCovariance;
   if (isSharedUpdate) {
      combinedCovariance = ((jacobian * cjt) + observationVariance);
   } else {
      Eigen::MatrixXd jcjt = sparseMultiplication(jacobian, 0, 0, jacobian.rows(), 5, cjt);
      combinedCovariance = jcjt + observationVariance;
   }
   
   const Eigen::MatrixXd combinedCovarianceInv = combinedCovariance.inverse();

   const Eigen::MatrixXd kalman = cjt * combinedCovarianceInv;
   MY_ASSERT(kalman.rows() == (int) DIM, "kalman rows unexpected");

   mean = mean + kalman * innovation;
   
   clipToField(mean);
   clipBallOutOfRobot(mean);

   for (int row = 0; row < mean.rows(); row++) {
      if (isHeadingRow(row)) {
         mean(row, 0) = normaliseTheta(mean(row, 0));
      }
   }

   Eigen::MatrixXd kj;
   if (isSharedUpdate) {
      kj = kalman * jacobian;
   } else {
      kj = sparseMultiplication(kalman, jacobian, 0, 0, jacobian.rows(), 5);
   }
   
   covariance = (identity - kj) * covariance;

   double weightAdjustment = 1.0;
   
   const Eigen::MatrixXd weightUpdateMatrix =
         innovation.transpose() * combinedCovarianceInv * innovation;
   MY_ASSERT(weightUpdateMatrix.rows() == 1 && weightUpdateMatrix.cols() == 1, "weighted matrix update unexpected");
   
   weightAdjustment = exp(-0.5 * weightUpdateMatrix(0, 0));   
   if (weightAdjustment != weightAdjustment || weightAdjustment < EPSILON || isnan(weightAdjustment)) {
      weightAdjustment = EPSILON;
   } else if (weightAdjustment > 1.0) {
      weightAdjustment = 1.0;
   }
      
   if (updateWeight) {
      weight *= weightAdjustment;
   }
   
   sanityCheck();
   return weightAdjustment;
}

int SimpleGaussian::addGoalPostMeasurement(
      const VisionUpdateBundle &visionBundle,
      const RRCoord &observedPostCoords,
      const AbsCoord &postWorldCoords,
      const bool useDistance,
      Eigen::MatrixXd &innovationOut,
      Eigen::MatrixXd &jacobianOut,
      Eigen::MatrixXd &observationVarianceOut,
      const int currentMeasurement) {
   MY_ASSERT(currentMeasurement < MAX_MEASUREMENT_DIM, "add goalpost measurement greater than max dim");

   const double dx = getNonZero(mean(ROBOT_X_DIM, 0) - postWorldCoords.x());
   const double dy = getNonZero(mean(ROBOT_Y_DIM, 0) - postWorldCoords.y());

   addGenericMeasurement(observedPostCoords, useDistance, innovationOut, jacobianOut,
         currentMeasurement, dx, dy);
   
   addObservationVariance(visionBundle, VarianceProvider::GOALPOST, observedPostCoords,
         useDistance, observationVarianceOut, currentMeasurement);
   
   return currentMeasurement + (useDistance ? 2 : 1);
}

int SimpleGaussian::addFieldFeatureMeasurement(
      const VisionUpdateBundle &visionBundle,
      const AbsCoord &fieldFeaturePosition,
      const RRCoord &observedFieldFeatureCoords,
      const bool useDistance,
      Eigen::MatrixXd &innovationOut,
      Eigen::MatrixXd &jacobianOut,
      Eigen::MatrixXd &observationVarianceOut,
      const int currentMeasurement) {

   MY_ASSERT(currentMeasurement < MAX_MEASUREMENT_DIM, "add field feature measurement greater than max dim");

   const double dx = getNonZero(mean(ROBOT_X_DIM, 0) - fieldFeaturePosition.x());
   const double dy = getNonZero(mean(ROBOT_Y_DIM, 0) - fieldFeaturePosition.y());

   addGenericMeasurement(observedFieldFeatureCoords, useDistance, innovationOut, jacobianOut,
         currentMeasurement, dx, dy);
   
   addObservationVariance(visionBundle, VarianceProvider::CENTRE_CIRCLE, observedFieldFeatureCoords,
         useDistance, observationVarianceOut, currentMeasurement);
   
   return currentMeasurement + (useDistance ? 2 : 1);
}

int SimpleGaussian::addBallMeasurement(
      const VisionUpdateBundle &visionBundle,
      const RRCoord &observedBallCoords, 
      const bool useDistance,
      Eigen::MatrixXd &innovationOut,
      Eigen::MatrixXd &jacobianOut,
      Eigen::MatrixXd &observationVarianceOut,
      const int currentMeasurement) {

   MY_ASSERT(currentMeasurement < MAX_MEASUREMENT_DIM, "add ball measurement greater than max dim");

   const double dx = getNonZero(mean(ROBOT_X_DIM, 0) - mean(BALL_X_DIM, 0));
   const double dy = getNonZero(mean(ROBOT_Y_DIM, 0) - mean(BALL_Y_DIM, 0));

   const double dx_2 = getNonZero(dx * dx);
   const double dy_2 = getNonZero(dy * dy);

   const double expectedDistance = sqrt(dx_2 + dy_2);

   addGenericMeasurement(observedBallCoords, useDistance, innovationOut, jacobianOut,
         currentMeasurement, dx, dy);

   jacobianOut(currentMeasurement, BALL_X_DIM) = dy / (dx_2 + dy_2);
   jacobianOut(currentMeasurement, BALL_Y_DIM) = -dx / (dx_2 + dy_2);
   
   if (useDistance) {
      jacobianOut(currentMeasurement+1, BALL_X_DIM) = -dx / expectedDistance;
      jacobianOut(currentMeasurement+1, BALL_Y_DIM) = -dy / expectedDistance;
   }
   
   addObservationVariance(visionBundle, VarianceProvider::BALL, observedBallCoords,
         useDistance, observationVarianceOut, currentMeasurement);
   
   return currentMeasurement + (useDistance ? 2 : 1);
}

int SimpleGaussian::addTeammateMeasurement(
      const VisionUpdateBundle &visionBundle,
      const RRCoord &observedRobotCoords, 
      const AbsCoord &teammatePosition,
      const bool useDistance,
      Eigen::MatrixXd &innovationOut,
      Eigen::MatrixXd &jacobianOut,
      Eigen::MatrixXd &observationVarianceOut,
      const int currentMeasurement) {

   MY_ASSERT(currentMeasurement < MAX_MEASUREMENT_DIM, "add teammate measurement greater than max dim");

   const double dx = getNonZero(mean(ROBOT_X_DIM, 0) - teammatePosition.x());
   const double dy = getNonZero(mean(ROBOT_Y_DIM, 0) - teammatePosition.y());

   addGenericMeasurement(observedRobotCoords, useDistance, innovationOut, jacobianOut,
         currentMeasurement, dx, dy);
   
   addObservationVariance(visionBundle, VarianceProvider::TEAMMATE_ROBOT, observedRobotCoords,
         useDistance, observationVarianceOut, currentMeasurement);
   
   return currentMeasurement + (useDistance ? 2 : 1);
}

void SimpleGaussian::addGenericMeasurement(
      const RRCoord &observedObjectCoords, 
      const bool useDistance,
      Eigen::MatrixXd &innovationOut,
      Eigen::MatrixXd &jacobianOut,
      const int currentMeasurement, 
      const double dx, const double dy) {

   const double dx_2 = getNonZero(dx * dx);
   const double dy_2 = getNonZero(dy * dy);

   const double expectedDistance = sqrt(dx_2 + dy_2);
   const double expectedHeading = normaliseTheta(atan2(-dy, -dx) - mean(ROBOT_H_DIM, 0));

   innovationOut(currentMeasurement, 0) =
         normaliseTheta(observedObjectCoords.heading() - expectedHeading);
   
   if (useDistance) {
      innovationOut(currentMeasurement+1, 0) = 
            observedObjectCoords.distance() - expectedDistance;
   }
   
   jacobianOut(currentMeasurement, ROBOT_X_DIM) = -dy / (dx_2 + dy_2);
   jacobianOut(currentMeasurement, ROBOT_Y_DIM) = dx / (dx_2 + dy_2);
   jacobianOut(currentMeasurement, ROBOT_H_DIM) = -1.0;
   
   if (useDistance) {
      jacobianOut(currentMeasurement+1, ROBOT_X_DIM) = dx / expectedDistance;
      jacobianOut(currentMeasurement+1, ROBOT_Y_DIM) = dy / expectedDistance;
      jacobianOut(currentMeasurement+1, ROBOT_H_DIM) = 0.0;
   }
}

void SimpleGaussian::addObservationVariance(const VisionUpdateBundle &visionBundle,
      VarianceProvider::ObservationType type,
      const RRCoord &observedObject, bool useDistance, 
      Eigen::MatrixXd &observationVarianceOut,
      const int currentMeasurement) {

   VarianceProvider varianceProvider = VarianceProvider::instance();
   VarianceProvider::Observation observation(
         observedObject.distance(), observedObject.heading());
   
   double unreliableDistanceScale = LocalisationConstantsProvider::instance().get(
            LocalisationConstantsProvider::UNRELIABLE_DISTANCE_VARIANCE_SCALE);
   double unreliableHeadingScale = LocalisationConstantsProvider::instance().get(
            LocalisationConstantsProvider::UNRELIABLE_HEADING_VARIANCE_SCALE);
   
   observationVarianceOut(currentMeasurement, currentMeasurement) = 
         varianceProvider.getHeadingObservationVariance(type, observation);
   if (!visionBundle.isHeadingReliable || !visionBundle.isDistanceReliable) {
      observationVarianceOut(currentMeasurement, currentMeasurement) *= unreliableHeadingScale;
   }
   
   if (useDistance) {
      observationVarianceOut(currentMeasurement+1, currentMeasurement+1) = 
            varianceProvider.getDistanceObservationVariance(type, observation);
      if (!visionBundle.isDistanceReliable) {
         observationVarianceOut(currentMeasurement+1, currentMeasurement+1) *= unreliableDistanceScale;
      }
   }
}

int SimpleGaussian::performICPUpdate(
      const VisionUpdateBundle &visionBundle,
      Eigen::MatrixXd &innovationOut,
      Eigen::MatrixXd &jacobianOut,
      Eigen::MatrixXd &observationVarianceOut,
      const int currentMeasurement, 
      const bool haveKeyICPFeatures) {
   
   AbsCoord ballRRC = getBallPosition().convertToRobotRelativeCartesian(getRobotPose());
   
   // TODO: all of this should be in constants provider.
   const double MAX_FEATURE_DIST = 4000.0;
   const double UNRELIABLE_DIST_SCALE = 0.66;
   const double NO_KEY_FEATURES_SCALE = 0.66;
   
   double FEATURE_DIST_CUTOFF = MAX_FEATURE_DIST;
   if (!visionBundle.isDistanceReliable) {
      FEATURE_DIST_CUTOFF *= UNRELIABLE_DIST_SCALE;
   }
   if (!haveKeyICPFeatures) {
      FEATURE_DIST_CUTOFF *= NO_KEY_FEATURES_SCALE;
   }
   
   std::vector<FieldFeatureInfo> filteredFeatures;
   for (unsigned i = 0; i < visionBundle.fieldFeatures.size(); i++) {
      if (visionBundle.fieldFeatures[i].distance() < FEATURE_DIST_CUTOFF) {
         filteredFeatures.push_back(visionBundle.fieldFeatures[i]);
      }
   }
   
   if (filteredFeatures.size() == 0) {
      return currentMeasurement;
   }
   
   std::vector<PostInfo> stubPosts;
   if (filteredFeatures.size() > 0) { // only include posts in ICP if there are also other features.
      stubPosts = getICPQualityPosts(visionBundle);
   }
   
   int icpResult = ICP::localise(getRobotPose(), filteredFeatures, 
         stubPosts, visionBundle.awayGoalProb, visionBundle.headYaw, 
         visionBundle.fieldEdges, ballRRC, false);
   
   if (icpResult <= 0) {
      return currentMeasurement;
   }

   AbsCoord icpUpdate = ICP::getCombinedObs();
   
   double dx = icpUpdate.x() - mean(ROBOT_X_DIM, 0);
   double dy = icpUpdate.y() - mean(ROBOT_Y_DIM, 0);
   double dh = normaliseTheta(icpUpdate.theta() - mean(ROBOT_H_DIM, 0));
   
   // Basically we dont trust ICP observations that have only a single line for example. A single line can
   // quite easily match to the wrong line and completely wreck our localisation. So if we see only a single
   // line (ie: dont have a key feature), we only perform ICP if the pose adjustment is not large.
   if (!haveKeyICPFeatures) {
      if (icpUpdate.var(0, 0) > 0.0 && fabs(dx) > 400.0) {
         return 0;
      }
      
      if (icpUpdate.var(1, 1) > 0.0 && fabs(dy) > 400.0) {
         return 0;
      }
      
      if (icpUpdate.var(2, 2) > 0.0 && fabs(dh) > UNSWDEG2RAD(15.0)) {
         return 0;
      }
   }
   
   float featureDistance = 0.0;
   unsigned numFeatures = filteredFeatures.size();
   for (unsigned i = 0; i < filteredFeatures.size(); i++) {
      featureDistance += filteredFeatures[i].distance();
   }
   
   numFeatures += stubPosts.size();
   for (unsigned i = 0; i < stubPosts.size(); i++) {
      featureDistance += stubPosts[i].rr.distance();
   }
   
   if (numFeatures == 0) {
      featureDistance = 0.0;
   } else {
      featureDistance /= numFeatures;
   }
   
   VarianceProvider varianceProvider = VarianceProvider::instance();
   VarianceProvider::Observation observation(featureDistance, 0.0);
   
   // TODO: introduce an enum field into varianceprovider for ICP, rather than using GOALPOST...
   double icpDistanceVariance = 
         varianceProvider.getDistanceObservationVariance(VarianceProvider::GOALPOST, observation) *
         LocalisationConstantsProvider::instance().get(LocalisationConstantsProvider::ICP_DISTANCE_VARIANCE_SCALE);
   double icpHeadingVariance =
         LocalisationConstantsProvider::instance().get(LocalisationConstantsProvider::ICP_HEADING_VARIANCE);
   
   unsigned numMeasurements = 0;
   
   double unreliableDistanceScale = LocalisationConstantsProvider::instance().get(
            LocalisationConstantsProvider::UNRELIABLE_DISTANCE_VARIANCE_SCALE);
   double unreliableHeadingScale = LocalisationConstantsProvider::instance().get(
            LocalisationConstantsProvider::UNRELIABLE_HEADING_VARIANCE_SCALE);
   
   if (icpUpdate.var(0, 0) > 0.0) {
      unsigned measurement = currentMeasurement + numMeasurements;
      innovationOut(measurement, 0) = dx;
      jacobianOut(measurement, ROBOT_X_DIM) = 1.0;
      observationVarianceOut(measurement, measurement) = icpDistanceVariance;
      if (!visionBundle.isDistanceReliable) {
         observationVarianceOut(measurement, measurement) *= unreliableDistanceScale;
      }
      numMeasurements++;
   }
   
   if (icpUpdate.var(1, 1) > 0.0) {
      unsigned measurement = currentMeasurement + numMeasurements;
      innovationOut(measurement, 0) = dy;
      jacobianOut(measurement, ROBOT_Y_DIM) = 1.0;
      observationVarianceOut(measurement, measurement) = icpDistanceVariance;
      if (!visionBundle.isDistanceReliable) {
         observationVarianceOut(measurement, measurement) *= unreliableDistanceScale;
      }
      numMeasurements++;
   }
   
   if (icpUpdate.var(2, 2) > 0.0) {
      unsigned measurement = currentMeasurement + numMeasurements;
      innovationOut(measurement, 0) = dh;
      jacobianOut(measurement, ROBOT_H_DIM) = 1.0;
      observationVarianceOut(measurement, measurement) = icpHeadingVariance;
      if (!visionBundle.isHeadingReliable) {
         observationVarianceOut(measurement, measurement) *= unreliableHeadingScale;
      }
      numMeasurements++;
   }
   
   return currentMeasurement + numMeasurements;
}

void SimpleGaussian::updateMeanVectorWithOdometry(const Odometry &odometry, const double dTimeSeconds,
      OdometryUpdateResult &outOdometryUpdateResult) {
   
   double dx = odometry.forward*cos(mean(ROBOT_H_DIM, 0)) - odometry.left*sin(mean(ROBOT_H_DIM, 0));
   double dy = odometry.forward*sin(mean(ROBOT_H_DIM, 0)) + odometry.left*cos(mean(ROBOT_H_DIM, 0));

   // Update the robot pose estimate.
   mean(ROBOT_H_DIM, 0) = normaliseTheta(mean(ROBOT_H_DIM, 0) + odometry.turn);

   mean(ROBOT_X_DIM, 0) += dx;
   mean(ROBOT_Y_DIM, 0) += dy;
   
   // Update the ball position estimate.
   if (!doingBallLineUp) {
      mean(BALL_X_DIM, 0) += mean(BALL_DX_DIM, 0) * dTimeSeconds;
      mean(BALL_Y_DIM, 0) += mean(BALL_DY_DIM, 0) * dTimeSeconds;
   }

   // Update the ball velocity estimate.
   const double frictionModulation = pow(constantsProvider.get(
         LocalisationConstantsProvider::BALL_FRICTION), dTimeSeconds);
   mean(BALL_DX_DIM, 0) *= frictionModulation;
   mean(BALL_DY_DIM, 0) *= frictionModulation;
   
   outOdometryUpdateResult.dx = dx;
   outOdometryUpdateResult.dy = dy;
   outOdometryUpdateResult.dh = odometry.turn;
   
   sanityCheck();
}

void SimpleGaussian::updateCovarianceWithOdometry(const Odometry &odometry, const double dTimeSeconds,
      const bool canSeeBall, OdometryUpdateResult &outOdometryUpdateResult) {
   processUpdateCovarianceMatrix(odometry, dTimeSeconds);
   
   // Add uncertainty to the main diagonal of the covariance matrix.
   additiveProcessNoiseUpdateCovarianceMatrix(odometry, dTimeSeconds, canSeeBall, outOdometryUpdateResult);
}

void SimpleGaussian::updateMeanVectorWithRemoteOdometry(
      const SharedLocalisationUpdateBundle &updateBundle, int teammateIndex) {
   const unsigned poseXIndex = getTeammateIndex(teammateIndex, ROBOT_X_DIM);
   const unsigned poseYIndex = getTeammateIndex(teammateIndex, ROBOT_Y_DIM);
   const unsigned poseHIndex = getTeammateIndex(teammateIndex, ROBOT_H_DIM);

   mean(poseXIndex, 0) += updateBundle.sharedDx;
   mean(poseYIndex, 0) += updateBundle.sharedDy;
   mean(poseHIndex, 0) += updateBundle.sharedDh;
}

void SimpleGaussian::updateCovarianceWithRemoteOdometry(
      const SharedLocalisationUpdateBundle &updateBundle, int teammateIndex) {
   MY_ASSERT(DIM == MAIN_DIM, "update covariance with odometry unexpected dim");
   
   const unsigned poseXIndex = getTeammateIndex(teammateIndex, ROBOT_X_DIM);
   const unsigned poseYIndex = getTeammateIndex(teammateIndex, ROBOT_Y_DIM);
   const unsigned poseHIndex = getTeammateIndex(teammateIndex, ROBOT_H_DIM);
   
   const double uncertaintyScale = constantsProvider.get(
         LocalisationConstantsProvider::TEAMMATE_ODOMETRY_UNCERTAINTY_SCALE);
   
   covariance(poseXIndex, poseXIndex) += updateBundle.sharedCovarianceDx * uncertaintyScale;
   covariance(poseYIndex, poseYIndex) += updateBundle.sharedCovarianceDy * uncertaintyScale;
   covariance(poseHIndex, poseHIndex) += updateBundle.sharedCovarianceDh * uncertaintyScale;
}

// This is basically the A*C*Atranspose part of the motion update
void SimpleGaussian::processUpdateCovarianceMatrix(const Odometry &odometry, const double dTimeSeconds) {
   const double frictionModulation = pow(constantsProvider.get(
         LocalisationConstantsProvider::BALL_FRICTION), dTimeSeconds);

   double useBallVelocity = doingBallLineUp ? 0.0 : 1.0;
   for (unsigned col = 0; col < DIM; col++) {
      covariance(col, BALL_X_DIM) += covariance(col, BALL_DX_DIM) * dTimeSeconds * useBallVelocity;
      covariance(col, BALL_Y_DIM) += covariance(col, BALL_DY_DIM) * dTimeSeconds * useBallVelocity;
      covariance(col, BALL_DX_DIM) *= frictionModulation;
      covariance(col, BALL_DY_DIM) *= frictionModulation;
   }

   for (unsigned row = 0; row < DIM; row++) {
      covariance(BALL_X_DIM, row) += covariance(BALL_DX_DIM, row) * dTimeSeconds * useBallVelocity;
      covariance(BALL_Y_DIM, row) += covariance(BALL_DY_DIM, row) * dTimeSeconds * useBallVelocity;
      covariance(BALL_DX_DIM, row) *= frictionModulation;
      covariance(BALL_DY_DIM, row) *= frictionModulation;
   }
}

void SimpleGaussian::additiveProcessNoiseUpdateCovarianceMatrix(
      const Odometry &odometry, const double dTimeSeconds, const bool canSeeBall,
      OdometryUpdateResult &outOdometryUpdateResult) {
   double dx = odometry.forward*cos(mean(ROBOT_H_DIM, 0)) - odometry.left*sin(mean(ROBOT_H_DIM, 0));
   double dy = odometry.forward*sin(mean(ROBOT_H_DIM, 0)) + odometry.left*cos(mean(ROBOT_H_DIM, 0));
   double dh = fabs(odometry.turn);
   
   // Increase the robot x,y pose covariance.
   double rPosA = constantsProvider.get(
         LocalisationConstantsProvider::ROBOT_POS_MOTION_UPDATE_COVARIANCE_A);
   double rPosC = constantsProvider.get(
         LocalisationConstantsProvider::ROBOT_POS_MOTION_UPDATE_COVARIANCE_C);
   outOdometryUpdateResult.covDx = rPosA*dx*dx;
   outOdometryUpdateResult.covDy = rPosA*dy*dy;
   covariance(ROBOT_X_DIM, ROBOT_X_DIM) += outOdometryUpdateResult.covDx + rPosC*dTimeSeconds;
   covariance(ROBOT_Y_DIM, ROBOT_Y_DIM) += outOdometryUpdateResult.covDy + rPosC*dTimeSeconds;
   
   // Increase the robot heading pose covariance.
   double rHeadingA = constantsProvider.get(
         LocalisationConstantsProvider::ROBOT_HEADING_MOTION_UPDATE_COVARIANCE_A);
   double rHeadingC = constantsProvider.get(
         LocalisationConstantsProvider::ROBOT_HEADING_MOTION_UPDATE_COVARIANCE_C);
   outOdometryUpdateResult.covDh = rHeadingA*dh*dh;
   covariance(ROBOT_H_DIM, ROBOT_H_DIM) += outOdometryUpdateResult.covDh + rHeadingC*dTimeSeconds;
   
   
   if (DIM == MAIN_DIM) {
      const double uncertaintyScale = constantsProvider.get(
            LocalisationConstantsProvider::TEAMMATE_ODOMETRY_UNCERTAINTY_SCALE);
      
      for (int teammateIndex = 0; teammateIndex < NUM_TEAMMATES; teammateIndex++) {
         const unsigned poseXIndex = getTeammateIndex(teammateIndex, ROBOT_X_DIM);
         const unsigned poseYIndex = getTeammateIndex(teammateIndex, ROBOT_Y_DIM);
         const unsigned poseHIndex = getTeammateIndex(teammateIndex, ROBOT_H_DIM);
         
         covariance(poseXIndex, poseXIndex) += uncertaintyScale*rPosC*dTimeSeconds;
         covariance(poseYIndex, poseYIndex) += uncertaintyScale*rPosC*dTimeSeconds;
         covariance(poseHIndex, poseHIndex) += uncertaintyScale*rHeadingC*dTimeSeconds;
      }
   }
   
   // If the ball is off the field we increase its variance faster since it is likely to be placed
   // at a throw in-position.
   bool ballOffField = 
         mean(BALL_X_DIM, 0) < -FIELD_X_CLIP ||
         mean(BALL_Y_DIM, 0) < -FIELD_Y_CLIP ||
         mean(BALL_X_DIM, 0) > FIELD_X_CLIP ||
         mean(BALL_Y_DIM, 0) > FIELD_Y_CLIP;
   
   // Increase the ball position covariance.
   double bPosC = constantsProvider.get(
         LocalisationConstantsProvider::BALL_POS_MOTION_UPDATE_COVARIANCE_C);
   double bVelC = constantsProvider.get(
         LocalisationConstantsProvider::BALL_VEL_MOTION_UPDATE_COVARIANCE_C);
   
   if (!canSeeBall || ballOffField) {
      bPosC = constantsProvider.get(
            LocalisationConstantsProvider::BALL_UNSEEN_POS_MOTION_UPDATE_COVARIANCE_C);
   } else if (doingBallLineUp) {
      bPosC = constantsProvider.get(
            LocalisationConstantsProvider::BALL_POS_MOTION_UPDATE_LINE_UP_COVARIANCE_C);
   }
   
   covariance(BALL_X_DIM, BALL_X_DIM) += bPosC * dTimeSeconds;
   covariance(BALL_Y_DIM, BALL_Y_DIM) += bPosC * dTimeSeconds;
   
   // Increase the ball velocity covariance.
   covariance(BALL_DX_DIM, BALL_DX_DIM) += bVelC * dTimeSeconds;
   covariance(BALL_DY_DIM, BALL_DY_DIM) += bVelC * dTimeSeconds;
}

bool SimpleGaussian::goalieUpdateDisagrees(const SharedLocalisationUpdateBundle &updateBundle) const {
   double symmetricYDistance = 
         fabs(updateBundle.sharedUpdateMean(BALL_Y_DIM, 0) + mean(BALL_Y_DIM, 0));
   
   return updateBundle.ballSeenFraction >= 0.75f && 
          mean(BALL_X_DIM, 0) > 1500.0 && 
          updateBundle.sharedUpdateMean(BALL_X_DIM, 0) < -1500.0 &&
          symmetricYDistance < 1000.0;
}

bool SimpleGaussian::isBallTooCloseForRemoteUpdate(void) const {
   double dx = mean(BALL_X_DIM, 0) - mean(ROBOT_X_DIM, 0);
   double dy = mean(BALL_Y_DIM, 0) - mean(ROBOT_Y_DIM, 0);
   
   return sqrt(dx*dx + dy*dy) < 500.0;
}

void SimpleGaussian::clipToField(Eigen::MatrixXd &pose) {
   pose(ROBOT_X_DIM, 0) = crop<double>(pose(ROBOT_X_DIM, 0), -FIELD_X_CLIP, FIELD_X_CLIP);
   pose(ROBOT_Y_DIM, 0) = crop<double>(pose(ROBOT_Y_DIM, 0), -FIELD_Y_CLIP, FIELD_Y_CLIP);
   
   pose(BALL_X_DIM, 0) = crop<double>(pose(BALL_X_DIM, 0), -FIELD_X_CLIP, FIELD_X_CLIP);
   pose(BALL_Y_DIM, 0) = crop<double>(pose(BALL_Y_DIM, 0), -FIELD_Y_CLIP, FIELD_Y_CLIP);
   
   pose(BALL_DX_DIM, 0) = crop<double>(pose(BALL_DX_DIM, 0), -MAX_BALL_VELOCITY, MAX_BALL_VELOCITY);
   pose(BALL_DY_DIM, 0) = crop<double>(pose(BALL_DY_DIM, 0), -MAX_BALL_VELOCITY, MAX_BALL_VELOCITY);
}

void SimpleGaussian::clipBallOutOfRobot(Eigen::MatrixXd &pose) {
   const double MIN_BALL_ROBOT_DIST = 10.0;
   
   double toBallX = pose(BALL_X_DIM, 0) - pose(ROBOT_X_DIM, 0);
   double toBallY = pose(BALL_Y_DIM, 0) - pose(ROBOT_Y_DIM, 0);
   double toBallLength = sqrt(toBallX*toBallX + toBallY*toBallY);

   // TODO: set the ball in front of the robot, based on heading, rather than just offset on X-axis.
   if (toBallLength < MIN_BALL_ROBOT_DIST) {
      if (toBallLength < EPSILON) {
         toBallX = 1.0;
         toBallY = 0.0;
         toBallLength = 1.0;
      }
      
      pose(BALL_X_DIM, 0) = pose(ROBOT_X_DIM, 0) + MIN_BALL_ROBOT_DIST * toBallX / toBallLength;
      pose(BALL_Y_DIM, 0) = pose(ROBOT_Y_DIM, 0) + MIN_BALL_ROBOT_DIST * toBallY / toBallLength;
      
      pose(BALL_DX_DIM, 0) = 0.0;
      pose(BALL_DY_DIM, 0) = 0.0;
   }
}

bool SimpleGaussian::isStateValid(void) const {
   if (weight < 0.0 || weight > 1.0 || weight != weight || !std::isfinite(weight)) {
      std::cout << "weight failed: " << weight << std::endl;
      return false;
   }
   
   for (int i = 0; i < mean.rows(); i++) {
      if (mean(i, 0) != mean(i, 0) || !std::isfinite(mean(i, 0))) {
         std::cout << "mean failed: " << mean << std::endl;
         return false;
      }
   }
   
   for (int i = 0; i < covariance.rows(); i++) {
      for (int j = 0; j < covariance.cols(); j++) {
         if (covariance(i, j) != covariance(i, j) || !std::isfinite(covariance(i, j))) {
            std::cout << "covariance failed: " << covariance(i, j) << std::endl;
            return false;
         }
      }
   }
   
   return true;
}

AbsCoord SimpleGaussian::getPenaltySpotPosition(void) {
   if (mean(ROBOT_X_DIM, 0) > FIELD_LENGTH / 5) {
      return AbsCoord(MARKER_CENTER_X, 0.0, 0.0);
   } else {
      return AbsCoord(-MARKER_CENTER_X, 0.0, 0.0);
   }
}

bool SimpleGaussian::canObservePenaltySpot(void) {
   return (mean(ROBOT_X_DIM, 0) > FIELD_LENGTH/5.0) || (mean(ROBOT_X_DIM, 0) < -FIELD_LENGTH/5.0);
}

double SimpleGaussian::getNonZero(double val) {
   if (val >= 0.0 && val < EPSILON) {
      return EPSILON;
   } else if (val <= 0.0 && val > -EPSILON) {
      return -EPSILON;
   } else {
      return val;
   }
}

AbsCoord SimpleGaussian::getGoalpostPosition(PostType type) {
   switch(type) {
   case MY_LEFT:
      return AbsCoord(-GOAL_POST_ABS_X, -GOAL_POST_ABS_Y, 0.0);
   case MY_RIGHT:
      return AbsCoord(-GOAL_POST_ABS_X, GOAL_POST_ABS_Y, 0.0);
   case OPPONENT_LEFT:
      return AbsCoord(GOAL_POST_ABS_X, GOAL_POST_ABS_Y, 0.0);
   case OPPONENT_RIGHT:
      return AbsCoord(GOAL_POST_ABS_X, -GOAL_POST_ABS_Y, 0.0);
   }
}
