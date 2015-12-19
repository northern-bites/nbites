#pragma once

#include <semaphore.h>
#include <bitset>
#include <string>
#include <vector>
#include "alcommon/albroker.h"
#include "alcommon/alproxy.h"
#include "alproxies/alloggerproxy.h"
#include "alproxies/almemoryproxy.h"
#include "alproxies/almotionproxy.h"
#include "alproxies/altexttospeechproxy.h"
#include "alcommon/almodule.h"
#include "alcore/alptr.h"
#include "altools/tools.h"
#include "alvalue/alvalue.h"
#include "altools/alxplatform.h"
#include "alproxies/dcmproxy.h"
#include "libagent/AgentData.hpp"
#include "utils/angles.hpp"
#include "utils/body.hpp"
#include "utils/options.hpp"
#include "utils/Timer.hpp"

namespace AL {
   class ALBroker;
}

#define MAX_SKIPS 50
#define MAX_CLICK_INTERVAL 18

const float sit_angles[Joints::NUMBER_OF_JOINTS] = {
   0.0,              // HeadYaw
   0.0,              // HeadPitch

   DEG2RAD(10),      // LShoulderPitch
   0.0,              // LShoulderRoll
   0.0,              // LElbowYaw
   DEG2RAD(7.5),     // LElbowRoll
   0.0,              // LWristYaw
   0.0,              // LHand

   DEG2RAD(-50),     // LHipYawPitch
   DEG2RAD(-20),     // LHipRoll
   DEG2RAD(-75),     // LHipPitch
   DEG2RAD(125),     // LKneePitch
   DEG2RAD(-70),     // LAnklePitch
   0.0,              // LAnkleRoll

   DEG2RAD(20),      // RHipRoll
   DEG2RAD(-75),     // RHipPitch
   DEG2RAD(125),     // RKneePitch
   DEG2RAD(-70),     // RAnklePitch
   0.0,              // RAnkleRoll

   DEG2RAD(10),      // RShoulderPitch
   0.0,              // RShoulderRoll
   0.0,              // RElbowYaw
   DEG2RAD(-7.5),    // RElbowRoll
   0.0,              // RWristYaw
   0.0               // RHand
};

/**
 * Acts an an agent between the Naoqi process and the rUNSWift soccer player
 * process, using shared memory and a named semaphore, to control the DCM and
 * read robot sensors. Provides safety measures if it loses contact with the
 * player. Inspired by libbhuman.
 */
#define AGENT_VERSION_MAJOR "0"
#define AGENT_VERSION_MINOR "0"
class Agent : public AL::ALModule {
   public:
      Agent(AL::ALPtr<AL::ALBroker> pBroker, const std::string& pName);
      virtual ~Agent();
      static const std::string name;
      void preCallback();
      void postCallback();
      void doLEDs(ActionCommand::LED& leds);
      void doButtons(bool chest, bool left, bool right);
      void doBattery(float charge, float current, int status);
      void doTemps();
      void doNetworking();
      void doAvahi();

   private:
      AL::ALLoggerProxy* log;

      /* DCM */
      AL::DCMProxy* dcm;
      AL::ALValue angle_command;
      AL::ALValue stiffness_command;
      AL::ALValue head_angle_command;
      AL::ALValue head_stiffness_command;
      AL::ALValue led_command;
      AL::ALValue sonar_command;
      int time_offset;

      /* ALMemory */
      AL::ALMemoryProxy* memory;
      std::vector<float*> sensor_pointers;
      std::vector<float*> joint_pointers;
      std::vector<float*> temperature_pointers;
      std::vector<float*> sonar_pointers;
      int* battery_status_pointer;

      std::vector<std::string> leg_names;
      std::vector<float> stand_angles;

      /* ALMotion */
      AL::ALMotionProxy* motion;

      /* ALTextToSpeech */
      AL::ALTextToSpeechProxy* speech;
      Timer sayTimer;

      /* Shared Memory */
      bool shuttingDown;
      int shared_fd;
      AgentData* shared_data;
      sem_t* semaphore;

      /* Safety */
      int skipped_frames;
      float sit_step;
      JointValues sit_joints;
      bool limp;
      bool head_limp;

      /* Button presses */
      // counters of how many 20 ms cycles button has been up or down for
      int chest_down;
      int chest_up;
      ButtonPresses buttons;
      // running counter of how many presses have been recorded so far
      unsigned int chest_presses;

      /* Battery */
      int old_battery;
      std::bitset<16> old_battery_status;

      /* Options */
      boost::program_options::variables_map vm;
      std::string wirelessIwconfigArgs;
      bool wirelessStatic;
      std::string wirelessIfconfigArgs;
      bool wiredStatic;
      std::string wiredIfconfigArgs;

      int teamNum;
      int playerNum;
};
