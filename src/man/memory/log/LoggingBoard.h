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
 * @author Octavian Neamtu
 */

#pragma once

#include <map>
#include <string>

#include "include/io/FileFDProvider.h"

#include "CodedFileLogger.h"
#include "ImageFDLogger.h"
#include "memory/MObject.h"

//forward declaration
namespace man {
namespace memory {
namespace log {
class LoggingBoard;
}
}
}

#include "memory/Memory.h"
#include "memory/MemoryIOBoard.h"

namespace man {
namespace memory {
namespace log {

class LoggingBoard : MemoryIOBoard<FDLogger> {

public:
    LoggingBoard(const Memory* memory,
                IOProvider::const_ptr ioProvider = IOProvider::NullBulkIO());
    virtual ~LoggingBoard() {};

    void log(const MObject* mobject);

    void newIOProvider(IOProvider::const_ptr ioProvider);
//    const ImageFDLogger* getImageLogger(const MImage* mimage) const;
//    const FDLogger* getLogger(const MObject* mobject) const;

private:
    const Memory* memory;

};
}
}
}
