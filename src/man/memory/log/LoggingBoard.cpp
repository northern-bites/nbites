#include "LoggingBoard.h"
#include "NaoPaths.h"

namespace memory {

namespace log {

const char* LoggingBoard::MVISION_PATH = NAO_LOG_DIR "/Vision.log";
const char* LoggingBoard::MMOTION_SENSORS_PATH = NAO_LOG_DIR "/MotionSensors.log";
const char* LoggingBoard::MVISION_SENSORS_PATH = NAO_LOG_DIR "/VisionSensors.log";
const char* LoggingBoard::MIMAGE_PATH = NAO_LOG_DIR "/Image.log";

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

}

}
