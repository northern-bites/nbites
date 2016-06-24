#pragma once

#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <queue>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <unistd.h>
#include "utils/Timer.hpp"


//#include <boost/thread.hpp>
//#include <boost/thread/condition_variable.hpp>
//#include <boost/thread/mutex.hpp>

class Speech
{
   public:

      static Speech& instance() {
         static Speech instance;
         return instance;
      }

      void add(std::string text) {
         pthread_mutex_lock(&mutex);
         sayText = text;
         pthread_mutex_unlock(&mutex);
      }

      std::string pop() {
         pthread_mutex_lock(&mutex);
         std::string returnText = sayText;
         sayText = "";
         pthread_mutex_unlock(&mutex);
         return returnText;
      }

   private:

      Speech() {
         int ret = pthread_mutex_init(&mutex, NULL);
         if (ret) std::cout << "failed to create say mutex" << std::endl;
      };
      
      // Declare copy constructors privately and don't implement them
      // This is to ensure singleton class
      Speech(Speech const& copy);
      Speech& operator=(Speech const& copy);

      std::string sayText;
      pthread_mutex_t mutex;
};

const inline void SAY(std::string text, bool blocking = false) {
   (Speech::instance()).add(text);
}

const inline std::string GET_SAYTEXT() {
    return (Speech::instance()).pop();
}
