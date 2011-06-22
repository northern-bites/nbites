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

    void getNext() {
        parsingBoard.parseAll();
        this->notifySubscribers(NEW_IMAGE);
    }

    boost::shared_ptr<const man::memory::Memory> getMemory() const {
        return memory;}

    void newDataSource(DataSource::ptr dataSource);

private:
    boost::shared_ptr<man::memory::Memory> memory;
    man::memory::parse::ParsingBoard parsingBoard;
    DataSource::ptr dataSource;

};

}
}
