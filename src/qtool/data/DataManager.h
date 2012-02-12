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

namespace qtool {
namespace data {

class DataManager: public RobotMemoryManager {

Q_OBJECT

ADD_SHARED_PTR(DataManager)

public:
    DataManager();

    virtual ~DataManager();

    void startRecordingToPath(std::string path);

public slots:
    void getNext() {
        parsingBoard.parseNextAll();
    }
    void getPrev() {
        parsingBoard.rewindAll();
    }

    void newInputProvider(common::io::InProvider::ptr newInput,
                          MObject_ID id);
    void reset();

protected:
    man::memory::parse::ParsingBoard parsingBoard;
    man::memory::log::LoggingBoard loggingBoard;

};

}
}
