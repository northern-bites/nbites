/**
 * MessageLogger.cpp
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

#include "MessageLogger.h"

namespace man {
namespace memory {

extern long long birth_time;

namespace log {

using namespace std;

MessageLogger::MessageLogger(FDProvider::const_ptr fdp,
		int logTypeID, MObject::const_ptr objectToLog) :
        FDLogger(fdp), logID(logTypeID), objectToLog(objectToLog) {
    // this helps us ID the log
    writeHead();
}

void MessageLogger::writeHead() {

    this->writeValue<int32_t>(logID);
    // this time stamps the log
    this->writeValue<int64_t>(birth_time);
}

void MessageLogger::writeToLog() {

    this->writeValue<uint32_t>(objectToLog->byteSize());
    objectToLog->serializeToString(&write_buffer);
    this->writeCharBuffer(write_buffer.data(), write_buffer.length());
}

}
}
}
