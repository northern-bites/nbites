/**
 * @class OutProvider
 *
 * An abstract class that has the only role of providing means of writing
 * asynchronously to a file descriptor
 *
 * Check out
 * http://www.ibm.com/developerworks/linux/library/l-async/
 *
 * @author Octavian Neamtu
 */


#pragma once

#include <boost/shared_ptr.hpp>
#include <string>
#include <iostream>
#include <unistd.h>
#include <aio.h>
#include <errno.h>
#include <string.h> //memset()
#include "ClassHelper.h"
#include "IOProvider.h"
#include "IOExceptions.h"

namespace common {
namespace io {

class OutProvider : public IOProvider {

ADD_SHARED_PTR(OutProvider);

public:
    OutProvider() : bytes_written(0), file_descriptor(-1) {
        //zeroes the control_block
        memset(&control_block, 0, sizeof(control_block));

    }
    virtual ~OutProvider() {};

    virtual std::string debugInfo() const = 0;
    virtual void openCommunicationChannel() throw (io_exception)= 0;
    virtual bool opened() const = 0;
    virtual void closeCommunicationChannel() = 0;

    virtual bool writingInProgress() const {
        return aio_error(&control_block) == EINPROGRESS;
    }

    //yields before the other write is done!
    virtual void writeCharBuffer(const char* buffer, uint32_t size) {
        if (!opened()) {
            std::cout<<"Cannot write to a not yet open channel "
                    <<debugInfo()<<std::endl;
            return;
        }

        //const_casting is bad(!!!) but aio_write demands a non-const buffer
        control_block.aio_fildes = file_descriptor;
        control_block.aio_buf = const_cast<char *>(buffer);
        control_block.aio_nbytes = size;
        control_block.aio_sigevent.sigev_notify = SIGEV_NONE;
        int result = aio_write(&control_block);

        if (result == -1) {
            std::cout<<"AIO write enque failed on "
                     <<debugInfo()<<std::endl
                     <<" with error " << strerror(errno) << std::endl;
        }

        while(writingInProgress()) {
            pthread_yield();
        }

        int bytes_written = aio_return(&control_block);

        if (bytes_written == -1) {
            std::cout<< "AIO write failed on "
                     << debugInfo()<<std::endl
                     << " with error " << strerror(aio_error(&control_block)) << std::endl;
            this->closeCommunicationChannel();
        } else {
            if ((uint32_t) (bytes_written) < size) {
                writeCharBuffer(buffer + bytes_written, size - bytes_written);
            }
        }
    }


    template <class T>
    void writeValue(const T &value) {
        writeCharBuffer(reinterpret_cast<const char *>(&value), sizeof(value));
    }

protected:
    struct aiocb control_block;
    unsigned long long bytes_written;
    int file_descriptor;
};

}
}
