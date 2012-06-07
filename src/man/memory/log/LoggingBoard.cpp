#include "LoggingBoard.h"
#include "NaoPaths.h"

#include <cstdio>
#include <iostream>

namespace man {
namespace memory {
namespace log {

using boost::shared_ptr;
using namespace common::io;
using namespace std;

LoggingBoard::LoggingBoard(Memory::const_ptr memory) :
    memory(memory), logging(true) {
}

void LoggingBoard::newOutputProvider(OutProvider::ptr outProvider, std::string name) {

    try {
        ProtobufMessage::const_ptr object = memory->getByName(name);
        MessageLogger::ptr logger(new MessageLogger(outProvider, object));
        objectIOMap[name] = logger;
        memory->subscribe(logger.get(), name);
        //start the logging thread
        logger->start();

    } catch (std::exception& e) {
        cerr << e.what() << endl;
        return ;
    }
}

MessageLogger::const_ptr LoggingBoard::getLogger(std::string name) const {
    ObjectIOMap::const_iterator it = objectIOMap.find(name);
    // if this is true, then we found a legitimate logger
    // corresponding to our mobject in the map
    if (it != objectIOMap.end()) {
        return it->second;
    } else {
        return MessageLogger::const_ptr();
    }
}

MessageLogger::ptr LoggingBoard::getMutableLogger(std::string name) {
    ObjectIOMap::iterator it = objectIOMap.find(name);
    // if this is true, then we found a legitimate logger
    // corresponding to our mobject in the map
    if (it != objectIOMap.end()) {
        return it->second;
    } else {
        return MessageLogger::ptr();
    }
}

void LoggingBoard::reset() {
    ObjectIOMap::iterator it;
    for (it = objectIOMap.begin(); it != objectIOMap.end(); it++) {
        memory->unsubscribe(it->second.get(), it->first);
    }
    objectIOMap.clear();
}

void LoggingBoard::startLogging() {
    printf("Starting logging!\n");
    logging = true;
}

void LoggingBoard::stopLogging() {
    printf("Stopped logging!\n");
    logging = false;
}

}
}
}
