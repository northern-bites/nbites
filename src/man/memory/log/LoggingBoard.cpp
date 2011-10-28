#include "LoggingBoard.h"
#include "NaoPaths.h"

#include <cstdio>

namespace man {
namespace memory {
namespace log {

using namespace man::include::paths;
using boost::shared_ptr;

LoggingBoard::LoggingBoard(Memory::const_ptr memory, IOProvider::const_ptr ioProvider) :
    memory(memory), logging(true) {
    newIOProvider(ioProvider);
}

void LoggingBoard::newIOProvider(IOProvider::const_ptr ioProvider) {

    this->ioProvider = ioProvider;

    const IOProvider::FDProviderMap* fdmap = ioProvider->getMap();
    for (IOProvider::FDProviderMap::const_iterator i = fdmap->begin();
            i!= fdmap->end(); i++) {

        MObject::const_ptr mobject =
                memory->getMObject(i->first);
        if (mobject != MObject::const_ptr()) {
            objectIOMap[i->first] = MObjectLogger::ptr(
                    new MObjectLogger(i->second,
                                      static_cast<int> (i->first), mobject));
            objectIOMap[i->first]->start();
        } else {
            std::cout<<"Invalid Object ID passed for logging: "
                    << "log ID: " << i->first << " "
                    << i->second->debugInfo() << std::endl;
        }
    }
}

void LoggingBoard::update(MObject_ID id) {
    this->log(id);
}

void LoggingBoard::log(MObject_ID id) {
    if (logging) {
        MObjectLogger::ptr logger = getMutableLogger(id);
        if (logger.get() != NULL) {
            logger->signalToLog();
        }
    }
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
