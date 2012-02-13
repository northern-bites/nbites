/*
 * @class ThreadedParser
 *
 * A Threaded Parser; Combined with the aio functionality of the InProvider,
 * allows us to not busy-wait on IO
 *
 * @author Octavian Neamtu
 *
 */

#include "synchro/synchro.h"
#include "Subscriber.h"
#include "io/InProvider.h"

namespace man {
namespace memory {
namespace parse {

class ThreadedParser: public Thread {

public:
    typedef boost::shared_ptr<ThreadedParser> ptr;
    typedef boost::shared_ptr<ThreadedParser> const_ptr;

protected:
    typedef common::io::InProvider InProvider;

public:
    ThreadedParser(InProvider::ptr in_provider, std::string name) :
            Thread(name), in_provider(in_provider) {
    }

    virtual ~ThreadedParser() {
    }

    virtual bool readNextMessage() = 0;
    virtual void readHeader() = 0;

    virtual void run() = 0;

    virtual void signalToParseNext() = 0;

protected:
    InProvider::ptr in_provider;

};

}
}
}
