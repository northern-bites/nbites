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
        loggingBoard(memory),
        is_recording(false) {

}

DataManager::~DataManager() {
}

<<<<<<< HEAD
void DataManager::newDataSource(DataSource::ptr dataSource) {
    parsingBoard.newIOProvider(dataSource);
    this->getNext();
=======
void DataManager::newInputProvider(common::io::InProvider::ptr newInput,
                                   MObject_ID id) {
    parsingBoard.newInputProvider(newInput, id);
>>>>>>> nbites
}

void DataManager::reset() {
    parsingBoard.reset();
    loggingBoard.reset();
}

void DataManager::startRecordingToPath(string path) {
    is_recording = true;
    log::OutputProviderFactory::AllFileOutput(&loggingBoard, path);
}

void DataManager::stopRecording() {
    is_recording = false;
    loggingBoard.reset();
}

}
}
