
#include "Logger.h"
#include "synchro.h"

namespace man {
namespace memory {
namespace log {

class ThreadedLogger : public Logger, public Thread{

public:
    typedef boost::shared_ptr<ThreadedLogger> ptr;
    typedef boost::shared_ptr<ThreadedLogger> const_ptr;

public:
    ThreadedLogger(FDProvider::const_ptr fdp,
                   boost::shared_ptr<Synchro> synchro, std::string name) :
                   Logger(fdp), Thread(synchro, name),
                   canLog(false), doneLogging(true){

    }

    virtual ~ThreadedLogger(){}

    virtual void writeToLog() = 0;

    virtual void run() {
        while (true) {
            pthread_mutex_lock(&needs_logging_mutex);
            if (!canLog) {
                pthread_cond_wait(&needs_logging_cond, &needs_logging_mutex);
            }
            canLog = false;
            pthread_mutex_unlock(&needs_logging_mutex);
            this->writeToLog();
        }
    }

    void signalToLog() {
        pthread_mutex_lock(&needs_logging_mutex);
        canLog = true;
        pthread_cond_signal(&needs_logging_cond);
        pthread_mutex_unlock(&needs_logging_mutex);
    }

private:
    bool canLog;
    pthread_cond_t needs_logging_cond;
    mutable pthread_mutex_t needs_logging_mutex;
    pthread_cond_t logging_done_cond;
    mutable pthread_mutex_t logging_done_mutex;


};

}
}
}
