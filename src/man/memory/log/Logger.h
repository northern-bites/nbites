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
    typedef common::io::IOProvider FDProvider;

public:
    /**
     * fdp : a FileFDProvider for the file descriptor where we want
     * to log to
     */
    Logger(FDProvider::ptr fdp):
        fd_provider(fdp),
        bytes_written(0) {
    }

    virtual ~Logger() {}
    /**
     * The writeToLog() method should write the message to some
     * sort of output buffer implemented in the respective
     * Logger subclass
     */
    virtual void writeToLog() = 0;

protected:

    // helper write methods
    //TODO: assert if we actually write everything
    // and detect if an error happens

    virtual void writeCharBuffer(const char* buffer, uint32_t size) {
        bytes_written += write(fd_provider->getFileDescriptor(), buffer, size);
    }

    template <class T>
    void writeValue(T value) {
        writeCharBuffer(reinterpret_cast<char *>(&value), sizeof(value));
    }

private:
    /**
     * This method should write to the head of the file; this is only done
     * once, versus the other logging which happens in a loop
     */
    virtual void writeHead() = 0;

//TODO: do NOT use file_descriptor
protected:
    const FDProvider::ptr fd_provider;
    unsigned long long bytes_written;
};

}
}
}
