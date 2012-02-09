#include "LoggingBoard.h"
#include "NaoPaths.h"

#include <cstdio>

namespace man {
namespace memory {
namespace log {

using boost::shared_ptr;

LoggingBoard::LoggingBoard(Memory::const_ptr memory) :
    memory(memory), logging(true) {
}

void LoggingBoard::newOutputProvider(OutProvider::ptr outProvider,
									 MObject_ID id) {

    MObjectLogger::ptr logger(
    			new MObjectLogger(outProvider,  memory->getMObject(id)));
    objectIOMap[id] = logger;
    memory->subscribe(logger.get(), id);
    //start the logging thread
    logger->start();
}

MObjectLogger::const_ptr LoggingBoard::getLogger(MObject_ID id) const {
    ObjectIOMap::const_iterator it = objectIOMap.find(id);
    // if this is true, then we found a legitimate logger
    // corresponding to our mobject in the map
    if (it != objectIOMap.end()) {
        return it->second;
    } else {
        return MObjectLogger::const_ptr();
    }
}

MObjectLogger::ptr LoggingBoard::getMutableLogger(MObject_ID id) {
    ObjectIOMap::iterator it = objectIOMap.find(id);
    // if this is true, then we found a legitimate logger
    // corresponding to our mobject in the map
    if (it != objectIOMap.end()) {
        return it->second;
    } else {
        return MObjectLogger::ptr();
    }
}

void LoggingBoard::startLogging() {
    printf("Starting logging!\n");
    logging = true;
}

void LoggingBoard::stopLogging() {
    printf("Stopped logging!\n");
    logging = false;
}

}
}
}
