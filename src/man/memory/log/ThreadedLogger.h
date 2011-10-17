
#include "Logger.h"
#include "synchro/synchro.h"

namespace man {
namespace memory {
namespace log {

class ThreadedLogger : public Logger, public Thread{

public:
    typedef boost::shared_ptr<ThreadedLogger> ptr;
    typedef boost::shared_ptr<ThreadedLogger> const_ptr;

public:
    ThreadedLogger(FDProvider::const_ptr fdp, std::string name) :
                   Logger(fdp), Thread(name) {
    }

    virtual ~ThreadedLogger(){}

    virtual void writeToLog() = 0;

    virtual void run() {
        while (running) {
            this->waitForSignal();
            this->writeToLog();
        }
    }

    void signalToLog() {
        this->signalToResume();
    }

};

}
}
}
