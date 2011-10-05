/**
 * Logger.hpp
 *
 * @class Logger
 *
 * An abstract class that provides the basic interface for
 * a proto message logger
 *
 *      Author: Octavian Neamtu
 *      E-mail: oneamtu@bowdoin.edu
 */

#pragma once

#include <iostream>
#include <boost/shared_ptr.hpp>
#include <stdint.h>
#include "include/io/FileFDProvider.h"

namespace man {
namespace memory {
namespace log {

class Logger {

public:
    typedef boost::shared_ptr<Logger> ptr;
    typedef boost::shared_ptr<Logger> const_ptr;

protected:
    typedef include::io::FDProvider FDProvider;

public:
    /**
     * fdp : a FileFDProvider for the file descriptor where we want
     * to log to
     */
    Logger(FDProvider::const_ptr fdp):
        file_descriptor_provider(fdp),
        file_descriptor(fdp->getFileDescriptor()), bytes_written(0) {
        if (file_descriptor < 0) {
            std::cout << "Warning: invalid file descriptor passed for logging!"
                    << std::endl;
        }
    }
    /**
     * The writeToLog() method should write the message to some
     * sort of output buffer implemented in the respective
     * Logger subclass
     */
    virtual void writeToLog() = 0;

protected:
    /*
     * Writes a value of type T to the current buffer.
     * It writes it at the specified offset and then
     * increments the offset with the size of the value written.
     */
    //TODO: assert if we actually write everything
    // and detect if an error happens
    template <class T>
    void writeValue(T value) {
        bytes_written += write(file_descriptor, &value, sizeof(value));
    }

    void writeCharBuffer(const char* buffer, uint32_t size) {
        bytes_written += write(file_descriptor, buffer, size);
    }

private:
    /**
     * This method should write to the head of the file; this is only done
     * once, versus the other logging which happens in a loop
     */
    virtual void writeHead() = 0;

//TODO: do NOT use file_descriptor
protected:
    const FDProvider::const_ptr file_descriptor_provider;
    int file_descriptor;
    unsigned long long bytes_written;
};

}
}
}
