/*
 * @class ThreadedParser
 *
 * Puts the parser in a thread of its own and puts in some
 * aio functionality so that the writes don't block anymore
 * (a blocking read will block the ENTIRE process and not only
 * one thread)
 *
 * @author Octavian Neamtu
 *
 */

#include "Parser.h"
#include "synchro/synchro.h"
#include "Subscriber.h"

namespace man {
namespace memory {
namespace parse {

class ThreadedParser : public Parser, public Thread {

public:
    typedef boost::shared_ptr<ThreadedParser> ptr;
    typedef boost::shared_ptr<ThreadedParser> const_ptr;

public:
    ThreadedParser(InProvider::ptr in_provider, std::string name) :
                   Parser(in_provider), Thread(name) {
    }

    virtual ~ThreadedParser(){}

    virtual bool getNext() = 0;
    virtual bool getPrev() = 0;

    virtual void readNextMessage() = 0;

    virtual void run() {
        //blocking for socket fds, (almost) instant for other ones
//        inProvider->openCommunicationChannel();
        std::cout << "reading head in" << std::endl;
        this->readHeader();
        while (running) {
            //in streaming we get messages continuously,
            //so there's no need to wait
            if (!inProvider->isOfTypeStreaming()) {
                this->waitForSignal();
            }
            this->getNext();

        }
    }

    void signalToParse() {
        this->signalToResume();
    }

};

}
}
}
