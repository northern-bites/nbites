/*
 * @class ThreadedLogger
 *
 * Puts the logger in a thread of its own and puts in some
 * aio functionality so that the writes don't block anymore
 * (a blocking write will block the ENTIRE process and not only
 * one thread)
 *
 * @author Octavian Neamtu
 *
 */


#include "Logger.h"
#include "synchro/synchro.h"
#include "Subscriber.h"

namespace man {
namespace memory {
namespace log {

class ThreadedLogger : public Logger, public Thread, public Subscriber {

public:
    typedef boost::shared_ptr<ThreadedLogger> ptr;
    typedef boost::shared_ptr<ThreadedLogger> const_ptr;

public:
    ThreadedLogger(OutProvider::ptr out_provider, std::string name) :
                   Logger(out_provider), Thread(name) {
    }

    virtual ~ThreadedLogger(){}

    virtual void writeToLog() = 0;

    virtual void run() {
        //blocking for socket fds, (almost) instant for other ones
        out_provider->openCommunicationChannel();
        while (running) {
            this->waitForSignal();
            this->writeToLog();
            while(out_provider->writingInProgress()) {
                this->yield();
            }
        }
    }

    void signalToLog() {
        this->signalToResume();
    }

    void update() {
    	this->signalToLog();
    }

};

}
}
}
