#include "ParsingBoard.h"

#include <vector>
#include <iostream>

namespace man {
namespace memory {
namespace parse {

using boost::shared_ptr;
using namespace std;
using namespace common::io;

ParsingBoard::ParsingBoard(Memory::ptr memory) :
    memory(memory) {
}

ParsingBoard::~ParsingBoard(){}

void ParsingBoard::newInputProvider(InProvider::ptr inProvider, MObject_ID id) {

    if (id == UNKNOWN_OBJECT) {
        //warning - if target is a socket, then this might block (potentially
        //forever)
        //TODO: find some way around that (the tricky part is that we use
        // the id we get from reading the log to identify what memory object
        // it's going to be parsed to, so we need to wait on the open is some
        // way)
        inProvider->openCommunicationChannel();
        id = inProvider->peekAndGet<MObject_ID>();
    }

    if (0 < id && id < LAST_OBJECT_ID) {
        MObjectParser::ptr mObjectParser(new MObjectParser(inProvider,
                                memory->getMutableMObject(id)));
        objectIOMap[id] = mObjectParser;
        mObjectParser->start();
    } else {
        cout<<"Could not read valid log ID " << id << " from input: "
            << inProvider->debugInfo() << endl;
    }
}

void ParsingBoard::parseNext(MObject_ID id) {

    ObjectIOMap::iterator it = objectIOMap.find(id);
    // if this is true, then we found a legitimate parser
    // corresponding to our mobject in the map
    if (it != objectIOMap.end()) {
        it->second->signalToParseNext();
    }
}

void ParsingBoard::parseNextAll() {
    for (ObjectIOMap::iterator it = objectIOMap.begin();
            it != objectIOMap.end(); it++ ) {
        it->second->signalToParseNext();
    }

}

void ParsingBoard::rewind(MObject_ID id) {

    ObjectIOMap::iterator it = objectIOMap.find(id);
    // if this is true, then we found a legitimate parser
    // corresponding to our mobject in the map
    if (it != objectIOMap.end()) {
        //it->second is the parser associated with the specified mobject
        it->second->getPrev();
    }
}

void ParsingBoard::rewindAll() {
    for (ObjectIOMap::iterator it = objectIOMap.begin();
            it != objectIOMap.end(); it++ ) {
        it->second->getPrev();
    }

}

}
}
}
