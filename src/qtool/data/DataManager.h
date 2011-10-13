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

#include <boost/shared_ptr.hpp>

#include "DataSource.h"
#include "man/memory/Memory.h"
#include "man/memory/parse/ParsingBoard.h"
#include "include/MultiProvider.h"
#include "DataTypes.h"

#include <iostream>

namespace qtool {
namespace data {

class DataManager : public MultiProvider<MObject_ID> {

public:
	typedef boost::shared_ptr<DataManager> ptr;

public:
    DataManager();

    virtual ~DataManager();

    void getNext() {
        parsingBoard.parseAll();
        this->notifySubscribers(man::memory::MIMAGE_ID);
    }

    void getPrev() {
        parsingBoard.rewindAll();
        this->notifySubscribers(man::memory::MIMAGE_ID);
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
