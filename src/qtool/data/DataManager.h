/**
 *
 * @class DataManager
 *
 * This class is an empowered RobotMemoryManager imbued with a parsing board and
 * a logging board for easy IO from and to the managed memory.
 *
 * @author Octavian Neamtu
 *
 */

#pragma once

#include <QObject>

#include "io/InProvider.h"
#include "man/memory/Memory.h"
#include "man/memory/parse/ParsingBoard.h"
#include "man/memory/log/LoggingBoard.h"
#include "DataTypes.h"
#include "ClassHelper.h"
#include "RobotMemoryManager.h"
#include "overseer/GroundTruth.h"

namespace qtool {
namespace data {

class DataManager: public RobotMemoryManager {

Q_OBJECT

ADD_SHARED_PTR(DataManager)

public:
    DataManager();

    virtual ~DataManager();

    void startRecordingToPath(std::string path);
    void stopRecording();
    bool isRecording() const { return is_recording; }

    overseer::GroundTruth::ptr getGroundTruth() { return groundTruth; }

public slots:
    void getNext() {
        parsingBoard.parseNextAll();
        if (groundTruthParser) {
            groundTruthParser->signalToParseNext();
        }
    }
    void getPrev() {
        parsingBoard.rewindAll();
        if (groundTruthParser) {
            groundTruthParser->getPrev();
        }
    }

    void newInputProvider(common::io::InProvider::ptr newInput,
                          MObject_ID id);
    void newGroundTruthProvider(common::io::InProvider::ptr input);
    void reset();

protected:
    overseer::GroundTruth::ptr groundTruth;
    //TODO: ground truth parsing/logging is kind of gross
    //we should abstract the parsing board and the logging board
    //to include the ground truth (and make it independent of memory)
    man::memory::log::MessageLogger* groundTruthLogger;
    man::memory::parse::MessageParser* groundTruthParser;
    man::memory::parse::ParsingBoard parsingBoard;
    man::memory::log::LoggingBoard loggingBoard;
    bool is_recording;

};

}
}
