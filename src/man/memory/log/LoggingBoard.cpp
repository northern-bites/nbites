#include "LoggingBoard.h"
#include "NaoPaths.h"

namespace man {
namespace memory {
namespace log {

using namespace man::include::paths;

const string LoggingBoard::MVISION_PATH = NAO_LOG_DIR + "/Vision.log";
const string LoggingBoard::MMOTION_SENSORS_PATH = NAO_LOG_DIR + "/MotionSensors.log";
const string LoggingBoard::MVISION_SENSORS_PATH = NAO_LOG_DIR + "/VisionSensors.log";
const string LoggingBoard::MIMAGE_PATH = NAO_LOG_DIR + "/Image.log";

LoggingBoard::LoggingBoard(const Memory* _memory) :
    memory(_memory) {
    initLoggingObjects();
}

void LoggingBoard::initLoggingObjects() {

    const MVision* mvision = memory->getMVision();
    FDProvider* mvisionFDprovider = new FileFDProvider(MVISION_PATH);
    objectFDProviderMap[mvision] = mvisionFDprovider;
    objectFDLoggerMap[mvision] = new CodedFileLogger(mvisionFDprovider,
            MVISION_ID, mvision);

    const MMotionSensors* mmotionSensors = memory->getMMotionSensors();
    FDProvider* mmotionSensorsFDprovider = new FileFDProvider(
            MMOTION_SENSORS_PATH);
    objectFDProviderMap[mmotionSensors] = mmotionSensorsFDprovider;
    objectFDLoggerMap[mmotionSensors] = new CodedFileLogger(
            mmotionSensorsFDprovider, MMOTION_SENSORS_ID, mmotionSensors);

    const MVisionSensors* mvisionSensors = memory->getMVisionSensors();
    FDProvider* mvisionSensorsFDprovider = new FileFDProvider(
            MVISION_SENSORS_PATH);
    objectFDProviderMap[mvisionSensors] = mvisionSensorsFDprovider;
    objectFDLoggerMap[mvisionSensors] = new CodedFileLogger(
            mvisionSensorsFDprovider, MVISION_SENSORS_ID, mvisionSensors);

    const MImage* mimage = memory->getMImage();
    FDProvider* mimageFDprovider = new FileFDProvider(MIMAGE_PATH);
    objectFDProviderMap[mimage] = mimageFDprovider;
    objectFDLoggerMap[mimage] = new ImageFDLogger(mimageFDprovider,
            MIMAGE_ID, mimage);
}

void LoggingBoard::log(const MObject* mobject) {

    ObjectFDLoggerMap::iterator it = objectFDLoggerMap.find(mobject);
    // if this is true, then we found a legitimate logger
    // corresponding to our mobject in the map
    if (it != objectFDLoggerMap.end()) {
        //it->second is the logger associated with the specified mobject
        it->second->write();
    }
}

const ImageFDLogger* LoggingBoard::getImageLogger(const MImage* mimage) const {
    return dynamic_cast<const ImageFDLogger*>(this->getLogger(mimage));
}

const FDLogger* LoggingBoard::getLogger(const MObject* mobject) const {
    ObjectFDLoggerMap::const_iterator it = objectFDLoggerMap.find(mobject);
    // if this is true, then we found a legitimate logger
    // corresponding to our mobject in the map
    if (it != objectFDLoggerMap.end()) {
        return it->second;
    } else {
        return NULL;
    }
}


}
}
}
