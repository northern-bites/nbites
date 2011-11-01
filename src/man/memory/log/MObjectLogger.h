/**
 * MObjectLogger.hpp
 *
 * @class MObjectLogger
 *
 *
 * some of the code is inspired from the example provided in the link
 *
 *      Author: Octavian Neamtu
 *      E-mail: oneamtu@bowdoin.edu
 */

#pragma once

#include <string>
#include "memory/MObject.h"
#include "ThreadedLogger.h"

namespace man {
namespace memory {
namespace log {

class MObjectLogger : public ThreadedLogger {

public:
    typedef boost::shared_ptr<MObjectLogger> ptr;
    typedef boost::shared_ptr<const MObjectLogger> const_ptr;

public:
    /**
     * Opens the file fileName and writes to its head
     *
     * @param fdp: this class provides a file descriptor
     * @param logTypeID : an ID written to the head identifying the log
     * @param objectToLog : the object we need to log
     * @return
     */
    MObjectLogger(FDProvider::const_ptr fdp,
                  int logTypeID, MObject::const_ptr objectToLog);

    virtual ~MObjectLogger() {}
    void writeToLog();
    void writeHead();

private:
    int logID;
    MObject::const_ptr objectToLog;
    std::string write_buffer;

};
}
}
}
