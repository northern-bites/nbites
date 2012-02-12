#include "DataManager.h"

#include <iostream>
#include "man/memory/log/OutputProviderFactory.h"

namespace qtool {
namespace data {

using namespace man::memory;
using namespace std;
using boost::shared_ptr;

DataManager::DataManager() :
        RobotMemoryManager(Memory::ptr(new Memory())),
        parsingBoard(memory),
        loggingBoard(memory) {

}

DataManager::~DataManager() {
}

void DataManager::newInputProvider(common::io::InProvider::ptr newInput,
                                   MObject_ID id) {
    parsingBoard.newInputProvider(newInput, id);
}

void DataManager::reset() {
    parsingBoard.reset();
    loggingBoard.reset();
}

void DataManager::startRecordingToPath(string path) {
    log::OutputProviderFactory::AllFileOutput(&loggingBoard, path);
}

}
}
