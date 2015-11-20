#include "motion/effector/AgentEffector.hpp"
#include <sys/mman.h>        /* For shared memory */
#include <fcntl.h>           /* For O_* constants */
#include <stdexcept>
#include "utils/Logger.hpp"
#include "utils/speech.hpp"

/*-----------------------------------------------------------------------------
 * Agent effector constructor
 *---------------------------------------------------------------------------*/
AgentEffector::AgentEffector() {
   // open shared memory as RW
   shared_fd = shm_open(AGENT_MEMORY, O_RDWR, 0600);
   if (shared_fd < 0) {
      throw std::runtime_error("AgentEffector: shm_open() failed");
   }
   // map shared memory to process memory
   shared_data = (AgentData*) mmap(NULL, sizeof(AgentData),
                                   PROT_READ | PROT_WRITE,
                                   MAP_SHARED, shared_fd, 0);
   if (shared_data == MAP_FAILED) {
      throw std::runtime_error("AgentEffector: mmap() failed");
   }

   llog(INFO) << "AgentEffector constructed" << std::endl;
}

/*-----------------------------------------------------------------------------
 * Agent effector destructor
 *---------------------------------------------------------------------------*/
AgentEffector::~AgentEffector() {
   if (shared_data != MAP_FAILED) {
      munmap(shared_data, sizeof(AgentData));
   }
   if (shared_fd >= 0) {
      close(shared_fd);
   }
   llog(INFO) << "AgentEffector destroyed" << std::endl;
}

/*-----------------------------------------------------------------------------
 * Agent effector - actuate the joints to the desired position
 *---------------------------------------------------------------------------*/
void AgentEffector::actuate(JointValues joints, ActionCommand::LED leds,
                            float sonar) {
   static bool kill_standing = false;
   uint8_t i;
   // Find the right index to write to
   // Roger: this should have been actually swapping between slots right?
   for (i = 0; i != shared_data->actuators_latest &&
        i != shared_data->actuators_read; ++i) ;
   shared_data->leds[i] = leds;
   shared_data->joints[i] = joints;
   shared_data->sonar[i] = sonar;
   std::string sayText = GET_SAYTEXT();
   int size = sizeof(shared_data->sayTexts[i]);
   strncpy(shared_data->sayTexts[i], sayText.c_str(), size);
   shared_data->sayTexts[i][size - 1] = 0;
   shared_data->actuators_latest = i;

   // effector needs to set standing to false if we got standing
   // we need to wait one cycle in case standing was set after AgentTouch is run
   shared_data->standing = kill_standing;
   if (kill_standing) {
      kill_standing = false;
      shared_data->standing = false;
   } else {
      kill_standing = true;
   }
}
