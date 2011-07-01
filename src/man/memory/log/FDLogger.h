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
#include "include/io/FileFDProvider.h"

namespace man {
namespace memory {
namespace log {

class FDLogger {

public:
    typedef boost::shared_ptr<FDLogger> ptr;
    typedef boost::shared_ptr<FDLogger> const_ptr;

protected:
    typedef include::io::FDProvider FDProvider;

public:
    /**
     * fdp : a FileFDProvider for the file descriptor where we want
     * to log to
     */
    FDLogger(FDProvider::const_ptr fdp):
        file_descriptor_provider(fdp),
        file_descriptor(fdp->getFileDescriptor())
    {
        if (file_descriptor < 0) {
            std::cout << "Warning: invalid file descriptor passed for logging! "
                    << std::endl;
        }
    }
    /**
     * The write() method should write the message to some
     * sort of output buffer implemented in the respective
     * Logger subclass
     */
    virtual void writeToLog() = 0;

private:
    /**
     * This method should write to the head of the file; this is only done
     * once, versus the other logging which happens in a loop
     */
    virtual void writeHead() = 0;


protected:
    const FDProvider::const_ptr file_descriptor_provider;
    int file_descriptor;
};

}
}
}
