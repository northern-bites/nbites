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
#include "io/OutProvider.h"

namespace man {
namespace memory {
namespace log {

class Logger {

public:
    typedef boost::shared_ptr<Logger> ptr;
    typedef boost::shared_ptr<Logger> const_ptr;

protected:
    typedef common::io::OutProvider OutProvider;

public:
    /**
     * fdp : a FileFDProvider for the file descriptor where we want
     * to log to
     */
    Logger(OutProvider::ptr out_provider):
        out_provider(out_provider) {
    }

    virtual ~Logger() {}
    /**
     * The writeToLog() method should write the message to some
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
    const OutProvider::ptr out_provider;
};

}
}
}
