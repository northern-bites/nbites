template<class T, void(T::*mem_fn) (Blackboard *)> void* thunk(void* args) {
   void *p = ((SafelyRunArgs*)args)->threadManager;
   Blackboard *b = ((SafelyRunArgs*)args)->blackboard;
   (static_cast<T*>(p)->*mem_fn)(b);
   return 0;
}

template <class T> void ThreadManager::run(Blackboard *bb) {
   // initialise logger(name)
   bb->thread.configCallbacks[name];
   args.blackboard = bb;
   args.threadManager = this;
   llog(INFO) << "Running ThreadManager for " << name << std::endl;
   if (name == "Motion") {
      // set up real-time priorities
      struct sched_param param;
      int policy;
      pthread_attr_t attr;
      pthread_attr_init(&attr);
      pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
      pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
      param.sched_priority = 65;
      pthread_attr_setschedparam(&attr, &param);
      pthread_create(&pthread, &attr, &thunk<ThreadManager, &ThreadManager::safelyRun<T> >, &args);
      pthread_attr_destroy(&attr);
      pthread_getschedparam(pthread, &policy, &param);
      llog(INFO) << "Motion granted priority: " << param.sched_priority << std::endl;
   } else {
      pthread_create(&pthread, NULL, &thunk<ThreadManager, &ThreadManager::safelyRun<T> >, &args);
   }
   running = true;

}

template <class T>
void ThreadManager::safelyRun(Blackboard *bb) {
   Thread::name = name.c_str();
   pthread_t threadID = pthread_self();
   llog(INFO) << "Registering thread name '" << name
              << "' with ID " << threadID << std::endl;

   if (name == "Perception") {
      signal(SIGALRM, overtimeAlert);
   }
   while (!attemptingShutdown) {
      try {
         // we don't care if this leaks
         // but it shouldn't reach this line again in this thread
         jumpPoints[threadID] =
            reinterpret_cast<jmp_buf*>(malloc(sizeof(jmp_buf)));
         if (!jumpPoints[threadID])
            llog(FATAL) << "malloc failed for" << name << "\n";

         llog(INFO) << "Thread '" << name << "' started\n";

         // register jump point for where to resume if we crash
         if (!setjmp(*jumpPoints[threadID])) {
            T t(bb);
            Timer timer;
            int32_t elapsed = 0.0;
            while (!attemptingShutdown) {
               timer.restart();

               if (cycleTime != -1) {
                  // set watchdog timer to alert us about stuck threads
                  if (name == "Perception") {
                     struct itimerval itval5;
                     itval5.it_value.tv_sec = 5;
                     itval5.it_value.tv_usec = 0;
                     itval5.it_interval.tv_sec = 0;
                     itval5.it_interval.tv_usec = 0;
                     setitimer(ITIMER_REAL, &itval5, NULL);
                  }

                  // Execute one cycle of the module
                  t.tick();

                  // unset watchdog timer to alert us about stuck threads
                  if (name == "Perception") {
                     struct itimerval itval0;
                     itval0.it_value.tv_sec = 0;
                     itval0.it_value.tv_usec = 0;
                     itval0.it_interval.tv_sec = 0;
                     itval0.it_interval.tv_usec = 0;
                     setitimer(ITIMER_REAL, &itval0, NULL);
                  }

                  elapsed = timer.elapsed_us();
                  llog(INFO) << "Thread '" << name << "' took "
                             << elapsed << " us." << std::endl;
                  if (elapsed < cycleTime) {
                     usleep(cycleTime - elapsed);
                  } else if (((name != "Motion") && (name != "Perception")) || ((name == "Perception") && (elapsed >= 50000))){
                     llog(ERROR) << "WARNING: Thread " + name +
                     " ran overtime" << ": " << elapsed / 1000  << "ms!" <<
                     std::endl;
                     if (elapsed >= 1000000 && name == "perception")
                        SAY("perception overtime");
                  }
               } else {
                  sleep(10);  // thread does not need a 'tick'
               }
            }
         }
         llog(INFO) << "Thread '" << name
                    << "' disabled." << std::endl;
      } catch(const std::exception & e) {
         SAY("exception caught");
         free(jumpPoints[threadID]);
         usleep(500000);
         llog(ERROR) << "exception derivative was caught with error msg: "
                     << e.what() << std::endl;
         llog(ERROR) << "in " << name
                     << " with id " << threadID << std::endl;
      } catch(...) {
         SAY("exception caught");
         free(jumpPoints[threadID]);
         usleep(500000);
         llog(ERROR) << "Something was thrown from "
                     << name
                     << " with id " << threadID << std::endl;
      }
   }
}
