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

void DataManager::newInputProvider(common::io::InProvider::const_ptr newInput) {
    parsingBoard.newInputProvider(newInput);
}

void DataManager::addSubscriber(Subscriber* subscriber, MObject_ID mobject_id) {
    memory->addSubscriber(subscriber, mobject_id);
}

}
}
