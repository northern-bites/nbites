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

#include "DataSource.h"
#include "man/memory/Memory.h"
#include "man/memory/parse/ParsingBoard.h"
#include "include/MultiProvider.h"

namespace qtool {
namespace data {

enum DataEvent {
    NEW_IMAGE = 17,
    NEW_VISION_SENSORS,
    NEW_MOTION_SENSORS,
    NEW_VISION_INFO
};

class DataManager : public MultiProvider<DataEvent> {

public:
    DataManager();

    virtual ~DataManager();

    void getNext() {
        parsingBoard.parse(man::memory::MIMAGE_ID);
        this->notifySubscribers(NEW_IMAGE);
    }

    man::memory::Memory::ptr getMemory() const {
        return memory;}

    void newDataSource(DataSource::ptr dataSource);

private:
    man::memory::Memory::ptr memory;
    man::memory::parse::ParsingBoard parsingBoard;
    DataSource::ptr dataSource;

};

}
}
