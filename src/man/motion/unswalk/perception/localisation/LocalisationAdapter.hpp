#pragma once

#include <string>
#include <fstream>

#include "blackboard/Adapter.hpp"
#include "types/Odometry.hpp"
#include "types/ActionCommand.hpp"
#include "VisionUpdateBundle.hpp"
#include "robotfilter/RobotFilter.hpp"

/* Forward declarations */
class Localiser;
class RobotFilter;

/* Adapter that allows Localisation to communicate with the Blackboard */
class LocalisationAdapter : Adapter {
   public:
      /* Constructor */
      LocalisationAdapter(Blackboard *bb);
      /* Destructor */
      ~LocalisationAdapter();
      /* One cycle of this thread */
      void tick();

   private:
      /* Filter module instances */
      Localiser *L;
      RobotFilter *robotFilter;
      bool firstCycle;
      int playerNumber;
      Odometry prevOdometry;
      int64_t prevTimestamp;
      uint8_t prevGameState;
      
      bool isInPenaltyShootout;
      
      std::ofstream logStream;
      
      bool canLocaliseInState(uint8_t state, std::string skill);
      bool canDoObservations(void);
      
      bool haveTransitionedSetToPlay(void);
      bool haveTransitionedIntoSet(void);
      std::vector<AbsCoord> getTeammatePoses(void);
      
      bool amWalking(const ActionCommand::All &commands);
      bool amTurningHead(const ActionCommand::All &commands);
      
      void writeResultToBlackboard(void);
      void handleMySharedDistribution(void);
      void handleIncomingSharedUpdate(void);
      
      // The below methods/variables are used for calibration purposes.
      std::vector<double> distanceObservations;
      std::vector<double> headingObservations;
      int numFramesSinceSeen;
      
      void handleObservationMeasurements(const VisionUpdateBundle &visionUpdateBundle);
      void outputVariances(void);
      
      double getMean(const std::vector<double> &vals, double outliersRatio);
      double getVariance(const std::vector<double> &vals, double mean, double outliersRatio);
};
