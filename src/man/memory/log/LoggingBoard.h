/**
 *
 * @LoggingBoard - a class that will handle all of the logging objects for each
 * of the memory object
 * Each item could potentially have a different logger (a threaded one, a gzip one, etc.)
 *
 * Also each logger depends on a FDProvider (that is a file descriptor
 * provider) to provide the file descriptor necessary. Some stuff
 * might go to a file, some might go to a socket, etc.
 *
 * @author Octavian Neamtu
 */

#pragma once

#include <map>
#include <string>

#include "include/io/FileFDProvider.h"

#include "MObjectLogger.h"
#include "memory/MObject.h"
#include "memory/Memory.h"
#include "memory/MemoryIOBoard.h"

namespace man {
namespace memory {
namespace log {

class LoggingBoard : public MemoryIOBoard<MObjectLogger> ,
                     public Subscriber<MObject_ID> {

public:
    LoggingBoard(Memory::const_ptr memory,
                IOProvider::const_ptr ioProvider = IOProvider::NullBulkIO());
    virtual ~LoggingBoard() {}

    void log(MObject_ID id);

    void newIOProvider(IOProvider::const_ptr ioProvider);
    void update(MObject_ID id);

    void startLogging();
    void stopLogging();
    bool isLogging() { return logging; }

protected:
    //returns a NULL pointer if such a logger doesn't exist
    MObjectLogger::const_ptr getLogger(MObject_ID id) const;
    //returns a NULL pointer if such a logger doesn't exist
    MObjectLogger::ptr getMutableLogger(MObject_ID id);

private:
    Memory::const_ptr memory;
    bool logging;
};
}
}
}
