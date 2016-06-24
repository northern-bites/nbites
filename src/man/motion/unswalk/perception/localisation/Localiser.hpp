#pragma once

#include <vector>
#include <memory>

#include "MultiGaussianDistribution.hpp"
#include "SharedDistribution.hpp"
#include "SharedLocalisationUpdateBundle.hpp"
#include "VisionUpdateBundle.hpp"
#include "LocalisationConstantsProvider.hpp"
#include "types/AbsCoord.hpp"
#include "types/Odometry.hpp"
#include "types/ActionCommand.hpp"
#include "types/BroadcastData.hpp"
#include "gamecontroller/RoboCupGameControlData.hpp"

struct LocaliserBundle {
   LocaliserBundle():
      odometry(),
      visionUpdateBundle(),
      acActive(),
      isPenalised(false),
      state(0),
      state2(0),
      dTimeSeconds(0.0) {}

   LocaliserBundle(const Odometry &odometry,
                   const VisionUpdateBundle &visionUpdateBundle,
                   const ActionCommand::All &acActive,
                   const bool &isPenalised,
                   const uint8 &state,
                   const uint8 &state2,
                   const double dTimeSeconds) :
                      odometry(odometry),
                      visionUpdateBundle(visionUpdateBundle),
                      acActive(acActive),
                      isPenalised(isPenalised),
                      state(state),
                      state2(state2),
                      dTimeSeconds(dTimeSeconds) {}
   
   Odometry odometry;
   VisionUpdateBundle visionUpdateBundle;
   ActionCommand::All acActive;
   bool isPenalised;
   uint8 state;
   uint8 state2;
   double dTimeSeconds;
};

class Localiser {
   public:
      Localiser(int playerNumber);
      ~Localiser();

      /**
       * Resets the distribution and sets the position hypothesis of the robot to be
       * at the side-line start positions.
       */
      void setReset(void);
      
      void resetToPenaltyShootout(void);
      
      /**
       * Sets whether the robot is currently lining up with the ball to kick it. This
       * is intended to allow localisation to perform special-cased behaviour and/or
       * use different constants.
       */
      void setLineUpMode(bool enabled);
      
      /**
       * During Ready we want to do some special case localisation stuff (ie: doing ICP on all modes,
       * ignoring any ball observations, etc).
       */
      void setReadyMode(bool enabled);
      
      /**
       * This is called the very first frame of PLAY when coming out of SET mode. This should trim the
       * localiser hypotheses using the knowledge that we MUST start play in our own half.
       */
      void startOfPlayReset(void);
      
      AbsCoord getRobotPose(void);
      AbsCoord getBallPosition(void);
      AbsCoord getBallVelocity(void);
      unsigned getBallLostCount(void);
      
      double getRobotPosUncertainty(void) const;
      double getRobotHeadingUncertainty(void) const;
      double getBallPosUncertainty(void) const;
      double getBallVelocityUncertainty(void) const;
      
      SharedLocalisationUpdateBundle getSharedUpdateData(void) const;
      void resetSharedUpdateData(void);
      
      void localise(const LocaliserBundle &lb, const bool canDoObservations);
      void applyRemoteUpdate(const BroadcastData &broadcastData, int playerNumber);
      
      double getLastObservationLikelyhood(void) const;

   private:
      int myPlayerNumber;
      unsigned ballLostCount;
      
      MultiGaussianDistribution *worldDistribution;
      SharedDistribution *sharedDistribution;
};

inline std::ostream& operator<<(std::ostream& os, const LocaliserBundle& bundle) {
   os.write((char*) &(bundle.odometry), sizeof(Odometry));
   os << bundle.visionUpdateBundle;
   os.write((char*) &(bundle.dTimeSeconds), sizeof(double));
   
   return os;
}

inline std::istream& operator>>(std::istream& is, LocaliserBundle& bundle) {
   is.read((char*) &(bundle.odometry), sizeof(Odometry));
   if (is.eof() || !is.good()) {
      return is;
   }
   
   is >> bundle.visionUpdateBundle;
   if (is.eof() || !is.good()) {
      return is;
   }
   
   is.read((char*) &(bundle.dTimeSeconds), sizeof(double));
   return is;
}
