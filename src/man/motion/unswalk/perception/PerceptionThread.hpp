#pragma once

#include <string>
#include "perception/vision/VisionAdapter.hpp"
#include "perception/localisation/LocalisationAdapter.hpp"
#include "perception/behaviour/BehaviourAdapter.hpp"
#include "perception/kinematics/KinematicsAdapter.hpp"
#include "perception/dumper/PerceptionDumper.hpp"
#include "blackboard/Adapter.hpp"

/* Wrapper class for vision, localisation and behaviour threads */
class PerceptionThread : Adapter {
   public:
      /* Constructor */
      PerceptionThread(Blackboard *bb);
      /* Destructor */
      ~PerceptionThread();

      /* Read from global options after update */
      void readOptions(const boost::program_options::variables_map &config);

      /* One cycle of this thread */
      void tick();

   private:
      KinematicsAdapter kinematicsAdapter;
      VisionAdapter visionAdapter;
      //SonarAdapter sonarAdapter;
      LocalisationAdapter localisationAdapter;
      BehaviourAdapter behaviourAdapter;

      PerceptionDumper *dumper;
      Timer dump_timer;
      unsigned int dump_rate;
};

