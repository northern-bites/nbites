/**
 *
 * @class DataManager
 *
 * This class should handle the notifying all subscribers to new data
 * coming into the system (be it online or offline)
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

class DataManager : public RobotMemoryManager {

    Q_OBJECT

ADD_SHARED_PTR(DataManager);

public:
    DataManager();

    virtual ~DataManager();

    void getNext() {
        parsingBoard.parseNextAll();
    }

    void getPrev() {
        parsingBoard.rewindAll();
    }

    void startRecordingToPath(std::string path);

public slots:
    void newInputProvider(common::io::InProvider::ptr newInput);

protected:
    man::memory::parse::ParsingBoard parsingBoard;
    man::memory::log::LoggingBoard loggingBoard;

};

}
}
