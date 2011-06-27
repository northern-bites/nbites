#include "LoggingBoard.h"
#include "NaoPaths.h"

namespace man {
namespace memory {
namespace log {

using namespace man::include::paths;
using boost::shared_ptr;

LoggingBoard::LoggingBoard(Memory::const_ptr memory,
        IOProvider::const_ptr ioProvider) :
    memory(memory) {
    newIOProvider(ioProvider);
}

void LoggingBoard::newIOProvider(IOProvider::const_ptr ioProvider) {

    this->ioProvider = ioProvider;

    const IOProvider::FDProviderMap* fdmap = ioProvider->getMap();
    for (IOProvider::FDProviderMap::const_iterator i = fdmap->begin();
            i!= fdmap->end(); i++) {

        if (i->first == MIMAGE_ID) {
            shared_ptr<const RoboImage> roboImage = memory->getRoboImage();
            objectIOMap[MIMAGE_ID] = FDLogger::ptr(new ImageLogger(i->second.get(),
                    static_cast<int>(i->first), roboImage.get()));
        } else {
            shared_ptr<const ProtoMessage> mobject =
                    memory->getProtoMessage(i->first);
            if (mobject != shared_ptr<ProtoMessage>()) {
                objectIOMap[i->first] = FDLogger::ptr(new CodedFileLogger(i->second.get(),
                        static_cast<int> (i->first), mobject.get()));
            } else {
                std::cout<<"Invalid Object ID passed for logging: "
                        << "log ID: " << i->first << " "
                        << i->second->debugInfo() << std::endl;
            }
        }
    }
}

void LoggingBoard::log(MObject_ID id) {
    FDLogger::ptr logger = getMutableLogger(id);
    if (logger.get() != NULL) {
        logger->writeToLog();
    }
}

ImageLogger::const_ptr LoggingBoard::getImageLogger(MObject_ID id) const {
    return boost::dynamic_pointer_cast<const ImageLogger>(
            this->getLogger(MIMAGE_ID));
}

FDLogger::const_ptr LoggingBoard::getLogger(MObject_ID id) const {
    ObjectIOMap::const_iterator it = objectIOMap.find(id);
    // if this is true, then we found a legitimate logger
    // corresponding to our mobject in the map
    if (it != objectIOMap.end()) {
        return it->second;
    } else {
        return FDLogger::const_ptr();
    }
}

FDLogger::ptr LoggingBoard::getMutableLogger(MObject_ID id) {
    ObjectIOMap::iterator it = objectIOMap.find(id);
    // if this is true, then we found a legitimate logger
    // corresponding to our mobject in the map
    if (it != objectIOMap.end()) {
        return it->second;
    } else {
        return FDLogger::ptr();
    }
}


}
}
}
