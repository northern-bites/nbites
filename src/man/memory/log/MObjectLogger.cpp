/**
 * MObjectLogger.cpp
 *
 *  The structure for a log file:
 *  -- ID number for the type of object logged
 *  -- the birth_time timestamp of the man process that parents the logger
 *
 *  -- for each message
 *  ---- size of serialized message
 *  ---- serialized message
 *
 *      Author: Octavian Neamtu
 *      E-mail: oneamtu@bowdoin.edu
 */

#include "MObjectLogger.h"

namespace man {
namespace memory {

extern long long birth_time;

namespace log {

using namespace std;

MObjectLogger::MObjectLogger(FDProvider::const_ptr fdp,
                             boost::shared_ptr<Synchro> synchro,
                             int logTypeID, MObject::const_ptr objectToLog) :
        ThreadedLogger(fdp, synchro, "Log" + MObject_names[logTypeID]),
        logID(logTypeID), objectToLog(objectToLog) {
    // this helps us ID the log
    writeHead();
}

void MObjectLogger::writeHead() {

    this->writeValue<int32_t>(logID);
    // this time stamps the log
    this->writeValue<int64_t>(birth_time);
}

void MObjectLogger::writeToLog() {

    this->writeValue<uint32_t>(objectToLog->byteSize());
    objectToLog->serializeToString(&write_buffer);
    this->writeCharBuffer(write_buffer.data(), write_buffer.length());
}

}
}
}
