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

#pragma once

#include "GaussianUpdateDebugData.hpp"
#include "PostType.hpp"
#include "ObservedPostsHistory.hpp"
#include "SharedLocalisationUpdateBundle.hpp"
#include "VisionUpdateBundle.hpp"
#include "VarianceProvider.hpp"
#include "ICP.hpp"
#include "LocalisationDefs.hpp"
#include "types/AbsCoord.hpp"
#include "types/Odometry.hpp"

#include <cassert>
#include <Eigen/Eigen>


/**
 * A normal vision update is inherently multi-modal. For example, in a vision bundle if 
 * we have a goal post object, it is not defined which of the 4 posts it corresponds to,
 * as they all look the same. What we do is generate multiple UniModalVisionUpdates from a
 * given VisionUpdateBundle that cover all of the possible correspondences from observation to
 * field landmark.
 */
struct UniModalVisionUpdate {
   UniModalVisionUpdate() {}
   
   UniModalVisionUpdate(const VisionUpdateBundle &visionBundle);

   UniModalVisionUpdate(const VisionUpdateBundle &visionBundle,
         const PostType &postType);

   UniModalVisionUpdate(const VisionUpdateBundle &visionBundle,
         const std::vector<PostType> &postTypes);
   
   VisionUpdateBundle visionBundle;
   std::vector<PostType> postTypes;
};


struct UniModalTeammateUpdate {
   UniModalTeammateUpdate(const VisionUpdateBundle &visionBundle, 
         const std::vector<UNSWRobotInfo> &teammateRobots, const AbsCoord &teammatePosition);
   
   UniModalTeammateUpdate(const VisionUpdateBundle &visionBundle, 
         const std::vector<UNSWRobotInfo> &teammateRobots, const std::vector<AbsCoord> &teammatePositions);
   
   VisionUpdateBundle visionBundle;
   std::vector<UNSWRobotInfo> teammateRobots;
   std::vector<AbsCoord> teammatePositions;
};


struct StoredICPUpdate {
   StoredICPUpdate() : updateDimension(0) {}
   
   StoredICPUpdate(int updateDimension, const Eigen::MatrixXd &innovation, 
         const Eigen::MatrixXd &jacobian, const Eigen::MatrixXd &observationVariance) :
            updateDimension(updateDimension), innovation(innovation), 
            jacobian(jacobian), observationVariance(observationVariance) {}
   
   int updateDimension;
   Eigen::MatrixXd innovation;
   Eigen::MatrixXd jacobian;
   Eigen::MatrixXd observationVariance;
};


struct OdometryUpdateResult {
   double dx, dy, dh;
   double covDx, covDy, covDh;
};

/**
 * A SimpleGaussian represents a single mode of a multi-model distribution. Each Gaussian mode
 * has an associated mean state, covariance matrix, and a weight. The weight represents the
 * confidence of the mode and is used as part of an overall distribution which is represented as
 * a weighted sum of Gaussians.
 */
class SimpleGaussian {
public:

   /**
    * Static constructor function to create baseline Gaussians with very high variance and low
    * weight.
    */
   static std::vector<SimpleGaussian*> createBaselineGaussians(void);
   static SimpleGaussian* createBaselineSharedGaussian(void);

   /**
    * Set the mean to the given vector.
    */
   void resetMean(const Eigen::MatrixXd &src);
   
   /**
    * Sets the covariance matrix to the given matrix.
    */
   void resetCovariance(const Eigen::MatrixXd &src);
   
   /**
    * Sets whether the robot is currently lining up to kick the ball. This is basically
    * an indication to the filter to disgregard ball velocity when calculating its position
    * to get a more stable, but potentially less accurate, result. See the corresponding method
    * in MultiGaussianDistribution for more info.
    */
   void setLineUpMode(bool enabled);
   
   /**
    * Enables/Disables the Ready mode. For more info, see the corresponding method in 
    * MultiGaussianDistribution.
    */
   void setReadyMode(bool enabled);
   
   /**
    * Performs the odometry update on the Gaussian.
    */
   OdometryUpdateResult processUpdate(const Odometry &odometry, const double dTimeSeconds,
         const bool canSeeBall);

   /**
    * Performs a vision update on this Gaussian, and returns a vector of newly generated hypotheses
    * which are in addition to this Gaussian. These should be inserted back into the distribution.
    */
   std::vector<SimpleGaussian*> visionUpdate(const VisionUpdateBundle &visionBundle);
   std::vector<SimpleGaussian*> visionTeammateRobotsUpdate(const VisionUpdateBundle &visionBundle);
   
   double doICPUpdate(const VisionUpdateBundle &visionBundle, const bool updateWeight);
   
   int uniModalVisionUpdate(const UniModalVisionUpdate &vu);
   void uniModalTeammateRobotVisionUpdate(const UniModalTeammateUpdate &vu);
   
   std::vector<SimpleGaussian*> applyRemoteUpdate(const SharedLocalisationUpdateBundle &updateBundle,
         int teammateIndex, bool amGoalie);
   
   double applyObservation(int obsDimension, const Eigen::MatrixXd &innovation, 
         const Eigen::MatrixXd &jacobian, const Eigen::MatrixXd &observationVariance,
         const bool updateWeight);
   
   /**
    * Returns the x,y,theta pose of the robot.
    */
   AbsCoord getRobotPose(void) const;
   
   /**
    * Returns the x,y ball world position
    */
   AbsCoord getBallPosition(void) const;
   
   /**
    * Returns the x',y' ball velocity in milli-metres per second.
    */
   AbsCoord getBallVelocity(void) const;
   
   /**
    * Returns a measure of uncertainty of the robots position estimation. This is the area
    * bound by the 1 standard deviation of the covariance matrix.
    */
   double getRobotPosUncertainty(void) const;
   
   /**
    * Returns the 1 standard deviation of the heading part of the covariance matrix.
    */
   double getRobotHeadingUncertainty(void) const;
   
   /**
    * Returns a measure of uncertainty of the ball position estimation. This is the area
    * bound by the 1 standard deviation of the covariance matrix.
    */
   double getBallPosUncertainty(void) const;
   
   /**
    * Returns a measure of uncertainty of the ball velocity estimation. This is the area
    * bound by the 1 standard deviation of the covariance mZatrix.
    */
   double getBallVelocityUncertainty(void) const;
   
   /**
    * Returns whether this and the other Gaussian are similar enough in their mean and variance to
    * merge.
    */
   bool isSimilarTo(const SimpleGaussian &other) const;

   /**
    * Combines this Gaussian with the other. The other Gaussian has its weight set to 0.
    */
   void mergeWith(SimpleGaussian &other);

   double getWeight(void) const;
   void setWeight(double weight);
   
   bool getHaveLastVisionUpdate(void) const;
   
   StoredICPUpdate getLastAppliedICPUpdate(void) const;
   UniModalVisionUpdate getLastAppliedVisionUpdate(void) const;
   
   Eigen::MatrixXd getMean(void) const;
   Eigen::MatrixXd getCovariance(void) const;
   
   void sanityCheck(void) const;
   
   /**
    * Creates a Gaussian that is a symmetric reflection of the current one.
    */
   SimpleGaussian* createSymmetricGaussian(void);
   
private:
   
   // The dimensionality of the state space. Should be either 7 or 19
   // DIM (0,1,2) => Robot x,y,theta world position.
   // DIM (3,4) => Ball x,y world position.
   // DIM (5,6) => Ball x,y world velocity.
   const unsigned DIM;
   const Eigen::MatrixXd identity;

   double weight;
   Eigen::MatrixXd mean; // DIM x 1
   Eigen::MatrixXd covariance; // DIM x DIM

   bool doingBallLineUp;
   bool isInReadyMode;
   bool haveLastVisionUpdate;
   UniModalVisionUpdate lastVisionUpdate;
   StoredICPUpdate storedICPUpdate;
   ObservedPostsHistory observedPostsHistory;
   
   /**
    * Constructs a Gaussian with the given mean, a covariance matrix with the given diagonal
    * values, and the given weight.
    */
   SimpleGaussian(
         const unsigned dim,
         const double weight,
         const Eigen::MatrixXd &mean,
         const Eigen::MatrixXd &diagonalVariance);
   
   // Basically a copy constructor but one that is used by createSplitGaussian and 
   // creates a new id.
   SimpleGaussian(
         const unsigned dim,
         const double weight,
         const Eigen::MatrixXd &mean,
         const Eigen::MatrixXd &covariance,
         bool doingBallLineUp,
         bool isInReadyMode,
         const ObservedPostsHistory &observedPostsHistory);
   
   std::vector<PostInfo> getICPQualityPosts(const VisionUpdateBundle &visionBundle);
   
   bool canUsePostType(PostType type, double distance);
   bool canUsePostType(PostType type0, double distance0, PostType type1, double distance1);
   double distanceTo(const AbsCoord &coord);
   
   /**
    * Creates a Gaussian that is a copy of the current one.
    */
   SimpleGaussian* createSplitGaussian(void);
   
   /**
    * Returns whether the given row index corresponds to a heading in the mean vector.
    */
   bool isHeadingRow(int row);

   /**
    * Returns an angle that corresponds to the given one, but is within the range [-pi, pi].
    */
   // TODO: replace this with a method from angles.hpp.
   static double normaliseTheta(double theta);
   
   void addNoPostsObservationMode(const VisionUpdateBundle &visionBundle,
            std::vector<SimpleGaussian*> &outModes);
   void addOnePostObservationModes(const VisionUpdateBundle &visionBundle,
         std::vector<SimpleGaussian*> &outModes);
   void addTwoPostObservationModes(const VisionUpdateBundle &visionBundle,
         std::vector<SimpleGaussian*> &outModes);
   
   double performKalmanUpdate(
         const int observationDim,
         const Eigen::MatrixXd &innovation,
         const Eigen::MatrixXd &jacobian,
         const Eigen::MatrixXd &observationVariance,
         const bool updateWeight);
   
   double performTrimmedKalmanUpdate(
         const Eigen::MatrixXd &innovation,
         const Eigen::MatrixXd &jacobian,
         const Eigen::MatrixXd &observationVariance,
         const bool isSharedUpdate,
         const bool updateWeight);

   int addGoalPostMeasurement(
         const VisionUpdateBundle &visionBundle,
         const RRCoord &observedPostCoords,
         const AbsCoord &postWorldCoords,
         const bool useDistance,
         Eigen::MatrixXd &innovationOut,
         Eigen::MatrixXd &jacobianOut,
         Eigen::MatrixXd &observationVarianceOut,
         const int currentMeasurement);
   
   int addFieldFeatureMeasurement(
         const VisionUpdateBundle &visionBundle,
         const AbsCoord &fieldFeaturePosition,
         const RRCoord &observedCentreCircleCoords,
         const bool useDistance,
         Eigen::MatrixXd &innovationOut,
         Eigen::MatrixXd &jacobianOut,
         Eigen::MatrixXd &observationVarianceOut,
         const int currentMeasurement);

   int addBallMeasurement(
         const VisionUpdateBundle &visionBundle,
         const RRCoord &observedBallCoords, 
         const bool useDistance,
         Eigen::MatrixXd &innovationOut,
         Eigen::MatrixXd &jacobianOut,
         Eigen::MatrixXd &observationVarianceOut,
         const int currentMeasurement);
   
   int addTeammateMeasurement(
         const VisionUpdateBundle &visionBundle,
         const RRCoord &observedRobotCoords, 
         const AbsCoord &teammatePosition,
         const bool useDistance,
         Eigen::MatrixXd &innovationOut,
         Eigen::MatrixXd &jacobianOut,
         Eigen::MatrixXd &observationVarianceOut,
         const int currentMeasurement);
   
   void addGenericMeasurement(
         const RRCoord &observedObjectCoords, 
         const bool useDistance,
         Eigen::MatrixXd &innovationOut,
         Eigen::MatrixXd &jacobianOut,
         const int currentMeasurement, 
         const double dx, const double dy);
   
   void addObservationVariance(
         const VisionUpdateBundle &visionBundle,
         VarianceProvider::ObservationType type, 
         const RRCoord &observedObject,
         bool useDistance, 
         Eigen::MatrixXd &observationVarianceOut,
         const int currentMeasurement);
   
   int performICPUpdate(
         const VisionUpdateBundle &visionBundle,
         Eigen::MatrixXd &innovationOut,
         Eigen::MatrixXd &jacobianOut,
         Eigen::MatrixXd &observationVarianceOut,
         const int currentMeasurement,
         const bool haveKeyICPFeatures);

   void updateMeanVectorWithOdometry(const Odometry &odometry, const double dTimeSeconds, 
         OdometryUpdateResult &outOdometryUpdateResult);
   void updateCovarianceWithOdometry(const Odometry &odometry, const double dTimeSeconds,
         const bool canSeeBall, OdometryUpdateResult &outOdometryUpdateResult);
   
   void updateMeanVectorWithRemoteOdometry(const SharedLocalisationUpdateBundle &updateBundle, int teammateIndex);
   void updateCovarianceWithRemoteOdometry(const SharedLocalisationUpdateBundle &updateBundle, int teammateIndex);
   
   void processUpdateCovarianceMatrix(const Odometry &odometry, const double dTimeSeconds);
   void additiveProcessNoiseUpdateCovarianceMatrix(const Odometry &odometry, const double dTimeSeconds,
         const bool canSeeBall, OdometryUpdateResult &outOdometryUpdateResult);
   
   void clipToField(Eigen::MatrixXd &pose);
   void clipBallOutOfRobot(Eigen::MatrixXd &pose);

   bool isStateValid(void) const;
   
   bool goalieUpdateDisagrees(const SharedLocalisationUpdateBundle &updateBundle) const;
   bool isBallTooCloseForRemoteUpdate(void) const;
   
   AbsCoord getPenaltySpotPosition(void);
   bool canObservePenaltySpot(void);
   
   /**
    * Returns a value that is on the same side of 0 on the number line as the input value, and is
    * as close as possible to the value but is not within EPSILON of 0.
    */
   static double getNonZero(double val);
   
   static AbsCoord getGoalpostPosition(PostType type);
};
