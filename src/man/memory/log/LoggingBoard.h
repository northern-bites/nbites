/**
 *
 * @LoggingBoard - a class that will handle all of the logging objects for each
 * of the memory object
 * e.g. MVision can have a ZeroCopyFileLogger, whereas
 * MSensors can have a CodedFileLogger
 * as loggers
 *
 * Such pairs are stored in a map
 */


#pragma once

#include <map.h>
#include "CodedFileLogger.hpp"
#include "memory/Memory.hpp"
#include "memory/MObject.hpp"

namespace memory {

namespace log {

typedef pair< const MObject*,  Logger*> objectLoggerPair;

class LoggingBoard {

public:
    LoggingBoard(const Memory* _memory);
    ~LoggingBoard();

    void log(const MObject* mobject);

private:
    map<const MObject*, Logger*> objectLoggerMap;

};

}

}
