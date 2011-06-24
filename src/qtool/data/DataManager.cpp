#include "DataManager.h"

#include <iostream>

namespace qtool {
namespace data {

using namespace man::memory;
using boost::shared_ptr;

DataManager::DataManager() :
    memory(Memory::ptr(new Memory())),
    parsingBoard(memory) {

}

DataManager::~DataManager() {
}

void DataManager::newDataSource(DataSource::ptr dataSource) {
    parsingBoard.newIOProvider(dataSource);
}

}
}
