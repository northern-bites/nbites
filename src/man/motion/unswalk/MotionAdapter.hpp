#pragma once

#include <string>
#include <map>
#include "motion/effector/Effector.hpp"
#include "motion/generator/Generator.hpp"
#include "motion/touch/FilteredTouch.hpp"
#include "blackboard/Adapter.hpp"
#include "motion/generator/BodyModel.hpp"
#include "perception/kinematics/Kinematics.hpp"
#include "motion/SonarRecorder.hpp"

/**
 * MotionAdapter - interfaces between Motion & rest of system via Blackboard
 *
 * The MotionAdapter is the controlling class for everything Motion. It reads
 * ActionCommands from the Blackboard, and SensorValues from its Touch instance.
 *
 * It then passes AC::Head, AC::Body & SensorValues to a Generator instance,
 * which processes them and determines the correct JointValues for the next DCM
 * cycle. In practice, the Generator will usually be a DistributorGenerator,
 * which will select the most appropriate Generator based on the AC's.
 *
 * These JointValues, plus AC::LED are passed to an Effector instance. The
 * effector actuates the joints and LEDs according to these instructions.
 */
class MotionAdapter : Adapter {
   public:
      /* Constructor */
      MotionAdapter(Blackboard *bb);
      /* Destructor */
      ~MotionAdapter();
      /* One cycle of this thread */
      void tick();
      /* Read values from global options */
      void readOptions(const boost::program_options::variables_map& config);
   private:

      /* Buffers so synchronises with vision thread */
      std::vector<Odometry> odometryBuffer;
      std::vector<SensorValues> sensorBuffer;
      /* Sonar window recorder */
      SonarRecorder sonarRecorder;
      /* Duration since we last were told to stand up by libagent (seconds) */
      float uptime;
      /* Maps of available switchable instances */
      std::map<std::string, Touch*> touches;
      std::map<std::string, Effector*> effectors;
      /* Motion module instance */
      Touch* nakedTouch;  //original, unfiltered
      Touch* touch;
      Generator* generator;
      Effector* effector;
      BodyModel bodyModel;
      Kinematics kinematics;
};
