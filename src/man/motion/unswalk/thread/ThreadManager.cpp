#include "ThreadManager.hpp"

class Blackboard;
class ThreadManager;

ConcurrentMap<pthread_t, jmp_buf*> jumpPoints;

using namespace std;

ThreadManager::ThreadManager(std::string name, int cycleTime) {
   this->name = name;
   this->cycleTime = cycleTime;
   running = false;
   // register thread name

}

void ThreadManager::join() {
   if (running) {
      pthread_join(pthread, NULL);
   }
   running = false;
}

ThreadManager::~ThreadManager() {
   join();
}


void overtimeAlert(int) {
   SAY(std::string(Thread::name) + " thread has frozen");
   signal(SIGALRM, overtimeAlert);
   struct itimerval itval5;
   itval5.it_value.tv_sec = 5;
   itval5.it_value.tv_usec = 0;
   itval5.it_interval.tv_sec = 0;
   itval5.it_interval.tv_usec = 0;
   setitimer(ITIMER_REAL, &itval5, NULL);
}

/**
 * The signal handler. Handles the signal and flag that the thread has died
 * and allow the watcher thread to restart it.
 * @param sigNumber The POSIX signal identifier
 * @param info Signal info struct for the signal
 * @see registerSignalHandler
 */
void handleSignals(int sigNumber, siginfo_t* info, void*) {
   // End the rUNSWift module [CTRL-C]. Call all destructors
   if (sigNumber == SIGINT) {
      cerr << endl;
      cerr << "###########################" << endl;
      cerr << "##    SIGINT RECEIVED    ##" << endl;
      cerr << "##  ATTEMPTING SHUTDOWN  ##" << endl;
      cerr << "###########################" << endl;
      attemptingShutdown = true;
   } else if (sigNumber == SIGTERM) {
      cerr << endl;
      cerr << "###########################" << endl;
      cerr << "##   SIGTERM RECEIVED    ##" << endl;
      cerr << "##  ATTEMPTING SHUTDOWN  ##" << endl;
      cerr << "###########################" << endl;
      attemptingShutdown = true;
   } else {
      // re-register the signal handler
      SAY("crash detected");
      registerSignalHandlers(sigNumber);
      pthread_t thread = pthread_self();

      cerr <<  string(Thread::name) << " with id " << thread <<
      " received signal " << sigNumber << " and is restarting" << endl;
      llog(ERROR) << string(Thread::name) << " with id "
                  << thread << " received signal "
                  << sigNumber << " and is restarting" << endl;

      longjmp(*jumpPoints[thread], 1);
   }
   return;
}


/**
 * @param signal default param is ALL_SIGNALS which is -1
 */
void registerSignalHandlers(int signal) {
   // setup the sigaction
   struct sigaction act;
   act.sa_sigaction = handleSignals;
   sigemptyset(&act.sa_mask);
   act.sa_flags = SA_SIGINFO | SA_RESETHAND;

   // register the signal handlers
   if (signal == SIGINT || signal == ALL_SIGNALS)
      sigaction(SIGINT, &act, NULL);   // CTRL-C termination
   if (signal == SIGTERM || signal == ALL_SIGNALS)
      sigaction(SIGTERM, &act, NULL);   // kill -15 termination
   if (signal == SIGSEGV || signal == ALL_SIGNALS)
      sigaction(SIGSEGV, &act, NULL);  // seg fault
   if (signal == SIGFPE || signal == ALL_SIGNALS)
      sigaction(SIGFPE, &act, NULL);   // floating point exception
   if (signal == SIGSTKFLT || signal == ALL_SIGNALS)
      sigaction(SIGSTKFLT, &act, NULL);   // stack faults
   if (signal == SIGHUP || signal == ALL_SIGNALS)
      sigaction(SIGHUP, &act, NULL);   // lost controlling terminal
}

