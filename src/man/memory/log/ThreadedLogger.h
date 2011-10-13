
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
                   Logger(fdp), Thread(synchro, name) {
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
