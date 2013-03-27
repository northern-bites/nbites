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
#include "ClassHelper.h"

#include "RobotMemoryManager.h"

#include "Typedefs.h"

#include "man/memory/parse/ParsingBoard.h"
#include "man/memory/log/LoggingBoard.h"
#include "overseer/GroundTruth.h"

namespace qtool {
namespace data {

class DataManager: public RobotMemoryManager {

Q_OBJECT

ADD_SHARED_PTR(DataManager)

public:
    DataManager(QObject* parent = 0);

    virtual ~DataManager();

    void startRecordingToPath(std::string path);
    void stopRecording();
    bool isRecording() const { return is_recording; }

public slots:
    void getNext() {
        parsingBoard.parseNextAll();
    }

    void getPrev() {
        parsingBoard.rewindAll();
    }

    void newInputProvider(common::io::InProvider::ptr newInput, std::string name);
    void reset();

protected:
    man::memory::parse::ParsingBoard parsingBoard;
    man::memory::log::LoggingBoard loggingBoard;
    bool is_recording;

};

}
}
