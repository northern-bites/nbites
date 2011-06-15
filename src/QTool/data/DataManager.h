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
#include "include/Provider.h"
#include "boost/shared_ptr.hpp"

namespace qtool {
namespace data {

class DataManager : public Provider {

public:
    DataManager();

    void getNext() {
        dataHandler->readNext();
        this->notify(0);
    }

    boost::shared_ptr<const memory::Memory> getMemory() const {
        return boost::shared_ptr<const memory::Memory>(memory);}

private:
    //TODO: make this a shared ptr
    memory::Memory* memory;
    DataHandler* dataHandler;

};

}
}
