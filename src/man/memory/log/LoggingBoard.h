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

#include <map>

#include "include/io/FileFDProvider.h"

#include "CodedFileLogger.h"
#include "ImageFDLogger.h"

#include "NaoPaths.h"
#include "memory/MObject.h"

//forward declaration
namespace memory {
namespace log {
class LoggingBoard;
}
}

#include "memory/Memory.h"

namespace memory {

namespace log {

typedef pair< const MObject*, FDLogger*> ObjectFDLoggerPair;
typedef pair< const MObject*, FDProvider*> ObjectFDProviderPair;

typedef map< const MObject*, FDLogger*> ObjectFDLoggerMap;
typedef map< const MObject*, FDProvider*> ObjectFDProviderMap;

class LoggingBoard {

public:
    LoggingBoard(const Memory* _memory);
    //~LoggingBoard();

    void log(const MObject* mobject);

private:
    void initLoggingObjects();

private:
    const Memory* memory;
    ObjectFDLoggerMap objectFDLoggerMap;
    ObjectFDProviderMap objectFDProviderMap;


};
}
}
