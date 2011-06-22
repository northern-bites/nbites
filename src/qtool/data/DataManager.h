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

#include "DataHandler.h"
#include "memory/Memory.h"
#include "include/MultiProvider.h"
#include "boost/shared_ptr.hpp"

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

    void getNext() const {
        dataHandler->readNext();
        this->notifySubscribers(NEW_IMAGE);
    }

    boost::shared_ptr<const man::memory::Memory> getMemory() const {
        return memory;}

private:
    boost::shared_ptr<man::memory::Memory> memory;
    DataHandler* dataHandler;

};

}
}
