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

#include "io/OutProvider.h"
#include "synchro/synchro.h"
#include "Subscriber.h"
#include "ClassHelper.h"

namespace man {
namespace memory {
namespace log {

class ThreadedLogger: public Thread, public Subscriber {

ADD_SHARED_PTR(ThreadedLogger)

protected:
    typedef common::io::OutProvider OutProvider;

public:
    ThreadedLogger(OutProvider::ptr out_provider, std::string name) :
            Thread(name), out_provider(out_provider) {

    }

    virtual ~ThreadedLogger() {
    }

    virtual void writeToLog() = 0;

    virtual void run() = 0;

    void signalToLog() {
        this->signalToResume();
    }

    void update() {
        this->signalToLog();
    }

protected:
    const OutProvider::ptr out_provider;

};

}
}
}
