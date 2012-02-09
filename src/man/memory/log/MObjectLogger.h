/**
 * MObjectLogger.hpp
 *
 * @class MObjectLogger - provides the methods to write serialized MObject data
 * to an OutProvider
 *
 * @author Octavian Neamtu
 *
 */

#pragma once

#include <string>
#include "memory/MObject.h"
#include "ThreadedLogger.h"

namespace man {
namespace memory {
namespace log {

class MObjectLogger : public ThreadedLogger{

public:
    typedef boost::shared_ptr<MObjectLogger> ptr;
    typedef boost::shared_ptr<const MObjectLogger> const_ptr;

public:
    MObjectLogger(OutProvider::ptr out_provider,
                  MObject::const_ptr objectToLog);

    virtual ~MObjectLogger() {}

    void writeToLog();
    void writeHead();

private:
    MObject::const_ptr objectToLog;
    std::string write_buffer;

};
}
}
}
