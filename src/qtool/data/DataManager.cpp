#include "DataManager.h"

#include <iostream>

namespace qtool {
namespace data {

using man::memory::Memory;
using boost::shared_ptr;

DataManager::DataManager() :
    memory(shared_ptr<Memory>(new Memory())),
    parsingBoard(memory.get()) {

}

void DataManager::newDataSource(DataSource::ptr dataSource) {
    std::cout << "yup" << std::endl;
}

}
}
