#pragma once

#include <vector>
#include <list>
#include "utils/body.hpp"


#define WINDOW_SIZE 4 // keep this a power of 2
#define CYCLES_PER_PING 10

// Time lag will be 10 cycles * 10ms * 3 directions * window size = 1200ms
// Need to allow at least 20ms between pings since sound only travels 3.3m in 10ms


/* Simple histogram based filter for noisy sonar observations */
class SonarRecorder {
   public:

      SonarRecorder();

      /* Update the rolling window of recent observations and return the next Sonar::Mode request*/
      float update(float sonar[Sonar::NUMBER_OF_READINGS]);

      // A rolling list of recent observations of range readings to potentially multiple obstacles
      std::vector< std::vector <int> > sonarWindow;


   private:

      bool first_call;
      int command_counter;
      int cycle_counter;
      std::vector<float> commands;

      std::list<int> leftDrop;
      std::list<int> middleDrop;
      std::list<int> rightDrop;

      float processUpdate(float sonar[Sonar::NUMBER_OF_READINGS]);

};
