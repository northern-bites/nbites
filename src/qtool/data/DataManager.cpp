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

void DataManager::addSubscriber(Subscriber<MObject_ID>* subscriber) {
    memory->addSubscriber(subscriber);
}

void DataManager::addSubscriber(Subscriber<MObject_ID>* subscriber,
            MObject_ID mobject_id) {
    memory->addSubscriber(subscriber, mobject_id);
}

}
}
