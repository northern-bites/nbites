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

#include <aio.h>
#include <errno.h>
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
        memset(&control_block, 0, sizeof(control_block));
        control_block.aio_fildes = fdp->getFileDescriptor();
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

    // aynchronous writing
    virtual void writeCharBuffer(const char* buffer, uint32_t size) {
        //const_casting is bad(!!!) but aio_buf is not const
        control_block.aio_buf = const_cast<char *>(buffer);
        control_block.aio_nbytes = size;
        bytes_written += size;
        aio_write(&control_block);
        while (aio_error(&control_block) == EINPROGRESS) {
            pthread_yield();
        }
    }

private:
    aiocb control_block;
};

}
}
}
