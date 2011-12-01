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
#include "Profiler.h"

namespace man {
namespace memory {

extern long long birth_time;

namespace log {

using namespace std;

MObjectLogger::MObjectLogger(OutProvider::ptr out_provider,
                             int logTypeID, MObject::const_ptr objectToLog) :
        ThreadedLogger(out_provider, "Log" + MObject_names[logTypeID]),
        logID(logTypeID), objectToLog(objectToLog) {
}

void MObjectLogger::writeHead() {
    // log ID
    out_provider->writeValue<int32_t>(logID);
    // the absolute time stamp of the log
    //(all other time stamps are relative to this)
    out_provider->writeValue<int64_t>(birth_time);
}

void MObjectLogger::writeToLog() {
    PROF_ENTER(P_LOGGING);
    out_provider->writeValue<uint32_t>(objectToLog->byteSize());
    objectToLog->serializeToString(&write_buffer);
    out_provider->writeCharBuffer(write_buffer.data(), write_buffer.length());
    PROF_EXIT(P_LOGGING);
}

}
}
}
