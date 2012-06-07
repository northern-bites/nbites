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

DataManager::DataManager() :
        RobotMemoryManager(Memory::ptr(new Memory())),
        groundTruth(new overseer::GroundTruth()),
        groundTruthParser(NULL),
        groundTruthLogger(NULL),
        parsingBoard(memory),
        loggingBoard(memory),
        is_recording(false) {

}

DataManager::~DataManager() {
    this->reset();
}

void DataManager::newInputProvider(InProvider::ptr newInput, std::string name) {
    parsingBoard.newInputProvider(newInput, name);
}

void DataManager::newGroundTruthProvider(InProvider::ptr input) {
    if (groundTruthParser) {
        delete groundTruthParser;
        groundTruthParser = NULL;
    }
    groundTruthParser = new MessageParser(input, groundTruth);
    groundTruthParser->start();
}

void DataManager::reset() {
    parsingBoard.reset();
    loggingBoard.reset();
    if (groundTruthParser) {
        delete groundTruthParser;
        groundTruthParser = NULL;
    }
    if (groundTruthLogger) {
        delete groundTruthLogger;
        groundTruthLogger = NULL;
    }
}

void DataManager::startRecordingToPath(string path) {
    is_recording = true;
    log::OutputProviderFactory::AllFileOutput(memory.get(), &loggingBoard, path);

    FileOutProvider::ptr file_out(new FileOutProvider(path + "/GroundTruth.log"));
    groundTruthLogger = new MessageLogger(file_out, groundTruth);
    groundTruth->addSubscriber(groundTruthLogger);
    groundTruthLogger->start();
}

void DataManager::stopRecording() {
    is_recording = false;
    loggingBoard.reset();
    if (groundTruthLogger) {
        groundTruth->unsubscribe(groundTruthLogger);
        delete groundTruthLogger;
        groundTruthLogger = NULL;
    }
}

}
}
