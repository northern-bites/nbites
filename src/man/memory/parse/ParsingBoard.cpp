#include "ParsingBoard.h"

namespace man {
namespace memory {
namespace parse {

//const char* ParsingBoard::MVISION_PATH = NAO_LOG_DIR "/Vision.log";
//const char* ParsingBoard::MMOTION_SENSORS_PATH = NAO_LOG_DIR "/MotionSensors.log";
//const char* ParsingBoard::MVISION_SENSORS_PATH = NAO_LOG_DIR "/VisionSensors.log";
//const char* ParsingBoard::MIMAGE_PATH = NAO_LOG_DIR "/Image.log";

using boost::shared_ptr;

ParsingBoard::ParsingBoard(Memory* memory) :
    memory(memory) {
    initParsingObjects();
}

void ParsingBoard::initParsingObjects() {

//    const MVision* mvision = memory->getMVision();
//    FDProvider* mvisionFDprovider = new FileFDProvider(MVISION_PATH);
//    objectFDProviderMap[mvision] = mvisionFDprovider;
//    objectFDLoggerMap[mvision] = new CodedFileLogger(mvisionFDprovider,
//            MVISION_ID, mvision);
//
//    const MMotionSensors* mmotionSensors = memory->getMMotionSensors();
//    FDProvider* mmotionSensorsFDprovider = new FileFDProvider(
//            MMOTION_SENSORS_PATH);
//    objectFDProviderMap[mmotionSensors] = mmotionSensorsFDprovider;
//    objectFDLoggerMap[mmotionSensors] = new CodedFileLogger(
//            mmotionSensorsFDprovider, MMOTION_SENSORS_ID, mmotionSensors);
//
//    const MVisionSensors* mvisionSensors = memory->getMVisionSensors();
//    FDProvider* mvisionSensorsFDprovider = new FileFDProvider(
//            MVISION_SENSORS_PATH);
//    objectFDProviderMap[mvisionSensors] = mvisionSensorsFDprovider;
//    objectFDLoggerMap[mvisionSensors] = new CodedFileLogger(
//            mvisionSensorsFDprovider, MVISION_SENSORS_ID, mvisionSensors);

    MImage* mimage = memory->getMutableMImage();
    FDProvider* mimageFDprovider = new FileFDProvider("/home/oneamtu/log/trillian/Image.log");
    objectFDProviderMap[mimage] = mimageFDprovider;
    objectParserMap[mimage] = new ImageParser("/home/oneamtu/log/spock/Image.log",
            shared_ptr<MImage>(mimage));
}

void ParsingBoard::parse(const MObject* mobject) {

    ObjectParserMap::iterator it = objectParserMap.find(mobject);
    // if this is true, then we found a legitimate logger
    // corresponding to our mobject in the map
    if (it != objectParserMap.end()) {
        //it->second is the logger associated with the specified mobject
        it->second->getNext();
    }
}

void ParsingBoard::parseAll() {

    for (ObjectParserMap::iterator it = objectParserMap.begin();
            it != objectParserMap.end(); it++ ) {
        it->second->getNext();
    }

}

//const ImageParser* ParsingBoard::getImageLogger(const MImage* mimage) const {
//    return dynamic_cast<const ImageParser*>(this->getLogger(mimage));
//}

//const Parser* ParsingBoard::getLogger(const MObject* mobject) const {
//    ObjectFDLoggerMap::const_iterator it = objectFDLoggerMap.find(mobject);
//    // if this is true, then we found a legitimate logger
//    // corresponding to our mobject in the map
//    if (it != objectFDLoggerMap.end()) {
//        return it->second;
//    } else {
//        return NULL;
//    }
//}


}
}
}
