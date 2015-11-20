#include "motion/touch/AgentTouch.hpp"
#include <sys/mman.h>        /* For shared memory */
#include <fcntl.h>           /* For O_* constants */
#include <cstdlib>
#include <stdexcept>
#include "utils/Logger.hpp"

AgentTouch::AgentTouch() {
   // open sempahore
   semaphore = sem_open(AGENT_SEMAPHORE, O_RDWR);
   if (semaphore < 0)
      throw std::runtime_error("AgentTouch: sem_open() failed");

   // open shared memory as RW
   shared_fd = shm_open(AGENT_MEMORY, O_RDWR, 0600);
   if (shared_fd < 0)
      throw std::runtime_error("AgentTouch: shm_open() failed");

   // map shared memory to process memory
   shared_data = (AgentData*) mmap(NULL, sizeof(AgentData),
                                   PROT_READ | PROT_WRITE,
                                   MAP_SHARED, shared_fd, 0);
   if (shared_data == MAP_FAILED)
      throw std::runtime_error("AgentTouch: mmap() failed");

   // prevents error below, doesn't check if runsiwft is already running
   pthread_mutex_unlock(&shared_data->lock); // TODO remove

   if (pthread_mutex_trylock(&shared_data->lock)) {
      llog(QUIET) << "Another runswift is already running!" << std::endl;
      std::cerr << "Another runswift is already running!" << std::endl;
      exit(1);
   }

   llog(INFO) << "AgentTouch constructed" << std::endl;
}

AgentTouch::~AgentTouch() {
   pthread_mutex_unlock(&shared_data->lock);
   if (shared_data != MAP_FAILED) munmap(shared_data, sizeof(AgentData));
   if (shared_fd >= 0) close(shared_fd);
   if (semaphore != SEM_FAILED) sem_close(semaphore);
   llog(INFO) << "AgentTouch destroyed" << std::endl;
}

SensorValues AgentTouch::getSensors(Kinematics &kinematics) {
   sem_wait(semaphore);
   while (!sem_trywait(semaphore)) ;
   shared_data->sensors_read = shared_data->sensors_latest;
   return shared_data->sensors[shared_data->sensors_read];
}

bool AgentTouch::getStanding() {
   return shared_data->standing;
}

ButtonPresses AgentTouch::getButtons() {
   return shared_data->buttons[shared_data->sensors_read];
}
