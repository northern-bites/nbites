#include "DataManager.h"

#include <iostream>

namespace qtool {
namespace data {

using namespace man::memory;
using boost::shared_ptr;

DataManager::DataManager() :
        RobotMemoryManager(Memory::ptr(new Memory())),
        parsingBoard(memory) {

}

DataManager::~DataManager() {
}

void DataManager::newInputProvider(common::io::InProvider::ptr newInput) {
    parsingBoard.newInputProvider(newInput);
}



}
}
