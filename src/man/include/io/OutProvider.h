/**
 * @class OutProvider
 *
 * An abstract class that has the only role of providing means of writing
 * asynchronously to a file descriptor
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

namespace common {
namespace io {

class OutProvider : public IOProvider {

ADD_SHARED_PTR(OutProvider);

public:
    OutProvider() : bytes_written(0) {
        //zeroes the control_block
        memset(&control_block, 0, sizeof(control_block));
    }
    virtual ~OutProvider() {};

    virtual std::string debugInfo() const = 0;
    virtual void openCommunicationChannel() = 0;
    virtual bool opened() const = 0;

    virtual bool writingInProgress() const {
        return aio_error(&control_block) == EINPROGRESS;
    }

    //busy blocks before the other write is done
    virtual void writeCharBuffer(const char* buffer, uint32_t size) {
        if (!opened()) {
            std::cout<<"Cannot write to a not yet open channel "
                    <<debugInfo()<<std::endl;
            return;
        }

        while(writingInProgress()) {
            //busy block
        }

        //const_casting is bad(!!!) but aio_write demands a non-const buffer
        control_block.aio_fildes = file_descriptor;
        control_block.aio_buf = const_cast<char *>(buffer);
        control_block.aio_nbytes = size;
        bytes_written += size;
        int result = aio_write(&control_block);

        if (result != 0) {
            std::cout<<"AIO write failed on "
                     <<debugInfo()<<std::endl
                     <<" with error " << strerror(errno) << std::endl;
        }
    }

    template <class T>
    void writeValue(const T &value) {
        writeCharBuffer(reinterpret_cast<const char *>(&value), sizeof(value));
    }

private:
    aiocb control_block;
    unsigned long long bytes_written;
};

}
}
