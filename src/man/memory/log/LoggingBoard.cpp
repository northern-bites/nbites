#include "LoggingBoard.h"
#include "NaoPaths.h"

namespace man {
namespace memory {
namespace log {

using namespace man::include::paths;
using boost::shared_ptr;

//const string LoggingBoard::MVISION_PATH = NAO_LOG_DIR + "/Vision.log";
//const string LoggingBoard::MMOTION_SENSORS_PATH = NAO_LOG_DIR + "/MotionSensors.log";
//const string LoggingBoard::MVISION_SENSORS_PATH = NAO_LOG_DIR + "/VisionSensors.log";
//const string LoggingBoard::MIMAGE_PATH = NAO_LOG_DIR + "/Image.log";

LoggingBoard::LoggingBoard(const Memory* memory,
        IOProvider::const_ptr ioProvider) :
    memory(memory) {
    newIOProvider(ioProvider);
}

void LoggingBoard::newIOProvider(IOProvider::const_ptr ioProvider) {

    const IOProvider::FDProviderMap* fdmap = ioProvider->getMap();
    for (IOProvider::FDProviderMap::const_iterator i = fdmap->begin();
            i!= fdmap->end(); i++) {

        if (i->first == MIMAGE_ID) {
            shared_ptr<const RoboImage> roboImage = memory->getRoboImage();
            objectParserMap[MIMAGE_ID] = FDLogger::ptr(new ImageFDLogger(i->second.get(),
                    static_cast<int>(i->first), roboImage.get()));
        } else {
            shared_ptr<const ProtoMessage> mobject =
                    memory->getProtoMessage(i->first);
            if (mobject != shared_ptr<ProtoMessage>()) {
                objectParserMap[i->first] = FDLogger::ptr(new CodedFileLogger(i->second.get(),
                        static_cast<int> (i->first), mobject.get()));
            } else {
                std::cout<<"Invalid Object ID passed for logging: "
                        << "log ID: " << i->first << " "
                        << i->second->debugInfo() << std::endl;
            }
        }
    }
}

//void LoggingBoard::log(const shared_ptr<MObject> mobject) {
//
//    //TODO:Octavian use getLogger
//    ObjectLoggerMap::iterator it = objectLoggerMap.find(mobject);
//    // if this is true, then we found a legitimate logger
//    // corresponding to our mobject in the map
//    if (it != objectLoggerMap.end()) {
//        //it->second is the logger associated with the specified mobject
//        it->second->write();
//    }
//}
//
//const ImageFDLogger* LoggingBoard::getImageLogger(const MImage* mimage) const {
//    return dynamic_cast<const ImageFDLogger*>(this->getLogger(mimage));
//}
//
//const FDLogger* LoggingBoard::getLogger(const MObject* mobject) const {
//    ObjectLoggerMap::const_iterator it = objectLoggerMap.find(mobject);
//    // if this is true, then we found a legitimate logger
//    // corresponding to our mobject in the map
//    if (it != objectLoggerMap.end()) {
//        return it->second;
//    } else {
//        return NULL;
//    }
//}


}
}
}
