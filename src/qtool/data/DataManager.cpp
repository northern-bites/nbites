#include "DataManager.h"

#include <iostream>
#include "man/memory/log/OutputProviderFactory.h"
#include "io/FileOutProvider.h"
#include "io/FileInProvider.h"

namespace qtool {
namespace data {

using namespace man::memory;
using namespace man::memory::log;
using namespace man::memory::parse;
using namespace overseer;
using namespace std;
using namespace common::io;
using boost::shared_ptr;

DataManager::DataManager(QObject* parent) :
        RobotMemoryManager(RobotMemory::ptr(new RobotMemory()), parent),
        parsingBoard(memory),
        loggingBoard(memory),
        is_recording(false) {

    // not really a memory object, but hey it works
    memory->addObject<overseer::GroundTruth>();
}

DataManager::~DataManager() {
    this->reset();
}

void DataManager::newInputProvider(InProvider::ptr newInput, std::string name) {
    parsingBoard.newInputProvider(newInput, name);
}

void DataManager::reset() {
    parsingBoard.reset();
    loggingBoard.reset();
}

void DataManager::startRecordingToPath(string path) {
    is_recording = true;
    log::OutputProviderFactory::AllFileOutput(memory.get(), &loggingBoard, path);
}

void DataManager::stopRecording() {
    is_recording = false;
    loggingBoard.reset();
}

}
}
