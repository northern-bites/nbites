/**
 *
 * @LoggingBoard - a class that will handle all of the logging objects for each
 * of the memory object
 * e.g. MVision can have a ZeroCopyFileLogger, whereas
 * MSensors can have a CodedFileLogger
 *
 * Also each logger depends on a FDProvider (that is a file descriptor
 * provider) to provide the file descriptor necessary. Some stuff
 * might go to a file, some might go to wifi, etc.
 *
 */


#pragma once

#include <map.h>
#include "CodedFileLogger.hpp"
#include "memory/Memory.hpp"
#include "memory/MObject.hpp
#include "include/io/FDProvider.h"

#include "NaoPaths.h"
#include "FDLogger.h"

namespace memory {

namespace log {

typedef pair< const MObject*, FDLogger*> objectFDLoggerPair;
typedef pair< const MObject*, FDProvider*> objectFDProviderPair;

class LoggingBoard {

public:
    LoggingBoard(const Memory* _memory);
    ~LoggingBoard();

    void log(const MObject* mobject);

private:
    const Memory* memory;
    map<const MObject*, FDLogger*> objectFDLoggerMap;
    map<const MObject*, FDProvider*> objectFDProviderMap;

};

}

}
